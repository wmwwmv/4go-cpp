#ifndef MAIN_H
#define MAIN_H

#include "object.h"

//
#include <QWidget>
//

//
class MyMainWindow: public QWidget
{

private:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);

//    QImage * image;
    QPainter * paint;

    board b;

public:
    MyMainWindow(QWidget * parent = 0);
    ~MyMainWindow();

    void redraw();
    void click_pos(int_type);

};

//


#endif // MAIN_H
