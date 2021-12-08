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
    RefCount SyncCount;
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

KLINE Bool ESyncCountIsGTNZero(ESync* sync)
{
    IF_NULL_RETURN_FALSE(sync);
    IF_GTN_RETURN(RefCountRead(&sync->SyncCount), 0, TRUE);
    return FALSE;
}

private void EWaitListInit(EWaitList* init);
private void ESyncInit(ESync* init);
public void EsemInit(Esem* init);
public void EMutexInit(EMutex* init);

#endif
