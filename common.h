#ifndef COMMON_H
#define COMMON_H

#define MAJOR_NUMBER "0"
#define MINOR_NUMBER "4"
#define PATCH_NUMBER "3"
#define VERSION ("v"+std::string(MAJOR_NUMBER)+"."+std::string(MINOR_NUMBER)+"."+std::string(PATCH_NUMBER))

#define ARCHITECTURE_FILES_DIRECTORY_PATH qApp->applicationDirPath()+"/ArchitectureFiles/"
#define INSTRUCTION_LIST_FILES_DIRECTORY_PATH qApp->applicationDirPath()+"/InstructionListFiles/"

#endif // COMMON_H
