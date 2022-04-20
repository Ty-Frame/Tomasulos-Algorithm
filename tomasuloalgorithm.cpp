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
    qDebug()<<"Clock Cycle: "<<mClockCycle;
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

    // Process Instructions trying to commit
    instruction = findFirstIssued(&commitPending);
    if(instruction!=nullptr){
        for(int i = 0; i<mScriptInstructionList->length(); i++){
            if(instruction==mScriptInstructionList->at(i) && (i==0 || mScriptInstructionList->at(i-1)->mCurrentPipelineStage==PipelineStages::Commited)){
                instruction->mCommitClockCycle = mClockCycle;
                instruction->mCurrentPipelineStage = PipelineStages::Commited;
                qDebug()<<instruction->mInstructionWhole<<" committed at clock cycle "<<mClockCycle<<".";
                undoRegisterDependencies(instruction);
                undoCommonDataBusDependencies(instruction);
                break;
            }
        }
    }
//    qDebug()<<"done processing instructions trying to commit.";

    // Process instructions exiting cdb
    CommonDataBusFunctionalUnit* cdb;
    for(int i = 0; i<exitingCdb.length(); i++){
        instruction = exitingCdb.at(i);
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

    // Move any stores that are in read write to committed
    for(int i = 0; i<doneReadOrWrite.length(); i++){
        instruction = doneReadOrWrite.at(i);
        if(instruction->mInstruction.mMemoryOptions==MemoryOptions::Store){
            instruction->mCurrentPipelineStage = PipelineStages::Commited;
            doneReadOrWrite.removeAt(i);
            i--;
        }
    }

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

    GeneralFunctionalUnit* genfu;
    for(int i = 0; i<mCommonDataBusFunctionalUnitList->length(); i++){
        instruction = findFirstIssued(&readWriteOrDoneExecutingAndNotMemoryInstructions);
        if(instruction==nullptr){
            break;
        }
        if(instruction->mInstruction.mInstructionType!=InstructionType::Memory){
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
        cdb = mCommonDataBusFunctionalUnitList->at(i);
        cdb->mBusy = true;
        cdb->mFunctionalUnitWithClaim = instruction->mDestinationRegister;
        cdb->mScriptInstruction = instruction;
        instruction->mCurrentPipelineStage = PipelineStages::WaitingToCommit;
        instruction->mWriteResultClockCycle = mClockCycle;
        //undoRegisterDependencies(instruction);
        qDebug()<<instruction->mInstructionWhole<<" passed through  "<<cdb->mFunctionalUnit.mName<<" and moved to waiting to commit at clock cycle "<<mClockCycle;
    }
//    qDebug()<<"Done processing instructions trying to get into cdb.";

    // Process done executing memory related instructions
    MemoryFunctionalUnit* memfu;
    for(int i = 0; i<doneExecuting.length(); i++){
        instruction = findFirstIssued(&doneExecuting);
        if(instruction==nullptr){
            break;
        }

        for(int j = 0; j<mMemoryFunctionalUnitList->length(); j++){
            memfu = mMemoryFunctionalUnitList->at(j);
            if(memfu->mBusy){
                memfu->mBusy = false;
                memfu->mOperation = "";
                memfu->mSourceOne = "";
                memfu->mReservationStationList.removeFirst();
                instruction->mCurrentPipelineStage = PipelineStages::ReadWriteAccess;
                instruction->mReadAccessClockCycle = mClockCycle;
                qDebug()<<instruction->mInstructionWhole<<" done executing in "<<memfu->mFunctionalUnit.mName<<" and moved to read write at clock cycle "<<mClockCycle;
            }
        }
    }
//    qDebug()<<"done moving memory related instructions to readwrite.";

    // Process currently executing instructions
    for(int i = 0; i<currentlyExecuting.length(); i++){
        instruction = currentlyExecuting.at(i);
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
                    qDebug()<<instruction->mInstructionWhole<<" done executing in "<<genfu->mFunctionalUnit.mName<<" at clock cycle "<<mClockCycle;
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
                        qDebug()<<instruction->mInstructionWhole<<" done executing in "<<memfu->mFunctionalUnit.mName<<" at clock cycle "<<mClockCycle;
                    }
                    memfu->mCountDown--;
                }
            }
        }
    }
//    qDebug()<<"done processing currecntly executing instructions";

    // Process instructions waiting to start execution
    for(int i = 0; i<waitingToStartExecution.length(); i++){
        instruction = waitingToStartExecution.at(i);
        bool found = false;
        for(int j = 0; j<mGeneralFunctionalUnitList->length(); j++){
            genfu = mGeneralFunctionalUnitList->at(j);
            if(!genfu->mReservationStationList.isEmpty() && genfu->mReservationStationList.contains(instruction)){
                found = true;
                if(genfu->mReservationStationList.first()==instruction && !doDependenciesExist(instruction)){
//                    setDependencies(genfu,instruction);
                    genfu->mBusy = true;
                    genfu->mCountDown = genfu->mFunctionalUnit.mLatency - 1;
                    genfu->mOperation = ToString(instruction->mInstruction.mArithmeticOptions);
                    genfu->mSourceOne = instruction->mSourceOneRegister;
                    genfu->mSourceTwo = instruction->mSourceTwoRegister;
                    instruction->mCurrentPipelineStage = PipelineStages::Execution;
                    instruction->mExecutionStartClockCycle = mClockCycle;
                    if(genfu->mCountDown==0){
                        instruction->mCurrentPipelineStage = PipelineStages::ExecutionDone;
                        instruction->mExecutionCompletionClockCycle = mClockCycle;
                    }
                    else{
                        genfu->mCountDown--;
                    }
                    qDebug()<<instruction->mInstructionWhole<<" started execution in "<<genfu->mFunctionalUnit.mName<<" at clock cycle "<<mClockCycle;
                }
            }
        }
        if(!found){
            for(int j = 0; j<mMemoryFunctionalUnitList->length(); j++){
                memfu = mMemoryFunctionalUnitList->at(j);
                if(!memfu->mReservationStationList.isEmpty() && memfu->mReservationStationList.contains(instruction)){
                    if(memfu->mReservationStationList.first()==instruction && !doDependenciesExist(instruction)){
//                        setDependencies(memfu,instruction);
                        found = true;
                        memfu->mBusy = true;
                        memfu->mCountDown = memfu->mFunctionalUnit.mLatency - 1;
                        memfu->mOperation = ToString(instruction->mInstruction.mMemoryOptions);
                        memfu->mSourceOne = instruction->mSourceOneRegister+" + "+instruction->mSourceTwoRegister;
                        instruction->mCurrentPipelineStage = PipelineStages::Execution;
                        instruction->mExecutionStartClockCycle = mClockCycle;
                        if(memfu->mCountDown==0){
                            instruction->mCurrentPipelineStage = PipelineStages::ExecutionDone;
                            instruction->mExecutionCompletionClockCycle = mClockCycle;
                        }
                        else{
                            memfu->mCountDown--;
                        }
                        qDebug()<<instruction->mInstructionWhole<<" started execution in "<<memfu->mFunctionalUnit.mName<<" at clock cycle "<<mClockCycle;
                    }
                }
            }
        }
        if(!found){
            qDebug()<<"Functional unit could not be found for "<<instruction->mInstructionWhole;
        }
    }
//    qDebug()<<"done processing instructions waiting to start executing";

    bool successfulIssue = false;
    int len;
    if(mIssueNumber<toBeIssued.length()){
        len = mIssueNumber;
    }
    else{
        len = toBeIssued.length();
    }
    for(int i = 0; i<len; i++){
        instruction = toBeIssued.at(i);
        qDebug()<<"attempting to issue "<<instruction->mInstructionWhole;
        genfu = getOptimalGeneralFunctionalUnit(instruction);
        if(genfu!=nullptr){
            setDependencies(genfu, instruction);
            genfu->mReservationStationList.append(instruction);
            instruction->mIssueClockCycle = mClockCycle;
            instruction->mCurrentPipelineStage = PipelineStages::Issued;
            successfulIssue = true;
            qDebug()<<instruction->mInstructionWhole<<" issued to "<<genfu->mFunctionalUnit.mName<<" at clock cycle "<<mClockCycle;
        }
        else{
            memfu = getOptimalMemoryFunctionalUnit(instruction);
            if(memfu!=nullptr){
                setDependencies(memfu, instruction);
                memfu->mReservationStationList.append(instruction);
                instruction->mIssueClockCycle = mClockCycle;
                instruction->mCurrentPipelineStage = PipelineStages::Issued;
                successfulIssue = true;
                qDebug()<<instruction->mInstructionWhole<<" issued to "<<memfu->mFunctionalUnit.mName<<" at clock cycle "<<mClockCycle;
            }
        }

        if(!successfulIssue){
            qDebug()<<"No Successful Issue for "<<instruction->mInstructionWhole;
            break;
        }
    }
//    qDebug()<<"done processing isntructions waiting to be issued";

    mClockCycle++;
    if(mScriptInstructionList->length()==dontCareBecuaseDone.length()){
        this->setRunStatus(TomasuloRunStatus::Done);
    }
    emit StepDone();

    /*

//     ---Entire Process---
//      Empty Common Data Bus with the first issued instruction
//      Move earliest issued AND completed instructions into a Common Data Bus OR memory related instrucitons that are in readwrite
//      If instruction is moved from the functional unit to Common Data Bus
//      Stage all functional units
//      Issue new instructions if capabale
//


    // Empty Data Buses
    CommonDataBusFunctionalUnit* cdbfu = nullptr;
    for(int i = 0; i<mCommonDataBusFunctionalUnitList->length(); i++){
        if(cdbfu==nullptr && mCommonDataBusFunctionalUnitList->at(i)->mBusy){
            cdbfu = mCommonDataBusFunctionalUnitList->at(i);
        }
        else if(cdbfu!=nullptr && mCommonDataBusFunctionalUnitList->at(i)->mBusy){
            if(mCommonDataBusFunctionalUnitList->at(i)->mScriptInstruction->mIssueClockCycle<cdbfu->mScriptInstruction->mIssueClockCycle){
                cdbfu = mCommonDataBusFunctionalUnitList->at(i);
            }
            else if(mCommonDataBusFunctionalUnitList->at(i)->mScriptInstruction->mIssueClockCycle==cdbfu->mScriptInstruction->mIssueClockCycle){
                if(mCommonDataBusFunctionalUnitList->at(i)->mScriptInstruction->mIssueIndex<cdbfu->mScriptInstruction->mIssueIndex){
                    cdbfu = mCommonDataBusFunctionalUnitList->at(i);
                }
            }
        }
    }
    if(cdbfu!=nullptr){
        if(cdbfu->mScriptInstruction!=nullptr){
            cdbfu->mScriptInstruction->mCurrentPipelineStage = PipelineStages::Done;
            cdbfu->mScriptInstruction->mCommitClockCycle = mClockCycle;
            undoCommonDataBusDependencies(cdbfu->mScriptInstruction);
//            qDebug()<<"Instruction Exited CDB: "<<cdbfu->mScriptInstruction->mInstructionWhole<<" CC: "<<mClockCycle;
        }
        cdbfu->mScriptInstruction = nullptr;
        cdbfu->mFunctionalUnitWithClaim = "";
        cdbfu->mBusy = false;
    }
    // Move issues that are completed into common data bus
    QList<ScriptInstruction*> completedInstructions;
    ScriptInstruction* instruct;
    for(int i = 0; i<mScriptInstructionList->length(); i++){
        instruct = mScriptInstructionList->at(i);
        if(instruct->mCurrentPipelineStage==PipelineStages::ReadWriteAccess || instruct->mCurrentPipelineStage==PipelineStages::ExecutionDone){
           completedInstructions.append(instruct);
        }
    }

    MemoryFunctionalUnit* memfu;
    for(int i = 0; i<mMemoryFunctionalUnitList->length(); i++){
        memfu = mMemoryFunctionalUnitList->at(i);
        if(memfu->mCountDown == 0 && !memfu->mReservationStationList.isEmpty()){
            memfu->mReservationStationList.first()->mCurrentPipelineStage = PipelineStages::ReadWriteAccess;
            memfu->mReservationStationList.first()->mReadAccessClockCycle = mClockCycle;
//            qDebug()<<"Instruction moved into ReadWriteAccess: "<<memfu->mReservationStationList.first()->mInstructionWhole<<" CC: "<<mClockCycle;
            memfu->mReservationStationList.remove(0);
            memfu->mOperation = "";
            memfu->mSourceOne = "";
            memfu->mBusy = false;
        }
    }

    GeneralFunctionalUnit* genfu;
    for(int i = 0; i<mGeneralFunctionalUnitList->length(); i++){
        genfu = mGeneralFunctionalUnitList->at(i);
        if(genfu->mCountDown == 0 && !genfu->mReservationStationList.isEmpty()){
            completedInstructions.append(genfu->mReservationStationList.first());
        }
    }

    QList<ScriptInstruction*> movedToCDB;
    if(completedInstructions.length()>0){
        int len;
        //qDebug()<<"move to cdb: "<<completedInstructions.length()<<"<"<<mCommonDataBusFunctionalUnitList->length();
        if(completedInstructions.length()<mCommonDataBusFunctionalUnitList->length()){
            len = completedInstructions.length();
        }
        else{
            len = mCommonDataBusFunctionalUnitList->length();
        }
        ScriptInstruction* firstIssuedInst;
        for(int i = 0; i<len; i++){

            // Find first issued instruction
            firstIssuedInst = completedInstructions.first();
            for(int j = 1; j<completedInstructions.length(); j++){
                if(completedInstructions.at(j)->mIssueClockCycle<firstIssuedInst->mIssueClockCycle){
                    firstIssuedInst = completedInstructions.at(j);
                }
                else if(completedInstructions.at(j)->mIssueClockCycle==firstIssuedInst->mIssueClockCycle){
                    if(completedInstructions.at(j)->mIssueIndex<firstIssuedInst->mIssueIndex){
                        firstIssuedInst = completedInstructions.at(j);
                    }
                }
            }
            // Remove first issued instruction from the completed instructions list
            completedInstructions.remove(completedInstructions.indexOf(firstIssuedInst));

            // Update common data bus with information
            mCommonDataBusFunctionalUnitList->at(i)->mBusy = true;
            mCommonDataBusFunctionalUnitList->at(i)->mFunctionalUnitWithClaim = firstIssuedInst->mDestinationRegister;
            mCommonDataBusFunctionalUnitList->at(i)->mScriptInstruction = firstIssuedInst;
            movedToCDB.append(firstIssuedInst);
            firstIssuedInst->mWriteResultClockCycle = mClockCycle;
            firstIssuedInst->mCurrentPipelineStage = PipelineStages::ExecutionDone;

            // Remove first issued instruction from the reservation list that it is in
            bool found = false;
            MemoryFunctionalUnit* memfu;
            for(int j = 0; j<mMemoryFunctionalUnitList->length(); j++){
                memfu = mMemoryFunctionalUnitList->at(j);
                if(!memfu->mReservationStationList.isEmpty() && memfu->mReservationStationList.first()==firstIssuedInst){
                    found = true;
//                    qDebug()<<"Instruction moved into CDB: "<<memfu->mReservationStationList.first()->mInstructionWhole<<" CC: "<<mClockCycle;
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
//                        qDebug()<<"Instruction moved into CDB: "<<genfu->mReservationStationList.first()->mInstructionWhole<<" CC: "<<mClockCycle;
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

    // Stage functional units
    updateFunctionalUnits();
//    qDebug()<<"Functional Units Updated";

    for(int i = 0; i<movedToCDB.length(); i++){
        undoRegisterDependencies(movedToCDB.at(i));
    }

    // Issue new instructions
    issueInstructions();
//    qDebug()<<"Instructions Issued";

    mClockCycle++;
    if(mScriptInstructionList->last()->mCurrentPipelineStage==PipelineStages::Done){
        this->runStatus = TomasuloRunStatus::Done;
    }
    emit StepDone();
*/
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
//    qDebug()<<"Do Deendencies Exist For Instruction "<<ins->mInstructionWhole<<"?";
    int len = mRegisterFunctionalUnitList->length();
    RegisterFunctionalUnit* rUnit;
    for(int i = 0; i<len; i++){
        rUnit = mRegisterFunctionalUnitList->at(i);
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
            mRegisterFunctionalUnitList->at(i)->mFunctionalUnitWithClaim.append(genfu->mFunctionalUnit.mName);
            return;
        }
    }
}

void TomasuloAlgorithm::setDependencies(MemoryFunctionalUnit *memfu, ScriptInstruction *instruct)
{
    for(int i = 0; i<mRegisterFunctionalUnitList->length(); i++){
        if(mRegisterFunctionalUnitList->at(i)->mFunctionalUnit.mName==instruct->mDestinationRegister){
            mRegisterFunctionalUnitList->at(i)->mInstruction = instruct;
            mRegisterFunctionalUnitList->at(i)->mFunctionalUnitWithClaim = memfu->mFunctionalUnit.mName;
            return;
        }
    }
}

void TomasuloAlgorithm::undoRegisterDependencies(ScriptInstruction *instruct)
{
    for(int i = 0; i<mRegisterFunctionalUnitList->length(); i++){
        if(mRegisterFunctionalUnitList->at(i)->mFunctionalUnit.mName==instruct->mDestinationRegister){
            mRegisterFunctionalUnitList->at(i)->mFunctionalUnitWithClaim = "";
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
