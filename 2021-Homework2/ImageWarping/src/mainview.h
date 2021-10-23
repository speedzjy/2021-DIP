#ifndef MAINVIEW_H
#define MAINVIEW_H
#include <QGraphicsView>
#include "mainscene.h"

class MainView : public QGraphicsView
{
public:
    MainView(MainScene* s);

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private:
    MainScene* scene;
};

#endif

