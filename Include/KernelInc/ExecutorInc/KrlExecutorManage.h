/**********************************************************
        执行体管理头文件KrlExecutorManage.h
***********************************************************
                彭东
**********************************************************/
#ifndef _KRLEXECUTORMANAGEHEAD
#define _KRLEXECUTORMANAGEHEAD
#include "HalSync.h"
#include "KrlExecutor.h"
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
    UInt ExecutorSumNR;
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
    Executor* CurrRunExecutor;
    Executor* CPUIdleExecutor;
    ExecutorBox ExecutorBoxor;
}ExecutorNode;

typedef struct GEXECUTORMANAGE
{
    ELock Lock;
	U64 Status;
	U64 Flags;
    ExecutorNode* DefaultExecutorNode;
    ExecutorNode* ExecutorNodePtrArr[EXECUTORNODE_MAX];
}GExecutorManage;

DefinedExecutorData(GExecutorManage, GExecutorManageData);
DefinedExecutorData(ExecutorNode, DefaultExecutorNode);

KLINE GExecutorManage* KrlExGetGExecutorManageDataAddr()
{
    return &GExecutorManageData;
}

KLINE ExecutorNode* KrlExGetDefaultExecutorNodeAddr()
{
    return &DefaultExecutorNode;
}

private void ELockInit(ELock* init);
private void ExecutorHeadInit(ExecutorHead* init);
private void ExecutorBoxInit(ExecutorBox* init);
private void ExecutorNodeInit(ExecutorNode* init);
private void GExecutorManageInit(GExecutorManage* init);
public void KrlExLocked(ELock* lock);
public void KrlExUnLock(ELock* lock);
private Bool KrlExAddExecutorToExecutorHead(GExecutorManage* gexem, ExecutorNode* exnode, ExecutorBox* box, ExecutorHead* exhead, Executor* executor);
private Bool KrlExDefaultAddExecutorRealizeCore(GExecutorManage* gexem, ExecutorNode* exnode, Executor* executor);
private Bool KrlExDefaultAddExecutorRealize(Executor* executor);
public Bool KrlExDefaultAddExecutor(Executor* executor);
private Bool KrlExAddCPUIdleExecutorRealizeCore(GExecutorManage* gexem, ExecutorNode* exnode, Executor* executor);
private Bool KrlExAddCPUIdleExecutorRealize(Executor* executor);
public Bool KrlExAddCPUIdleExecutor(Executor* executor);
public Bool KrlExecutorManageInit();
#endif
