#include "chip.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <cstdlib>

Chip::Chip(QObject *parent) : QObject(parent)
{

}

RawChip Chip::generate(Chip::generateType type, int n, QVector<int> inputPipes, QVector<int> outputPipes)
{
    RawChip ret;
    int P = type;
    int m1 = 2*n*(n-1)+inputPipe+outputPipe;
    int m0 = 2*n*(n-1);
    ret.n = n;
    ret.width.resize(m1);
    for (int i=0; i<m0; i++)
    {
        int x = rand()%100;
        if (x<P)
            ret.width[i] = pipeWidthDefault;
        else
            ret.width[i] = 0;
    }
    for (int i=m0; i<m1; i++)
        ret.width[i] = pipeWidthDefault;
    ret.input = inputPipes;
    std::sort(ret.input.begin(), ret.input.end());
    ret.output = outputPipes;
    std::sort(ret.output.begin(), ret.output.end());
    return ret;
}

RawChip Chip::design(QVector<double> c)
{
    qDebug() << "designChip ";
    return Chip().value();
}

RawChip Chip::load(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "can not open " << path;
        return RawChip();
    }
    RawChip chip;
    QDataStream is(&file);
    is >> chip;
    file.close();
    return chip;
}

void Chip::print(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "can not open " << path;
        return;
    }
    QDataStream os(&file);
    os << value();
    file.close();
}

RawChip Chip::addPipe(int id)
{
    if (c.getType(id) != 0 && c.getType(id) != 1)
        return c;
    RawChip ret = c;
    ret.width[id] = Chip::pipeWidthDefault;
    return ret;
}

RawChip Chip::deletePipe(int id)
{
    if (c.getType(id) != 0 && c.getType(id) != 1)
        return c;
    RawChip ret = c;
    ret.width[id] = 0;
    return ret;
}

RawChip Chip::changePipe(int id, int width)
{
    if (c.getType(id) != 0 && c.getType(id) != 1)
        return c;
    RawChip ret = c;
    ret.width[id] = width;
    return ret;
}

bool RawChip::validID(int id) const
{
    return id>=0 && id<n*(n-1)*2+input.size()+output.size();
}

int RawChip::getType(int id) const
{
    if (!validID(id))
    {
        return -1;
    }
    if (id < n*(n-1))
        return 0;
    id -= n*(n-1);
    if (id < n*(n-1))
        return 1;
    id -= n*(n-1);
    if (id < input.size())
        return 2;
    else
        return 3;
}

int RawChip::X(int id) const
{
    int m = n*(n-1);
    switch (getType(id))
    {
        case 0:
            return id/(n-1);
            break;
        case 1:
            id -= m;
            return id/n;
            break;
        case 2:
            id -= 2*m;
            return input[id];
            break;
        case 3:
            id -= 2*m+input.size();
            return output[id];
            break;
        default:
            return -1;
    }
}

int RawChip::Y(int id) const
{
    int m = n*(n-1);
    switch (getType(id))
    {
        case 0:
            return id%(n-1)+1;
            break;
        case 1:
            id -= m;
            return id%n+1;
            break;
        case 2:
            return 0;
        case 3:
            return n;
    }
}

int RawChip::getID(bool isVer, int x, int y) const
{
    int m = n*(n-1);
    if (isVer==1)
    {
        if (x<0 || x>=n || y<0 || y>=n+1)
            return -1;
        if (y==0)
        {
            for (int i=0; i<input.size(); i++)
                if (input[i]==x)
                    return 2*m+i;
            return -1;
        }
        if (y==n)
        {
            for (int i=0; i<output.size(); i++)
                if (output[i]==x)
                    return 2*m+input.size()+i;
            return -1;
        }
        return x*(n-1)+y-1;
    }
    else
    {
        if (x<0 || x>=(n-1) || y<1 || y>=n+1)
            return -1;
        return m+x*n+y-1;
    }
}

bool RawChip::isVertical(int id) const
{
    return getType(id)!=-1 && getType(id)!=1;
}

double RawChip::UpLeftWidth(int id) const
{
    int x = X(id), y=Y(id);
    bool isVer = 0;
    if (isVertical(id))
    {
        isVer = 0;
        x--;
        if (!getWidth(getID(isVer, x, y)) && getWidth(getID(isVer^1, x, y-1)))
            x--;
    }
    else
    {
        isVer = 1;
        y--;
        if (!getWidth(getID(isVer, x, y)) && getWidth(getID(isVer^1, x-1, y)))
            y--;
    }
    if (x<0 || x>=n || y<0 || y>=n+1)
        return 0;
    else
    {
        int id = getID(isVer, x, y);
        if (id==-1) return 0;
        return width[id];
    }
}

double RawChip::UpRightWidth(int id) const
{
    int x = X(id), y=Y(id);
    bool isVer = 0;
    if (isVertical(id))
    {
        isVer = 0;
        if (!getWidth(getID(isVer, x, y)) && getWidth(getID(isVer^1, x+1, y-1)))
            x++;
    }
    else
    {
        x++; y--;
        isVer = 1;
        if (!getWidth(getID(isVer, x, y)) && getWidth(getID(isVer^1, x, y)))
            y--;
    }
    if (x<0 || x>=n || y<0 || y>=n+1)
        return 0;
    else
    {
        int id = getID(isVer, x, y);
        if (id==-1) return 0;
        return width[id];
    }

}

double RawChip::DownLeftWidth(int id) const
{
    int x = X(id), y=Y(id);
    bool isVer = 0;
    if (isVertical(id))
    {
        isVer = 0;
        x--; y++;
        if (!getWidth(getID(isVer, x, y)) && getWidth(getID(isVer^1, x, y)))
            x--;
    }
    else
    {
        isVer = 1;
        if (!getWidth(getID(isVer, x, y)) && getWidth(getID(isVer^1, x-1, y+1)))
            y++;
    }
    if (x<0 || x>=n || y<0 || y>=n+1)
        return 0;
    else
    {
        int id = getID(isVer, x, y);
        if (id==-1) return 0;
        return width[id];
    }
}

double RawChip::DownRightWidth(int id) const
{
    int x = X(id), y=Y(id);
    bool isVer = 0;
    if (isVertical(id))
    {
        isVer = 0;
        y++;
        if (!getWidth(getID(isVer, x, y)) && getWidth(getID(isVer^1, x+1, y)))
            x++;
    }
    else
    {
        isVer = 1;
        x++;
        if (!getWidth(getID(isVer, x ,y)) && getWidth(getID(isVer^1, x, y+1)))
            y++;
    }
    if (x<0 || x>=n || y<0 || y>=n+1)
        return 0;
    else
    {
        int id = getID(isVer, x, y);
        if (id==-1) return 0;
        return width[id];
    }
}

double RawChip::upWidth(int x, int y) const
{
    int id = upID(x, y);
    return id==-1 ? 0 : width[id];
}

double RawChip::downWidth(int x, int y) const
{
    int id = downID(x, y);
    return id==-1 ? 0 : width[id];
}

double RawChip::leftWidth(int x, int y) const
{
    int id = leftID(x, y);
    return id==-1 ? 0 : width[id];
}

double RawChip::rightWidth(int x, int y) const
{
    int id = rightID(x, y);
    return id==-1 ? 0 : width[id];
}

int RawChip::fromVector(const QVector<int> vec, int id) const
{
    if (validID(id))
        return vec[id];
    else
        return -1;
}

int RawChip::leftID(int x, int y) const
{
    return getID(0, x-1, y);
}

int RawChip::rightID(int x, int y) const
{
    return getID(0, x, y);
}

int RawChip::upID(int x, int y) const
{
    return getID(1, x, y-1);
}

int RawChip::downID(int x, int y) const
{
    return getID(1, x, y);
}

int RawChip::getWidth(int id) const
{
    if (!validID(id))
        return 0;
    return width[id];
}

int RawChip::getMaxWidth(int id) const
{
    if (!validID(id))
        return 0;
    int len = Chip::pipeLength;
    if (isVertical(id))
    {
        int x = X(id), y = Y(id);
        int L = x-1, R = x+1;
        while (L>=0 && getWidth(getID(1,L,y))==0)
            L--;
        L = L<0 ? -len : L*len+getWidth(getID(1,L,y))/2+Chip::pipeWidthDefault;
        while (R<n && getWidth(getID(1,R,y))==0)
            R++;
        R = R>=n ? n*len : R*len-getWidth(getID(1,R,y))/2-Chip::pipeWidthDefault;
        int mid = x*len;
        if (L>mid || R<mid)
            return 0;
        else
            return std::min((R-mid, mid-L)*2, 2*(Chip::pipeLength-Chip::pipeWidthDefault));
    }
    else
    {
        int x = X(id), y = Y(id);
        int U = y-1, D = y+1;
        while (U>=0 && getWidth(getID(0,x,U))==0)
            U--;
        U = U<0 ? -len : U*len+getWidth(getID(0,x,U))/2+Chip::pipeWidthDefault;
        while (D<=n && getWidth(getID(0,x,D))==0)
            D++;
        D = D>n ? (n+1)*len : D*len-getWidth(getID(0,x,D))/2-Chip::pipeWidthDefault;
        int mid = y*len;
        if (U>mid || D<mid)
            return 0;
        else
            return std::min(std::min(D-mid, mid-U)*2, 2*(Chip::pipeLength-Chip::pipeWidthDefault));
    }
}

bool RawChip::canChange(int id) const
{
    return this->getType(id) == 0 || this->getType(id) == 1;
}

int find(QVector<int>& fa, int x) { return fa[x] == x ? x : fa[x] = find(fa, fa[x]); }

QVector<int> RawChip::getConnectivity() const
{
    QVector<int> fa;
    int m = width.size();
    fa.resize(m);
    for (int i=0; i<m; i++)
        fa[i] = i;
    auto link = [&fa](int x, int y)
    {
        int f1 = find(fa, x), f2 = find(fa, y);
        fa[f1] = f2;
    };
    for (int i=0; i<n; i++)
        for (int j=1; j<=n; j++)
        {
            QVector<int> pipe;
            auto insert = [&] (int id)
            {
                if (!validID(id))
                    return;
                if (width[id])
                    pipe.append(id);
            };
            insert(leftID(i, j));
            insert(rightID(i, j));
            insert(upID(i, j));
            insert(downID(i, j));
            for (int x = 0; x < pipe.size(); x++)
                for (int y = 0; y < pipe.size(); y++)
                    link(pipe[x], pipe[y]);
        }
    for (int i=0; i<m; i++)
        fa[i] = find(fa, fa[i]);
    return fa;
}

bool RawChip::ioLink() const
{
    QVector<int> fa = getConnectivity();
    int stdBlock = fa[width.size()-1];
    for (int i = width.size() - Chip::inputPipe - Chip::outputPipe; i < width.size(); i++)
        if (fa[i] != stdBlock)
            return 0;
    return 1;
}

QDataStream& operator <<(QDataStream &os, const RawChip &chip)
{
    os << chip.n << chip.input << chip.output << chip.width;
    return os;
}

QDataStream &operator >>(QDataStream &is, RawChip &chip)
{
    is >> chip.n >> chip.input >> chip.output >> chip.width;
    return is;
}
