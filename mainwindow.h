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
#include <QMenu>
#include <QInputDialog>
#include <QTimer>
#include "tomasuloalgorithm.h"
#include "architecturefileeditordialog.h"
#include "instructionlistfileeditordialog.h"

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
    void pauseClock();

    void on_actionCreate_Architecture_triggered();

    void on_actionLoad_Architecture_triggered();

    void on_actionEdit_Architecture_triggered();

    void on_actionCreate_Instruction_List_triggered();

    void on_actionEdit_Instruction_List_triggered();

    void on_actionLoad_Instruction_List_triggered();

    void on_actionEdit_Script_triggered();

    void on_actionCreate_Script_triggered();

    void on_actionLoad_Script_triggered();

    void on_actionClose_Application_triggered();

//    void popupStartMenu();

    void individualStep();

    void fullSpeedStep();

    void on_actionUnload_Files_triggered();

    void on_actionReset_Algorithm_triggered();

    void on_actionLoad_Files_triggered();

    void on_actionStep_Algorithm_triggered();

    void on_actionPause_Algorithm_triggered();

    void on_actionManual_Step_Algorithm_triggered();

    void on_actionClock_Step_Algorithm_triggered();

    void on_actionFull_Speed_Algorithm_triggered();

    void on_actionPrerun_Check_triggered();

    void updateRunStatusOptions();

private:
    Ui::MainWindow *ui;

    // Algorithm Object
    TomasuloAlgorithm* mTomasuloAlgorithm;
    int mIssueNumber = 1;

    // Run Capability Stuff
    QTimer* mRunClock = new QTimer();
    void startClock();
    void updateAllTables();

    // Status bar widgets and such
    QWidget* mStatusBarWidget = nullptr;
    QHBoxLayout* mStatusBarLayout = nullptr;
    QLabel* mLoadedArchitectureFile = nullptr;
    QLabel* mLoadedInstructionListFile = nullptr;
    QLabel* mLoadedScriptFile = nullptr;

    // Lists to keep track of instructions, memory functional units, other funcitonal units, and registers
    QList<GeneralFunctionalUnit*>* mGeneralFunctionalUnitList = new QList<GeneralFunctionalUnit*>();
    QList<MemoryFunctionalUnit*>* mMemoryFunctionalUnitList = new QList<MemoryFunctionalUnit*>();
    QList<RegisterFunctionalUnit*>* mRegisterFunctionalUnitList = new QList<RegisterFunctionalUnit*>();
    QList<CommonDataBusFunctionalUnit*>* mCommonDataBusFunctionalUnitList = new QList<CommonDataBusFunctionalUnit*>();
    QList<ScriptInstruction*>* mScriptInstructionList = new QList<ScriptInstruction*>();
    QList<Instruction>* mInstructionList = new QList<Instruction>();

    void populateFunctionalUnitReservationTable();
    void populateCommonDataBusAndRegisterTable();
    void populateMemoryReservationTable();
    void populateInstructionTable();
    void populateExecutionTable();
    void clearAllTables();


    void initializeWindow();
    void loadArchitecture(QString filename);
    void loadScript(QString filename);
    void loadInstructionList(QString filename);
    void unloadFiles();
    void resetAlgorithm();
    bool checkNotRunning();

signals:
    void processStep();
};
#endif // MAINWINDOW_H
