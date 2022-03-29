#ifndef TOMASULOALGORITHM_H
#define TOMASULOALGORITHM_H

#include <QObject>
#include "functionalunit.h"
#include "instruction.h"

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
};

#endif // TOMASULOALGORITHM_H
