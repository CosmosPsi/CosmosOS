/**********************************************************
        执行体头文件KrlExecutor.h
***********************************************************
                彭东
**********************************************************/
#ifndef _KRLEXECUTORHEAD
#define _KRLEXECUTORHEAD

#include "KrlExecutorManage.h"

typedef struct EXAFFILIATION
{
    UInt CPUID;
    UInt MNodeID;
    ExecutorBox* ExecutorBoxPtr;
    ExecutorNode* ExecutorNodePtr;
    //User
    //Transfer
    //Group
}ExAffiliation;

typedef struct THREADHEAD
{
     UInt ThreadNR;
     List ThreadLists;
}ThreadHead;

typedef struct THREADBOX
{
    ELock Lock;
	U64 Status;
	U64 Flags;
    UInt ThreadSumNR;
    ThreadHead NewHead;
    ThreadHead RunHead;
    ThreadHead SleepHead;
    ThreadHead BlockHead;
    ThreadHead WaitHead;
    ThreadHead DeadHead;
    ThreadHead CacheHead;
}ThreadBox;

typedef struct EXECUTOR
{
    List Lists;
    ELock Lock;
	U64 Status;
	U64 Flags;
    UInt Type;
	UInt RunStatus;
    ExAffiliation Affiliation;
    ThreadBox ExThreadBox;
    VMS ExeVMS;   
    void* Res;
    void* Priv;
    void* Ext;
}Executor;

#endif