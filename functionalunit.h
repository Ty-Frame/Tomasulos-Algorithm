#ifndef FUNCTIONALUNIT_H
#define FUNCTIONALUNIT_H

#include <QString>
#include <QRegularExpression>
#include <QDebug>

using namespace std;

// FunctionalUnitType start
enum class FunctionalUnitType{
    None = 0,
    Arithmetic = 1,
    Memory = 2,
    CommonDataBus = 4,
    Register = 8
};

static const FunctionalUnitType AllFunctionalUnitType[] = {FunctionalUnitType::None , FunctionalUnitType::Arithmetic, FunctionalUnitType::Memory, FunctionalUnitType::CommonDataBus, FunctionalUnitType::Register};

inline FunctionalUnitType operator|(FunctionalUnitType a, FunctionalUnitType b){
    return static_cast<FunctionalUnitType>(static_cast<int>(a) | static_cast<int>(b));
}

inline FunctionalUnitType operator&(FunctionalUnitType a, FunctionalUnitType b){
    return static_cast<FunctionalUnitType>(static_cast<int>(a) & static_cast<int>(b));
}

inline const QString ToString(FunctionalUnitType a){
    switch(a){
        case FunctionalUnitType::None: return QString("None");
        case FunctionalUnitType::Arithmetic: return QString("Arithmetic");
        case FunctionalUnitType::Memory: return QString("Memory");
        case FunctionalUnitType::CommonDataBus: return QString("Common Data Bus");
        case FunctionalUnitType::Register: return QString("Register");
        default: throw QString("[Unknown FuncitonalUnitType]");
    }
}

inline FunctionalUnitType StringToFunctionalUnitType(QString a){
    for(auto fuType : AllFunctionalUnitType){
        if(ToString(fuType) == a) return fuType;
    }
    throw QString("[Unknown FunctionalUnitType]");
}

// FunctionalUnitDataType start
enum class FunctionalUnitDataType{
    None = 0,
    Integer = 1,
    Float = 2
};

static const FunctionalUnitDataType AllFunctionalUnitDataType[] = {FunctionalUnitDataType::None, FunctionalUnitDataType::Integer, FunctionalUnitDataType::Float};

inline FunctionalUnitDataType operator|(FunctionalUnitDataType a, FunctionalUnitDataType b){
    return static_cast<FunctionalUnitDataType>(static_cast<int>(a) | static_cast<int>(b));
}

inline FunctionalUnitDataType operator&(FunctionalUnitDataType a, FunctionalUnitDataType b){
    return static_cast<FunctionalUnitDataType>(static_cast<int>(a) & static_cast<int>(b));
}

inline const QString ToString(FunctionalUnitDataType a){
    switch(a){
        case FunctionalUnitDataType::None: return QString("None");
        case FunctionalUnitDataType::Integer: return QString("Integer");
        case FunctionalUnitDataType::Float: return QString("Float");
        default: throw QString("[Unknown FunctionalUnitDataType]");
    }
}

inline FunctionalUnitDataType StringToFunctionalUnitDataType(QString a){
    for(auto fuDType : AllFunctionalUnitDataType){
        if(ToString(fuDType) == a) return fuDType;
    }
    throw QString("[Unknown FunctionalUnitDataType]");
}

// MemoryOptions start
enum class MemoryOptions{
    None = 0,
    Load = 1,
    Store = 2
};

static const MemoryOptions AllMemoryOptions[] = {MemoryOptions::None,  MemoryOptions::Load,  MemoryOptions::Store};

inline MemoryOptions operator|(MemoryOptions a, MemoryOptions b){
    return static_cast<MemoryOptions>(static_cast<int>(a) | static_cast<int>(b));
}

inline MemoryOptions operator&(MemoryOptions a, MemoryOptions b){
    return static_cast<MemoryOptions>(static_cast<int>(a) & static_cast<int>(b));
}

inline const QString ToString(MemoryOptions a){
    switch(a){
        case MemoryOptions::None: return QString("None");
        case MemoryOptions::Load: return QString("Load");
        case MemoryOptions::Store: return QString("Store");
        default: throw QString("[Unknown MemoryOptions]");
    }
}

inline MemoryOptions StringToMemoryOptions(QString a){
    for(auto memOps : AllMemoryOptions){
        if(ToString(memOps) == a) return memOps;
    }
    throw QString("[Unknown MemoryOptions]");
}

// ArithmeticOptions start
enum class ArithmeticOptions{
    None = 0,
    Add = 1,
    Subtract = 2,
    Multiply = 4,
    Divide = 8,
    BranchEvaluation = 16
};

static const ArithmeticOptions AllArithmeticOptions[] = {ArithmeticOptions::None, ArithmeticOptions::Add, ArithmeticOptions::Subtract, ArithmeticOptions::Multiply, ArithmeticOptions::Divide, ArithmeticOptions::BranchEvaluation};

inline ArithmeticOptions operator|(ArithmeticOptions a, ArithmeticOptions b){
    return static_cast<ArithmeticOptions>(static_cast<int>(a) | static_cast<int>(b));
}

inline ArithmeticOptions operator&(ArithmeticOptions a, ArithmeticOptions b){
    return static_cast<ArithmeticOptions>(static_cast<int>(a) & static_cast<int>(b));
}

inline const QString ToString(ArithmeticOptions a){
    switch(a){
        case ArithmeticOptions::None: return QString("None");
        case ArithmeticOptions::Add: return QString("Add");
        case ArithmeticOptions::Subtract: return QString("Subtract");
        case ArithmeticOptions::Multiply: return QString("Multiply");
        case ArithmeticOptions::Divide: return QString("Divide");
        case ArithmeticOptions::BranchEvaluation: return QString("Branch Evaluation");
        default: throw QString("[Unknown ArithmeticOptions]");
    }
}

inline ArithmeticOptions StringToArithmeticOptions(QString a){
    for(auto airthOps : AllArithmeticOptions){
        if(ToString(airthOps) == a) return airthOps;
    }
    throw QString("[Unknown ArithmeticOptions]");
}

struct FunctionalUnit{
    QString mName;
    unsigned short int mFunctionalUnitCount = 1;
    unsigned int mLatency = 1;
    FunctionalUnitType mFunctionalUnitType = FunctionalUnitType::None;
    FunctionalUnitDataType mFunctionalUnitDataType = FunctionalUnitDataType::None;
    ArithmeticOptions mArithmeticOptions = ArithmeticOptions::None;
    MemoryOptions mMemoryOptions = MemoryOptions::None;
};

inline bool operator==(FunctionalUnit a, FunctionalUnit b){
    return (a.mName == b.mName &&
            a.mFunctionalUnitCount == b.mFunctionalUnitCount &&
            a.mLatency == b.mLatency &&
            a.mFunctionalUnitType == b.mFunctionalUnitType &&
            a.mFunctionalUnitDataType == b.mFunctionalUnitDataType &&
            a.mArithmeticOptions == b.mArithmeticOptions &&
            a.mMemoryOptions == b.mMemoryOptions
            );
}

inline bool IsOfFunctionalUnitType(FunctionalUnit* fu, FunctionalUnitType fuType){
    return (fu->mFunctionalUnitType & fuType) == fuType;
};

inline bool IsOfFunctionalUnitDataType(FunctionalUnit* fu, FunctionalUnitDataType fuDType){
    return (fu->mFunctionalUnitDataType & fuDType) == fuDType;
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

    returnString += ", FunctionalUnitDataType{";
    for(auto fuDType: AllFunctionalUnitDataType){
        if(fuDType != FunctionalUnitDataType::None && (a.mFunctionalUnitDataType & fuDType) == fuDType) {
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

    for(auto fuDType : match.captured(5).split(", ")){
        if(fuDType.isEmpty()) continue;
        try {
            fu.mFunctionalUnitDataType = fu.mFunctionalUnitDataType | StringToFunctionalUnitDataType(fuDType);
        }  catch (QString e) {
            throw QString(e+QString("\nInput: ")+strFU+QString("\nInvalid: ")+fuDType);
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

#endif // FUNCTIONALUNIT_H
