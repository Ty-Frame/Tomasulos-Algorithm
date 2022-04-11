#include "tomasuloalgorithm.h"
#include<QDebug>
#include <iostream>

TomasuloAlgorithm::TomasuloAlgorithm(QList<GeneralFunctionalUnit>* generalFunctionalUnitList,
                                     QList<MemoryFunctionalUnit>* memoryFunctionalUnitList,
                                     QList<RegisterFunctionalUnit>* registerFunctionalUnitList,
                                     QList<CommonDataBusFunctionalUnit>* commonDataBusFunctionalUnitList,
                                     QList<ScriptInstruction*>* scriptInstructionList,
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

    // Assuming one cbd for now
    CommonDataBusFunctionalUnit cbd = mCommonDataBusFunctionalUnitList->at(0);
    cbd.mBusy = false;
    cbd.mFunctionalUnitWithClaim = "";
    mCommonDataBusFunctionalUnitList->replace(0,cbd);


    updateFunctionalUnits();

    // Get Instruction (need to check if things are available)
    ScriptInstruction* ins;

    // Get next instruction
    if(mCurrentInstruction < mScriptInstructionList->length()){
        ins = mScriptInstructionList->value(mCurrentInstruction);
        qDebug() << "Optimal" << getOptimalFunctionalUnit(*ins);

        // Attempt to Issue Instruction to Unit
        issueInstruction(ins);

            //mScriptInstructionList->replace(mCurrentInstruction,ins);
            mCurrentInstruction++;
            qInfo() << ins->mIssueClockCycle;
    }

    mClockCycle++;
    emit StepDone();
}

unsigned int TomasuloAlgorithm::currentInstruction() const
{
    return mCurrentInstruction;
}

void TomasuloAlgorithm::setCurrentInstruction(unsigned int newCurrentInstruction)
{
    mCurrentInstruction = newCurrentInstruction;
}

int TomasuloAlgorithm::issueNumber() const
{
    return mIssueNumber;
}

void TomasuloAlgorithm::setIssueNumber(int newIssueNumber)
{
    mIssueNumber = newIssueNumber;
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

    // TODO handle how to remove proper instruction from register units
    for(int i = 0; i < mGeneralFunctionalUnitList->length(); i++) {
         GeneralFunctionalUnit unit = mGeneralFunctionalUnitList->at(i);

         if(unit.mReservationStationList.empty()) {
             continue;
         }

         ScriptInstruction *ins = unit.mReservationStationList.at(0);
         // TODO Check for Dependencies
         if(ins->mCurrentPipelineStage == PipelineStages::Issue && !unit.mBusy && !doDependenciesExist(ins)) {  // Move instruction into Exection stage
             ins->mCurrentPipelineStage = PipelineStages::Execution;
             ins->mExecutionStartClockCycle = mClockCycle;
             unit.mBusy = true;

         } else if(ins->mCurrentPipelineStage == PipelineStages::Execution && unit.mCountDown > 0) {  // Instruction continue in the execution stage
             unit.mCountDown--;

         } else if(ins->mCurrentPipelineStage == PipelineStages::Execution && unit.mCountDown == 0) {  // Instruction finished the execution stage
             ins->mExecutionCompletionClockCycle = mClockCycle;
             unit.mBusy = false;
             ins->mCurrentPipelineStage = PipelineStages::Writeback;


         } else if(ins->mCurrentPipelineStage == PipelineStages::Writeback && !mCommonDataBusFunctionalUnitList->at(0).mBusy) {
             // TODO check fo CDB to be available
             ins->mCurrentPipelineStage = PipelineStages::Commit;
             ins->mWriteResultClockCycle = mClockCycle;
             CommonDataBusFunctionalUnit cbd = mCommonDataBusFunctionalUnitList->at(0);
             cbd.mBusy = true;
             cbd.mFunctionalUnitWithClaim = unit.mOperation; // TODO update to correct functional unit
             mCommonDataBusFunctionalUnitList->replace(0,cbd);

             for(int j = 0; j < mRegisterFunctionalUnitList->length(); j++) {
                 RegisterFunctionalUnit reg = mRegisterFunctionalUnitList->at(j);
                 if(reg.mInstruction == ins) {
                     //reg.mFunctionalUnit = ;
                     reg.mFunctionalUnitWithClaim = "";
                     reg.mInstruction = nullptr;
                     mRegisterFunctionalUnitList->replace(j,reg);
                     break;
                 }

             }

         } else if(ins->mCurrentPipelineStage == PipelineStages::Commit) {
             // TODO update to commit at correct time
             ins->mCurrentPipelineStage = PipelineStages::None;
             ins->mCommitClockCycle = mClockCycle;
             unit.mReservationStationList.pop_front();




         }

         //unit.mReservationStationList.replace(0,ins);

         mGeneralFunctionalUnitList->replace(i,unit);
    }

    // TODO handle how to remove proper instruction from register units
    for(int i = 0; i < mMemoryFunctionalUnitList->length(); i++) {
         MemoryFunctionalUnit unit = mMemoryFunctionalUnitList->at(i);

         if(unit.mReservationStationList.empty()) {
             continue;
         }

         ScriptInstruction *ins = unit.mReservationStationList.at(0);
         // TODO Check for Dependencies
         if(ins->mCurrentPipelineStage == PipelineStages::Issue && !unit.mBusy && !doDependenciesExist(ins)) {  // Move instruction into Exection stage
             ins->mCurrentPipelineStage = PipelineStages::Execution;
             ins->mExecutionStartClockCycle = mClockCycle;
             unit.mBusy = true;

         } else if(ins->mCurrentPipelineStage == PipelineStages::Execution) {  // Instruction finished the execution stage
             ins->mExecutionCompletionClockCycle = mClockCycle;
             ins->mReadAccessClockCycle = mClockCycle + 1;
             unit.mBusy = false;

             if(ins->mInstruction.mMemoryOptions == MemoryOptions::Store) {
                 ins->mCurrentPipelineStage = PipelineStages::Commit;
             } else {
                 ins->mCurrentPipelineStage = PipelineStages::Writeback;
             }



         } else if(ins->mCurrentPipelineStage == PipelineStages::Writeback && !mCommonDataBusFunctionalUnitList->at(0).mBusy) {
             // TODO check fo CDB to be available
             ins->mCurrentPipelineStage = PipelineStages::Commit;
             ins->mWriteResultClockCycle = mClockCycle;
             CommonDataBusFunctionalUnit cbd = mCommonDataBusFunctionalUnitList->at(0);
             cbd.mBusy = true;
             cbd.mFunctionalUnitWithClaim = unit.mOperation; // TODO update to correct functional unit
             mCommonDataBusFunctionalUnitList->replace(0,cbd);

             for(int j = 0; j < mRegisterFunctionalUnitList->length(); j++) {
                 RegisterFunctionalUnit reg = mRegisterFunctionalUnitList->at(j);
                 if(reg.mInstruction == ins) {
                     //reg.mFunctionalUnit = ;
                     reg.mFunctionalUnitWithClaim = "";
                     reg.mInstruction = nullptr;
                     mRegisterFunctionalUnitList->replace(j,reg);
                     break;
                 }

             }

         } else if(ins->mCurrentPipelineStage == PipelineStages::Commit) {
             // TODO update to commit at correct time
             ins->mCurrentPipelineStage = PipelineStages::None;
             ins->mCommitClockCycle = mClockCycle;
             unit.mReservationStationList.pop_front();

         }

         //unit.mReservationStationList.replace(0,ins);

         mMemoryFunctionalUnitList->replace(i,unit);
    }

}

void TomasuloAlgorithm::issueInstruction(ScriptInstruction *ins) {
    // Checks to see if a Reservation Station is available to issue instruction
    int unitIndex = getOptimalFunctionalUnit(*ins);

    if(unitIndex == -1) {
        qDebug() << "Unable to issue instruction";
        return;
    }
    // Arithmetic ins
    FunctionalUnit functionalUnit;
    // Find Available FunctionalUnit

    if(ins->mInstruction.mInstructionType == InstructionType::Arithmetic) {

        GeneralFunctionalUnit unit = mGeneralFunctionalUnitList->at(unitIndex);

        ins->mCurrentPipelineStage = PipelineStages::Issue;
        ins->mIssueClockCycle = mClockCycle;
        unit.mSourceOne = ins->mSourceOneRegister;
        unit.mSourceTwo = ins->mSourceTwoRegister;
        unit.mReservationStationList.append(ins);
        unit.mCountDown = unit.mFunctionalUnit.mLatency + 1; // Assumming countDown is for counting down when execution is done
        //mScriptInstructionList->replace(mCurrentInstruction,*ins);
        mGeneralFunctionalUnitList->replace(unitIndex,unit);
        issueInsToRegUnit(ins);

     } else if(ins->mInstruction.mInstructionType == InstructionType::Memory) {

        MemoryFunctionalUnit unit = mMemoryFunctionalUnitList->at(unitIndex);

        ins->mCurrentPipelineStage = PipelineStages::Issue;
        ins->mIssueClockCycle = mClockCycle;
        unit.mSourceOne = ins->mSourceOneRegister; // Is this correct register
        unit.mReservationStationList.append(ins);
        mMemoryFunctionalUnitList->replace(unitIndex,unit);
        //mScriptInstructionList->replace(mCurrentInstruction,*ins);
        issueInsToRegUnit(ins);
    }




}

void TomasuloAlgorithm::issueInsToRegUnit(ScriptInstruction *ins) {
    // Run optimalInstruction before this
    int len = mRegisterFunctionalUnitList->length();
    for(int i = 0; i<len; i++){
        RegisterFunctionalUnit rUnit = mRegisterFunctionalUnitList->at(i);
        if(rUnit.mFunctionalUnitWithClaim.isEmpty()){
           rUnit.mFunctionalUnit.mName = ins->mDestinationRegister;
           rUnit.mInstruction = ins;
           rUnit.mFunctionalUnitWithClaim = ins->mInstructionName;  // TODO

           mRegisterFunctionalUnitList->replace(i,rUnit);
           break;
        }

    }

}

bool TomasuloAlgorithm::doDependenciesExist(ScriptInstruction *ins)
{
    int len = mRegisterFunctionalUnitList->length();
    for(int i = 0; i<len; i++){
        RegisterFunctionalUnit rUnit = mRegisterFunctionalUnitList->at(i);
        if(ins->mSourceOneRegister == rUnit.mFunctionalUnit.mName && !rUnit.mFunctionalUnitWithClaim.isEmpty() && rUnit.mInstruction->mIssueClockCycle < ins->mIssueClockCycle){
            return true;
        }
        if(ins->mSourceTwoRegister == rUnit.mFunctionalUnit.mName && !rUnit.mFunctionalUnitWithClaim.isEmpty() && rUnit.mInstruction->mIssueClockCycle < ins->mIssueClockCycle){
            return true;
        }
    }

    return false;
}


int TomasuloAlgorithm::getOptimalFunctionalUnit(ScriptInstruction ins)
{
    int shortestIndex = -1;

    if(ins.mInstruction.mMemoryOptions != MemoryOptions::Store) {
        bool registerUnitAvailable = false;
        for(auto unit : *mRegisterFunctionalUnitList) {
            if(unit.mFunctionalUnitWithClaim.isEmpty()) {
                registerUnitAvailable = true;
                break;
            }
        }

        if(!registerUnitAvailable) // TODO does this memoryoptions also just return all memory options?
            return shortestIndex;
    }

    if(ins.mInstruction.mInstructionType == InstructionType::Arithmetic) {
        int shortest = mGeneralFunctionalUnitList->at(0).mFunctionalUnit.mReservationStationCount;
        // Probably need to change for loops
        for(int i = 0; i < mGeneralFunctionalUnitList->length(); i++) {
            if(!mGeneralFunctionalUnitList->at(i).mBusy && (mGeneralFunctionalUnitList->at(i).mReservationStationList.length() == 0)){
                return i;
            }
            if(mGeneralFunctionalUnitList->at(i).mReservationStationList.length() < shortest) {
                shortest = mGeneralFunctionalUnitList->at(i).mReservationStationList.length();
                shortestIndex = i;
            }
        }
    } else if(ins.mInstruction.mInstructionType == InstructionType::Memory) {
        int shortest = mMemoryFunctionalUnitList->at(0).mFunctionalUnit.mReservationStationCount;
        for(int i = 0; i < mMemoryFunctionalUnitList->length(); i++) {
            if(!mMemoryFunctionalUnitList->at(i).mBusy && (mMemoryFunctionalUnitList->at(i).mReservationStationList.length() == 0)){
                return i;
            }
            if(mMemoryFunctionalUnitList->at(i).mReservationStationList.length() < shortest) {
                shortest = mMemoryFunctionalUnitList->at(i).mReservationStationList.length();
                shortestIndex = i;
            }
        }
    }

    return shortestIndex;
}
