/**********************************************************
        执行体管理头文件KrlExecutorManage.h
***********************************************************
                彭东
**********************************************************/
#ifndef _KRLEXECUTORMANAGEHEAD
#define _KRLEXECUTORMANAGEHEAD
#include "HalSync.h"

#define EXECUTORNODE_MAX (64)
#define EXECUTORDATA_SECTION __attribute__((section(".executor.data")))
#define DefinedEXECUTORData(vartype,varname) EXECUTORDATA_SECTION vartype varname

typedef struct ELOCK
{
    SPinLock Locks;
    // SEM
}ELock;

typedef struct EXECUTORHEAD
{
     UInt ExecutorNR;
     List ExecutorLists;
}ExecutorHead;

#endif
