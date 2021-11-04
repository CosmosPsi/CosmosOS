/**********************************************************
        执行体管理文件KrlExecutorManage.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "HalSync.h"
#include "KrlExecutorManage.h"

DefinedExecutorData(GExecutorManage, GExecutorManageData);

private void ELockInit(ELock* init)
{
    IF_NULL_RETURN(init);
    SPinLockInit(&init->Locks);
    return;
}

private void ExecutorHeadInit(ExecutorHead* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->ExecutorLists);
    return;
}

private void ExecutorBoxInit(ExecutorBox* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ELockInit(&init->Lock);
    ExecutorHeadInit(&init->NewHead);
    ExecutorHeadInit(&init->RunHead);
    ExecutorHeadInit(&init->SleepHead);
    ExecutorHeadInit(&init->BlockHead);
    ExecutorHeadInit(&init->WaitHead);
    ExecutorHeadInit(&init->DeadHead);
    ExecutorHeadInit(&init->CacheHead);
    return;
}

private void ExecutorNodeInit(ExecutorNode* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ELockInit(&init->Lock);
    ExecutorBoxInit(&init->ExecutorBoxor);
    return;
}

public void KrlExLocked(ELock* lock)
{
    IF_NULL_DEAD(lock);
    HalSPinLock(&lock->Locks);
    return;
}

public void KrlExUnLock(ELock* lock)
{
    IF_NULL_DEAD(lock);
    HalUnSPinLock(&lock->Locks);
    return;
}

