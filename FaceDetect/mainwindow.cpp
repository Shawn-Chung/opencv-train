#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "easylogging++.h"


using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_iImgIndex(0)
{
    ui->setupUi(this);

    findImgFiles("D:/Document/opencv-train/FaceDetect/image/");

    if(m_cFaceCade.load("../haarcascades/haarcascade_frontalface_alt.xml"))
    {
        LOG(ERROR)<<"load the model failed!";
        return;
    }
    if(m_cEyeCade.load("../haarcascades/haarcascade_eye.xml"))
    {
        LOG(ERROR)<<"load the model failed!";
        return;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::findImgFiles(QString path)
{
    QDir dir(path);
    if(!dir.exists())
    {
        return;
    }
    dir.setFilter(QDir::Files | QDir::NoSymLinks);

    QFileInfoList list = dir.entryInfoList();
    if(list.size() <= 0)
    {
        return;
    }

    for(int i=0; i<list.size(); i++)
    {
        QFileInfo info = list.at(i);
        QString suffix = info.suffix();
        if(QString::compare(suffix, QString("jpg"), Qt::CaseInsensitive) == 0)
        {
            m_vecFileName.append(info.absoluteFilePath());
        }
    }
}

void MainWindow::detectFace(Mat &input, QVector<QRect> &faces)
{


    double scale = 1;
    vector<Rect> face;
    Mat gray;
    cvtColor(input, gray, CV_BGR2GRAY);
    cv::resize(gray, gray, Size(), 1./scale, 1./scale, INTER_AREA);

    equalizeHist(gray, gray);
    imshow("input", gray);
    double time = (double)getTickCount();
    m_cFaceCade.detectMultiScale(gray, face, 1.1, 3, 0|CV_HAAR_SCALE_IMAGE, Size(30,30));
    time = (double)getTickCount() - time;
    LOG(DEBUG)<<"detect time is: " << time/((double)getTickFrequency()*1000.);

    LOG(DEBUG)<<"detect %d faces."<<face.size();

    vector<Rect>::const_iterator iter;
    for(iter = face.begin(); iter != face.end(); ++iter)
    {
        QRect rect;
        rect.setX(iter->x * scale);
        rect.setY(iter->y * scale);
        rect.setWidth(iter->width * scale);
        rect.setHeight(iter->height * scale);

        faces.append(rect);
    }
}

void MainWindow::on_m_btnNext_clicked()
{
    if(m_iImgIndex >= m_vecFileName.size())
    {

    }
    QString file = m_vecFileName.at(m_iImgIndex);
    LOG(DEBUG)<<file;
    m_iImgIndex++;
    if(m_iImgIndex >= m_vecFileName.size())
    {
        m_iImgIndex = 0;
    }
    //load the input image
    m_matInput = imread(file.toStdString());
    if(m_matInput.empty())
    {
        LOG(ERROR)<<"load the image file failed!";
    }
    //detect faces
    QVector<QRect> faces;
    detectFace(m_matInput, faces);

    //display the image on the widget
    QImage img;
    img.load(file);
    QPixmap pix = QPixmap::fromImage(img);
    QPainter painter(&pix);
    for(int i=0; i<faces.count(); i++)
    {
        painter.drawRect(faces.at(i));
        LOG(DEBUG)<<"the face"<<i<<faces.at(i).x()<<faces.at(i).y()<<faces.at(i).width()<<faces.at(i).height();
    }

    ui->m_lbShow->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::on_m_btnExcute_clicked()
{



}

void MainWindow::on_m_btnExit_clicked()
{
    this->close();
}
