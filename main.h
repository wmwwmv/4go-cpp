#ifndef MAIN_H
#define MAIN_H

//
#include <QWidget>
//


//
class MyMainWindow: public QWidget
{

public:
    MyMainWindow(QWidget *parent = 0);
    ~MyMainWindow();

private:
    void paintEvent(QPaintEvent *);
    QPainter *paint;
};




#endif // MAIN_H
