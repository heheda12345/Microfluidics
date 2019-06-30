#include "colorbar.h"

#include <QPainter>
#include <QLinearGradient>
#include "mycolor.h"

ColorBar::ColorBar(QWidget *parent) : QFrame(parent)
{

}

void ColorBar::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QPen pen = p.pen();
    pen.setWidth(1);
    pen.setColor(Qt::black);
    p.setPen(pen);
    QLinearGradient g(0, 0, this->width()-2, this->height()-2);
    g.setColorAt(0.0, MyColor::get(0.0));
    g.setColorAt(0.5, MyColor::get(0.5));
    g.setColorAt(1.0, MyColor::get(1.0));
    p.setBrush(g);
    p.drawRect(1, 1, this->width()-2, this->height()-2);
}
