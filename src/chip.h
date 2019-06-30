#ifndef CHIP_H
#define CHIP_H

#include <QObject>

#include <QVector>
#include <QDataStream>

struct RawChip
{
    int n;
    QVector<double> width;
    QVector<int> input;
    QVector<int> output;
    bool validID(int id) const;
    int getType(int id) const; //0 竖着的 1 横着的 2 input 3 output
    int X(int id) const;
    int Y(int id) const;
    int getID(bool isVer, int x, int y) const;
    bool isVertical(int id) const;
    double UpLeftWidth(int id) const;
    double UpRightWidth(int id) const;
    double DownLeftWidth(int id) const;
    double DownRightWidth(int id) const;
    double upWidth(int x, int y) const;
    double downWidth(int x, int y) const;
    double leftWidth(int x, int y) const;
    double rightWidth(int x, int y) const;
    int fromVector(const QVector<int> vec, int id) const;
    int leftID(int x, int y) const;
    int rightID(int x, int y) const;
    int upID(int x, int y) const;
    int downID(int x, int y) const;
    int getWidth(int id) const;
    int getMaxWidth(int id) const;
    bool canChange(int id) const;
    QVector<int> getConnectivity() const;
    bool ioLink() const;
};
QDataStream& operator << (QDataStream &os, const RawChip &chip);
QDataStream& operator >> (QDataStream &is, RawChip &chip);
class Chip : public QObject
{
    Q_OBJECT
public:
    explicit Chip(QObject *parent = nullptr);
    enum generateType
    {
        empty=0, random=90, full=100
    };
    static const int inputPipe = 2;
    static const int outputPipe = 3;
    static const int pipeLength = 1800;
    static const int pipeWidthDefault = 200;
    static const int pipeMinimalWidth = 20;
    static RawChip generate(generateType type, int n, QVector<int> inputPipes, QVector<int> outputPipes);
    static RawChip design(QVector<double> c);
    static RawChip load(QString path);
    void print(QString path);
    RawChip addPipe(int id);
    RawChip deletePipe(int id);
    RawChip changePipe(int id, int width);
    RawChip value() const { return c; }
    const RawChip* valueDir() const { return &c;}
    void setValue(const RawChip& cc) { c = cc; }

private:
    RawChip c;

signals:

public slots:

};

#endif // CHIP_H
