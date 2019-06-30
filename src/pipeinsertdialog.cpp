#include "pipeinsertdialog.h"
#include "ui_pipeinsertdialog.h"

PipeInsertDialog::PipeInsertDialog(int min, int max, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PipeInsertDialog)
{
    ui->setupUi(this);
    ui->spinBox->setRange(min, max);
    ui->label_2->setText(QString("(%1-%2)").arg(min).arg(max));
}

PipeInsertDialog::~PipeInsertDialog()
{
    delete ui;
}

int PipeInsertDialog::getData()
{
    return ui->spinBox->value();
}
