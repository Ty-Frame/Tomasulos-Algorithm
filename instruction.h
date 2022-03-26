#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "descriptiveenumerations.h"

struct Instruction{
    QString mName;
    InstructionType mInstructionType = InstructionType::None;
    DataType mDataType = DataType::None;
    ArithmeticOptions mArithmeticOptions = ArithmeticOptions::None;
    MemoryOptions mMemoryOptions = MemoryOptions::None;
};

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

    returnString += ", FunctionalUnitDataType{";
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

    return returnString;
}

inline Instruction StringToFunctionalUnit(QString strFU){
    Instruction isnt;
    QString expression("(.*): InstructionType{(.*)}, FunctionalUnitDataType{(.*)}, ArithmeticOptions{(.*)}, MemoryOptions{(.*)}");
    QRegularExpression re(expression, QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch match = re.match(strFU);

    if(!match.hasMatch()){
        throw QString("Input line does not match necessary format.\nFormat: "+expression+"\nInput: "+strFU);
    }

    isnt.mName = match.captured(1);

    for(auto iType : match.captured(4).split(", ")){
        if(iType.isEmpty()) continue;
        try {
            isnt.mInstructionType = isnt.mInstructionType | StringToInstructionType(iType);
        }  catch (QString e) {
            throw QString(e+QString("\nInput: ")+strFU+QString("\nInvalid: ")+iType);
        }
    }

    for(auto dType : match.captured(5).split(", ")){
        if(dType.isEmpty()) continue;
        try {
            isnt.mDataType = isnt.mDataType | StringToDataType(dType);
        }  catch (QString e) {
            throw QString(e+QString("\nInput: ")+strFU+QString("\nInvalid: ")+dType);
        }
    }

    for(auto arithOpt : match.captured(6).split(", ")){
        if(arithOpt.isEmpty()) continue;
        try {
            isnt.mArithmeticOptions = isnt.mArithmeticOptions | StringToArithmeticOptions(arithOpt);
        }  catch (QString e) {
            throw QString(e+QString("\nInput: ")+strFU+QString("\nInvalid: ")+arithOpt);
        }
    }

    for(auto memOpt : match.captured(7).split(", ")){
        if(memOpt.isEmpty()) continue;
        try {
            isnt.mMemoryOptions = isnt.mMemoryOptions | StringToMemoryOptions(memOpt);
        }  catch (QString e) {
            throw QString(e+QString("\nInput: ")+strFU+QString("\nInvalid: ")+memOpt);
        }
    }

    return isnt;
}

#endif // INSTRUCTION_H
