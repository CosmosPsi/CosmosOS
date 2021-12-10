/**********************************************************
        执行体同步文件KrlExecutorSync.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "Atomic.h"
#include "List.h"
#include "HalSync.h"
#include "KrlThread.h"
#include "KrlExecutorSync.h"

private void EWaitListHeadInit(EWaitListHead* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->Lists);
    return;
}

public void EWaitListInit(EWaitList* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->Lists);
    return;
}

private void ESyncInit(ESync* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    SPinLockInit(&init->Lock);
    RefCountInit(&init->SyncCount);
    EWaitListHeadInit(&init->WaitListHead);
    return;
}

public void ESemInit(ESem* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ESyncInit(&init->Sync);
    return;
}

public void EMutexInit(EMutex* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ESyncInit(&init->Sync);
    RefCountInc(&init->Sync.SyncCount);
    return;
}

private Bool EWaitListAddToEWaitListHead(EWaitListHead* head, EWaitList* wait, void* thread)
{
    IF_NULL_RETURN_FALSE(head);
    IF_NULL_RETURN_FALSE(wait);
    ListAdd(&wait->Lists, &head->Lists);
    head->WaitNR++;
    wait->Parent = head;
    wait->Thread = thread;
    return TRUE;
}


private Bool KrlExEMutexLockedRealizeCore(EMutex* mutex)
{
    CPUFlg cpuflags = 0;
    IF_NULL_RETURN_FALSE(mutex);

    ESyncSelfLockedCli(&mutex->Sync, &cpuflags);
    if(ESyncCountIsEQTOne(&mutex->Sync) == TRUE)
    {
        if(ESyncCountDec(&mutex->Sync) == FALSE)
        {
            ESyncSelfUnLockSti(&mutex->Sync, &cpuflags);
            return FALSE;
        }
        ESyncSelfUnLockSti(&mutex->Sync, &cpuflags);
        return TRUE;
    }
    ESyncSelfUnLockSti(&mutex->Sync, &cpuflags);
    return FALSE;
}

private Bool KrlExEMutexLockedFailEntryWait(EMutex* mutex, EWaitList* wait, Thread* thread)
{
    IF_NULL_RETURN_FALSE(mutex);
    IF_NULL_RETURN_FALSE(wait);
    IF_NULL_RETURN_FALSE(thread);
    
    IF_NEQ_RETURN(TRUE, EWaitListAddToEWaitListHead(&mutex->Sync.WaitListHead, wait, (void*)thread), FALSE);
    return TRUE;
}

private Bool KrlExEMutexLockedWaitRealizeCore(EMutex* mutex)
{
    CPUFlg cpuflags = 0;
    Thread* thread = NULL;
    IF_NULL_RETURN_FALSE(mutex);

restart:
    ESyncSelfLockedCli(&mutex->Sync, &cpuflags);

    if(ESyncCountIsEQTZero(&mutex->Sync) == TRUE)
    {
        thread = KrlExGetCurrentThread();
        IF_NULL_RETURN_FALSE(thread);
        IF_NEQ_DEAD(TRUE, 
            KrlExEMutexLockedFailEntryWait(mutex, &thread->Affiliation.WaitList, thread), 
            "KrlExEMutexLockedFailEntryWait Is Fail\n");
        ESyncSelfUnLockSti(&mutex->Sync, &cpuflags);
        KrlExThreadWait(thread);
        goto restart;
    }

    if(ESyncCountIsEQTOne(&mutex->Sync) == TRUE)
    {
        if(ESyncCountDec(&mutex->Sync) == FALSE)
        {
            ESyncSelfUnLockSti(&mutex->Sync, &cpuflags);
            return FALSE;
        }
        ESyncSelfUnLockSti(&mutex->Sync, &cpuflags);
        return TRUE;
    }
    
    ESyncSelfUnLockSti(&mutex->Sync, &cpuflags);
    return FALSE;
}

private Bool KrlExEMutexLockedRealize(EMutex* mutex, UInt flags)
{
    if(MUTEX_FLG_NOWAIT == flags)
    {
        mutex->Flags = flags;
        return KrlExEMutexLockedRealizeCore(mutex);
    }
    else if(MUTEX_FLG_WAIT == flags)
    {
        mutex->Flags = flags;
        return KrlExEMutexLockedWaitRealizeCore(mutex);
    }
    return FALSE;
}

public Bool KrlExEMutexLocked(EMutex* mutex, UInt flags)
{
    IF_NULL_RETURN_FALSE(mutex);
    return KrlExEMutexLockedRealize(mutex, flags);
}

private Bool KrlExEMutexUnLockRealizeCore(EMutex* mutex)
{
    CPUFlg cpuflags = 0;
    IF_NULL_RETURN_FALSE(mutex);

    ESyncSelfLockedCli(&mutex->Sync, &cpuflags);
    if(ESyncCountIsEQTZero(&mutex->Sync) == TRUE)
    {
        if(ESyncCountInc(&mutex->Sync) == FALSE)
        {
            ESyncSelfUnLockSti(&mutex->Sync, &cpuflags);
            return FALSE;
        }
        ESyncSelfUnLockSti(&mutex->Sync, &cpuflags);
        return TRUE;
    }
    
    ESyncSelfUnLockSti(&mutex->Sync, &cpuflags);
    return FALSE;
}

private Bool KrlExEMutexUnLockAwakenEntry(EMutex* mutex)
{
    EWaitListHead* head = NULL;
    EWaitList* wait = NULL;
    List* list = NULL;
    IF_NULL_RETURN_FALSE(mutex);
    
    head = &mutex->Sync.WaitListHead;

    IF_LTN_RETURN(head->WaitNR, 1, TRUE);
restart:
    ListForEach(list, &head->Lists)
    {
        wait = ListEntry(list, EWaitList, Lists);
        if(NULL != wait->Thread)
        {
            ListDel(&wait->Lists);
            wait->Parent = NULL;
            KrlExThreadAwaken((Thread*)wait->Thread);
            goto restart;
        }
    }
    return TRUE;
}

private Bool KrlExEMutexUnLockAwakenRealizeCore(EMutex* mutex)
{
    CPUFlg cpuflags = 0;
    IF_NULL_RETURN_FALSE(mutex);

    ESyncSelfLockedCli(&mutex->Sync, &cpuflags);
    if(ESyncCountIsEQTZero(&mutex->Sync) == TRUE)
    {
        if(ESyncCountInc(&mutex->Sync) == FALSE)
        {
            ESyncSelfUnLockSti(&mutex->Sync, &cpuflags);
            return FALSE;
        }
        KrlExEMutexUnLockAwakenEntry(mutex);
        ESyncSelfUnLockSti(&mutex->Sync, &cpuflags);
        return TRUE;
    }
    
    ESyncSelfUnLockSti(&mutex->Sync, &cpuflags);
    return FALSE;
}

private Bool KrlExEMutexUnLockRealize(EMutex* mutex, UInt flags)
{
    if(MUTEX_FLG_NOWAIT == flags)
    {
        IF_NEQ_DEAD(MUTEX_FLG_NOWAIT, mutex->Flags, "mutex->Flags Is Fail\n");
        return KrlExEMutexUnLockRealizeCore(mutex);
    }
    else if(MUTEX_FLG_WAIT == flags)
    {
        IF_NEQ_DEAD(MUTEX_FLG_WAIT, mutex->Flags, "mutex->Flags Is Fail\n");
        return KrlExEMutexUnLockAwakenRealizeCore(mutex);
    }
    return FALSE;
}

public Bool KrlExEMutexUnLock(EMutex* mutex, UInt flags)
{
    IF_NULL_RETURN_FALSE(mutex);
    return KrlExEMutexUnLockRealize(mutex, flags);
}

private Bool KrlExESemObtainFailEntryWait(ESem* sem, EWaitList* wait, Thread* thread)
{
    IF_NULL_RETURN_FALSE(sem);
    IF_NULL_RETURN_FALSE(wait);
    IF_NULL_RETURN_FALSE(thread);
    
    IF_NEQ_RETURN(TRUE, EWaitListAddToEWaitListHead(&sem->Sync.WaitListHead, wait, (void*)thread), FALSE);
    return TRUE;
}

private Bool KrlExESemObtainRealizeCore(ESem* sem)
{
    CPUFlg cpuflags = 0;
    IF_NULL_RETURN_FALSE(sem);

    ESyncSelfLockedCli(&sem->Sync, &cpuflags);
    if(ESyncCountIsGTNZero(&sem->Sync) == TRUE)
    {
        if(ESyncCountDec(&sem->Sync) == FALSE)
        {
            ESyncSelfUnLockSti(&sem->Sync, &cpuflags);
            return FALSE;
        }
        ESyncSelfUnLockSti(&sem->Sync, &cpuflags);
        return TRUE;
    }
    ESyncSelfUnLockSti(&sem->Sync, &cpuflags);
    return FALSE;
}

private Bool KrlExESemObtainWaitRealizeCore(ESem* sem)
{
    CPUFlg cpuflags = 0;
    Thread* thread = NULL;
    IF_NULL_RETURN_FALSE(sem);

restart:
    ESyncSelfLockedCli(&sem->Sync, &cpuflags);

    if(ESyncCountIsEQTZero(&sem->Sync) == TRUE)
    {
        thread = KrlExGetCurrentThread();
        IF_NULL_RETURN_FALSE(thread);
        IF_NEQ_DEAD(TRUE, 
            KrlExESemObtainFailEntryWait(sem, &thread->Affiliation.WaitList, thread), 
            "KrlExESemObtainFailEntryWait Is Fail\n");
        ESyncSelfUnLockSti(&sem->Sync, &cpuflags);
        KrlExThreadWait(thread);
        goto restart;
    }

    if(ESyncCountIsGTNZero(&sem->Sync) == TRUE)
    {
        if(ESyncCountDec(&sem->Sync) == FALSE)
        {
            ESyncSelfUnLockSti(&sem->Sync, &cpuflags);
            return FALSE;
        }
        ESyncSelfUnLockSti(&sem->Sync, &cpuflags);
        return TRUE;
    }
    ESyncSelfUnLockSti(&sem->Sync, &cpuflags);
    return FALSE;
}

private Bool KrlExESemObtainRealize(ESem* sem, UInt flags)
{
    if(MUTEX_FLG_NOWAIT == flags)
    {
        sem->Flags = flags;
        return KrlExESemObtainRealizeCore(sem);
    }
    else if(MUTEX_FLG_WAIT == flags)
    {
        sem->Flags = flags;
        return KrlExESemObtainWaitRealizeCore(sem);
    }
    return FALSE;
}
