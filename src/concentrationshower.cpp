#include "concentrationshower.h"
#include <QDebug>

ConcentrationShower::ConcentrationShower(QWidget *parent): ChipShower(parent)
{

}

void ConcentrationShower::calcData()
{
    //qDebug() << "calc data " << curChip;
     maxValue=1;
     data.resize(curChip->width.size());
     for (int i=0; i<data.size(); i++)
         data[i] = rand()%20/100.0+0.25;
}
