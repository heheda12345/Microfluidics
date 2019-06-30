#ifndef VELOCITYSHOWER_H
#define VELOCITYSHOWER_H

#include <Vector>
#include "chipshower.h"

class VelocityShower : public ChipShower
{
    Q_OBJECT
public:
    VelocityShower(QWidget *parent = nullptr);
    void calcData();
private:
    void getPhi(int n, const std::vector<std::vector<double> > &R,
                int ni, int input[], int no,  int output[],
                const std::vector<bool> &del, std::vector<double> &phi);
    bool getPara(int input[], int output[],
                 std::vector<std::vector<double> > &R, std::vector<bool> &del);
    void getConcentration();
    int mID(int i, int j);
    int m;
    int input[Chip::inputPipe];
    int output[Chip::outputPipe];
    vector<bool> del;
    vector<double> phi;
    vector<vector<double> > R;

    vector<double> con;
};

#endif // VELOCITYSHOWER_H
