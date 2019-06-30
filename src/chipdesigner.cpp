#include "chipdesigner.h"

#include <cmath>
#include <QDebug>
#include "calculator.h"
using namespace std;

ChipDesigner::ChipDesigner(const RawChip &c, int randRound_, int simuRound_,
                           double T_initial_, double dropSpeed, unsigned int seed_):
    T_initial(T_initial_), speed(dropSpeed),
    randRound(randRound_), simuRound(simuRound_), seed(seed_),
    curChip(c), m(c.width.size()-Chip::inputPipe-Chip::outputPipe)
{
}

RawChip ChipDesigner::design(const QVector<double> &tar)
{
    srand(this->seed);
    target = tar;
    randomGenerate();
    simulatedAnnealing();
    return curChip;
}

double ChipDesigner::loss(const std::vector<double> &result)
{
    double sum = 0;
    int m = result.size() - Chip::outputPipe;
    for (int i=0; i<Chip::outputPipe; i++)
        sum += (target[i]-result[i+m])*(target[i]-result[i+m]);
    //qDebug("loss %lf %lf %lf : %lf", result[m], result[m+1], result[m+2], sum);
    return sum;
}

double ChipDesigner::rand01()
{
    return (((rand()&32767)<<15)|(rand()&32767))*1.0/((32767<<15)|32767);
}

void ChipDesigner::randomGenerate()
{
    double minLoss = 1e100;
    Calculator calc(&curChip);
    if (curChip.ioLink())
        minLoss = loss(calc.getC());
    RawChip newChip;
    for (int i=0; i<randRound || minLoss>1e50; i++)
    {
        newChip = Chip::generate(Chip::random, curChip.n, curChip.input, curChip.output);
        if (!newChip.ioLink())
            continue;
        calc.setCurChip(&newChip);
        double curLoss = loss(calc.getC());
        if (curLoss < minLoss)
        {
            minLoss = curLoss;
            curChip = newChip;
        }
    }
}

void ChipDesigner::simulatedAnnealing()
{
    double T_cur = this->T_initial;
    Calculator calc(&curChip);
    velo = calc.getV();
    con = calc.getC();
    double ans = loss(con);
    RawChip newChip = curChip, lastChip = curChip;
    for (int i=0; i<simuRound; i++)
    {
        lastChip = newChip;
        nextStatus(newChip);
        //qDebug("loss query");
        //qDebug() << newChip.width;
        Calculator calc(&newChip);
        //calc.setCurChip(&newChip);
        velo = calc.getV();
        con = calc.getC();
        double cost = loss(con);

        qDebug("round %d : T: %.10lf exp %.10lf %.10lf", i, T_cur, ans-cost, exp((ans-cost)/T_cur));
        if (cost<=ans || exp((ans-cost)/T_cur)>rand01())
        {
            ans = cost;
            curChip = newChip;
        }
        else
        {
            newChip = lastChip;
        }
        T_cur *= speed;
        qDebug() << "round: " << i << " cost: " << cost << " best: " << ans;
    }
}

void ChipDesigner::nextStatus(RawChip &chip)
{
    //qDebug() << "nextStatus";
    while (1)
    {
        int id = rand()%m;
        if (chip.width[id])
        {
            //if (fabs(velo[id]) < Calculator::eps)
            //    continue;
            int w = chip.width[id];
            chip.width[id] = 0;
            if (chip.ioLink())
                break;
            chip.width[id] = w;
        }
        else
        {
            if (chip.getMaxWidth(id) >= Chip::pipeWidthDefault)
            {
                chip.width[id] = Chip::pipeWidthDefault;
                break;
            }
        }
    }
    //qDebug() << "end next status";
}
