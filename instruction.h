#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <QMessageBox>
#include <QFile>
#include "descriptiveenumerations.h"

struct Instruction{
    QString mName;
    InstructionType mInstructionType = InstructionType::None;
    DataType mDataType = DataType::None;
    ArithmeticOptions mArithmeticOptions = ArithmeticOptions::None;
    MemoryOptions mMemoryOptions = MemoryOptions::None;
    PipelineStages mPipelineStages = PipelineStages::None;
};

struct ScriptInstruction{
    Instruction mInstruction;
    QString mInstructionName;
    QString mDestinationRegister;
    QString mSourceOneRegister;
    QString mSourceTwoRegister;
    int mIssueClockCycle = -1;
    int mExecutionCompletionClockCycle = -1;
    int mWriteResultClockCycle = -1;
};

inline ScriptInstruction StringToScriptInstruction(QString scrInstString){
    QStringList fields = scrInstString.split(QRegularExpression("\\s+"));
    ScriptInstruction returnScrInst;

    if(fields.length() != 4){
        throw QString("Expected format: INSTRUCTION_NAME DESTINATION_REGISTER SOURCE_REGISTER_1 SOURCE_REGISTER_2\n\t\tOR INSTRUCTION_NAME DESTINATION_REGISTER SOURCE_REGISTER_1 #IMMEDIATE_VALUE\nInvalid input: " + scrInstString);
    }

    returnScrInst.mInstructionName = fields[0];
    returnScrInst.mDestinationRegister = fields[1];
    returnScrInst.mSourceOneRegister = fields[2];
    returnScrInst.mSourceTwoRegister = fields[3];
    return returnScrInst;
}

inline bool operator==(Instruction a, Instruction b){
    return (a.mName == b.mName &&
            a.mInstructionType == b.mInstructionType &&
            a.mDataType == b.mDataType &&
            a.mArithmeticOptions == b.mArithmeticOptions &&
            a.mMemoryOptions == b.mMemoryOptions
            );
}

inline bool IsOfInstructionType(Instruction* fu, InstructionType iType){
    return (fu->mInstructionType & iType) == iType;
};

inline bool IsOfDataType(Instruction* fu, DataType fuDType){
    return (fu->mDataType & fuDType) == fuDType;
};

inline bool IsOfArithmeticOptions(Instruction* fu, ArithmeticOptions arithOpt){
    return (fu->mArithmeticOptions & arithOpt) == arithOpt;
};

inline bool IsOfMemoryOptions(Instruction* fu, MemoryOptions memOpt){
    return (fu->mMemoryOptions & memOpt) == memOpt;
};

inline bool IsOfPipelineStages(Instruction* fu, PipelineStages pStage){
    return (fu->mPipelineStages & pStage) == pStage;
};

inline QString ToString(Instruction a){
    QString returnString = a.mName + ": InstructionType{";
    for(auto iType: AllInstructionType){
        if(iType != InstructionType::None && (a.mInstructionType & iType) == iType) {
            returnString += ToString(iType);
            returnString += ", ";
        }
    }
    if(returnString.lastIndexOf(QString(", ")) == returnString.length()-2){
        returnString.chop(2);
    }
    returnString += "}";

    returnString += ", DataType{";
    for(auto fuDType: AllDataType){
        if(fuDType != DataType::None && (a.mDataType & fuDType) == fuDType) {
            returnString += ToString(fuDType);
            returnString += ", ";
        }
    }
    if(returnString.lastIndexOf(QString(", ")) == returnString.length()-2){
        returnString.chop(2);
    }
    returnString += "}";

    returnString += ", ArithmeticOptions{";
    for(auto arithOpt: AllArithmeticOptions){
        if(arithOpt != ArithmeticOptions::None && (a.mArithmeticOptions & arithOpt) == arithOpt) {
            returnString += ToString(arithOpt);
            returnString += ", ";
        }
    }
    if(returnString.lastIndexOf(QString(", ")) == returnString.length()-2){
        returnString.chop(2);
    }
    returnString += "}";

    returnString += ", MemoryOptions{";
    for(auto memhOpt: AllMemoryOptions){
        if(memhOpt != MemoryOptions::None && (a.mMemoryOptions & memhOpt) == memhOpt) {
            returnString += ToString(memhOpt);
            returnString += ", ";
        }
    }
    if(returnString.lastIndexOf(QString(", ")) == returnString.length()-2){
        returnString.chop(2);
    }
    returnString += "}";

    returnString += ", PipelineStages{";
    for(auto pStage: AllPipelineStages){
        if(pStage != PipelineStages::None && (a.mPipelineStages & pStage) == pStage) {
            returnString += ToString(pStage);
            returnString += ", ";
        }
    }
    if(returnString.lastIndexOf(QString(", ")) == returnString.length()-2){
        returnString.chop(2);
    }
    returnString += "}";

    return returnString;
}

inline Instruction StringToInstruction(QString strFU){
    Instruction isnt;
    QString expression("(.*): InstructionType{(.*)}, DataType{(.*)}, ArithmeticOptions{(.*)}, MemoryOptions{(.*)}, PipelineStages{(.*)}");
    QRegularExpression re(expression, QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch match = re.match(strFU);

    if(!match.hasMatch()){
        throw QString("Input line does not match necessary format.\nFormat: "+expression+"\nInput: "+strFU);
    }

    isnt.mName = match.captured(1);

    for(auto iType : match.captured(2).split(", ")){
        if(iType.isEmpty()) continue;
        try {
            isnt.mInstructionType = isnt.mInstructionType | StringToInstructionType(iType);
        }  catch (QString e) {
            throw QString(e+QString("\nInput: ")+strFU+QString("\nInvalid: ")+iType);
        }
    }

    for(auto dType : match.captured(3).split(", ")){
        if(dType.isEmpty()) continue;
        try {
            isnt.mDataType = isnt.mDataType | StringToDataType(dType);
        }  catch (QString e) {
            throw QString(e+QString("\nInput: ")+strFU+QString("\nInvalid: ")+dType);
        }
    }

    for(auto arithOpt : match.captured(4).split(", ")){
        if(arithOpt.isEmpty()) continue;
        try {
            isnt.mArithmeticOptions = isnt.mArithmeticOptions | StringToArithmeticOptions(arithOpt);
        }  catch (QString e) {
            throw QString(e+QString("\nInput: ")+strFU+QString("\nInvalid: ")+arithOpt);
        }
    }

    for(auto memOpt : match.captured(5).split(", ")){
        if(memOpt.isEmpty()) continue;
        try {
            isnt.mMemoryOptions = isnt.mMemoryOptions | StringToMemoryOptions(memOpt);
        }  catch (QString e) {
            throw QString(e+QString("\nInput: ")+strFU+QString("\nInvalid: ")+memOpt);
        }
    }

    for(auto pStage : match.captured(6).split(", ")){
        if(pStage.isEmpty()) continue;
        try {
            isnt.mPipelineStages = isnt.mPipelineStages | StringToPipelineStages(pStage);
        }  catch (QString e) {
            throw QString(e+QString("\nInput: ")+strFU+QString("\nInvalid: ")+pStage);
        }
    }

    return isnt;
}

inline QList<Instruction>* readInInstructionListFile(QString *filename){
    QFile inFile(*filename);
    if(!inFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::critical(NULL, QString::fromStdString("Error Opening Architecture File"), QString::fromStdString(std::string("Could not open file:\n")) + *filename);
        return nullptr;
    }

    QTextStream inFileStream(&inFile);
    QList<Instruction>* list = new QList<Instruction>();
    while(!inFileStream.atEnd()){
        QString iString = inFileStream.readLine();
        try {
            Instruction inst = StringToInstruction(iString);
            list->append(inst);
        }  catch (QString e) {
            QMessageBox::critical(NULL, "Error Reading In Architecture Item", e);
        }
    }
    inFile.close();
    return list;
}

#endif // INSTRUCTION_H
