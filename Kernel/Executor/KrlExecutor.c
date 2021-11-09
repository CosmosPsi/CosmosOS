/**********************************************************
        执行体文件KrlExecutor.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "HalSync.h"
#include "KrlMmManage.h"
#include "KrlMmPool.h"
#include "KrlMmVmem.h"
#include "KrlExecutorManage.h"
#include "KrlExecutor.h"

private void ExNameInit(ExName* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    return;
}

private void ExResourceBoxInit(ExResourceBox* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    return;
}

private void ExAffiliationInit(ExAffiliation* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    return;
}

private void ThreadHeadInit(ThreadHead* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->ThreadLists);
    return;
}

private void ThreadBoxInit(ThreadBox* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ELockInit(&init->Lock);
    ThreadHeadInit(&init->NewHead);
    ThreadHeadInit(&init->RunHead);
    ThreadHeadInit(&init->SleepHead);
    ThreadHeadInit(&init->BlockHead);
    ThreadHeadInit(&init->WaitHead);
    ThreadHeadInit(&init->DeadHead);
    ThreadHeadInit(&init->CacheHead);
    return;
}

private void ExecutorInit(Executor* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->Lists);
    ELockInit(&init->Lock);
    ExNameInit(&init->Name);
    ExAffiliationInit(&init->Affiliation);
    ThreadBoxInit(&init->ExThreadBox);
    VMSInit(&init->ExVMS);
    ExResourceBoxInit(&init->ResourceBox);
    return;
}

private Executor* NewExecutor()
{
    Executor* executor = NULL;

    executor = (Executor*)KrlMmNew(sizeof(Executor));
    IF_NULL_RETURN_NULL(executor);
    
    ExecutorInit(executor);
    return executor;
}

public Bool KrlExSetAffiliationExNode(Executor* executor, ExecutorNode* exnode)
{
    IF_NULL_RETURN_FALSE(executor);
    IF_NULL_RETURN_FALSE(exnode);
    KrlExLocked(&executor->Lock);
    executor->Affiliation.ExecutorNodePtr = exnode;
    KrlExUnLock(&executor->Lock);
    return TRUE;
}

public Bool KrlExSetAffiliationExBox(Executor* executor, ExecutorBox* exbox)
{
    IF_NULL_RETURN_FALSE(executor);
    IF_NULL_RETURN_FALSE(exbox);
    KrlExLocked(&executor->Lock);
    executor->Affiliation.ExecutorBoxPtr = exbox;
    KrlExUnLock(&executor->Lock);
    return TRUE;
}
