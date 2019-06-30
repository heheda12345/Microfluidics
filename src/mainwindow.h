#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUndoStack>
#include <QCloseEvent>
#include "chip.h"
#include "calculator.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_emptyChipAction_triggered();
    void on_randomChipAction_triggered();
    void on_exportChipAction_triggered();
    void on_loadChipAction_triggered();
    void on_designChipAction_triggered();
    void on_undoAction_triggered();
    void on_redoAction_triggered();
    void insertPipe(int id);
    void deletePipe(int id);
    void changePipe(int id);

    void on_fullAction_triggered();

protected:
    void closeEvent(QCloseEvent *event);

private:
    int saveChip();
    void generateChip(Chip::generateType type);
    void setCurChip(RawChip chip);
    void enableChipAction();
    void setExistChip(bool isExist);
    void showChip();
    bool loadCheck();
    Ui::MainWindow *ui;
    bool existChip;
    Chip curChip;
    QUndoStack chipStack;
};

#endif // MAINWINDOW_H
