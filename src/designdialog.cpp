#include "designdialog.h"
#include "ui_designdialog.h"

#include <QMessageBox>
#include <QCloseEvent>

DesignDialog::DesignDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DesignDialog)
{
    ui->setupUi(this);
}

DesignDialog::~DesignDialog()
{
    delete ui;
}

QVector<double> DesignDialog::getConcentration() const
{
    QVector<double> ret;
    ret.append(ui->doubleSpinBox0->value());
    ret.append(ui->doubleSpinBox1->value());
    ret.append(ui->doubleSpinBox2->value());
    return ret;
}

void DesignDialog::accept()
{
    double v0 = ui->doubleSpinBox0->value();
    double v1 = ui->doubleSpinBox1->value();
    double v2 = ui->doubleSpinBox2->value();
    //qDebug("get %lf %lf %lf", v0, v1, v2);
    if (v0<v1 || v1<v2)
    {
        QString text = "If the output concentration of the three pipes is not increasing, "
                "the calculation accuracy will not be guaranteed. \n"
                "Do you want to continue?";
        int ret = QMessageBox::warning(this, "Invalid input",
                                       text,
                                       QMessageBox::Yes | QMessageBox::Cancel,
                                       QMessageBox::Cancel);
        if (ret == QMessageBox::Cancel)
            return;
    }
    QDialog::accept();
}


