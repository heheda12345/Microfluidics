#ifndef NEWCHIPDIALOG_H
#define NEWCHIPDIALOG_H

#include <QDialog>
#include <QVector>
#include <QKeyEvent>
namespace Ui {
class NewChipDialog;
}

class NewChipDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewChipDialog(QWidget *parent = 0);
    ~NewChipDialog();
    int width() const;
    static const int nMin = 3;
    static const int nMax = 8;
    QVector<int> inputPipes() const;
    QVector<int> outputPipes() const;
protected:
    bool eventFilter(QObject *obj, QEvent *e);
    void keyPressEvent(QKeyEvent *e);
private:
    Ui::NewChipDialog *ui;
    bool isValid();
public slots:
    void check();

};

#endif // NEWCHIPDIALOG_H
