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
#include "KrlExecutorRes.h"
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

private Executor* KrlExCreateExecutorRealizeCore(Executor* executor)
{
    Bool rets = FALSE;
    ExecutorRes* res = NULL;
    IF_NULL_RETURN_NULL(executor);
    res = KrlExGetExecutorRes();
    IF_NULL_RETURN_FALSE(res);
    executor->ResourceBox.Res = res;
    rets = KrlExDefaultAddExecutor(executor);
    IF_EQT_RETURN(FALSE, rets, NULL);
    return executor;
}

private Executor* KrlExCreateExecutorRealize()
{
    Executor* executor = NULL;
    executor = NewExecutor();
    IF_NULL_RETURN_NULL(executor);
    return KrlExCreateExecutorRealizeCore(executor);
}

public Executor* KrlExCreateExecutor()
{
    return KrlExCreateExecutorRealize();
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

public Executor* KrlExGetCPUIdleExecutor()
{
    UInt cpuid = 0;
    GExecutorManage* gexm = NULL;
    ExecutorNode* exnode = NULL;
    Executor* executor = NULL;
    
    gexm = KrlExGetGExecutorManageDataAddr();
    IF_NULL_RETURN_FALSE(gexm);

    cpuid = HalCPUID();
    IF_GTN_DEAD(cpuid, (EXECUTORNODE_MAX - 1), "CPUID GNT EXECUTORNODE_MAX\n");

    exnode = gexm->ExecutorNodePtrArr[cpuid];
    IF_EQT_DEAD(NULL, exnode, "Executor node is NULL\n");
    executor = exnode->CPUIdleExecutor;
    IF_EQT_DEAD(NULL, executor, "CPUIdle Executor is NULL\n");
    return executor;
}

public Executor* KrlExGetCurrentRunExecutor()
{
    UInt cpuid = 0;
    GExecutorManage* gexm = NULL;
    ExecutorNode* exnode = NULL;
    Executor* executor = NULL;
    
    gexm = KrlExGetGExecutorManageDataAddr();
    IF_NULL_RETURN_FALSE(gexm);

    cpuid = HalCPUID();
    IF_GTN_DEAD(cpuid, (EXECUTORNODE_MAX - 1), "CPUID GNT EXECUTORNODE_MAX\n");

    exnode = gexm->ExecutorNodePtrArr[cpuid];
    IF_EQT_DEAD(NULL, exnode, "Executor node is NULL\n");
    executor = exnode->CurrRunExecutor;
    IF_EQT_DEAD(NULL, executor, "Current Run Executor is NULL\n");
    return executor;
}

public Bool KrlExSetCurrentRunExecutor(Executor* executor)
{
    UInt cpuid = 0;
    GExecutorManage* gexm = NULL;
    ExecutorNode* exnode = NULL;
    IF_NULL_RETURN_FALSE(executor);
    cpuid = executor->Affiliation.CPUID;
    IF_GTN_RETURN(cpuid, (EXECUTORNODE_MAX - 1), FALSE);

    gexm = KrlExGetGExecutorManageDataAddr();
    IF_NULL_RETURN_FALSE(gexm);

    exnode = gexm->ExecutorNodePtrArr[cpuid];
    IF_NULL_RETURN_FALSE(exnode);

    KrlExLocked(&exnode->Lock);
    exnode->CurrRunExecutor = executor; 
    KrlExUnLock(&exnode->Lock);

    return TRUE;
}
