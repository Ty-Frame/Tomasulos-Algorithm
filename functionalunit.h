#ifndef FUNCTIONALUNIT_H
#define FUNCTIONALUNIT_H

#include "descriptiveenumerations.h"

using namespace std;

struct FunctionalUnit{
    QString mName;
    unsigned short int mFunctionalUnitCount = 1;
    unsigned int mLatency = 1;
    FunctionalUnitType mFunctionalUnitType = FunctionalUnitType::None;
    DataType mDataType = DataType::None;
    ArithmeticOptions mArithmeticOptions = ArithmeticOptions::None;
    MemoryOptions mMemoryOptions = MemoryOptions::None;
};

inline bool operator==(FunctionalUnit a, FunctionalUnit b){
    return (a.mName == b.mName &&
            a.mFunctionalUnitCount == b.mFunctionalUnitCount &&
            a.mLatency == b.mLatency &&
            a.mFunctionalUnitType == b.mFunctionalUnitType &&
            a.mDataType == b.mDataType &&
            a.mArithmeticOptions == b.mArithmeticOptions &&
            a.mMemoryOptions == b.mMemoryOptions
            );
}

inline bool IsOfFunctionalUnitType(FunctionalUnit* fu, FunctionalUnitType fuType){
    return (fu->mFunctionalUnitType & fuType) == fuType;
};

inline bool IsOfDataType(FunctionalUnit* fu, DataType dType){
    return (fu->mDataType & dType) == dType;
};

inline bool IsOfArithmeticOptions(FunctionalUnit* fu, ArithmeticOptions arithOpt){
    return (fu->mArithmeticOptions & arithOpt) == arithOpt;
};

inline bool IsOfMemoryOptions(FunctionalUnit* fu, MemoryOptions memOpt){
    return (fu->mMemoryOptions & memOpt) == memOpt;
};

inline QString ToString(FunctionalUnit a){
    QString returnString = a.mName + ": Count{" + QString::fromStdString(to_string(a.mFunctionalUnitCount)) + "}";
    returnString.append(", Latency{" + QString::fromStdString(to_string(a.mLatency)) + "}");

    returnString += ", FunctionalUnitType{";
    for(auto fuType: AllFunctionalUnitType){
        if(fuType != FunctionalUnitType::None && (a.mFunctionalUnitType & fuType) == fuType) {
            returnString += ToString(fuType);
            returnString += ", ";
        }
    }
    if(returnString.lastIndexOf(QString(", ")) == returnString.length()-2){
        returnString.chop(2);
    }
    returnString += "}";

    returnString += ", DataType{";
    for(auto dType: AllDataType){
        if(dType != DataType::None && (a.mDataType & dType) == dType) {
            returnString += ToString(dType);
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

inline FunctionalUnit StringToFunctionalUnit(QString strFU){
    FunctionalUnit fu;
    QString expression("(.*): Count{(\\d+)}, Latency{(\\d+)}, FunctionalUnitType{(.*)}, FunctionalUnitDataType{(.*)}, ArithmeticOptions{(.*)}, MemoryOptions{(.*)}");
    QRegularExpression re(expression, QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch match = re.match(strFU);

    if(!match.hasMatch()){
        throw QString("Input line does not match necessary format.\nFormat: "+expression+"\nInput: "+strFU);
    }

    fu.mName = match.captured(1);
    fu.mFunctionalUnitCount = match.captured(2).toUShort();
    fu.mLatency = match.captured(3).toUInt();

    for(auto fuType : match.captured(4).split(", ")){
        if(fuType.isEmpty()) continue;
        try {
            fu.mFunctionalUnitType = fu.mFunctionalUnitType | StringToFunctionalUnitType(fuType);
        }  catch (QString e) {
            throw QString(e+QString("\nInput: ")+strFU+QString("\nInvalid: ")+fuType);
        }
    }

    for(auto dType : match.captured(5).split(", ")){
        if(dType.isEmpty()) continue;
        try {
            fu.mDataType = fu.mDataType | StringToDataType(dType);
        }  catch (QString e) {
            throw QString(e+QString("\nInput: ")+strFU+QString("\nInvalid: ")+dType);
        }
    }

    for(auto arithOpt : match.captured(6).split(", ")){
        if(arithOpt.isEmpty()) continue;
        try {
            fu.mArithmeticOptions = fu.mArithmeticOptions | StringToArithmeticOptions(arithOpt);
        }  catch (QString e) {
            throw QString(e+QString("\nInput: ")+strFU+QString("\nInvalid: ")+arithOpt);
        }
    }

    for(auto memOpt : match.captured(7).split(", ")){
        if(memOpt.isEmpty()) continue;
        try {
            fu.mMemoryOptions = fu.mMemoryOptions | StringToMemoryOptions(memOpt);
        }  catch (QString e) {
            throw QString(e+QString("\nInput: ")+strFU+QString("\nInvalid: ")+memOpt);
        }
    }

    return fu;
}

struct GeneralFunctionalUnit{
    FunctionalUnit mFunctionalUnit;
    unsigned int mCountDown = 0;
    bool mBusy = false;
    QString mOperation = "";
    QString mSourceOne = "";
    QString mSourceTwo = "";
};

struct MemoryFunctionalUnit{
    FunctionalUnit mFunctionalUnit;
    bool mBusy = false;
    QString mOperation = "";
    QString mSourceOne = "";
};

struct RegisterFunctionalUnit{
    FunctionalUnit mFunctionalUnit;
    QString mFunctionalUnitWithClaim = "";
};

struct CommonDataBusFunctionalUnit{
    FunctionalUnit mFunctionalUnit;
    bool mBusy = false;
    QString mFunctionalUnitWithClaim = "";
};

#endif // FUNCTIONALUNIT_H
