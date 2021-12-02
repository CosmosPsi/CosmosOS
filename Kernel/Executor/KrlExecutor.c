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
#include "KrlThread.h"

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

private Bool DelExecutor(Executor* executor)
{
    IF_NULL_RETURN_FALSE(executor);
    return KrlMmDel((void*)executor);
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

private Bool KrlExDestroyExecutorRealizeCore(Executor* executor)
{
    IF_NULL_RETURN_FALSE(executor);
    KrlExLocked(&executor->Lock);
    
    if(0 < executor->ExThreadBox.ThreadSumNR)
    {
        KrlExUnLock(&executor->Lock);
        return FALSE;
    }
    
    if(RefCountRead(&executor->ResourceBox.Res->Count) > 0)
    {
        KrlExUnLock(&executor->Lock);
        return FALSE;
    }
    
    if(RefCountRead(&executor->ExVMS.Count) > 0)
    {
        KrlExUnLock(&executor->Lock);
        return FALSE;
    }

    KrlExUnLock(&executor->Lock);
    return DelExecutor(executor);
}

private Bool KrlExDestroyExecutorRealize(Executor* executor)
{
    if(EXECUTOR_DEAD_STATUS != executor->RunStatus && 
        EXECUTOR_NEW_STATUS != executor->RunStatus && 
        EXECUTOR_INIT_STATUS != executor->RunStatus)
    {
        return FALSE;
    }
    return KrlExDestroyExecutorRealizeCore(executor);
}

public Bool KrlExDestroyExecutor(Executor* executor)
{
    IF_NULL_RETURN_FALSE(executor);
    return KrlExDestroyExecutorRealize(executor);
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

private Bool KrlExThreadAddToThreadBoxHead(Executor* executor, ThreadBox* box, ThreadHead* head, Thread* thread)
{
    IF_NULL_RETURN_FALSE(executor);
    IF_NULL_RETURN_FALSE(box);
    IF_NULL_RETURN_FALSE(head);
    IF_NULL_RETURN_FALSE(thread);
    ListAdd(&thread->Lists, &head->ThreadLists);
    head->ThreadNR++;
    box->ThreadSumNR++;
    thread->Affiliation.ExecutorPtr = executor;
    return FALSE;
}

private Bool KrlExThreadDelOnThreadBoxHead(Executor* executor, ThreadBox* box, ThreadHead* head, Thread* thread)
{
    IF_NULL_RETURN_FALSE(executor);
    IF_NULL_RETURN_FALSE(box);
    IF_NULL_RETURN_FALSE(head);
    IF_NULL_RETURN_FALSE(thread);
    ListDel(&thread->Lists);
    head->ThreadNR--;
    box->ThreadSumNR--;
    thread->Affiliation.ExecutorPtr = NULL;
    return FALSE;
}

private Bool KrlExThreadDelOnExecutorRealizeCore(Executor* executor, Thread* thread)
{
    ThreadBox* box = NULL;
    Bool rets = FALSE;

    box = &executor->ExThreadBox;
    KrlExLocked(&executor->Lock);
    KrlExLocked(&box->Lock);
    switch(thread->RunStatus)
    {
    case THREAD_NEW_STATUS:
        rets = KrlExThreadDelOnThreadBoxHead(executor, box, &box->NewHead, thread);
        break;
    case THREAD_RUN_STATUS:
        rets = KrlExThreadDelOnThreadBoxHead(executor, box, &box->RunHead, thread);
        break;
    case THREAD_SLEEP_STATUS:
        rets = KrlExThreadDelOnThreadBoxHead(executor, box, &box->SleepHead, thread);
        break;
    case THREAD_BLOCK_STATUS:
        rets = KrlExThreadDelOnThreadBoxHead(executor, box, &box->BlockHead, thread);
        break;
    case THREAD_WAIT_STATUS:
        rets = KrlExThreadDelOnThreadBoxHead(executor, box, &box->WaitHead, thread);
        break;
    case THREAD_DEAD_STATUS:
        rets = KrlExThreadDelOnThreadBoxHead(executor, box, &box->DeadHead, thread);  
        break;
    default:
        rets = FALSE;
        break;
    }
    KrlExUnLock(&box->Lock);
    KrlExUnLock(&executor->Lock);
    return rets;
}

private Bool KrlExThreadAddToExecutorRealizeCore(Executor* executor, Thread* thread)
{
    ThreadBox* box = NULL;
    Bool rets = FALSE;

    box = &executor->ExThreadBox;
    KrlExLocked(&executor->Lock);
    KrlExLocked(&box->Lock);
    switch(thread->RunStatus)
    {
    case THREAD_NEW_STATUS:
        rets = KrlExThreadAddToThreadBoxHead(executor, box, &box->NewHead, thread);
        break;
    case THREAD_RUN_STATUS:
        rets = KrlExThreadAddToThreadBoxHead(executor, box, &box->RunHead, thread);
        break;
    case THREAD_SLEEP_STATUS:
        rets = KrlExThreadAddToThreadBoxHead(executor, box, &box->SleepHead, thread);
        break;
    case THREAD_BLOCK_STATUS:
        rets = KrlExThreadAddToThreadBoxHead(executor, box, &box->BlockHead, thread);
        break;
    case THREAD_WAIT_STATUS:
        rets = KrlExThreadAddToThreadBoxHead(executor, box, &box->WaitHead, thread);
        break;
    case THREAD_DEAD_STATUS:
        rets = KrlExThreadAddToThreadBoxHead(executor, box, &box->DeadHead, thread);  
        break;
    default:
        rets = FALSE;
        break;
    }
    KrlExUnLock(&box->Lock);
    KrlExUnLock(&executor->Lock);
    return rets;
}

private Bool KrlExThreadAddToExecutorRealize(Executor* executor, Thread* thread)
{
    return KrlExThreadAddToExecutorRealizeCore(executor, thread);
}

private Bool KrlExThreadDelOnExecutorRealize(Executor* executor, Thread* thread)
{
    return KrlExThreadDelOnExecutorRealizeCore(executor, thread);
}

public Bool KrlExThreadDelOnExecutor(Executor* executor, Thread* thread)
{
    IF_NULL_RETURN_FALSE(executor);
    IF_NULL_RETURN_FALSE(thread);
    return KrlExThreadDelOnExecutorRealize(executor, thread);
}

public Bool KrlExThreadAddToExecutor(Executor* executor, Thread* thread)
{
    IF_NULL_RETURN_FALSE(executor);
    IF_NULL_RETURN_FALSE(thread);
    return KrlExThreadAddToExecutorRealize(executor, thread);
}