#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStatusBar>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpacerItem>
#include <QWidget>
#include <QFileDialog>
#include <QDir>
#include <QCheckBox>
#include "architecturefileeditordialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionCreate_Architecture_triggered();

    void on_actionLoad_Architecture_triggered();

    void on_actionEdit_Architecture_triggered();

private:
    Ui::MainWindow *ui;

    // Status bar widgets and such
    QWidget* mStatusBarWidget = nullptr;
    QLabel* mStatusBarLabel = nullptr;
    QPushButton* mStatusBarStartPauseButton = nullptr;
    QPushButton* mStatusBarCancelButton = nullptr;
    QHBoxLayout* mStatusBarLayout = nullptr;

    // Lists to keep track of instructions, memory functional units, other funcitonal units, and registers
    QList<GeneralFunctionalUnit>* mGeneralFunctionalUnitList = new QList<GeneralFunctionalUnit>();
    QList<MemoryFunctionalUnit>* mMemoryFunctionalUnitList = new QList<MemoryFunctionalUnit>();
    QList<RegisterFunctionalUnit>* mRegisterFunctionalUnitList = new QList<RegisterFunctionalUnit>();
    QList<CommonDataBusFunctionalUnit>* mCommonDataBusFunctionalUnitList = new QList<CommonDataBusFunctionalUnit>();
    //QList<Instruction>* mInstructionList = new QList<Instruction>();

    void populateFunctionalUnitReservationTable();
    void populateCommonDataBusAndRegisterTable();
    void populateMemoryReservationTable();


    void initializeWindow();
    void loadArchitecture(QString filename);
};
#endif // MAINWINDOW_H
