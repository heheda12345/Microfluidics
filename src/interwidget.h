#ifndef INTERWIDGET_H
#define INTERWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QPixmap>
#include <QColor>

class interWidget : public QWidget
{
    Q_OBJECT
public:
    explicit interWidget(QWidget *parent = nullptr);
    void setNeighbor(int l, int r, int u, int d)
    {
        L=l; R=r; U=u; D=d;
    }
    void setD(int DL, int DR, int DU, int DD)
    {
        dl = DL; dr = DR; du = DU; dd = DD;
    }
    void setColor(double rate);
    void setColor(QColor c);
    void updatePix();

protected:
    void paintEvent(QPaintEvent* e);

private:
    int L, R, U, D;
    int dl, dr, du, dd;
    QColor backgroundColor;
    QPixmap myPix;

signals:

public slots:

};

#endif // INTERWIDGET_H
