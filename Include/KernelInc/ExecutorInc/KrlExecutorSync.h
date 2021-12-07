/**********************************************************
        执行体同步头文件KrlExecutorSync.h
***********************************************************
                彭东
**********************************************************/
#ifndef _KRLEXECUTORSYNCHEAD
#define _KRLEXECUTORSYNCHEAD

typedef struct EWAITLIST
{
    UInt WaitNR;
    List Lists;
}EWaitList;

typedef struct ESYNC
{
    SPinLock Lock;
    RefCount LockCount;
    EWaitList WaitList;
}ESync;

typedef struct ESEM
{
    ESync Sync;
}Esem;

typedef struct EMUTEX
{
    ESync Sync;
}EMutex;

public void EWaitListInit(EWaitList* init);
public void ESyncInit(ESync* init);

#endif
