#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QColor>
#include <QImage>
#include <QString>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QImage* loadImage();

public slots:
    void loadSourceImage();
    void loadBackgroundImage();
    void clearDraw();
    void force();
    void possion();

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::MainWindow *ui;
    QImage *source, *background;
    QImage *source_tmp, *background_tmp;
    QImage *forceCloneImg, *possionCloneImg;
};

#endif // MAINWINDOW_H
