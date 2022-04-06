#include "tomasuloalgorithm.h"
#include<QDebug>
#include <iostream>

TomasuloAlgorithm::TomasuloAlgorithm(QList<GeneralFunctionalUnit>* generalFunctionalUnitList,
                                     QList<MemoryFunctionalUnit>* memoryFunctionalUnitList,
                                     QList<RegisterFunctionalUnit>* registerFunctionalUnitList,
                                     QList<CommonDataBusFunctionalUnit>* commonDataBusFunctionalUnitList,
                                     QList<ScriptInstruction>* scriptInstructionList,
                                     int issueNumber,
                                     QObject *parent) : QObject(parent)
{
    mGeneralFunctionalUnitList = generalFunctionalUnitList;
    mMemoryFunctionalUnitList = memoryFunctionalUnitList;
    mRegisterFunctionalUnitList = registerFunctionalUnitList;
    mCommonDataBusFunctionalUnitList = commonDataBusFunctionalUnitList;
    mScriptInstructionList = scriptInstructionList;
    mIssueNumber = issueNumber;

}

void TomasuloAlgorithm::processStep()
{

    updateFunctionalUnits();

    std::cout << "Hello" << endl;
    std::cout << mScriptInstructionList->length() << endl;

    // Get Instruction (need to check if things are available)
    ScriptInstruction ins;

    // Get next instruction
    ins = mScriptInstructionList->value(currentInstruction);

    // Attempt to Issue Instruction to Unit
    if(issueInstruction(&ins)) {

        mScriptInstructionList->replace(currentInstruction,ins);
        currentInstruction++;
        qInfo() << ins.mIssueClockCycle;
    }

    mClockCycle++;
    emit StepDone();
}

TomasuloRunStatus TomasuloAlgorithm::getRunStatus() const
{
    return runStatus;
}

void TomasuloAlgorithm::setRunStatus(TomasuloRunStatus newRunStatus)
{
    runStatus = newRunStatus;
}

bool TomasuloAlgorithm::isDone() const
{
    return done;
}

void TomasuloAlgorithm::updateFunctionalUnits() {
    // Initiate Execution
    for(int i = 0; i < mGeneralFunctionalUnitList->length(); i++) {
         GeneralFunctionalUnit unit = mGeneralFunctionalUnitList->at(i);
         if(unit.mBusy) {
             ScriptInstruction *ins = unit.mReservationStationList.at(0);
             // TODO Check for Dependencies
             if(ins->mInstruction.mPipelineStages == PipelineStages::Issue) {  // Move instruction into Exection stage
                 ins->mInstruction.mPipelineStages = PipelineStages::Execution;
                 ins->mExecutionStartClockCycle = mClockCycle;
                 unit.mBusy = true;

             } else if(ins->mInstruction.mPipelineStages == PipelineStages::Execution && unit.mCountDown > 0) {  // Instruction continue in the execution stage
                 unit.mCountDown--;

             } else if(ins->mInstruction.mPipelineStages == PipelineStages::Execution && unit.mCountDown == 0) {  // Instruction finished the execution stage
                ins->mExecutionCompletionClockCycle = mClockCycle;
                unit.mBusy = false;
                ins->mInstruction.mPipelineStages = PipelineStages::Writeback;


             } else if(ins->mInstruction.mPipelineStages == PipelineStages::Writeback) {
                 // TODO check fo CDB to be available
                 ins->mInstruction.mPipelineStages = PipelineStages::Commit;

             } else if(ins->mInstruction.mPipelineStages == PipelineStages::Commit) {
                 // TODO check fo CDB to be available
                 ins->mInstruction.mPipelineStages = PipelineStages::None;


             }
            unit.mReservationStationList.replace(0,ins);
         }
         mGeneralFunctionalUnitList->replace(i,unit);
    }

}

bool TomasuloAlgorithm::issueInstruction(ScriptInstruction *ins) {
    // Checks to see if a Reservation Station is available to issue instruction

    // Does each Reservation count correspond to all units or just one individual unit?

    // Arithmetic ins
    FunctionalUnit functionalUnit;
    // Find Available FunctionalUnit
    if(ins->mInstruction.mInstructionType == InstructionType::Arithmetic) {
        for(int i = 0; i < mGeneralFunctionalUnitList->length(); i++) {
            GeneralFunctionalUnit unit = mGeneralFunctionalUnitList->at(i);
            if(unit.mBusy) // TODO I think unit type is only busy when instruction is in EX stage
                continue;
            for(int j=0; j < unit.mFunctionalUnit.mReservationStationCount; j++) {
                if(unit.mReservationStationList.length() == 0 || unit.mReservationStationList.value(j)->mInstructionName.isNull()) {
                    ins->mInstruction.mPipelineStages = PipelineStages::Issue;
                    ins->mIssueClockCycle = mClockCycle;
                    unit.mReservationStationList.append(ins);
                    unit.mCountDown = unit.mFunctionalUnit.mLatency; // Assumming countDown is for counting down when execution is done
                    unit.mBusy = true;
                    mGeneralFunctionalUnitList->replace(i,unit);
                    return true;

                }
            }

        }
    }

    // Memory ins
    if(ins->mInstruction.mInstructionType == InstructionType::Memory) {
        for(int i = 0; i < mMemoryFunctionalUnitList->length(); i++) {
            MemoryFunctionalUnit unit = mMemoryFunctionalUnitList->at(i);
            if(unit.mBusy) // TODO I think unit type is only busy when instruction is in EX stage
                continue;
            for(int j=0; j < unit.mFunctionalUnit.mReservationStationCount; j++) {
                if(unit.mReservationStationList.length() == 0 || unit.mReservationStationList.value(j)->mInstructionName.isNull()) {
                    ins->mInstruction.mPipelineStages = PipelineStages::Issue;
                    ins->mIssueClockCycle = mClockCycle;
                    unit.mReservationStationList.append(ins);
                    unit.mBusy = true;

                    mMemoryFunctionalUnitList->replace(i,unit);
                    return true;

                }
            }
        }
    }

    return false;
}
