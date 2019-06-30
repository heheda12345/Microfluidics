#include "newchipdialog.h"
#include "ui_newchipdialog.h"

#include <QDebug>

NewChipDialog::NewChipDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewChipDialog)
{
    ui->setupUi(this);
    check();
    qDebug() << this->children();
    qDebug() << ui->gridLayout->children();
    for (QObject* ob : this->children())
    {
        if (ob->metaObject()->className() == QStringLiteral("QFrame"))
        {
            ob->installEventFilter(this);
            for (QObject* obj : ob->children())
            {
                obj->installEventFilter(this);
                if (obj->inherits("QSpinBox"))
                {
                    qDebug() << obj;
                    connect(qobject_cast<QSpinBox*>(obj), SIGNAL(valueChanged(int)),this, SLOT(check()));
                    for (QObject* obb : ob->children())
                    {
                        obb->installEventFilter(this);
                    }
                    obj->installEventFilter(this);
                }
            }
        }
    }/*
    for (auto obj : ui->widthFrame->children())
        connect(qobject_cast<QSpinBox*>(obj), SIGNAL(valueChanged(int)),this, SLOT(check()));
    for (auto obj : ui->inputFrame->children())
        connect(qobject_cast<QSpinBox*>(obj), SIGNAL(valueChanged(int)),this, SLOT(check()));
    for (auto obj : ui->outputFrame->children())
        connect(qobject_cast<QSpinBox*>(obj), SIGNAL(valueChanged(int)),this, SLOT(check()));
    */
}

NewChipDialog::~NewChipDialog()
{
    delete ui;
}

int NewChipDialog::width() const
{
    return ui->widthSpinBox->value();
}

QVector<int> NewChipDialog::inputPipes() const
{
    QVector<int> ret;
    ret.append(ui->inputSpinBox0->value());
    ret.append(ui->inputSpinBox1->value());
    return ret;
}

QVector<int> NewChipDialog::outputPipes() const
{
    QVector<int> ret;
    ret.append(ui->outputSpinBox0->value());
    ret.append(ui->outputSpinBox1->value());
    ret.append(ui->outputSpinBox2->value());
    return ret;
}

bool NewChipDialog::eventFilter(QObject *obj, QEvent *e)
{
    //qDebug() << "enter pressed";
    if (e->type()==QEvent::KeyPress)
    {
        if (static_cast<QKeyEvent*>(e)->key() == Qt::Key_Return)
        {
            //qDebug() << "enter pressed " << obj;
            return false;
        }
        //else return false;
    }
    return QDialog::eventFilter(obj, e);
}

void NewChipDialog::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Return)
    {
        if (isValid())
            accept();
    }
}

bool NewChipDialog::isValid()
{
    int n = width();
    if (n<nMin || n>nMax)
    {
        return 0;
    }
    auto checkPipe = [&](const QVector<int>& vec)
    {
        for (int i=0; i<vec.size(); i++)
        {
            if (vec[i]<0 || vec[i]>=n)
            {
                return 0;
            }
        }
        for (int i=0; i<vec.size(); i++)
            for (int j=i+1; j<vec.size(); j++)
                if (vec[i] == vec[j])
                {
                    return 0;
                }
        return 1;
    };
    if (!checkPipe(inputPipes()))
        return 0;
    if (!checkPipe(outputPipes()))
        return 0;
    return 1;
}


void NewChipDialog::check()
{
    if (isValid())
    {
        ui->buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    }
    else
    {
        ui->buttonBox->setStandardButtons(QDialogButtonBox::Cancel);
    };


}
