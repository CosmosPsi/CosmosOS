/**********************************************************
        执行体同步文件KrlExecutorSync.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "Atomic.h"
#include "List.h"
#include "HalSync.h"
#include "KrlExecutorSync.h"

private void EWaitListInit(EWaitList* init)
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
    EWaitListInit(&init->WaitList);
    return;
}

public void EsemInit(Esem* init)
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
    return;
}

private Bool KrlExEMutexLockedRealizeCore(EMutex* mutex)
{
    CPUFlg cpuflags = 0;
    IF_NULL_RETURN_FALSE(mutex);

    ESyncSelfLockedCli(&mutex->Sync, &cpuflags);
    if(ESyncCountIsEQTOne(&mutex->Sync) == FALSE)
    {
        ESyncSelfUnLockSti(&mutex->Sync, &cpuflags);
        return FALSE;
    }
    
    if(ESyncCountDec(&mutex->Sync) == FALSE)
    {
        ESyncSelfUnLockSti(&mutex->Sync, &cpuflags);
        return FALSE;
    }
    
    ESyncSelfUnLockSti(&mutex->Sync, &cpuflags);
    return TRUE;
}
