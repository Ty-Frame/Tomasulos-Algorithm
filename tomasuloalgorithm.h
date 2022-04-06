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
    Paused
};

class TomasuloAlgorithm : public QObject
{
    Q_OBJECT
public:
    explicit TomasuloAlgorithm(QList<GeneralFunctionalUnit>* generalFunctionalUnitList,
                               QList<MemoryFunctionalUnit>* memoryFunctionalUnitList,
                               QList<RegisterFunctionalUnit>* registerFunctionalUnitList,
                               QList<CommonDataBusFunctionalUnit>* commonDataBusFunctionalUnitList,
                               QList<ScriptInstruction>* scriptInstructionList,\
                               int issueNumber,
                               QObject *parent = nullptr);

    bool isDone() const;

    TomasuloRunStatus getRunStatus() const;
    void setRunStatus(TomasuloRunStatus newRunStatus);

signals:
    void StepDone();

public slots:
    void processStep();

private:
    QList<GeneralFunctionalUnit>* mGeneralFunctionalUnitList;
    QList<MemoryFunctionalUnit>* mMemoryFunctionalUnitList;
    QList<RegisterFunctionalUnit>* mRegisterFunctionalUnitList;
    QList<CommonDataBusFunctionalUnit>* mCommonDataBusFunctionalUnitList;
    QList<ScriptInstruction>* mScriptInstructionList;

    unsigned int currentInstruction = 0;
    unsigned int mClockCycle = 1;
    bool done = false;
    int mIssueNumber;
    TomasuloRunStatus runStatus = TomasuloRunStatus::NotStarted;

    void updateFunctionalUnits();
    void checkForDependencies(ScriptInstruction* ins); // Check for data depencides for instruction
    bool issueInstruction(ScriptInstruction* ins);
};

#endif // TOMASULOALGORITHM_H
