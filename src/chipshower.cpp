#include "chipshower.h"

#include <cstdlib>
#include <QDebug>
#include <QPainter>

#include "mycolor.h"

int ChipShower::m_newWidth;

ChipShower::ChipShower(QWidget *parent)
    : QWidget(parent),
      pipeInsertdMapper(nullptr),
      pipeDeletedMapper(nullptr),
      pipeChangedMapper(nullptr)
{
}

void ChipShower::setCurChip(const RawChip *chip)
{
    curChip = chip;
}

void ChipShower::setData(const std::vector<double> &vec)
{
    data.resize(vec.size());
    maxValue = 0;
    for (int i=0; i<data.size(); i++)
    {
        if (vec[i] < 0)
            data[i] = -vec[i];
        else
            data[i] = vec[i];
        if (myType == con && data[i]>10)
            continue;
        maxValue = std::max(maxValue, data[i]);
    }
}

void ChipShower::setType(ChipShower::ChipType ty)
{
    myType = ty;
}

void ChipShower::drawChip()
{
    //qDebug() << "chipshower.draw " << data;
    //qDebug() << "draw Chip";
    RawChip myChip = convert(curChip);
    int n = myChip.n;
    this->resize((n+1)*Chip::pipeLength*MUL, (n+1)*Chip::pipeLength*MUL);
    //qDebug() << this->rect();
    qDebug() << parentWidget();
    qDebug("size : %d %d %d", parentWidget()->width(), this->width(), parentWidget()->width()-this->width());
    this->setGeometry((parentWidget()->width()-this->width())/2,
                      (parentWidget()->height()-this->height())/2,
                      this->width(),
                      this->height());
    //qDebug() << "chip Shower drawChip";
    update();

    drawPipe(myChip);
    drawInter(myChip);
}

void ChipShower::updateChip()
{
    //qDebug() << "updateChip()";
    //calcData();
    drawChip();
}

double ChipShower::max() const
{
    return maxValue;
}

double ChipShower::getData(int id) const
{
    if (id<0 || id>=data.size())
        return -2;
    if (curChip->width[id] == 0)
        return -2;
    if (myType == con)
    {
        if (data[id] > 10)
            return -1;
    }
    return data[id];
}

const QVector<double>& ChipShower::getData() const
{
    return data;
}

RawChip ChipShower::convert(const RawChip *chip)
{
    RawChip ret = *chip;
    for (int i=0; i<ret.width.size(); i++)
    {
        int w = ret.width[i];
        if (w != 0 && w < SCALE)
            w = SCALE;
        ret.width[i] = round(w*1.0/SCALE)*SCALE;
    }
    return ret;
}

int ChipShower::newWidth()
{
    return m_newWidth;
}

void ChipShower::paintEvent(QPaintEvent *e)
{
    //qDebug() << "chip shower repaint";
    QPainter painter(this);
    painter.fillRect(this->rect(), MyColor::backGround());
}


void ChipShower::insertedSLOT(int id)
{
    emit inserted(id);
}

void ChipShower::deletedSLOT(int id)
{
    emit deleted(id);
}

void ChipShower::changedSLOT(int id)
{
    emit changed(id);
}

void ChipShower::interInitialize(int m)
{
    if (inters.size()==m) return;
    for (auto inter: inters)
    {
        delete inter;
    }
    inters.resize(m);
    for (int i=0; i<m; i++)
    {
        inters[i] = new interWidget(this);
        inters[i]->show();
    }
}

void ChipShower::pipeInitialize(int m)
{
    if (pipes.size() == m) return;
    for (auto pipe : pipes)
    {
        delete pipe;
    }
    pipes.resize(m);
    for (int i=0; i<m; i++)
    {
        pipes[i] = new PipeWidget(this);
        pipes[i]->setWidthSaver(&m_newWidth);
        pipes[i]->show();
    }

    if (pipeInsertdMapper != nullptr)
        delete pipeInsertdMapper;
    pipeInsertdMapper = new QSignalMapper(this);
    for (int i=0; i<m; i++)
    {
        connect(pipes[i], SIGNAL(inserted()), pipeInsertdMapper, SLOT(map()));
        pipeInsertdMapper->setMapping(pipes[i], i);
    }
    connect(pipeInsertdMapper, SIGNAL(mapped(int)), this, SLOT(insertedSLOT(int)));

    if (pipeDeletedMapper != nullptr)
        delete pipeDeletedMapper;
    pipeDeletedMapper = new QSignalMapper(this);
    for (int i=0; i<m; i++)
    {
        connect(pipes[i], SIGNAL(deleted()), pipeDeletedMapper, SLOT(map()));
        pipeDeletedMapper->setMapping(pipes[i], i);
    }
    connect(pipeDeletedMapper, SIGNAL(mapped(int)), this, SLOT(deletedSLOT(int)));

    if (pipeChangedMapper != nullptr)
        delete pipeChangedMapper;
    pipeChangedMapper = new QSignalMapper(this);
    for (int i=0; i<m; i++)
    {
        connect(pipes[i], SIGNAL(changed()), pipeChangedMapper, SLOT(map()));
        pipeChangedMapper->setMapping(pipes[i], i);
    }
    connect(pipeChangedMapper, SIGNAL(mapped(int)), this, SLOT(changedSLOT(int)));
    d0.resize(m);
    d1.resize(m);
}

void ChipShower::drawPipe(const RawChip &chip)
{
    //curChip = &chip;
    int m = curChip->width.size();
    pipeInitialize(m);
    setPipeStatus(m);
    setPipeInfo(m, chip);
    for (int i=0; i<m; i++)
        pipes[i]->update();
}

void ChipShower::drawInter(const RawChip& myChip)
{
    int n = myChip.n, m = n*n;
    interInitialize(m);
    for (int i=0; i<n; i++)
        for (int j=0; j<n; j++)
        {
            int id = i*n+j;
            int L = myChip.leftWidth(i, j+1);
            int R = myChip.rightWidth(i, j+1);
            int U = myChip.upWidth(i, j+1);
            int D = myChip.downWidth(i, j+1);
            if (!L && !R && !U && !D)
                inters[id]->hide();
            else
                inters[id]->show();
            //int W = std::max(U,D);
            //int H = std::max(L,R);
            //if (!W)
            //    W = Chip::pipeWidthDefault;
            //if (!H)
            //    H = Chip::pipeWidthDefault;
            int Du = myChip.fromVector(d1, myChip.upID(i, j+1));
            int Dd = myChip.fromVector(d0, myChip.downID(i, j+1));
            int Dl = myChip.fromVector(d1, myChip.leftID(i, j+1));
            int Dr = myChip.fromVector(d0, myChip.rightID(i, j+1));
            auto myScale = [](int x)
            {
                if (x!=0 && x<SCALE)
                    x = SCALE;
                return round(x*1.0/SCALE)*SCALE;
            };
            if (Dl == -1)
                Dl = myScale(std::max(curChip->upWidth(i, j+1), curChip->downWidth(i, j+1)))/2;
            if (Dr == -1)
                Dr = myScale(std::max(curChip->upWidth(i, j+1), curChip->downWidth(i, j+1)))/2;
            if (Du == -1)
                Du = myScale(std::max(curChip->leftWidth(i, j+1), curChip->rightWidth(i, j+1)))/2;
            if (Dd == -1)
                Dd = myScale(std::max(curChip->leftWidth(i, j+1), curChip->rightWidth(i, j+1)))/2;
            if (Du == 0)
                Du = Chip::pipeWidthDefault/2;
            if (Dd == 0)
                Dd = Chip::pipeWidthDefault/2;
            if (Dl == 0)
                Dl = Chip::pipeWidthDefault/2;
            if (Dr == 0)
                Dr = Chip::pipeWidthDefault/2;

            inters[i*n+j]->setGeometry(toRect((i+1)*Chip::pipeLength-Dl,
                                          (j+1)*Chip::pipeLength-Du,
                                          Dl+Dr,Du+Dd));
            inters[id]->setNeighbor(L*MUL, R*MUL, U*MUL, D*MUL);
            inters[id]->setD(Dl*MUL, Dr*MUL, Du*MUL, Dd*MUL);

            double LData = getData(myChip.leftID(i, j+1));
            double RData = getData(myChip.rightID(i, j+1));
            double UData = getData(myChip.upID(i, j+1));
            double DData = getData(myChip.downID(i, j+1));
            auto myMean = [LData, RData, UData, DData]() -> double
            {
                double sum=0;
                int cnt=0;
                if (LData>=0)
                    sum+=LData, cnt++;
                if (RData>=0)
                    sum+=RData, cnt++;
                if (UData>=0)
                    sum+=UData, cnt++;
                if (DData>=0)
                    sum+=DData, cnt++;
                if (cnt==0)
                    return -1;
                else
                    return sum/cnt;
            };
            double mean = myMean();
            if (mean < -0.5)
                inters[id]->setColor(MyColor::unKnownColor());
            else
                inters[id]->setColor(myMean()/maxValue);
            inters[id]->update();
            //qDebug("%d %d : %d %d %d %d | %d %d %d %d", i, j, L, R, U, D, Dl, Dr, Du, Dd);
        }
}

void ChipShower::setPipeStatus(int m)
{
    for (int id=0; id<m; id++)
    {
        int pipeWidth = curChip->width[id];
        if (!pipeWidth)
        {
            pipes[id]->setStatus(PipeWidget::notExist);
            //myChip.width[id] = SCALE;
            //myChip.width[id] = round(Chip::pipeWidthDefault*1.0/SCALE)*SCALE;
        }
        else
        {
            if (myType == con && data[id] > 10)
            {
                pipes[id]->setStatus(PipeWidget::unKnown);
            }
            else
                pipes[id]->setStatus(PipeWidget::exist);
        }
    }
    QVector<int> connective = curChip->getConnectivity();
    int stdBlock = connective[m-1];
    bool ioLink = 1;
    for (int i = m - Chip::inputPipe - Chip::outputPipe; i < m; i++)
        if (connective[i] != stdBlock)
            ioLink = 0;
    if (!ioLink)
    {
        for (int i=0; i<m; i++)
            if (curChip->width[i])
                pipes[i]->setStatus(PipeWidget::notLink);
    }
    else
    {
        for (int i=0; i<m; i++)
            if (curChip->width[i] && connective[i] != stdBlock)
                pipes[i]->setStatus(PipeWidget::notLink);
    }
}

void ChipShower::setPipeInfo(int m, const RawChip& myChip)
{
    //RawChip myChip = *curChip;
    int scaledWidth = round(Chip::pipeWidthDefault*1.0/SCALE)*SCALE;
    for (int id=0; id<m; id++)
    {
        int x = myChip.X(id), y = myChip.Y(id);
        int pipeWidth = myChip.width[id];
        if (pipeWidth == 0)
            pipeWidth = scaledWidth;
        pipes[id]->setMaxWidth(curChip->getMaxWidth(id));
        pipes[id]->setCanChange(myChip.canChange(id));
        pipes[id]->setRealData(curChip->width[id], data[id]);
        if (myChip.isVertical(id))
        {
            int upWidth = std::max(myChip.UpLeftWidth(id), myChip.UpRightWidth(id))/2;
            int downWidth = std::max(myChip.DownLeftWidth(id), myChip.DownRightWidth(id))/2;
            d0[id] = upWidth;
            d1[id] = downWidth;
            if (!upWidth)
                upWidth = scaledWidth/2;
            if (!downWidth)
                downWidth = scaledWidth/2;
            pipes[id]->setGeometry(toRect((x+1)*Chip::pipeLength-pipeWidth/2,
                                        y*Chip::pipeLength+upWidth,
                                        pipeWidth,
                                        Chip::pipeLength-upWidth-downWidth)
                                  );
            pipes[id]->setVertical(true);
        }
        else
        {
            int leftWidth = std::max(myChip.UpLeftWidth(id), myChip.DownLeftWidth(id))/2;
            int rightWidth = std::max(myChip.UpRightWidth(id), myChip.DownRightWidth(id))/2;
            d0[id] = leftWidth;
            d1[id] = rightWidth;
            if (!leftWidth)
                leftWidth = scaledWidth/2;
            if (!rightWidth)
                rightWidth = scaledWidth/2;
            pipes[id]->setGeometry(toRect((x+1)*Chip::pipeLength+leftWidth,
                                        y*Chip::pipeLength-pipeWidth/2,
                                        Chip::pipeLength-leftWidth-rightWidth,
                                        pipeWidth)
                                  );
            pipes[id]->setVertical(false);
        }
        pipes[id]->setColor(data[id]/maxValue);
    }
}

QRect ChipShower::toRect(int x, int y, int w, int h)
{
    return QRect(x*MUL, y*MUL, w*MUL, h*MUL);
}

