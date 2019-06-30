#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "chip.h"
#include <vector>


class Calculator
{
public:
    Calculator(const RawChip* cur);
    void setCurChip(const RawChip* cur);
    const std::vector<double>& getV() const;
    const std::vector<double>& getC() const;
    constexpr static double eps = 1e-4;

private:
    void getPhi(int n, const std::vector<std::vector<double> > &R,
                int ni, int input[], int no,  int output[],
                const std::vector<bool> &del, std::vector<double> &phi);
    bool getPara(int input[], int output[],
                 std::vector<std::vector<double> > &R, std::vector<bool> &del);
    void getConcentration();
    void calcData();
    int mID(int i, int j);
    int m;
    int input[Chip::inputPipe];
    int output[Chip::outputPipe];
    std::vector<bool> del;
    std::vector<double> phi;
    std::vector<std::vector<double> > R;
    std::vector<double> data;
    std::vector<double> con;
    const RawChip* curChip;

};

#endif // CALCULATOR_H
