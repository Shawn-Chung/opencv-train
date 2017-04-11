#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h> 
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstdlib>

using namespace cv;
using namespace std;

const double pi = 3.1415926535897932384626433832795;

class CFT1D{

public:
	//一维 离散傅里叶变换/逆变换 DFT
	void DFT(int n, bool inverse, const double *gRe, const double *gIm, double *GRe, double *GIm)
	{
		for (int w = 0; w < n; w++)
		{
			GRe[w] = GIm[w] = 0;
			for (int x = 0; x < n; x++)
			{
				double a = -2 * pi * w * x / float(n);
				if (inverse) a = -a;
				double ca = cos(a);
				double sa = sin(a);
				//复数乘法
				GRe[w] += gRe[x] * ca - gIm[x] * sa;
				GIm[w] += gRe[x] * sa + gIm[x] * ca;
			}
			if (!inverse)
			{
				GRe[w] /= n;
				GIm[w] /= n;
			}
		}
	}
	//一维 离散快速傅里叶变换/逆变换 FFT
	//按时间抽取的 基2 FFT算法
	void FFT(int n, bool inverse, const double *fRe, const double *fIm, double *GRe, double *GIm)
	{

		//Calculate m=log_2(n)
		//计算可分解为 m 阶蝶形运算
		int m = 0;
		int p = 1;
		while (p < n)
		{
			p *= 2;
			m++;
		}
		//Bit reversal 蝶形运算需要，用了雷德算法 实现（下标）倒序数
		GRe[n - 1] = fRe[n - 1];
		GIm[n - 1] = fIm[n - 1];
		int j = 0;
		for (int i = 0; i < n - 1; i++)
		{
			GRe[i] = fRe[j];
			GIm[i] = fIm[j];
			int k = n / 2;
			while (k <= j)
			{
				j -= k;
				k /= 2;
			}
			j += k;
		}
		//Calculate the FFT
		double ca = -1.0;
		double sa = 0.0;
		int l1 = 1, l2 = 1;
		for (int l = 0; l < m; l++)
		{
			l1 = l2;
			l2 *= 2;
			double u1 = 1.0;
			double u2 = 0.0;
			for (int j = 0; j < l1; j++)
			{
				for (int i = j; i < n; i += l2)
				{
					int i1 = i + l1;
					double t1 = u1 * GRe[i1] - u2 * GIm[i1];
					double t2 = u1 * GIm[i1] + u2 * GRe[i1];
					GRe[i1] = GRe[i] - t1;
					GIm[i1] = GIm[i] - t2;
					GRe[i] += t1;
					GIm[i] += t2;
				}
				double z = u1 * ca - u2 * sa;
				u2 = u1 * sa + u2 * ca;
				u1 = z;
			}
			sa = sqrt((1.0 - ca) / 2.0);
			if (!inverse) sa = -sa;
			ca = sqrt((1.0 + ca) / 2.0);
		}
		//Divide through n if it isn't the IDFT
		if (!inverse)
		for (int i = 0; i < n; i++)
		{
			GRe[i] /= n;
			GIm[i] /= n;
		}
	}
	//计算幅度
	void calculateAmp(int n, double *gAmp, const double *gRe, const double *gIm)
	{
		for (int x = 0; x < n; x++)
		{
			gAmp[x] = sqrt(gRe[x] * gRe[x] + gIm[x] * gIm[x]);
		}
	}

};

class CFT2D{
	//二维（图像）傅里叶变换思路：先按行进行一维变换，在此结果之上按列进行一维变换
	//此类中的一维 DFT 和 FFT 实现同一维中的方法，只不过针对图像就够进行了步长的修改
public:
	void DFT2D(int w, int h, bool inverse, const double *gRe, const double *gIm, double *GRe, double *GIm)
	{
		//temporary buffers
		std::vector<double> Gr2(h * w * 3);
		std::vector<double> Gi2(h * w * 3);

		for (int y = 0; y < h; y++) // for each row
		for (int c = 0; c < 3; c++) // for each color channel
		{
			DFT(w, inverse, &gRe[y * w * 3 + c], &gIm[y * w * 3 + c], &Gr2[y * w * 3 + c], &Gi2[y * w * 3 + c], 3, 1);
		}
		for (int x = 0; x < w; x++) // for each column
		for (int c = 0; c < 3; c++) // for each color channel
		{
			DFT(h, inverse, &Gr2[x * 3 + c], &Gi2[x * 3 + c], &GRe[x * 3 + c], &GIm[x * 3 + c], w * 3, inverse ? w : h);
		}
	}
	void FFT2D(int w, int h, bool inverse, const double *gRe, const double *gIm, double *GRe, double *GIm)
	{
		//temporary buffers
		std::vector<double> Gr2(h * w * 3);
		std::vector<double> Gi2(h * w * 3);

		for (int y = 0; y < h; y++) // for each row
		for (int c = 0; c < 3; c++) // for each color channel
		{
			FFT(w, inverse, &gRe[y * w * 3 + c], &gIm[y * w * 3 + c], &Gr2[y * w * 3 + c], &Gi2[y * w * 3 + c], 3, 1);
		}
		for (int x = 0; x < w; x++) // for each column
		for (int c = 0; c < 3; c++) // for each color channel
		{
			FFT(h, inverse, &Gr2[x * 3 + c], &Gi2[x * 3 + c], &GRe[x * 3 + c], &GIm[x * 3 + c], w * 3, inverse ? w : h);
		}
	}
	void calculateAmp(int w, int h, double *gAmp, const double *gRe, const double *gIm)
	{
		for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
		for (int c = 0; c < 3; c++)
		{
			gAmp[w * 3 * y + 3 * x + c] = sqrt(gRe[w * 3 * y + 3 * x + c] * gRe[w * 3 * y + 3 * x + c] + gIm[w * 3 * y + 3 * x + c] * gIm[w * 3 * y + 3 * x + c]);
		}
	}
private:
	void DFT(int n, bool inverse, const double *gRe, const double *gIm, double *GRe, double *GIm, int stride, double factor)
	{
		for (int n = 0; n < n; n++)
		{
			GRe[n * stride] = GIm[n * stride] = 0;
			for (int x = 0; x < n; x++)
			{
				double a = -2 * pi * n * x / float(n);
				if (inverse) a = -a;
				double ca = cos(a);
				double sa = sin(a);
				GRe[n * stride] += gRe[x * stride] * ca - gIm[x * stride] * sa;
				GIm[n * stride] += gRe[x * stride] * sa + gIm[x * stride] * ca;
			}
			//Divide through the factor, e.g. n
			GRe[n * stride] /= factor;
			GIm[n * stride] /= factor;
		}
	}
	void FFT(int n, bool inverse, const double *gRe, const double *gIm, double *GRe, double *GIm, int stride, double factor)
	{
		//Calculate h=log_2(n)
		int h = 0;
		int p = 1;
		while (p < n)
		{
			p *= 2;
			h++;
		}
		//Bit reversal
		GRe[(n - 1) * stride] = gRe[(n - 1) * stride];
		GIm[(n - 1) * stride] = gIm[(n - 1) * stride];
		int j = 0;
		for (int i = 0; i < n - 1; i++)
		{
			GRe[i * stride] = gRe[j * stride];
			GIm[i * stride] = gIm[j * stride];
			int k = n / 2;
			while (k <= j)
			{
				j -= k;
				k /= 2;
			}
			j += k;
		}
		//Calculate the FFT
		double ca = -1.0;
		double sa = 0.0;
		int l1 = 1, l2 = 1;
		for (int l = 0; l < h; l++)
		{
			l1 = l2;
			l2 *= 2;
			double u1 = 1.0;
			double u2 = 0.0;
			for (int j = 0; j < l1; j++)
			{
				for (int i = j; i < n; i += l2)
				{
					int i1 = i + l1;
					double t1 = u1 * GRe[i1 * stride] - u2 * GIm[i1 * stride];
					double t2 = u1 * GIm[i1 * stride] + u2 * GRe[i1 * stride];
					GRe[i1 * stride] = GRe[i * stride] - t1;
					GIm[i1 * stride] = GIm[i * stride] - t2;
					GRe[i * stride] += t1;
					GIm[i * stride] += t2;
				}
				double z = u1 * ca - u2 * sa;
				u2 = u1 * sa + u2 * ca;
				u1 = z;
			}
			sa = sqrt((1.0 - ca) / 2.0);
			if (!inverse) sa = -sa;
			ca = sqrt((1.0 + ca) / 2.0);
		}
		//Divide through the factor, e.g. n
		for (int i = 0; i < n; i++)
		{
			GRe[i * stride] /= factor;
			GIm[i * stride] /= factor;
		}
	}
};




int main(int argc, char* argv[]){
	




	int n = 16;
	double fRe[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
	double fIm[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
	double gRe[16];
	double gIm[16];
	CFT1D fft1d;
	fft1d.FFT(n, false, fRe, fIm, gRe, gIm);






	waitKey(0);
	return 0;
}
