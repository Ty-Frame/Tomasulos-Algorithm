#ifndef TOMASULOALGORITHM_H
#define TOMASULOALGORITHM_H

#include <QObject>
#include "functionalunit.h"
#include "instruction.h"



enum TomasuloRunStatus{
    NotStarted,
    ManualStep,
    ClockStep,
    AutomaticStep,
    Paused,
    Done
};

class TomasuloAlgorithm : public QObject
{
    Q_OBJECT
public:
    explicit TomasuloAlgorithm(QList<GeneralFunctionalUnit>* generalFunctionalUnitList,
                               QList<MemoryFunctionalUnit>* memoryFunctionalUnitList,
                               QList<RegisterFunctionalUnit>* registerFunctionalUnitList,
                               QList<CommonDataBusFunctionalUnit>* commonDataBusFunctionalUnitList,
                               QList<ScriptInstruction*>* scriptInstructionList,\
                               int issueNumber,
                               QObject *parent = nullptr);


    TomasuloRunStatus getRunStatus() const;
    void setRunStatus(TomasuloRunStatus newRunStatus);

    int issueNumber() const;
    void setIssueNumber(int newIssueNumber);

    unsigned int currentInstruction() const;
    void setCurrentInstruction(unsigned int newCurrentInstruction);

    unsigned int clockCycle() const;

    void reset();

signals:
    void StepDone();

public slots:
    void processStep();

private:
    QList<GeneralFunctionalUnit>* mGeneralFunctionalUnitList;
    QList<MemoryFunctionalUnit>* mMemoryFunctionalUnitList;
    QList<RegisterFunctionalUnit>* mRegisterFunctionalUnitList;
    QList<CommonDataBusFunctionalUnit>* mCommonDataBusFunctionalUnitList;
    QList<ScriptInstruction*>* mScriptInstructionList;

    unsigned int mCurrentInstruction = 0;
    unsigned int mClockCycle = 1;
    int mIssueNumber;
    TomasuloRunStatus runStatus = TomasuloRunStatus::NotStarted;

    void updateFunctionalUnits();
    void checkForDependencies(ScriptInstruction* ins); // Check for data depencides for instruction
    void issueInstruction(ScriptInstruction* ins);
    bool doDependenciesExist(ScriptInstruction* ins);
    int getOptimalFunctionalUnit(ScriptInstruction ins); // Finds OptimalFunctionalUnit for General and Memory Units
    MemoryFunctionalUnit* getRegisterFunctionalUnit(MemoryFunctionalUnit* unit);
    void issueInsToRegUnit(ScriptInstruction *ins);
};

#endif // TOMASULOALGORITHM_H
