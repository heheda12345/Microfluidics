#ifndef MYCOLOR_H
#define MYCOLOR_H

#include <QColor>

class MyColor
{
public:
    MyColor();
    static QColor get(double rate);
    static QColor backGround();
    static QColor unKnownColor();
};

#endif // MYCOLOR_H
