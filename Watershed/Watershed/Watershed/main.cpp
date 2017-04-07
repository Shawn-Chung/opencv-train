#include <iostream>

#include <opencv2\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

using namespace std;
using namespace cv;


typedef struct CvWSNode
{
	struct CvWSNode* next;
	int mask_ofs;
	int img_ofs;
}
CvWSNode;

typedef struct CvWSQueue
{
	CvWSNode* first;
	CvWSNode* last;
}
CvWSQueue;

CvWSNode* icvAllocWSNodes(CvMemStorage* storage)
{
	CvWSNode* n = 0;

	int i, count = (storage->block_size - sizeof(CvMemBlock)) / sizeof(*n) - 1;

	n = (CvWSNode*)cvMemStorageAlloc(storage, count*sizeof(*n));
	for (i = 0; i < count - 1; i++)
		n[i].next = n + i + 1;
	n[count - 1].next = 0;

	return n;
}
void Watershed(InputArray __src, InputOutputArray __markers)
{

	CvMat __srcarr = __src.getMat(), __dstarr = __markers.getMat();
	CvMat *srcarr = &__srcarr, *dstarr = &__dstarr;


	const int IN_QUEUE = -2;
	const int WSHED = -1;
	const int NQ = 256;
	cv::Ptr<CvMemStorage> storage;

	CvMat sstub, *src;
	CvMat dstub, *dst;
	CvSize size;
	CvWSNode* free_node = 0, *node;
	CvWSQueue q[NQ];
	int active_queue;
	int i, j;
	//int db, dg, dr;
	int* mask;
	uchar* img;
	int mstep, istep;
	int subs_tab[513];

	// MAX(a,b) = b + MAX(a-b,0)
#define ws_max(a,b) ((b) + subs_tab[(a)-(b)+NQ])
	// MIN(a,b) = a - MAX(a-b,0)
#define ws_min(a,b) ((a) - subs_tab[(a)-(b)+NQ])

#define ws_push(idx,mofs,iofs)  \
	{                               \
	if (!free_node)            \
	free_node = icvAllocWSNodes(storage); \
	node = free_node;           \
	free_node = free_node->next; \
	node->next = 0;             \
	node->mask_ofs = mofs;      \
	node->img_ofs = iofs;       \
	if (q[idx].last)           \
	q[idx].last->next = node; \
		else                        \
		q[idx].first = node;    \
		q[idx].last = node;         \
	}

#define ws_pop(idx,mofs,iofs)   \
	{                               \
	node = q[idx].first;        \
	q[idx].first = node->next;  \
	if (!node->next)           \
	q[idx].last = 0;        \
	node->next = free_node;     \
	free_node = node;           \
	mofs = node->mask_ofs;      \
	iofs = node->img_ofs;       \
	}

#define c_diff(ptr1,ptr2,diff)      \
	{                                   \
	diff = abs((ptr1)[0] - (ptr2)[0]); \
	assert(0 <= diff && diff <= 255); \
	}

	src = cvGetMat(srcarr, &sstub);
	dst = cvGetMat(dstarr, &dstub);

	if (CV_MAT_TYPE(src->type) != CV_8UC1)
		CV_Error(CV_StsUnsupportedFormat, "Only 8-bit, 1-channel input images are supported");

	if (CV_MAT_TYPE(dst->type) != CV_32SC1)
		CV_Error(CV_StsUnsupportedFormat,
		"Only 32-bit, 1-channel output images are supported");

	if (!CV_ARE_SIZES_EQ(src, dst))
		CV_Error(CV_StsUnmatchedSizes, "The input and output images must have the same size");

	size.height = src->rows;
	size.width = src->cols;

	storage = cvCreateMemStorage();

	istep = src->step;
	img = src->data.ptr;
	mstep = dst->step / sizeof(mask[0]);
	mask = dst->data.i;

	memset(q, 0, NQ*sizeof(q[0]));

	for (i = 0; i < 256; i++)
		subs_tab[i] = 0;
	for (i = 256; i <= 512; i++)
		subs_tab[i] = i - 256;

	// draw a pixel-wide border of dummy "watershed" (i.e. boundary) pixels
	for (j = 0; j < size.width; j++)
		mask[j] = mask[j + mstep*(size.height - 1)] = WSHED;

	// initial phase: put all the neighbor pixels of each marker to the ordered queue -
	// determine the initial boundaries of the basins
	for (i = 1; i < size.height - 1; i++)
	{
		img += istep; mask += mstep;
		mask[0] = mask[size.width - 1] = WSHED;

		for (j = 1; j < size.width - 1; j++)
		{
			int* m = mask + j;
			if (m[0] < 0) m[0] = 0;
			//当前像素是未知像素，且其上下左右至少有一个是标记像素
			if (m[0] == 0 && (m[-1] > 0 || m[1] > 0 || m[-mstep] > 0 || m[mstep] > 0))
			{
				uchar* ptr = img + j;//原图形是三通道，所以*3
				int idx = 256, t;
				if (m[-1] > 0)
					c_diff(ptr, ptr - 1, idx);//idx返回RGB三通道差值最大的一个
				if (m[1] > 0)
				{
					c_diff(ptr, ptr + 1, t);
					idx = ws_min(idx, t);
				}
				if (m[-mstep] > 0)
				{
					c_diff(ptr, ptr - istep, t);
					idx = ws_min(idx, t);
				}
				if (m[mstep] > 0)
				{
					c_diff(ptr, ptr + istep, t);
					idx = ws_min(idx, t);
				}
				assert(0 <= idx && idx <= 255);
				ws_push(idx, i*mstep + j, i*istep + j);//将该像素在 标记图像和原图像中的位置信息压如队列
				m[0] = IN_QUEUE;
			}
		}
	}

	// find the first non-empty queue
	for (i = 0; i < NQ; i++)
	if (q[i].first)
		break;

	// if there is no markers, exit immediately
	if (i == NQ)
		return;

	active_queue = i;
	img = src->data.ptr;
	mask = dst->data.i;

	// recursively fill the basins
	for (;;)
	{
		int mofs, iofs;
		int lab = 0, t;
		int* m;
		uchar* ptr;

		if (q[active_queue].first == 0)
		{
			for (i = active_queue + 1; i < NQ; i++)
			if (q[i].first)
				break;
			if (i == NQ)
				break;
			active_queue = i;
		}

		ws_pop(active_queue, mofs, iofs);

		m = mask + mofs;
		ptr = img + iofs;
		t = m[-1];
		if (t > 0) lab = t;
		t = m[1];
		if (t > 0)
		{
			if (lab == 0) lab = t;
			else if (t != lab) lab = WSHED;
		}
		t = m[-mstep];
		if (t > 0)
		{
			if (lab == 0) lab = t;
			else if (t != lab) lab = WSHED;
		}
		t = m[mstep];
		if (t > 0)
		{
			if (lab == 0) lab = t;
			else if (t != lab) lab = WSHED;
		}
		assert(lab != 0);
		m[0] = lab;
		if (lab == WSHED)
			continue;

		if (m[-1] == 0)
		{
			c_diff(ptr, ptr - 1, t);
			ws_push(t, mofs - 1, iofs - 1);
			active_queue = ws_min(active_queue, t);
			m[-1] = IN_QUEUE;
		}
		if (m[1] == 0)
		{
			c_diff(ptr, ptr + 1, t);
			ws_push(t, mofs + 1, iofs + 1);
			active_queue = ws_min(active_queue, t);
			m[1] = IN_QUEUE;
		}
		if (m[-mstep] == 0)
		{
			c_diff(ptr, ptr - istep, t);
			ws_push(t, mofs - mstep, iofs - istep);
			active_queue = ws_min(active_queue, t);
			m[-mstep] = IN_QUEUE;
		}
		if (m[mstep] == 0)
		{
			c_diff(ptr, ptr + istep, t);
			ws_push(t, mofs + mstep, iofs + istep);
			active_queue = ws_min(active_queue, t);
			m[mstep] = IN_QUEUE;
		}
	}
}



int main(int argc, char **argv)
{
	string fileName = "D:/Document/VS workplace/barCode/srcImage/timg.jpg";
	Mat src = imread(fileName, CV_LOAD_IMAGE_GRAYSCALE);
	imshow("src", src);
	
	Mat gray = src.clone();
	//cvtColor(src, gray, CV_BGR2GRAY);

	Mat threod;
	threshold(gray, threod, 60, 255, CV_THRESH_BINARY);
	imshow("threshold", threod);

	Mat fg;
	erode(threod, fg, Mat(), Point(-1, -1), 8);
	imshow("fg", fg);

	Mat bg;
	dilate(threod, bg, Mat(), Point(-1, -1), 8);
	threshold(bg, bg, 1, 128, THRESH_BINARY_INV);
	imshow("bg", bg);

	Mat marker = fg + bg;
    imshow("marker", marker);
	marker.convertTo(marker, CV_32S);

	Watershed(src, marker);
	Mat result;
	marker.convertTo(result, CV_8U);
	imshow("result", result);




	waitKey(0);
	return 0;
}