#ifndef MAINSCENE_H
#define MAINSCENE_H
#include <QGraphicsScene>
#include <QVector>
#include <QMouseEvent>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QFileDialog>
#include <QGraphicsItemGroup>
#include <QMessageBox>
#include <QPen>
#include "idw.h"
#include "rbf.h"

class MainScene : public QGraphicsScene
{
public:
    MainScene();
    void press(QMouseEvent *event);
    void moving(QMouseEvent *event);
    void release(QMouseEvent *event);

    void open();
    void save();
    void invert();
    void grayscale();
    void restore();
    void mirrorH();
    void mirrorV();
    void deleteALL();
    void idwFunc();
    void rbfFunc();

    QPoint start, end;
    QVector<QPoint> starts, ends;
    QVector<QGraphicsItem *> items;
    QGraphicsPixmapItem *pixmapItem;
    QImage image;
    QGraphicsItemGroup *group;
    QString path;
    IDW idw;
    RBF rbf;
    bool editable;
};

#endif
