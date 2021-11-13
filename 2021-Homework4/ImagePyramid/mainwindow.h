#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <string>

#include <QMainWindow>
#include <QFileDialog>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QColor>
#include <QImage>
#include <QString>

#include <opencv2/opencv.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QImage* loadImage();
    QImage* cv_loadImage();

public slots:
    void loadSourceImage();
    void downSamplePool();
    void downSampleGauss();
    void upSampleLaplace();
    void upSampleNN();
    void upSampleLinear();
    void genDiff();

protected:
    void paintEvent(QPaintEvent *event);

private:
    Ui::MainWindow *ui;
    QImage *source, *source_tmp;
    QImage *downSampleImage, *upSampleImage, *diffImage;
    cv::Mat cv_source, cv_tmp;
    cv::Mat cv_dtmp, cv_utmp;
    cv::Mat cv_diffImage;

    cv::Mat maxPooling(const cv::Mat img);

    QImage cvMat2QImage(const cv::Mat &mat, bool clone = true, bool rb_swap = true);
    cv::Mat QImage2cvMat(QImage &image, bool clone = false, bool rb_swap = true);

    double getPSNR(const cv::Mat &I1, const cv::Mat &I2);
    cv::Scalar getMSSIM(const cv::Mat &i1, const cv::Mat &i2);
};

#endif // MAINWINDOW_H
