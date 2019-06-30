#include "interwidget.h"

#include <QPainter>
#include <QPolygon>
#include <QDebug>
#include <QPalette>
#include <QBitmap>

#include "mycolor.h"

interWidget::interWidget(QWidget *parent) :
    QWidget(parent),
    myPix(this->width(), this->height())
{
    //myPix.fill(Qt::transparent);
    /*
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, Qt::red);
    setPalette(palette);
    */
    /*setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);*/
    //setAttribute(Qt::WA_OpaquePaintEvent);
    //setStyleSheet("background-color: rgba(0,0,0,0)");
    //setStyleSheet ("background-color: rgb(0,0,0);color: rgb(255,255,255);");
}

void interWidget::setColor(double rate)
{
    backgroundColor = MyColor::get(rate);
}

void interWidget::setColor(QColor c)
{
    backgroundColor = c;
}

void interWidget::updatePix()
{

}

void interWidget::paintEvent(QPaintEvent *e)
{
    QBitmap bm(this->width(), this->height());
    QPainter bmp(&bm);
    bm.fill(Qt::white);
    bmp.setBrush(Qt::black);
    bmp.setPen(Qt::black);
    int cnt = int(0) + (L>0) + (R>0) + (U>0) + (D>0);
    int W = this->width();
    int H = this->height();
    int dL, dR, dU, dD;
    QPoint p[7];
    QPolygon poly;
    using std::max;
    auto insertPoly = [&](QPoint* p, int d) {
        for (int i=0; i<d; i++)
            poly << p[i];
    };
    switch (cnt) {
        case 0:
            return;
            break;
        case 1:
            return;
            break;
        case 2:
            if (L && R)
            {
                dL = max(R-L, 0)/2;
                dR = max(L-R, 0)/2;
                poly << QPoint(0, dL) << QPoint(0, H-dL-1)
                     << QPoint(W-1, H-dR-1) << QPoint(W-1, dR);
            }
            else if (U && D)
            {
                dU = max(D-U, 0)/2;
                dD = max(U-D, 0)/2;
                poly << QPoint(dU, 0) << QPoint(W-dU-1, 0)
                     << QPoint(W-dD-1, H-1) << QPoint(dD, H-1);
            }
            else
            {
                p[0] = QPoint(0,0);
                p[1] = QPoint(W-1, 0);
                p[2] = QPoint(W-1, H-1);
                p[3] = QPoint(0, H-1);
                p[4] = p[0];
                p[5] = p[1];
                p[6] = p[2];
                if (!D && !L)
                    insertPoly(p+0, 3);
                else if (!L && !U)
                    insertPoly(p+1, 3);
                else if (!U && !R)
                    insertPoly(p+2, 3);
                else if (!R && !D)
                    insertPoly(p+3, 3);
            }
            break;
        case 3:
            p[0] = QPoint(0,0);
            p[1] = QPoint(W-1, 0);
            p[2] = QPoint(W-1, H-1);
            p[3] = QPoint(0, H-1);
            dL = max(R-L, 0)/2;
            dR = max(L-R, 0)/2;
            dU = max(D-U, 0)/2;
            dD = max(U-D, 0)/2;
            if (!U)
            {
                p[0].setY(dL);
                p[1].setY(dR);
            }
            else if (!D)
            {
                p[2].setY(H-dR-1);
                p[3].setY(H-dL-1);
            }
            else if (!L)
            {
                p[0].setX(dU);
                p[3].setX(dD);
            }
            else if (!R)
            {
                p[1].setX(W-dU-1);
                p[2].setX(W-dD-1);
            }
            insertPoly(p, 4);
            //painter.drawPolygon(p, 4);
            break;
        case 4:
            if (L==R || U==D)
            {
                poly << QPoint(0,0) << QPoint(0, H-1)
                     << QPoint(W-1, H-1) << QPoint(W-1, 0);
                break;
            }
            int hh = std::abs(L-R)/2;
            int ww = std::abs(U-D)/2;
            if (U<D && L<R)
            {
                p[1] = QPoint(0, hh);
                p[2] = QPoint(ww, 0);
                p[3] = QPoint(W-1, 0);
                p[4] = QPoint(W-1, H-1);
                p[5] = QPoint(0, H-1);
            }
            if (U<D && R<L)
            {
                p[1] = QPoint(W-ww-1, 0);
                p[2] = QPoint(W-1, hh);
                p[3] = QPoint(W-1, H-1);
                p[4] = QPoint(0, H-1);
                p[5] = QPoint(0,0);
            }
            if (D<U && L<R)
            {
                p[1] = QPoint(0, H-hh-1);
                p[2] = QPoint(ww, H-1);
                p[3] = QPoint(W-1, H-1);
                p[4] = QPoint(W-1, 0);
                p[5] = QPoint(0,0);
            }
            if (D<U && R<L)
            {
                p[1] = QPoint(W-1, H-hh-1);
                p[2] = QPoint(W-ww-1, H-1);
                p[3] = QPoint(0, H-1);
                p[4] = QPoint(0,0);
                p[5] = QPoint(W-1, 0);
            }
            insertPoly(p+1, 5);
            //painter.drawPolygon(p+1, 5);
    }
    bmp.drawPolygon(poly);
    bmp.setPen(Qt::NoPen);
    bmp.setBrush(Qt::white);
    auto inPoly = [&](int x, int y)
    {
        QPoint p(x,y);
        for (QPoint pp : poly)
        {
            if (pp == p)
                return 1;
        }
        return 0;
    };
    if (inPoly(0, 0) && dl-U/2>0 && du-L/2>0 && U && L)
    {
        //qDebug("trans %d %d %d %d\n", dl, du, L, U);
        bmp.drawRect(0,0,dl-U/2,du-L/2);
    }
    if (inPoly(width()-1, 0) && U && R)
    {
        int dx = dr-U/2;
        int dy = du-R/2;
        bmp.drawRect(QRect(W-dx,0,dx,dy));
    }
    if (inPoly(0, height()-1) && D && L)
    {
        int dx = dl-D/2;
        int dy = dd-L/2;
        bmp.drawRect(QRect(0,H-dy,dx,dy));
    }
    if (inPoly(width()-1, height()-1) && D && R)
    {
        int dx = dr-D/2;
        int dy = dd-R/2;
        bmp.drawRect(QRect(W-dx,H-dy,dx,dy));
    }
    this->setMask(bm);
    QPainter pp(this);
    pp.fillRect(this->rect(), backgroundColor);
}
