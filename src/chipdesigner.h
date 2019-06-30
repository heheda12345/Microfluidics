#ifndef CHIPDESIGN_H
#define CHIPDESIGN_H

#include <QVector>
#include "chip.h"

class ChipDesigner
{
public:
    ChipDesigner(const RawChip& c, int randRound_ = 10, int simuRound_ = 1000,
                 double T_initial_ = 0.0001, double dropSpeed = 0.99, unsigned int seed_ = 0);
    RawChip design(const QVector<double>& tar);
    double loss(const std::vector<double>& result);
private:
    double rand01();
    void randomGenerate();
    void simulatedAnnealing();
    void nextStatus(RawChip& chip);
    double T_initial, T_final, speed;
    int randRound, simuRound;
    unsigned int seed;
    RawChip curChip;
    QVector<double> target;
    int m;
    std::vector<double> velo;
    std::vector<double> con;
};

#endif // CHIPDESIGN_H
