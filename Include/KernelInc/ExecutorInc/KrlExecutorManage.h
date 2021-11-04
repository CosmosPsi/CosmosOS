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
#define DefinedExecutorData(vartype,varname) EXECUTORDATA_SECTION vartype varname

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

typedef struct EXECUTORBOX
{
    ELock Lock;
	U64 Status;
	U64 Flags;
    ExecutorHead NewHead;
    ExecutorHead RunHead;
    ExecutorHead SleepHead;
    ExecutorHead BlockHead;
    ExecutorHead WaitHead;
    ExecutorHead DeadHead;
    ExecutorHead CacheHead;
}ExecutorBox;

typedef struct EXECUTORNODE
{
    ELock Lock;
	U64 Status;
	U64 Flags;
    UInt ExecutorNR;
    ExecutorBox ExecutorBoxor;
}ExecutorNode;

typedef struct GEXECUTORMANAGE
{
    ELock Lock;
	U64 Status;
	U64 Flags;
    ExecutorNode DefaultExecutorNode;
    ExecutorNode ExecutorNodePtrArr[EXECUTORNODE_MAX];
}GExecutorManage;

KLINE GExecutorManage* KrlExGetGExecutorManageDataAddr()
{
    return &GExecutorManageData;
}

private void ELockInit(ELock* init);
public void KrlExLocked(ELock* lock);
public void KrlExUnLock(ELock* lock);
#endif
