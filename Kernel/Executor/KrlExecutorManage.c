/**********************************************************
        执行体管理文件KrlExecutorManage.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "HalSync.h"
#include "KrlExecutorManage.h"
#include "KrlExecutor.h"

DefinedExecutorData(GExecutorManage, GExecutorManageData);
DefinedExecutorData(ExecutorNode, DefaultExecutorNode);

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

private void GExecutorManageInit(GExecutorManage* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ELockInit(&init->Lock);
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

private Bool KrlExAddExecutorToExecutorHead(GExecutorManage* gexem, ExecutorNode* exnode, ExecutorBox* box, ExecutorHead* exhead, Executor* executor)
{
    KrlExLocked(&box->Lock);
    ListAdd(&executor->Lists, &exhead->ExecutorLists);
    exhead->ExecutorNR++;
    box->ExecutorSumNR++;
    exnode->ExecutorNR++;
    KrlExUnLock(&box->Lock);
    KrlExSetAffiliationExBox(executor, box);
    KrlExSetAffiliationExNode(executor, exnode);
    return TRUE;
}

public Bool KrlExecutorManageInit()
{
    GExecutorManage* gexm = NULL;
    ExecutorNode* exnode = NULL;
    gexm = KrlExGetGExecutorManageDataAddr();
    IF_NULL_RETURN_FALSE(gexm);

    exnode = KrlExGetDefaultExecutorNodeAddr();
    IF_NULL_RETURN_FALSE(exnode);
    
    GExecutorManageInit(gexm);
    ExecutorNodeInit(exnode);

    gexm->DefaultExecutorNode = exnode;
    
    return TRUE;
}