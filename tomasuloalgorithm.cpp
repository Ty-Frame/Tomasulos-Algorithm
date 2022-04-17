#include "tomasuloalgorithm.h"
#include<QDebug>
#include <iostream>

TomasuloAlgorithm::TomasuloAlgorithm(QList<GeneralFunctionalUnit*>* generalFunctionalUnitList,
                                     QList<MemoryFunctionalUnit*>* memoryFunctionalUnitList,
                                     QList<RegisterFunctionalUnit*>* registerFunctionalUnitList,
                                     QList<CommonDataBusFunctionalUnit*>* commonDataBusFunctionalUnitList,
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
    /* ---Entire Process---
     * Empty Common Data Buses
     * Move earliest issued AND completed instructions into a Common Data Bus
     * If instruction is moved from the functional unit to Common Data Bus, move one from reservation station into funcitonal unit
     * Stage all functional units
     * Issue new instructions if capabale
     */

    // Stage Memory Post ExecutionDone Instructions
    ScriptInstruction* instruct;
    for(int i = 0; i<mScriptInstructionList->length(); i++){
        instruct = mScriptInstructionList->at(i);
        if(instruct->mInstruction.mInstructionType==InstructionType::Memory){
            if(instruct->mCurrentPipelineStage==PipelineStages::ExecutionDone){

            }
        }
    }

    // Empty Data Buses
    CommonDataBusFunctionalUnit* cdbfu;
    for(int i = 0; i<mCommonDataBusFunctionalUnitList->length(); i++){
        cdbfu = mCommonDataBusFunctionalUnitList->at(i);
        if(cdbfu->mBusy){
            if(cdbfu->mScriptInstruction!=nullptr){
                cdbfu->mScriptInstruction->mCurrentPipelineStage = PipelineStages::Done;
                cdbfu->mScriptInstruction->mCommitClockCycle = mClockCycle;
                cdbfu->mScriptInstruction->mFunctionalUnit = nullptr;
                cdbfu->mScriptInstruction->mFunctionalUnitType = FunctionalUnitType::None;
                undoCommonDataBusDependencies(cdbfu->mScriptInstruction);
                qDebug()<<"Instruction Done: "<<cdbfu->mScriptInstruction->mInstructionWhole;
            }
            cdbfu->mScriptInstruction = nullptr;
            cdbfu->mFunctionalUnitWithClaim = "";
            cdbfu->mBusy = false;
        }
    }
//    qDebug()<<"Common Data Buses Emptied";

    // Move issues that are completed into common data bus
    QList<ScriptInstruction*> completedInstructions;
    MemoryFunctionalUnit* memfu;
    for(int i = 0; i<mMemoryFunctionalUnitList->length(); i++){
        memfu = mMemoryFunctionalUnitList->at(i);
        if(memfu->mCountDown == 0 && !memfu->mReservationStationList.isEmpty()){
            completedInstructions.append(memfu->mReservationStationList.first());
        }
    }
//    qDebug()<<"Completed Instructions For Memory Functional Units Gathered";

    GeneralFunctionalUnit* genfu;
    for(int i = 0; i<mGeneralFunctionalUnitList->length(); i++){
        genfu = mGeneralFunctionalUnitList->at(i);
        if(genfu->mCountDown == 0 && !genfu->mReservationStationList.isEmpty()){
            completedInstructions.append(genfu->mReservationStationList.first());
        }
    }
//    qDebug()<<"Completed Instructions For General Functional Units Gathered";

    if(completedInstructions.length()>0){
        ScriptInstruction* firstIssuedInst;
        for(int i = 0; i<mCommonDataBusFunctionalUnitList->length(); i++){

            // Find first issued instruction
            firstIssuedInst = completedInstructions.first();
            for(int j = 1; j<completedInstructions.length(); j++){
                if(completedInstructions.at(j)->mIssueClockCycle<firstIssuedInst->mIssueClockCycle){
                    firstIssuedInst = completedInstructions.at(j);
                }
            }
            // Remove first issued instruction from the completed instructions list
            completedInstructions.remove(completedInstructions.indexOf(firstIssuedInst));

            // Update common data bus with information
            mCommonDataBusFunctionalUnitList->at(i)->mBusy = true;
            mCommonDataBusFunctionalUnitList->at(i)->mFunctionalUnitWithClaim = firstIssuedInst->mDestinationRegister;
            mCommonDataBusFunctionalUnitList->at(i)->mScriptInstruction = firstIssuedInst;
            if(firstIssuedInst->mInstruction.mMemoryOptions==MemoryOptions::Load || firstIssuedInst->mInstruction.mMemoryOptions==MemoryOptions::Store){
                firstIssuedInst->mCurrentPipelineStage = PipelineStages::ReadWriteAccess;
                firstIssuedInst->mReadAccessClockCycle = mClockCycle;
            }
            else{
                firstIssuedInst->mWriteResultClockCycle = mClockCycle;
                firstIssuedInst->mCurrentPipelineStage = PipelineStages::ExecutionDone;
            }

            // Remove first issued instruction from the reservation list that it is in
            bool found = false;
            MemoryFunctionalUnit* memfu;
            for(int j = 0; j<mMemoryFunctionalUnitList->length(); j++){
                memfu = mMemoryFunctionalUnitList->at(j);
                if(!memfu->mReservationStationList.isEmpty() && memfu->mReservationStationList.first()==firstIssuedInst){
                    found = true;
                    undoRegisterDependencies(memfu);
                    memfu->mReservationStationList.remove(0);
                    memfu->mCountDown = -1;
                    memfu->mOperation = "";
                    memfu->mSourceOne = "";
                    memfu->mBusy = false;
                    break;
                }
            }
            if(!found){
                GeneralFunctionalUnit* genfu;
                for(int j = 0; j<mGeneralFunctionalUnitList->length(); j++){
                    genfu = mGeneralFunctionalUnitList->at(j);
                    if(!genfu->mReservationStationList.isEmpty() && genfu->mReservationStationList.first()==firstIssuedInst){
                        undoRegisterDependencies(genfu);
                        genfu->mReservationStationList.remove(0);
                        genfu->mCountDown = -1;
                        genfu->mOperation = "";
                        genfu->mSourceOne = "";
                        genfu->mSourceTwo = "";
                        genfu->mBusy = false;
                        break;
                    }
                }
            }
        }
    }
//    qDebug()<<"Completed Instructions Moved Into Common Data Bus";

    // Stage functional units
    updateFunctionalUnits();
//    qDebug()<<"Functional Units Updated";

    // Issue new instructions
    issueInstructions();
//    qDebug()<<"Instructions Issued";

    mClockCycle++;
    if(mScriptInstructionList->last()->mCurrentPipelineStage==PipelineStages::Done){
        this->runStatus = TomasuloRunStatus::Done;
    }
    emit StepDone();
}

void TomasuloAlgorithm::reset()
{
    this->runStatus = TomasuloRunStatus::NotStarted;
    this->mClockCycle = 1;
    this->mCurrentInstruction = 0;
}

unsigned int TomasuloAlgorithm::clockCycle() const
{
    return mClockCycle;
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
    emit UpdateRunStatus();
}

void TomasuloAlgorithm::updateFunctionalUnits() {

    GeneralFunctionalUnit* genfu;
    ScriptInstruction* instruct;
    for(int i = 0; i<mGeneralFunctionalUnitList->length(); i++){
        genfu = mGeneralFunctionalUnitList->at(i);
        if(genfu->mReservationStationList.isEmpty()){
            //qDebug()<<"Functional Unit "<<genfu->mFunctionalUnit.mName<<" Has An Empty Reservation Station List.";
            continue;
        }

        instruct = genfu->mReservationStationList.first();
        qDebug()<<"Do Dependencies Exist For "<<instruct->mInstructionWhole<<"? "<<doDependenciesExist(instruct);
        if(instruct->mCurrentPipelineStage == PipelineStages::Issue && !doDependenciesExist(instruct)){
            instruct->mCurrentPipelineStage = PipelineStages::Execution;
            instruct->mExecutionStartClockCycle = mClockCycle;
            genfu->mBusy = true;
            genfu->mCountDown = genfu->mFunctionalUnit.mLatency;
            genfu->mOperation = ToString(instruct->mInstruction.mInstructionType);
            genfu->mSourceOne = instruct->mSourceOneRegister;
            genfu->mSourceTwo = instruct->mSourceTwoRegister;
//            qDebug()<<"Instruction Stepped in Functional Unit: "<<instruct->mInstructionWhole<<" CountDown: "<<genfu->mCountDown;
        }
        else if(instruct->mCurrentPipelineStage == PipelineStages::Execution){
            genfu->mCountDown--;
            if(genfu->mCountDown==0){
                instruct->mExecutionCompletionClockCycle = mClockCycle;
            }
//            qDebug()<<"Instruction Stepped in Functional Unit: "<<instruct->mInstructionWhole<<" CountDown: "<<genfu->mCountDown;
        }
    }

    MemoryFunctionalUnit* memfu;
    for(int i = 0; i<mMemoryFunctionalUnitList->length(); i++){
        memfu = mMemoryFunctionalUnitList->at(i);
        if(memfu->mReservationStationList.isEmpty()){
            //qDebug()<<"Functional Unit "<<memfu->mFunctionalUnit.mName<<" Has An Empty Reservation Station List.";
            continue;
        }

        instruct = memfu->mReservationStationList.first();
        qDebug()<<"Do Dependencies Exist For "<<instruct->mInstructionWhole<<"? "<<doDependenciesExist(instruct);
        if(instruct->mCurrentPipelineStage == PipelineStages::Issue && !doDependenciesExist(instruct)){
            instruct->mCurrentPipelineStage = PipelineStages::Execution;
            instruct->mExecutionStartClockCycle = mClockCycle;
            memfu->mBusy = true;
            memfu->mCountDown = memfu->mFunctionalUnit.mLatency;
            memfu->mOperation = ToString(instruct->mInstruction.mInstructionType);
            memfu->mSourceOne = instruct->mSourceOneRegister+" + "+instruct->mSourceTwoRegister;
            qDebug()<<"Instruction Stepped in Functional Unit: "<<instruct->mInstructionWhole<<" CountDown: "<<memfu->mCountDown;
        }
        else if(instruct->mCurrentPipelineStage == PipelineStages::Execution){
            memfu->mCountDown--;
            if(memfu->mCountDown==0){
                instruct->mExecutionCompletionClockCycle = mClockCycle;
            }
            qDebug()<<"Instruction Stepped in Functional Unit: "<<instruct->mInstructionWhole<<" CountDown: "<<memfu->mCountDown;
        }
    }
}

void TomasuloAlgorithm::issueInstructions() {
    ScriptInstruction* instruct;
    for(int i = 0; i<mIssueNumber; i++){
        int issueIndex = getUnissuedInstructionIndex();
        if(issueIndex<0) break; // Means all instructions have been issued
        instruct = mScriptInstructionList->at(issueIndex);
        GeneralFunctionalUnit* genfu = getOptimalGeneralFunctionalUnit(instruct);
        if(genfu!=nullptr){
            genfu->mReservationStationList.append(instruct);
            instruct->mIssueClockCycle = mClockCycle;
            instruct->mCurrentPipelineStage = PipelineStages::Issue;
            mCurrentInstruction++;
            setDependencies(genfu, instruct);
//            qDebug()<<"Instruction Issued: "<<instruct->mInstructionWhole<<" to "<<genfu->mFunctionalUnit.mName;
        }
        else{
            MemoryFunctionalUnit* memfu = getOptimalMemoryFunctionalUnit(instruct);
            if(memfu!=nullptr){
                memfu->mReservationStationList.append(instruct);
                instruct->mIssueClockCycle = mClockCycle;
                instruct->mCurrentPipelineStage = PipelineStages::Issue;
                mCurrentInstruction++;
                setDependencies(memfu, instruct);
//                qDebug()<<"Instruction Issued: "<<instruct->mInstructionWhole<<" to "<<memfu->mFunctionalUnit.mName;
            }
        }
    }
}

bool TomasuloAlgorithm::doDependenciesExist(ScriptInstruction *ins)
{
    qDebug()<<"Do Deendencies Exist For Instruction "<<ins->mInstructionWhole<<"?";
    int len = mRegisterFunctionalUnitList->length();
    RegisterFunctionalUnit* rUnit;
    for(int i = 0; i<len; i++){
        rUnit = mRegisterFunctionalUnitList->at(i);
        if(rUnit->mInstruction!=nullptr){
            qDebug()<<"Register: "<<rUnit->mFunctionalUnit.mName;
            qDebug()<<"Register Instruction Address: "<<rUnit->mInstruction;
            qDebug()<<"Instruction Source One Register: "<<ins->mSourceOneRegister;
            qDebug()<<"Instruction Source Two Register: "<<ins->mSourceTwoRegister;
            qDebug()<<"Register FunctionalUnitWithClaim: "<<rUnit->mFunctionalUnitWithClaim;
            qDebug()<<"Register Instruction Issue Cycle: "<<rUnit->mInstruction->mIssueClockCycle;
            qDebug()<<"Instruction Issue Cycle: "<<ins->mIssueClockCycle;
        }
        else{
            //qDebug()<<"Register "<<rUnit->mFunctionalUnit.mName<<" Has No Instruction";
        }
        if(rUnit->mInstruction!=nullptr && ins->mSourceOneRegister == rUnit->mFunctionalUnit.mName && !rUnit->mFunctionalUnitWithClaim.isEmpty() && rUnit->mInstruction->mIssueClockCycle < ins->mIssueClockCycle){
            return true;
        }
        if(rUnit->mInstruction!=nullptr && ins->mSourceTwoRegister == rUnit->mFunctionalUnit.mName && !rUnit->mFunctionalUnitWithClaim.isEmpty() && rUnit->mInstruction->mIssueClockCycle < ins->mIssueClockCycle){
            return true;
        }
    }

    return false;
}

void TomasuloAlgorithm::setDependencies(GeneralFunctionalUnit* genfu, ScriptInstruction *instruct)
{
    for(int i = 0; i<mRegisterFunctionalUnitList->length(); i++){
        if(mRegisterFunctionalUnitList->at(i)->mFunctionalUnit.mName==instruct->mDestinationRegister){
            mRegisterFunctionalUnitList->at(i)->mInstruction = instruct;
            mRegisterFunctionalUnitList->at(i)->mFunctionalUnitWithClaim = genfu->mFunctionalUnit.mName;
            break;
        }
    }
}

void TomasuloAlgorithm::setDependencies(MemoryFunctionalUnit *memfu, ScriptInstruction *instruct)
{
    for(int i = 0; i<mRegisterFunctionalUnitList->length(); i++){
        if(mRegisterFunctionalUnitList->at(i)->mFunctionalUnit.mName==instruct->mDestinationRegister){
            mRegisterFunctionalUnitList->at(i)->mInstruction = instruct;
            mRegisterFunctionalUnitList->at(i)->mFunctionalUnitWithClaim = memfu->mFunctionalUnit.mName;
            break;
        }
    }
}

void TomasuloAlgorithm::undoRegisterDependencies(GeneralFunctionalUnit *genfu)
{
    for(int i = 0; i<mRegisterFunctionalUnitList->length(); i++){
        if(mRegisterFunctionalUnitList->at(i)->mFunctionalUnitWithClaim==genfu->mFunctionalUnit.mName){
            mRegisterFunctionalUnitList->at(i)->mFunctionalUnitWithClaim = "";
            break;
        }
    }
}

void TomasuloAlgorithm::undoRegisterDependencies(MemoryFunctionalUnit *memfu)
{
    for(int i = 0; i<mRegisterFunctionalUnitList->length(); i++){
        if(mRegisterFunctionalUnitList->at(i)->mFunctionalUnitWithClaim==memfu->mFunctionalUnit.mName){
            mRegisterFunctionalUnitList->at(i)->mFunctionalUnitWithClaim = "";
            break;
        }
    }
}

void TomasuloAlgorithm::undoCommonDataBusDependencies(ScriptInstruction *instruct)
{
    for(int i = 0; i<mCommonDataBusFunctionalUnitList->length(); i++){
        if(mCommonDataBusFunctionalUnitList->at(i)->mFunctionalUnitWithClaim==instruct->mDestinationRegister){
            mCommonDataBusFunctionalUnitList->at(i)->mFunctionalUnitWithClaim = "";
            break;
        }
    }
}

int TomasuloAlgorithm::getUnissuedInstructionIndex()
{
    for(int i = 0; i<mScriptInstructionList->length(); i++){
        if(mScriptInstructionList->at(i)->mCurrentPipelineStage==PipelineStages::None) return i;
    }
    return -1;
}

GeneralFunctionalUnit *TomasuloAlgorithm::getOptimalGeneralFunctionalUnit(ScriptInstruction *ins)
{
    GeneralFunctionalUnit* retFu = nullptr;

    // Get list of genral functional units that can process this instruction AND have room in their reservation list
    GeneralFunctionalUnit* holdFu;
    QList<GeneralFunctionalUnit*> capableFuList;
    for(int i = 0; i<mGeneralFunctionalUnitList->length(); i++){
        holdFu = mGeneralFunctionalUnitList->at(i);
        if(ins->mInstruction.mArithmeticOptions!=ArithmeticOptions::None &&
           IsOfArithmeticOptions(&(holdFu->mFunctionalUnit),ins->mInstruction.mArithmeticOptions) &&
           holdFu->mReservationStationList.length() < holdFu->mFunctionalUnit.mReservationStationCount+1){
            capableFuList.append(holdFu);
        }
    }

    if(capableFuList.isEmpty()){
        return retFu;
    }

    // Find functional unit in list that has the fewest instructions in the reservation station list
    for(int i = capableFuList.length()-2; i>=0; i--){
        if(capableFuList.at(i)->mReservationStationList.length()>capableFuList.last()->mReservationStationList.length()){
            capableFuList.remove(i);
        }
        else{
            capableFuList.remove(capableFuList.length()-1);
        }
    }

    if(capableFuList.length()==1){
        retFu = capableFuList.first();
    }

    return retFu;
}

MemoryFunctionalUnit *TomasuloAlgorithm::getOptimalMemoryFunctionalUnit(ScriptInstruction *ins)
{
    MemoryFunctionalUnit* retFu = nullptr;

    // Get list of genral functional units that can process this instruction AND have room in their reservation list
    MemoryFunctionalUnit* holdFu;
    QList<MemoryFunctionalUnit*> capableFuList;
    for(int i = 0; i<mMemoryFunctionalUnitList->length(); i++){
        holdFu = mMemoryFunctionalUnitList->at(i);
        if(ins->mInstruction.mMemoryOptions!=MemoryOptions::None &&
           IsOfMemoryOptions(&(holdFu->mFunctionalUnit),ins->mInstruction.mMemoryOptions) &&
           holdFu->mReservationStationList.length() < holdFu->mFunctionalUnit.mReservationStationCount+1){
            capableFuList.append(holdFu);
        }
    }

    if(capableFuList.isEmpty()){
        return retFu;
    }

    // Find functional unit in list that has the fewest instructions in the reservation station list
    for(int i = capableFuList.length()-2; i>=0; i--){
        if(capableFuList.at(i)->mReservationStationList.length()>capableFuList.last()->mReservationStationList.length()){
            capableFuList.remove(i);
        }
        else{
            capableFuList.remove(capableFuList.length()-1);
        }
    }

    if(capableFuList.length()==1){
        retFu = capableFuList.first();
    }

    return retFu;
}
