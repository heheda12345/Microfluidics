#include "mycolor.h"

MyColor::MyColor()
{

}

QColor MyColor::get(double rate)
{
   if (rate<0.5)
       return QColor(255*(0.5-rate)*2, 255, 0);
   else
       return QColor(0, 255, 255*(rate-0.5)*2);
}

QColor MyColor::backGround()
{
    return Qt::white;
}

QColor MyColor::unKnownColor()
{
    return QColor("#FFF0F5");
}

//QColor("#F0F0F0"));
