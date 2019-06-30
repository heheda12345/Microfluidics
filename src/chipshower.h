#ifndef CHIPSHOWER_H
#define CHIPSHOWER_H

#include <QWidget>
#include <QSignalMapper>
#include <QPaintEvent>
#include <vector>

#include "chip.h"
#include "pipewidget.h"
#include "interwidget.h"

class ChipShower : public QWidget
{
    Q_OBJECT
public:
    explicit ChipShower(QWidget *parent = nullptr);
    static const int SCALE = 50;
    constexpr static const double MUL = 2.0/SCALE;
    enum ChipType {velo, con};
    //friend class PipeWidget;
    void setCurChip(const RawChip* chip);
    void setData(const std::vector<double>& vec);
    void setType(ChipType ty);
    void drawChip();
    void updateChip();
    double max() const;
    double getData(int id) const;
    const QVector<double> &getData() const;

    RawChip convert(const RawChip* chip);
    static int newWidth();


protected:
    void paintEvent(QPaintEvent *e);

signals:
    void inserted(int id);
    void deleted(int id);
    void changed(int id);

public slots:
    void insertedSLOT(int id);
    void deletedSLOT(int id);
    void changedSLOT(int id);

protected:
    QVector<double> data;
    QVector<PipeWidget*> pipes;
    QVector<interWidget*> inters;
    QVector<int> d0, d1; //两头分别扣了多少
    double maxValue;
    const RawChip* curChip;
    QSignalMapper *pipeInsertdMapper, *pipeDeletedMapper, *pipeChangedMapper;
    void interInitialize(int m);
    void pipeInitialize(int m);
    void drawPipe(const RawChip& chip);
    void drawInter(const RawChip& myChip);
    void setPipeStatus(int m);
    void setPipeInfo(int m, const RawChip &myChip);
    QRect toRect(int x, int y, int w, int h);
    static int m_newWidth;
    ChipType myType;
};

#endif // CHIPSHOWER_H
