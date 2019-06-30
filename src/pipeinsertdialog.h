#ifndef PIPEINSERTERDIALOG_H
#define PIPEINSERTERDIALOG_H

#include <QDialog>

namespace Ui {
class PipeInsertDialog;
}

class PipeInsertDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PipeInsertDialog(int min, int max, QWidget *parent = 0);
    ~PipeInsertDialog();
    int getData();

private:
    Ui::PipeInsertDialog *ui;
};

#endif // PIPEINSERTERDIALOG_H
