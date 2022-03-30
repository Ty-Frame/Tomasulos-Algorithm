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
                               QList<ScriptInstruction>* scriptInstructionList,
                               QObject *parent = nullptr);

    bool isDone() const;

    TomasuloRunStatus getRunStatus() const;
    void setRunStatus(TomasuloRunStatus newRunStatus);

signals:
    void StepDone();

public slots:
    void ProcessStep();

private:
    QList<GeneralFunctionalUnit>* mGeneralFunctionalUnitList;
    QList<MemoryFunctionalUnit>* mMemoryFunctionalUnitList;
    QList<RegisterFunctionalUnit>* mRegisterFunctionalUnitList;
    QList<CommonDataBusFunctionalUnit>* mCommonDataBusFunctionalUnitList;
    QList<ScriptInstruction>* mScriptInstructionList;

    bool done = false;
    TomasuloRunStatus runStatus = TomasuloRunStatus::NotStarted;
};

#endif // TOMASULOALGORITHM_H
