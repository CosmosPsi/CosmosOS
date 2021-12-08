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

KLINE void ESyncSelfLocked(ESync* sync)
{
    IF_NULL_DEAD(sync);
    HalSPinLock(&sync->Lock);
    return;
}

KLINE void ESyncSelfLockedCli(ESync* sync, CPUFlg* flags)
{
    IF_NULL_DEAD(sync);
    IF_NULL_DEAD(flags);
    HalSPinLockSaveFlagsCli(&sync->Lock, flags);
    return;
}

KLINE void ESyncSelfUnLock(ESync* sync)
{
    IF_NULL_DEAD(sync);
    HalUnSPinLock(&sync->Lock);
    return;
}

KLINE void ESyncSelfUnLockSti(ESync* sync, CPUFlg* flags)
{
    IF_NULL_DEAD(sync);
    IF_NULL_DEAD(flags);
    HalUnSPinLockRestoreFlagsSti(&sync->Lock, flags);
    return;
}

KLINE Bool ESyncCountIsGTNZero(ESync* sync)
{
    IF_NULL_RETURN_FALSE(sync);
    IF_GTN_RETURN(RefCountRead(&sync->SyncCount), 0, TRUE);
    return FALSE;
}

KLINE Bool ESyncCountIsEQTZero(ESync* sync)
{
    IF_NULL_RETURN_FALSE(sync);
    IF_EQT_RETURN(RefCountRead(&sync->SyncCount), 0, TRUE);
    return FALSE;
}

KLINE Bool ESyncCountIsEQTOne(ESync* sync)
{
    IF_NULL_RETURN_FALSE(sync);
    IF_EQT_RETURN(RefCountRead(&sync->SyncCount), 1, TRUE);
    return FALSE;
}

KLINE Bool ESyncCountInc(ESync* sync)
{
    IF_NULL_RETURN_FALSE(sync);
    RefCountInc(&sync->SyncCount);
    return TRUE;
}

KLINE Bool ESyncCountDec(ESync* sync)
{
    IF_NULL_RETURN_FALSE(sync);
    RefCountDec(&sync->SyncCount);
    return TRUE;
}

private void EWaitListInit(EWaitList* init);
private void ESyncInit(ESync* init);
public void EsemInit(Esem* init);
public void EMutexInit(EMutex* init);
private Bool KrlExEMutexLockedRealizeCore(EMutex* mutex);
private Bool KrlExEMutexLockedRealize(EMutex* mutex);

#endif
