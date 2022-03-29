#include "tomasuloalgorithm.h"

TomasuloAlgorithm::TomasuloAlgorithm(QList<GeneralFunctionalUnit>* generalFunctionalUnitList,
                                     QList<MemoryFunctionalUnit>* memoryFunctionalUnitList,
                                     QList<RegisterFunctionalUnit>* registerFunctionalUnitList,
                                     QList<CommonDataBusFunctionalUnit>* commonDataBusFunctionalUnitList,
                                     QList<ScriptInstruction>* scriptInstructionList,
                                     QObject *parent) : QObject(parent)
{
    mGeneralFunctionalUnitList = generalFunctionalUnitList;
    mMemoryFunctionalUnitList = memoryFunctionalUnitList;
    mRegisterFunctionalUnitList = registerFunctionalUnitList;
    mCommonDataBusFunctionalUnitList = commonDataBusFunctionalUnitList;
    mScriptInstructionList = scriptInstructionList;
}

void TomasuloAlgorithm::ProcessStep()
{

    emit StepDone();
}
