#ifndef COLORBAR_H
#define COLORBAR_H

#include <QFrame>

class ColorBar : public QFrame
{
    Q_OBJECT
public:
    explicit ColorBar(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event);

signals:

public slots:
};

#endif // COLORBAR_H
