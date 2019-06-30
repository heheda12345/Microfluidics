#ifndef PIPEWIDGET_H
#define PIPEWIDGET_H

#include <QWidget>
#include <QPalette>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QTimer>

class PipeWidget : public QWidget
{
    Q_OBJECT
public:
    enum Status { exist, notExist, notLink,unKnown };
    explicit PipeWidget(QWidget *parent = nullptr);
    void setWidthSaver(int* saver);
    void setColor(double rate);
    void setMaxWidth(int w);
    void setVertical(bool v) { isVertical = v; }
    void setStatus(Status s) { status = s; }
    void setCanChange(bool c) { canChange = c; }
    void setRealData(int width, double value);

protected:
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    bool event(QEvent *event);

private:
    QColor backgroundColor;
    bool isVertical;
    Status status;
    //QPalette palette;
    int* newWidthSaver;
    int maxWidth;
    bool canChange;
    int realWidth;
    double realValue;
    //QTimer *timer;


signals:
    void inserted();
    void deleted();
    void changed();

public slots:
    void insertDelete();
    void changeWidth();
};

#endif // PIPEWIDGET_H
