#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QVector>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QDir>

#include "newchipdialog.h"
#include "designdialog.h"
#include "chipdesigner.h"
#include "myundocommand.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    existChip = 0;
    ui->setupUi(this);
    ui->chipVelocity->setCurChip(curChip.valueDir());
    ui->chipVelocity->setType(ChipShower::velo);
    ui->chipConcentration->setCurChip(curChip.valueDir());
    ui->chipConcentration->setType(ChipShower::con);
    ui->frame_2->setStyleSheet("background-color:white;");
    ui->frame_3->setStyleSheet("background-color:white;");
    //qDebug() << ui->centralWidget->children();
    for (QObject* x : ui->centralWidget->children())
    {
        if (!x->inherits("QWidget"))
            continue;
        qobject_cast<QWidget*>(x)->hide();
    }
    connect(ui->chipVelocity, SIGNAL(inserted(int)), this, SLOT(insertPipe(int)));
    connect(ui->chipVelocity, SIGNAL(deleted(int)), this, SLOT(deletePipe(int)));
    connect(ui->chipVelocity, SIGNAL(changed(int)), this, SLOT(changePipe(int)));
    connect(ui->chipConcentration, SIGNAL(inserted(int)), this, SLOT(insertPipe(int)));
    connect(ui->chipConcentration, SIGNAL(deleted(int)), this, SLOT(deletePipe(int)));
    connect(ui->chipConcentration, SIGNAL(changed(int)), this, SLOT(changePipe(int)));
    setWindowIcon(QIcon("/icon/img/icon.png"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_emptyChipAction_triggered()
{
    if (!loadCheck())
        return;
    generateChip(Chip::empty);
}

void MainWindow::on_randomChipAction_triggered()
{
    if (!loadCheck())
        return;
    generateChip(Chip::random);
}

void MainWindow::generateChip(Chip::generateType type)
{
    qDebug("generate %d", type);
    //Sleep(5000);
    NewChipDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        int n = dialog.width();
        QVector<int> inputPipes = dialog.inputPipes();
        QVector<int> outputPipes = dialog.outputPipes();
        curChip.setValue(Chip::generate(type, n, inputPipes, outputPipes));
        chipStack.clear();
        setExistChip(true);
        showChip();
        //setCurChip(Chip::generate(type, n, inputPipes, outputPipes));
    }
}

void MainWindow::setCurChip(RawChip chip)
{
    MyUndoCommand<Chip, RawChip>* cmd = new MyUndoCommand<Chip, RawChip>(curChip, chip);
    chipStack.push(cmd);
    showChip();
}

int MainWindow::saveChip()
{
    int ret = QMessageBox::question(this,
                                     "Save chip", "Do you want to save this chip?",
                                     QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                     QMessageBox::Yes);

    if (ret == QMessageBox::Yes)
    {
        on_exportChipAction_triggered();
    }
    return ret;
}

void MainWindow::setExistChip(bool isExist)
{
    existChip = isExist;
    ui->exportChipAction->setEnabled(isExist);
    ui->designChipAction->setEnabled(isExist);
    ui->undoAction->setEnabled(isExist);
    ui->redoAction->setEnabled(isExist);
}

QString threeSignificant (double dt)
{
    //qDebug("three %lf", dt);
    if (dt == 0)
        return "0.00";
    double x = dt;
    int mul = 0;
    while (x<100)
    {
        x*=10;
        mul++;;
    }
    while (x>=1000)
    {
        x/=10;
        mul--;
    }
    int rd = round(x);
    if (rd == 1000)
    {
        x/=10;
        rd/=10;
        mul--;
    }
    QString ret0 = QString::number(rd), ret1 = "";
    while (mul<0)
    {
        ret0.append("0");
        mul++;
    }
    while (mul>0)
    {
        int l = ret0.length();
        ret1 = ret0[l-1] + ret1;
        if (l==1)
            ret0 = "0";
        else
            ret0.remove(l-1, 1);
        mul--;
    }
    qDebug() << ret0+"."+ret1;
    if (ret1 == "")
        return ret0;
    else
        return ret0+"."+ret1;
}

void MainWindow::showChip()
{
    int n=curChip.value().n;
    int fixSize = (n+1)*Chip::pipeLength*ChipShower::MUL+10;
    setFixedSize(210+fixSize*2, 316+fixSize);
    ui->widget->resize(210+fixSize*2, 250+fixSize);
    ui->frame_2->setFixedSize(fixSize, fixSize);
    ui->frame_3->setFixedSize(fixSize, fixSize);
    qDebug("mainwindow %d %d %d %d", n, fixSize, ui->frame_2->width(), ui->frame_3->width());

    for (QObject* x : ui->centralWidget->children())
    {
        if (!x->inherits("QWidget"))
            continue;
        qobject_cast<QWidget*>(x)->show();
    }

    Calculator calc(curChip.valueDir());

    ui->chipConcentration->setData(calc.getC());
    ui->chipConcentration->updateChip();

    ui->chipVelocity->setData(calc.getV());
    ui->chipVelocity->updateChip();
    const QVector<double> &velo = ui->chipVelocity->getData();
    const QVector<double> &con = ui->chipConcentration->getData();
    QString info;
    QTextStream(&info) << "Velocity: ";
    for (int i = velo.size()-Chip::outputPipe; i < velo.size(); i++)
        QTextStream(&info) << QString("%1 ").arg(velo[i]);
    QTextStream(&info) << "  Concentration: ";
    for (int i = con.size()-Chip::outputPipe; i < con.size(); i++)
        QTextStream(&info) << QString("%1 ").arg(con[i]);
    qDebug() << info;
    int ad = curChip.value().width.size();
    ad -= Chip::outputPipe;
    ui->label_3->setText(QString("Column %1: %2").arg(curChip.value().output[0])
            .arg(threeSignificant(velo[0+ad])));
    ui->label_4->setText(QString("Column %1: %2").arg(curChip.value().output[1])
            .arg(threeSignificant(velo[1+ad])));
    ui->label_5->setText(QString("Column %1: %2").arg(curChip.value().output[2])
            .arg(threeSignificant(velo[2+ad])));
    ui->label_8->setText(QString("Column %1: %2").arg(curChip.value().output[0])
            .arg(threeSignificant(con[0+ad])));
    ui->label_9->setText(QString("Column %1: %2").arg(curChip.value().output[1])
            .arg(threeSignificant(con[1+ad])));
    ui->label_10->setText(QString("Column %1: %2").arg(curChip.value().output[2])
            .arg(threeSignificant(con[2+ad])));
    ui->label0->setText(QString("min: %1")
                        .arg(threeSignificant(0)));
    ui->label1->setText(QString("max: %1")
                        .arg(threeSignificant(ui->chipVelocity->max())));
    ui->label2->setText(QString("min: %1")
                        .arg(threeSignificant(0)));
    ui->label3->setText(QString("max: %1")
                        .arg(threeSignificant(ui->chipConcentration->max())));
    //ui->statusBar->showMessage(info);
}

bool MainWindow::loadCheck()
{
    if (existChip)
    {
        int ret = saveChip();
        if (ret == QMessageBox::Cancel)
            return 0;
    }
    return 1;
}

void MainWindow::on_exportChipAction_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                 QDir::currentPath(),
                                 tr("*.chip"));
    if (fileName.isEmpty())
        return;
    curChip.print(fileName);
}

void MainWindow::on_loadChipAction_triggered()
{
    if (!loadCheck())
        return;
    QFileDialog fd(this);
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                 QDir::currentPath(),
                                 tr("*.chip"));
    if (fileName.isEmpty())
        return;
    curChip.setValue(Chip::load(fileName));
    chipStack.clear();
    showChip();
    setExistChip(true);
}

void MainWindow::on_designChipAction_triggered()
{
    DesignDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        ChipDesigner cd(curChip.value());
        curChip.setValue(cd.design(dialog.getConcentration()));
        chipStack.clear();
        showChip();
        setExistChip(true);
    }
}

void MainWindow::on_undoAction_triggered()
{
    chipStack.undo();
    showChip();
}

void MainWindow::on_redoAction_triggered()
{
    chipStack.redo();
    showChip();
}

void MainWindow::insertPipe(int id)
{
    setCurChip(curChip.addPipe(id));
}

void MainWindow::deletePipe(int id)
{
    setCurChip(curChip.deletePipe(id));
}

void MainWindow::changePipe(int id)
{
    setCurChip(curChip.changePipe(id, ChipShower::newWidth()));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!loadCheck())
        event->ignore();
    else
        event->accept();
}

void MainWindow::on_fullAction_triggered()
{
    if (!loadCheck())
        return;
    generateChip(Chip::full);
}
