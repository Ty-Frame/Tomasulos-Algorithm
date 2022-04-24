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
     * Process Instructions trying to commit
     *  - Get first issued one and move it to commited stage and set commit cycle
     * Process Instructions trying to get into a common data bus
     *  - Empty data buses
     *  - Move pending instructions into common data bus based on first issued
     */

    QList<ScriptInstruction*> toBeIssued,waitingToStartExecution,currentlyExecuting,doneExecuting,doneReadOrWrite,exitingCdb,commitPending,dontCareBecuaseDone;
    ScriptInstruction* instruction;
    for(int i = 0; i<mScriptInstructionList->length(); i++){
        instruction = mScriptInstructionList->at(i);
        switch (instruction->mCurrentPipelineStage) {
        case PipelineStages::None:
            toBeIssued.append(instruction);
            break;
        case PipelineStages::Issued:
            waitingToStartExecution.append(instruction);
            break;
        case PipelineStages::Execution:
            currentlyExecuting.append(instruction);
            break;
        case PipelineStages::ExecutionDone:
            doneExecuting.append(instruction);
            break;
        case PipelineStages::ReadWriteAccess:
            doneReadOrWrite.append(instruction);
            break;
        case PipelineStages::Writeback:
            exitingCdb.append(instruction);
            break;
        case PipelineStages::WaitingToCommit:
            commitPending.append(instruction);
            break;
        case PipelineStages::Commited:
            dontCareBecuaseDone.append(instruction);
            break;
        }
    }
    qDebug()<<"----------------------Clock Cycle: "<<mClockCycle;
//    qDebug()<<"Total: "<<mScriptInstructionList->length()<<", Sum: "<<(toBeIssued.length()+
//                                                                       waitingToStartExecution.length()+
//                                                                       currentlyExecuting.length()+
//                                                                       doneExecuting.length()+
//                                                                       doneReadOrWrite.length()+
//                                                                       exitingCdb.length()+
//                                                                       commitPending.length()+
//                                                                       dontCareBecuaseDone.length());
//    qDebug()<<"to be issued "<<toBeIssued.length();
//    for(int i = 0; i<toBeIssued.length(); i++){
//        qDebug()<<toBeIssued.at(i)->mInstructionWhole;
//    }
//    qDebug()<<"";
//    qDebug()<<"waiting to start execution "<<waitingToStartExecution.length();
//    for(int i = 0; i<waitingToStartExecution.length(); i++){
//        qDebug()<<waitingToStartExecution.at(i)->mInstructionWhole;
//    }
//    qDebug()<<"";
//    qDebug()<<"currently executing "<<currentlyExecuting.length();
//    for(int i = 0; i<currentlyExecuting.length(); i++){
//        qDebug()<<currentlyExecuting.at(i)->mInstructionWhole;
//    }
//    qDebug()<<"";
//    qDebug()<<"done executing "<<doneExecuting.length();
//    for(int i = 0; i<doneExecuting.length(); i++){
//        qDebug()<<doneExecuting.at(i)->mInstructionWhole;
//    }
//    qDebug()<<"";
//    qDebug()<<"done read write "<<doneReadOrWrite.length();
//    for(int i = 0; i<doneReadOrWrite.length(); i++){
//        qDebug()<<doneReadOrWrite.at(i)->mInstructionWhole;
//    }
//    qDebug()<<"";
//    qDebug()<<"exiting cdb "<<exitingCdb.length();
//    for(int i = 0; i<exitingCdb.length(); i++){
//        qDebug()<<exitingCdb.at(i)->mInstructionWhole;
//    }
//    qDebug()<<"";
//    qDebug()<<"commit pending "<<commitPending.length();
//    for(int i = 0; i<commitPending.length(); i++){
//        qDebug()<<commitPending.at(i)->mInstructionWhole;
//    }
//    qDebug()<<"";
//    qDebug()<<"done "<<dontCareBecuaseDone.length();
//    for(int i = 0; i<dontCareBecuaseDone.length(); i++){
//        qDebug()<<dontCareBecuaseDone.at(i)->mInstructionWhole;
//    }
//    qDebug()<<"";

//    QList<Instruction*> committedThisCycle;

    // Process Instructions trying to commit
//    qDebug()<<"Very start of algorithm";
    processCommitPending(&commitPending);

    // Process instructions exiting cdb
    processExitingCdb(&exitingCdb);

    // Move any stores that are in read write to committed
    processDoneReadOrWrite(&doneReadOrWrite);

    // Process Instructions trying to get into a common data bus (instructions in readwrite OR are not memory instructions AND are done executing)
    QList<ScriptInstruction*> readWriteOrDoneExecutingAndNotMemoryInstructions;
    readWriteOrDoneExecutingAndNotMemoryInstructions.append(doneReadOrWrite);
    for(int i = 0; i<doneExecuting.length(); i++){
        instruction = doneExecuting.at(i);
        if(instruction->mInstruction.mInstructionType!=InstructionType::Memory){
            readWriteOrDoneExecutingAndNotMemoryInstructions.append(instruction);
            doneExecuting.removeAt(i);
            i--;
        }
    }
//    readWriteOrDoneExecutingAndNotMemoryInstructions.append(doneExecuting);

    processDoneExecuting(&readWriteOrDoneExecutingAndNotMemoryInstructions);

    // Process done executing memory related instructions
    for(int i = 0; i<doneExecuting.length(); i++){
//        instruction = findFirstIssued(&doneExecuting);
//        if(instruction==nullptr){
//            break;
//        }
        instruction = doneExecuting.at(i);

        clearInstructionFromMemFU(instruction);
        if(instruction->mInstruction.mMemoryOptions==MemoryOptions::Store){
            if(!doStoreDependenciesExist(instruction)){
                instruction->mCurrentPipelineStage=PipelineStages::WaitingToCommit;
                instruction->mReadAccessClockCycle=mClockCycle;
            }
            else{
                instruction->mCurrentPipelineStage=PipelineStages::ExecutionDone;
                instruction->mReadAccessClockCycle=-1;
            }
        }
    }
//    qDebug()<<"done moving memory related instructions to readwrite.";

    // Process currently executing instructions
    processCurrentlyExecuting(&currentlyExecuting);

    // Process instructions waiting to start execution
    processWaitingToStartExecution(&waitingToStartExecution);


    processToBeIssued(&toBeIssued);

    for(int i = 0; i<mScriptInstructionList->length(); i++){
        instruction = mScriptInstructionList->at(i);
        if(instruction->mInstruction.mMemoryOptions==MemoryOptions::Store && instruction->mCurrentPipelineStage==PipelineStages::ExecutionDone){

        }
    }
    qDebug()<<"";

    mClockCycle++;
    if(mScriptInstructionList->length()==dontCareBecuaseDone.length()){
        this->setRunStatus(TomasuloRunStatus::Done);
    }
    emit StepDone();
}

void TomasuloAlgorithm::processToBeIssued(QList<ScriptInstruction *> *toBeIssued)
{
    GeneralFunctionalUnit* genfu;
    MemoryFunctionalUnit* memfu;
    ScriptInstruction* instruction;
    bool successfulIssue = false;
    int len;
    if(mIssueNumber<toBeIssued->length()){
        len = mIssueNumber;
    }
    else{
        len = toBeIssued->length();
    }
    for(int i = 0; i<len; i++){
        instruction = toBeIssued->at(i);
//        qDebug()<<"attempting to issue "<<instruction->mInstructionWhole;
        genfu = getOptimalGeneralFunctionalUnit(instruction);
        if(genfu!=nullptr){
//            setDependencies(genfu, instruction);
            genfu->mReservationStationList.append(instruction);
            instruction->mIssueClockCycle = mClockCycle;
            instruction->mCurrentPipelineStage = PipelineStages::Issued;
            successfulIssue = true;
//            qDebug()<<instruction->mInstructionWhole<<" issued to "<<genfu->mFunctionalUnit.mName<<" at clock cycle "<<mClockCycle;
        }
        else{
            memfu = getOptimalMemoryFunctionalUnit(instruction);
            if(memfu!=nullptr){
//                setDependencies(memfu, instruction);
                memfu->mReservationStationList.append(instruction);
                instruction->mIssueClockCycle = mClockCycle;
                instruction->mCurrentPipelineStage = PipelineStages::Issued;
                successfulIssue = true;
//                qDebug()<<instruction->mInstructionWhole<<" issued to "<<memfu->mFunctionalUnit.mName<<" at clock cycle "<<mClockCycle;
            }
        }

        if(!successfulIssue){
            qDebug()<<"No Successful Issue for "<<instruction->mInstructionWhole;
            break;
        }
    }
//    qDebug()<<"done processing isntructions waiting to be issued";
}

void TomasuloAlgorithm::processWaitingToStartExecution(QList<ScriptInstruction *> *waitingToStartExecution)
{
    ScriptInstruction* instruction;
    GeneralFunctionalUnit* genfu;
    MemoryFunctionalUnit*memfu;
    for(int i = 0; i<waitingToStartExecution->length(); i++){
        instruction = waitingToStartExecution->at(i);
        bool found = false;
        for(int j = 0; j<mGeneralFunctionalUnitList->length(); j++){
            genfu = mGeneralFunctionalUnitList->at(j);
            if(!genfu->mReservationStationList.isEmpty() && genfu->mReservationStationList.contains(instruction)){
                found = true;
                if(genfu->mReservationStationList.first()==instruction && !doDependenciesExist(instruction)){
                    genfu->mBusy = true;
                    genfu->mCountDown = genfu->mFunctionalUnit.mLatency - 1;
                    genfu->mOperation = ToString(instruction->mInstruction.mArithmeticOptions);
                    genfu->mSourceOne = instruction->mSourceOneRegister;
                    genfu->mSourceTwo = instruction->mSourceTwoRegister;
                    setDependencies(genfu, instruction);
                    instruction->mCurrentPipelineStage = PipelineStages::Execution;
                    instruction->mExecutionStartClockCycle = mClockCycle;
                    if(genfu->mCountDown==0){
                        genfu->mCountDown--;
                        instruction->mCurrentPipelineStage = PipelineStages::ExecutionDone;
                        instruction->mExecutionCompletionClockCycle = mClockCycle;
                    }
                    else{
                        genfu->mCountDown--;
                    }
//                    qDebug()<<instruction->mInstructionWhole<<" started execution in "<<genfu->mFunctionalUnit.mName<<" at clock cycle "<<mClockCycle;
                }
            }
        }
        if(!found){
            for(int j = 0; j<mMemoryFunctionalUnitList->length(); j++){
                memfu = mMemoryFunctionalUnitList->at(j);
                if(!memfu->mReservationStationList.isEmpty() && memfu->mReservationStationList.contains(instruction)){
                    if(memfu->mReservationStationList.first()==instruction && !doDependenciesExist(instruction)){
                        found = true;
                        memfu->mBusy = true;
                        memfu->mCountDown = memfu->mFunctionalUnit.mLatency - 1;
                        memfu->mOperation = ToString(instruction->mInstruction.mMemoryOptions);
                        memfu->mSourceOne = instruction->mSourceOneRegister+" + "+instruction->mSourceTwoRegister;
                        setDependencies(memfu, instruction);
                        instruction->mCurrentPipelineStage = PipelineStages::Execution;
                        instruction->mExecutionStartClockCycle = mClockCycle;
                        if(memfu->mCountDown==0){
                            memfu->mCountDown--;
                            instruction->mCurrentPipelineStage = PipelineStages::ExecutionDone;
                            instruction->mExecutionCompletionClockCycle = mClockCycle;
                        }
                        else{
                            memfu->mCountDown--;
                        }
//                        qDebug()<<instruction->mInstructionWhole<<" started execution in "<<memfu->mFunctionalUnit.mName<<" at clock cycle "<<mClockCycle;
                    }
                }
            }
        }
        if(!found){
            qDebug()<<"Functional unit could not be found for "<<instruction->mInstructionWhole;
        }
    }
//    qDebug()<<"done processing instructions waiting to start executing";
}

void TomasuloAlgorithm::processCurrentlyExecuting(QList<ScriptInstruction *> *currentlyExecuting)
{
    ScriptInstruction* instruction;
    GeneralFunctionalUnit* genfu;
    MemoryFunctionalUnit* memfu;
    for(int i = 0; i<currentlyExecuting->length(); i++){
        instruction = currentlyExecuting->at(i);
        if(instruction==nullptr){
            break;
        }
        bool found = false;
        for(int j = 0; j<mGeneralFunctionalUnitList->length(); j++){
            genfu = mGeneralFunctionalUnitList->at(j);
            if(!genfu->mReservationStationList.isEmpty() && genfu->mReservationStationList.first()->mInstructionWhole == instruction->mInstructionWhole){
                found = true;
                if(genfu->mCountDown==0){
                    instruction->mExecutionCompletionClockCycle = mClockCycle;
                    instruction->mCurrentPipelineStage = PipelineStages::ExecutionDone;
//                    clearInstructionFromGenFU(instruction);
//                    qDebug()<<instruction->mInstructionWhole<<" done executing in "<<genfu->mFunctionalUnit.mName<<" at clock cycle "<<mClockCycle;
                }
                genfu->mCountDown--;
            }
        }
        if(!found){
            for(int j = 0; j<mMemoryFunctionalUnitList->length(); j++){
                memfu = mMemoryFunctionalUnitList->at(j);
                if(!memfu->mReservationStationList.isEmpty() && memfu->mReservationStationList.first()->mInstructionWhole == instruction->mInstructionWhole){
                    if(memfu->mCountDown==0){
                        instruction->mExecutionCompletionClockCycle = mClockCycle;
                        instruction->mCurrentPipelineStage = PipelineStages::ExecutionDone;
//                        clearInstructionFromMemFU(instruction);
//                        qDebug()<<instruction->mInstructionWhole<<" done executing in "<<memfu->mFunctionalUnit.mName<<" at clock cycle "<<mClockCycle;
                    }
                    memfu->mCountDown--;
                }
            }
        }
    }
//    qDebug()<<"done processing currecntly executing instructions";
}

void TomasuloAlgorithm::processDoneExecuting(QList<ScriptInstruction *> *doneExecuting)
{
    ScriptInstruction* instruction;
    CommonDataBusFunctionalUnit* cdb;
    for(int i = 0; i<mCommonDataBusFunctionalUnitList->length(); i++){
        instruction = findFirstIssued(doneExecuting);
        if(instruction==nullptr){
            break;
        }
        if(instruction->mInstruction.mInstructionType!=InstructionType::Memory){
            clearInstructionFromGenFU(instruction);
        }

        if(instruction->mInstruction.mInstructionType==InstructionType::Branch){
            instruction->mCurrentPipelineStage = PipelineStages::WaitingToCommit;
            doneExecuting->remove(doneExecuting->indexOf(instruction));
            i--;
            continue;
        }

        cdb = mCommonDataBusFunctionalUnitList->at(i);
        cdb->mBusy = true;
        cdb->mFunctionalUnitWithClaim = instruction->mDestinationRegister;
        cdb->mScriptInstruction = instruction;
        instruction->mCurrentPipelineStage = PipelineStages::WaitingToCommit;
        instruction->mWriteResultClockCycle = mClockCycle;
        qDebug()<<instruction->mInstructionWhole<<" passed through  "<<cdb->mFunctionalUnit.mName<<" and moved to waiting to commit at clock cycle "<<mClockCycle;
    }
//    qDebug()<<"Done processing instructions trying to get into cdb.";
}

void TomasuloAlgorithm::processDoneReadOrWrite(QList<ScriptInstruction *> *doneReadOrWrite)
{
    ScriptInstruction* instruction;
    for(int i = 0; i<doneReadOrWrite->length(); i++){
        instruction = doneReadOrWrite->at(i);
        if(instruction->mInstruction.mMemoryOptions==MemoryOptions::Store){
            instruction->mCurrentPipelineStage = PipelineStages::WaitingToCommit;
            doneReadOrWrite->removeAt(i);
            i--;
        }
    }
}

void TomasuloAlgorithm::processExitingCdb(QList<ScriptInstruction *> *exitingCdb)
{
    CommonDataBusFunctionalUnit* cdb;
    ScriptInstruction* instruction;
    for(int i = 0; i<exitingCdb->length(); i++){
        instruction = exitingCdb->at(i);
        instruction->mWriteResultClockCycle = mClockCycle;
        instruction->mCurrentPipelineStage = PipelineStages::WaitingToCommit;
        for(int j = 0; j<mCommonDataBusFunctionalUnitList->length(); j++){
            cdb = mCommonDataBusFunctionalUnitList->at(j);
            if(cdb->mBusy){
                cdb->mBusy = false;
                cdb->mFunctionalUnitWithClaim = "";
                cdb->mScriptInstruction = nullptr;
            }
        }
    }
//    qDebug()<<"Done processing isntructions exiting cdb.";
}

void TomasuloAlgorithm::processCommitPending(QList<ScriptInstruction *> *commitPending)
{
    ScriptInstruction* instruction = findFirstIssued(commitPending);
    if(instruction!=nullptr){
        for(int i = 0; i<mScriptInstructionList->length(); i++){
            if(instruction==mScriptInstructionList->at(i) && (i==0 || mScriptInstructionList->at(i-1)->mCurrentPipelineStage==PipelineStages::Commited)){
                instruction->mCommitClockCycle = mClockCycle;
                instruction->mCurrentPipelineStage = PipelineStages::Commited;
                qDebug()<<instruction->mInstructionWhole<<" committed at clock cycle "<<mClockCycle<<".";
                break;
            }
        }
    }
    for(int i = 0; i<commitPending->length(); i++){
        qDebug()<<"commit pending inst: "<<commitPending->at(i)->mInstructionWhole<<" at cc: "<<mClockCycle;
        instruction = commitPending->at(i);
        undoCommonDataBusDependencies(instruction);
        undoRegisterDependencies(instruction);
    }
    //    qDebug()<<"done processing instructions trying to commit.";
}

void TomasuloAlgorithm::clearInstructionFromGenFU(ScriptInstruction *instruction)
{
    GeneralFunctionalUnit* genfu;
    for(int j = 0; j<mGeneralFunctionalUnitList->length(); j++){
        genfu = mGeneralFunctionalUnitList->at(j);
        if(!genfu->mReservationStationList.isEmpty() && genfu->mReservationStationList.first()->mInstructionWhole==instruction->mInstructionWhole){
            genfu->mBusy = false;
            genfu->mOperation = "";
            genfu->mSourceOne = "";
            genfu->mSourceTwo = "";
            genfu->mReservationStationList.removeFirst();
        }
    }
}

void TomasuloAlgorithm::clearInstructionFromMemFU(ScriptInstruction *instruction)
{
    MemoryFunctionalUnit* memfu;
    for(int j = 0; j<mMemoryFunctionalUnitList->length(); j++){
        memfu = mMemoryFunctionalUnitList->at(j);
        if(!memfu->mReservationStationList.isEmpty() && memfu->mReservationStationList.first()==instruction){
            memfu->mBusy = false;
            memfu->mOperation = "";
            memfu->mSourceOne = "";
            memfu->mReservationStationList.removeFirst();
            instruction->mCurrentPipelineStage = PipelineStages::ReadWriteAccess;
            instruction->mReadAccessClockCycle = mClockCycle;
//                qDebug()<<instruction->mInstructionWhole<<" done executing in "<<memfu->mFunctionalUnit.mName<<" and moved to read write at clock cycle "<<mClockCycle;
        }
    }
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
//        qDebug()<<"Do Dependencies Exist For "<<instruct->mInstructionWhole<<"? "<<doDependenciesExist(instruct);
        if(instruct->mCurrentPipelineStage == PipelineStages::Issued && !doDependenciesExist(instruct)){
            instruct->mCurrentPipelineStage = PipelineStages::Execution;
            instruct->mExecutionStartClockCycle = mClockCycle;
            genfu->mBusy = true;
            genfu->mCountDown = genfu->mFunctionalUnit.mLatency - 1;
            if(genfu->mCountDown==0){
                instruct->mExecutionCompletionClockCycle = mClockCycle;
                instruct->mCurrentPipelineStage = PipelineStages::ExecutionDone;
            }
            genfu->mOperation = ToString(instruct->mInstruction.mInstructionType);
            genfu->mSourceOne = instruct->mSourceOneRegister;
            genfu->mSourceTwo = instruct->mSourceTwoRegister;
            setDependencies(genfu, instruct);
//            qDebug()<<"Instruction Stepped in Functional Unit: "<<instruct->mInstructionWhole<<" CountDown: "<<genfu->mCountDown;
        }
        else if(instruct->mCurrentPipelineStage == PipelineStages::Execution){
            genfu->mCountDown--;
            if(genfu->mCountDown==0){
                instruct->mExecutionCompletionClockCycle = mClockCycle;
                instruct->mCurrentPipelineStage = PipelineStages::ExecutionDone;
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
//        qDebug()<<"Do Dependencies Exist For "<<instruct->mInstructionWhole<<"? "<<doDependenciesExist(instruct);
        if(instruct->mCurrentPipelineStage == PipelineStages::Issued && !doDependenciesExist(instruct)){
            instruct->mCurrentPipelineStage = PipelineStages::Execution;
            instruct->mExecutionStartClockCycle = mClockCycle;
            memfu->mBusy = true;
            memfu->mCountDown = memfu->mFunctionalUnit.mLatency - 1;
            if(memfu->mCountDown==0){
                instruct->mExecutionCompletionClockCycle = mClockCycle;
                instruct->mCurrentPipelineStage = PipelineStages::ExecutionDone;
            }
            memfu->mOperation = ToString(instruct->mInstruction.mInstructionType);
            memfu->mSourceOne = instruct->mSourceOneRegister+" + "+instruct->mSourceTwoRegister;
            setDependencies(memfu, instruct);
//            qDebug()<<"Instruction Stepped in Functional Unit: "<<instruct->mInstructionWhole<<" CountDown: "<<memfu->mCountDown;
        }
        else if(instruct->mCurrentPipelineStage == PipelineStages::Execution){
            memfu->mCountDown--;
            if(memfu->mCountDown==0){
                instruct->mExecutionCompletionClockCycle = mClockCycle;
                instruct->mCurrentPipelineStage = PipelineStages::ExecutionDone;
            }
//            qDebug()<<"Instruction Stepped in Functional Unit: "<<instruct->mInstructionWhole<<" CountDown: "<<memfu->mCountDown;
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
            instruct->mIssueIndex = i;
            instruct->mCurrentPipelineStage = PipelineStages::Issued;
            mCurrentInstruction++;
//            setDependencies(genfu, instruct);
            qDebug()<<"Instruction Issued: "<<instruct->mInstructionWhole<<" to "<<genfu->mFunctionalUnit.mName;
        }
        else{
            MemoryFunctionalUnit* memfu = getOptimalMemoryFunctionalUnit(instruct);
            if(memfu!=nullptr){
                memfu->mReservationStationList.append(instruct);
                instruct->mIssueClockCycle = mClockCycle;
                instruct->mIssueIndex = i;
                instruct->mCurrentPipelineStage = PipelineStages::Issued;
                mCurrentInstruction++;
//                setDependencies(memfu, instruct);
                qDebug()<<"Instruction Issued: "<<instruct->mInstructionWhole<<" to "<<memfu->mFunctionalUnit.mName;
            }
        }
    }
}

bool TomasuloAlgorithm::doDependenciesExist(ScriptInstruction *ins)
{
    bool sOneFound, sTwoFound = false;
    ScriptInstruction* beforeInstruction;
    for(int i = mScriptInstructionList->indexOf(ins)-1; i>=0; i--){
        beforeInstruction = mScriptInstructionList->at(i);
        if(beforeInstruction->mDestinationRegister == ins->mSourceOneRegister || beforeInstruction->mDestinationRegister == ins->mSourceTwoRegister){
            if(beforeInstruction->mDestinationRegister==ins->mSourceOneRegister){
                sOneFound = true;
            }
            else{
                sTwoFound = true;
            }
            switch(beforeInstruction->mInstruction.mInstructionType){
            case InstructionType::Arithmetic:
                if(beforeInstruction->mIssueClockCycle>0 && (beforeInstruction->mWriteResultClockCycle<0 || beforeInstruction->mWriteResultClockCycle==mClockCycle)){
                    qDebug()<<"dependency found for "<<ins->mInstructionWhole<<" at "<<beforeInstruction->mInstructionWhole;
                    return true;
                }
                break;
            case InstructionType::Memory:
                if(beforeInstruction->mInstruction.mMemoryOptions==MemoryOptions::Load){
                    if(beforeInstruction->mIssueClockCycle>0 && (beforeInstruction->mWriteResultClockCycle<0 || beforeInstruction->mWriteResultClockCycle==mClockCycle)){
                        qDebug()<<"dependency found for "<<ins->mInstructionWhole<<" at "<<beforeInstruction->mInstructionWhole;
                        return true;
                    }
                }
                else{
                    if(beforeInstruction->mIssueClockCycle>0 && (beforeInstruction->mReadAccessClockCycle<0 || beforeInstruction->mReadAccessClockCycle==mClockCycle)){
                        qDebug()<<"dependency found for "<<ins->mInstructionWhole<<" at "<<beforeInstruction->mInstructionWhole;
                        return true;
                    }
                }
                break;
            case InstructionType::Branch:
//                if(beforeInstruction->mCurrentPipelineStage==PipelineStages::Execution){
//                   qDebug()<<"dependency found for "<<ins->mInstructionWhole<<" at "<<beforeInstruction->mInstructionWhole;
//                   return true;
//                }
                break;
            default:
                qDebug()<<"ERROR IN DODEPENDENCIESEXIST";
                return false;
            }
            if(sOneFound && sTwoFound){
                return false;
            }
        }
    }

    return false;
}

bool TomasuloAlgorithm::doStoreDependenciesExist(ScriptInstruction *ins)
{
    bool sOneFound, sTwoFound = false;
    ScriptInstruction* beforeInstruction;
    for(int i = mScriptInstructionList->indexOf(ins)-1; i>=0; i--){
        beforeInstruction = mScriptInstructionList->at(i);
        if(beforeInstruction->mDestinationRegister == ins->mSourceOneRegister || beforeInstruction->mDestinationRegister == ins->mDestinationRegister){
            if(beforeInstruction->mDestinationRegister==ins->mSourceOneRegister){
                sOneFound = true;
            }
            else{
                sTwoFound = true;
            }
            switch(beforeInstruction->mInstruction.mInstructionType){
            case InstructionType::Arithmetic:
                if(beforeInstruction->mIssueClockCycle>0 && (beforeInstruction->mWriteResultClockCycle<0/* || beforeInstruction->mWriteResultClockCycle==mClockCycle*/)){
                    qDebug()<<"dependency found for "<<ins->mInstructionWhole<<" at "<<beforeInstruction->mInstructionWhole;
                    return true;
                }
                break;
            case InstructionType::Memory:
                if(beforeInstruction->mInstruction.mMemoryOptions==MemoryOptions::Load){
                    if(beforeInstruction->mIssueClockCycle>0 && (beforeInstruction->mWriteResultClockCycle<0/* || beforeInstruction->mWriteResultClockCycle==mClockCycle*/)){
                        qDebug()<<"dependency found for "<<ins->mInstructionWhole<<" at "<<beforeInstruction->mInstructionWhole;
                        return true;
                    }
                }
                else{
                    if(beforeInstruction->mIssueClockCycle>0 && (beforeInstruction->mReadAccessClockCycle<0/* || beforeInstruction->mReadAccessClockCycle==mClockCycle*/)){
                        qDebug()<<"dependency found for "<<ins->mInstructionWhole<<" at "<<beforeInstruction->mInstructionWhole;
                        return true;
                    }
                }
                break;
            case InstructionType::Branch:
//                if(beforeInstruction->mCurrentPipelineStage==PipelineStages::Execution){
//                   qDebug()<<"dependency found for "<<ins->mInstructionWhole<<" at "<<beforeInstruction->mInstructionWhole;
//                   return true;
//                }
                break;
            default:
                qDebug()<<"ERROR IN DODEPENDENCIESEXIST";
                return false;
            }
            if(sOneFound && sTwoFound){
                return false;
            }
        }
    }

    return false;
}

void TomasuloAlgorithm::setDependencies(GeneralFunctionalUnit* genfu, ScriptInstruction *instruct)
{
    for(int i = 0; i<mRegisterFunctionalUnitList->length(); i++){
        if(mRegisterFunctionalUnitList->at(i)->mFunctionalUnit.mName==instruct->mDestinationRegister && !(mRegisterFunctionalUnitList->at(i)->mInstruction.contains(instruct))){
            mRegisterFunctionalUnitList->at(i)->mInstruction.append(instruct);
            mRegisterFunctionalUnitList->at(i)->mFunctionalUnitWithClaim.append(genfu->mFunctionalUnit.mName);
            return;
        }
    }
}

void TomasuloAlgorithm::setDependencies(MemoryFunctionalUnit *memfu, ScriptInstruction *instruct)
{
    for(int i = 0; i<mRegisterFunctionalUnitList->length(); i++){
        if(mRegisterFunctionalUnitList->at(i)->mFunctionalUnit.mName==instruct->mDestinationRegister && !(mRegisterFunctionalUnitList->at(i)->mInstruction.contains(instruct))){
            mRegisterFunctionalUnitList->at(i)->mInstruction.append(instruct);
            mRegisterFunctionalUnitList->at(i)->mFunctionalUnitWithClaim.append(memfu->mFunctionalUnit.mName);
            return;
        }
    }
}

void TomasuloAlgorithm::undoRegisterDependencies(ScriptInstruction *instruct)
{
//    qDebug()<<"trying to undo dep: "<<instruct->mInstructionWhole;
    for(int i = 0; i<mRegisterFunctionalUnitList->length(); i++){
        if(!mRegisterFunctionalUnitList->at(i)->mInstruction.isEmpty() && instruct->mInstructionWhole.contains("div")){
            qDebug()<<mRegisterFunctionalUnitList->at(i)->mFunctionalUnit.mName<<" is empty: "<<mRegisterFunctionalUnitList->at(i)->mInstruction.isEmpty();
            qDebug()<<"number of instrucitons in register list: "<<mRegisterFunctionalUnitList->length();
            qDebug()<<"Index of register inst in script: "<<mScriptInstructionList->indexOf(mRegisterFunctionalUnitList->at(i)->mInstruction.first());
            qDebug()<<"Index of inst in script: "<<mScriptInstructionList->indexOf(mRegisterFunctionalUnitList->at(i)->mInstruction.first());
        }
        if(!mRegisterFunctionalUnitList->at(i)->mInstruction.isEmpty() && mRegisterFunctionalUnitList->at(i)->mInstruction.first()==instruct){
            qDebug()<<"Unoding dependecy. Inst: "<<mRegisterFunctionalUnitList->at(i)->mInstruction.first()->mInstructionWhole<<" FU: "<<mRegisterFunctionalUnitList->at(i)->mFunctionalUnitWithClaim.first();
            mRegisterFunctionalUnitList->at(i)->mInstruction.removeFirst();
            mRegisterFunctionalUnitList->at(i)->mFunctionalUnitWithClaim.removeFirst();
            return;
        }
    }
}

void TomasuloAlgorithm::undoCommonDataBusDependencies(ScriptInstruction *instruct)
{
    for(int i = 0; i<mCommonDataBusFunctionalUnitList->length(); i++){
        if(mCommonDataBusFunctionalUnitList->at(i)->mFunctionalUnitWithClaim==instruct->mDestinationRegister){
            mCommonDataBusFunctionalUnitList->at(i)->mFunctionalUnitWithClaim = "";
            return;
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
           ins->mInstruction.mDataType!=DataType::None &&
           IsOfDataType(&(holdFu->mFunctionalUnit),ins->mInstruction.mDataType) &&
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

ScriptInstruction *TomasuloAlgorithm::findFirstIssued(QList<ScriptInstruction *> *instructionList)
{
    if(instructionList->isEmpty()){
        return nullptr;
    }

    ScriptInstruction* returnInstruct = instructionList->first();
    for(int i = 1; i<instructionList->length(); i++){
        if(instructionList->at(i)->mIssueClockCycle<returnInstruct->mIssueClockCycle || // Overwrite the returnInstruct if the current list index was issued before
                (instructionList->at(i)->mIssueClockCycle==returnInstruct->mIssueClockCycle && // OR if it was issued in the same clock cycle AND has a lower issue index
                  instructionList->at(i)->mIssueIndex<returnInstruct->mIssueIndex)){
            returnInstruct = instructionList->at(i);
        }
    }
    return returnInstruct;
}
