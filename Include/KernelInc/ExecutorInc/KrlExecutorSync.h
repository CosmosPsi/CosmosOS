/**********************************************************
        执行体同步头文件KrlExecutorSync.h
***********************************************************
                彭东
**********************************************************/
#ifndef _KRLEXECUTORSYNCHEAD
#define _KRLEXECUTORSYNCHEAD

#define MUTEX_FLG_NOWAIT (0)
#define MUTEX_FLG_WAIT (1)

#define SEM_FLG_NOWAIT (0)
#define SEM_FLG_WAIT (1)

typedef struct EWAITLISTHEAD
{
    UInt WaitNR;
    List Lists;
}EWaitListHead;

typedef struct EWAITLIST
{
    List Lists;
    EWaitList* Parent;
    void* Thread;
}EWaitList;

typedef struct ESYNC
{
    SPinLock Lock;
    RefCount SyncCount;
    EWaitListHead WaitListHead;
}ESync;

typedef struct ESEM
{
    UInt Flags;
    ESync Sync;
}ESem;

typedef struct EMUTEX
{
    UInt Flags;
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

private void EWaitListHeadInit(EWaitListHead* init);
public void EWaitListInit(EWaitList* init);
private void ESyncInit(ESync* init);
public void ESemInit(ESem* init);
public void EMutexInit(EMutex* init);
private Bool EWaitListAddToEWaitListHead(EWaitListHead* head, EWaitList* wait, void* thread);
private Bool KrlExEMutexLockedRealizeCore(EMutex* mutex);
private Bool KrlExEMutexLockedFailEntryWait(EMutex* mutex, EWaitList* wait, Thread* thread);
private Bool KrlExEMutexLockedWaitRealizeCore(EMutex* mutex);
private Bool KrlExEMutexLockedRealize(EMutex* mutex, UInt flags);
public Bool KrlExEMutexLocked(EMutex* mutex, UInt flags);
private Bool KrlExEMutexUnLockRealizeCore(EMutex* mutex);
private Bool KrlExEMutexUnLockAwakenEntry(EMutex* mutex);
private Bool KrlExEMutexUnLockAwakenRealizeCore(EMutex* mutex);
private Bool KrlExEMutexUnLockRealize(EMutex* mutex, UInt flags);
public Bool KrlExEMutexUnLock(EMutex* mutex, UInt flags);
private Bool KrlExESemObtainFailEntryWait(ESem* sem, EWaitList* wait, Thread* thread);

#endif
