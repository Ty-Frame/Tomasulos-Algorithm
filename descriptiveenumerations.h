#ifndef DESCRIPTIVEENUMERATIONS_H
#define DESCRIPTIVEENUMERATIONS_H

#include <QString>
#include <QDebug>
#include <QRegularExpression>

// InstructionType start
enum class InstructionType{
    None = 0,
    Arithmetic = 1,
    Memory = 2
};

static const InstructionType AllInstructionType[] = {InstructionType::None , InstructionType::Arithmetic, InstructionType::Memory};

inline InstructionType operator|(InstructionType a, InstructionType b){
    return static_cast<InstructionType>(static_cast<int>(a) | static_cast<int>(b));
}

inline InstructionType operator&(InstructionType a, InstructionType b){
    return static_cast<InstructionType>(static_cast<int>(a) & static_cast<int>(b));
}

inline const QString ToString(InstructionType a){
    switch(a){
        case InstructionType::None: return QString("None");
        case InstructionType::Arithmetic: return QString("Arithmetic");
        case InstructionType::Memory: return QString("Memory");
        default: throw QString("[Unknown InstructionType]");
    }
}

inline InstructionType StringToInstructionType(QString a){
    for(auto iType : AllInstructionType){
        if(ToString(iType) == a) return iType;
    }
    throw QString("[Unknown InstructionType]");
}

// FunctionalUnitType start
enum class FunctionalUnitType{
    None = 0,
    Arithmetic = 1,
    Memory = 2,
    CommonDataBus = 4,
    Register = 8,
    Issuer = 16
};

static const FunctionalUnitType AllFunctionalUnitType[] = {FunctionalUnitType::None , FunctionalUnitType::Arithmetic, FunctionalUnitType::Memory, FunctionalUnitType::CommonDataBus, FunctionalUnitType::Register, FunctionalUnitType::Issuer};

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
        case FunctionalUnitType::Issuer: return QString("Issuer");
        default: throw QString("[Unknown FuncitonalUnitType]");
    }
}

inline FunctionalUnitType StringToFunctionalUnitType(QString a){
    for(auto fuType : AllFunctionalUnitType){
        if(ToString(fuType) == a) return fuType;
    }
    throw QString("[Unknown FunctionalUnitType]");
}

// DataType start
enum class DataType{
    None = 0,
    Integer = 1,
    Float = 2
};

static const DataType AllDataType[] = {DataType::None, DataType::Integer, DataType::Float};

inline DataType operator|(DataType a, DataType b){
    return static_cast<DataType>(static_cast<int>(a) | static_cast<int>(b));
}

inline DataType operator&(DataType a, DataType b){
    return static_cast<DataType>(static_cast<int>(a) & static_cast<int>(b));
}

inline const QString ToString(DataType a){
    switch(a){
        case DataType::None: return QString("None");
        case DataType::Integer: return QString("Integer");
        case DataType::Float: return QString("Float");
        default: throw QString("[Unknown DataType]");
    }
}

inline DataType StringToDataType(QString a){
    for(auto fuDType : AllDataType){
        if(ToString(fuDType) == a) return fuDType;
    }
    throw QString("[Unknown DataType]");
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

// PipelineStages start
enum class PipelineStages{
    None = 0,
    Issued = 1,
    Execution = 2,
    ExecutionDone = 4,
    ReadWriteAccess = 8,
    Writeback = 16,
    WaitingToCommit = 32,
    Commited = 64
};

static const PipelineStages AllPipelineStages[] = {PipelineStages::None , PipelineStages::Issued, PipelineStages::Execution, PipelineStages::ExecutionDone, PipelineStages::ReadWriteAccess, PipelineStages::Writeback, PipelineStages::WaitingToCommit, PipelineStages::Commited};

inline PipelineStages operator|(PipelineStages a, PipelineStages b){
    return static_cast<PipelineStages>(static_cast<int>(a) | static_cast<int>(b));
}

inline PipelineStages operator&(PipelineStages a, PipelineStages b){
    return static_cast<PipelineStages>(static_cast<int>(a) & static_cast<int>(b));
}

inline const QString ToString(PipelineStages a){
    switch(a){
        case PipelineStages::None: return QString("None");
        case PipelineStages::Issued: return QString("Issued");
        case PipelineStages::Execution: return QString("Execution");
        case PipelineStages::ExecutionDone: return QString("Execution Done");
        case PipelineStages::ReadWriteAccess: return QString("Read Write Access");
        case PipelineStages::Writeback: return QString("Writeback");
    case PipelineStages::WaitingToCommit: return QString("Waiting To Commit");
        case PipelineStages::Commited: return QString("Commited");
        default: throw QString("[Unknown PipelineStages]");
    }
}

inline PipelineStages StringToPipelineStages(QString a){
    for(auto stage : AllPipelineStages){
        if(ToString(stage) == a) return stage;
    }
    throw QString("[Unknown PipelineStages]");
}

#endif // DESCRIPTIVEENUMERATIONS_H
