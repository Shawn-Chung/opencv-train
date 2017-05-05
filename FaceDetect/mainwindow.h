#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QVector>
#include <QString>
#include <QMainWindow>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QDebug>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    void findImgFiles(QString path);
    void detectFace(Mat &input, QVector<QRect> &faces);
private slots:
    void on_m_btnNext_clicked();

    void on_m_btnExcute_clicked();

    void on_m_btnExit_clicked();

private:
    Ui::MainWindow *ui;

    QVector<QString> m_vecFileName;
    int m_iImgIndex;
    Mat m_matInput;
    CascadeClassifier m_cFaceCade;
    CascadeClassifier m_cEyeCade;
};

#endif // MAINWINDOW_H
