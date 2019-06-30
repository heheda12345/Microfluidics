#include "pipewidget.h"

#include <QPainter>
#include <QApplication>
#include <QMessageBox>
#include <cmath>
#include <QDebug>
#include "pipeinsertdialog.h"
#include "chip.h"
#include "mycolor.h"


PipeWidget::PipeWidget(QWidget *parent) : QWidget(parent)
{
    //timer = new QTimer(this);
    //connect(timer, SIGNAL(timeout()), this, SLOT(slotClicked()));
}

void PipeWidget::setWidthSaver(int *saver)
{
    newWidthSaver = saver;
}

void PipeWidget::setColor(double rate)
{
    //qDebug() << "set color " << rate;
    backgroundColor = MyColor::get(rate);
}

void PipeWidget::setMaxWidth(int w)
{
    maxWidth = w;
}

void PipeWidget::setRealData(int width, double value)
{
    realValue = value;
    realWidth = width;
}

void PipeWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    switch (status)
    {
        case (exist):
            painter.fillRect(this->rect(), backgroundColor);
            painter.setPen(Qt::black);
            /*
            if (isVertical)
            {
                painter.drawLine(0, 0, 0, this->height()-1);
                painter.drawLine(this->width()-1, 0, this->width()-1, this->height()-1);
            }
            else
            {
                painter.drawLine(0, 0, this->width()-1, 0);
                painter.drawLine(0, this->height()-1, this->width()-1, this->height()-1);
            }
            */
            break;
        case (notExist):
            painter.fillRect(this->rect(), QColor("#F0F0F0"));
            break;
        case (notLink):
            painter.fillRect(this->rect(), Qt::red);
            break;
        case (unKnown):
            painter.fillRect(this->rect(), MyColor::unKnownColor());

        /*case (highlight):
            painter.fillRect(this->rect(), Qt::yellow);
            break;*/
    }
}

void PipeWidget::mousePressEvent(QMouseEvent *e)
{
    //timer->start(QApplication::doubleClickInterval());
    switch ( e->button()) {
    case Qt::LeftButton:
        insertDelete();
        break;
    case Qt::RightButton:
        changeWidth();
        break;
    default:
        break;
    }
}

void PipeWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    //timer->stop();

}


bool PipeWidget::event(QEvent *event)
{
    if (status == exist)
    {
        if (event->type() == QEvent::ToolTip)
        {
            setToolTip(QString("Width: %1\nValue: %2").arg(realWidth).arg(realValue));
        }
        else if (event->type() == QEvent::Leave)
        {
            setToolTip("");
        }
    }
    return QWidget::event(event);
}

void PipeWidget::insertDelete()
{
    if (!canChange)
        return;
    //timer->stop();
    if (status == exist || status == notLink || status == unKnown)
    {
        //status = notExist;
        emit deleted();
    }
    else if (status == notExist)
    {
        if (this->maxWidth < Chip::pipeWidthDefault)
            return;
        //status = exist;
        emit inserted();
    }
}

void PipeWidget::changeWidth()
{
    if (!canChange)
        return;
    if (Chip::pipeMinimalWidth>maxWidth)
    {
        QMessageBox::warning(this, "Error", "Can't add a new pipe, as there's no enough space.");
        return;
    }
    PipeInsertDialog dialog(Chip::pipeMinimalWidth, maxWidth);
    //qDebug("legal width %d %d", Chip::pipeMinimalWidth, maxWidth);
    if (dialog.exec() == QDialog::Accepted)
    {
        *newWidthSaver = dialog.getData();
        emit changed();
    }
}

