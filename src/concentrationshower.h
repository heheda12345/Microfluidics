#ifndef CONCENTRATIONSHOWER_H
#define CONCENTRATIONSHOWER_H

#include "chipshower.h"

class ConcentrationShower : public ChipShower
{
    Q_OBJECT
public:
    ConcentrationShower(QWidget *parent = nullptr);
    void calcData();
};

#endif // CONCENTRATIONSHOWER_H
