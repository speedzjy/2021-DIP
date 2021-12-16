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
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QImage* loadImage();
    QImage* cv_loadSrcImage();
    QImage* cv_loadBkdImage();

public slots:
    void loadSourceImage();
    void loadBackgroundImage();
    void extractFore();
    void extractBack();
    void morphing();
    void saveImage();

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    Ui::MainWindow *ui;

    int sx = 0, sy = 0;
    int ex = 0, ey = 0;

    cv::Point sp, ep;

    QImage *source, *source_tmp;
    QImage *background, *background_tmp;
    cv::Mat cv_source, cv_tmp;
    cv::Mat cv_background, cv_background_tmp;

    QImage *foreImage, *backImage;
    cv::Mat cv_foreImage, cv_backImage;

    QImage *fusionImage;
    cv::Mat cv_fusionImage;

    cv::Mat mask;

    QImage cvMat2QImage(const cv::Mat &mat, bool clone = true, bool rb_swap = true);
    cv::Mat QImage2cvMat(QImage &image, bool clone = false, bool rb_swap = true);
};
#endif // MAINWINDOW_H
