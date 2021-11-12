/**********************************************************
        线程文件KrlThread.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "RBTree.h"
#include "HalSync.h"
#include "KrlMmManage.h"
#include "KrlMmAlloc.h"
#include "KrlMmPool.h"
#include "KrlMmVmem.h"
#include "KrlExecutorManage.h"
#include "KrlExecutor.h"
#include "KrlExecutorRes.h"
#include "KrlTransfer.h"
#include "KrlThread.h"
#include "KrlLog.h"

private void TAffiliationInit(TAffiliation* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    return;
}

private void TContextInit(TContext* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    return;
}

private void ThreadInit(Thread* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->Lists);
    ELockInit(&init->Lock);
    TAffiliationInit(&init->Affiliation);
    TransferInitForThread(&init->ThreadTransfer, init);
    TContextInit(&init->ThreadContext);
    return;
}

private Bool DelThread(Thread* thread)
{
    IF_NULL_RETURN_FALSE(thread);
    return KrlMmDel((void*)thread);
}

private Thread* NewThread()
{
    Thread* thread = NULL;
    thread = (Thread*)KrlMmNew(sizeof(Thread));
    IF_NULL_RETURN_NULL(thread);
    ThreadInit(thread);
    return thread;
}

private Bool KrlExNewThreadKernelStack(Thread* thread)
{
    Addr kstack = NULL;
    PMSAD* msad = NULL; 
    IF_NULL_RETURN_FALSE(thread);
    msad = KrlMmAllocKernPMSADs(THREAD_KERNELSTACK_PMSADNR);
    IF_NULL_RETURN_FALSE(msad);
    kstack = PMSADRetVAddr(msad);
    IF_NULL_RETURN_FALSE(kstack);
    thread->ThreadContext.KrlStackAddr = kstack;
    thread->ThreadContext.KrlStackSize = THREAD_KERNELSTACK_SIZE;
    thread->ThreadContext.KrlStackTop = ((kstack + (THREAD_KERNELSTACK_SIZE - 1)) & ~0x0fUL);
    thread->ThreadContext.KrlStackPmsad = msad;
    return TRUE;
}

private Thread* KrlExCreateThreadRealizeCore()
{
    Thread* thread = NULL;
    Bool rets = FALSE;
    thread = NewThread();
    IF_NULL_RETURN_NULL(thread);
    rets = KrlExNewThreadKernelStack(thread);
    if(FALSE == rets)
    {
        if(DelThread(thread) == FALSE)
        {
            KrlErrorCrashDead("KrlExCreateThreadRealizeCore is Fail\n");            
        }
        return NULL;
    }
    return thread;
}

private Thread* KrlExCreateThreadRealize()
{
    return KrlExCreateThreadRealizeCore();
}

public Thread* KrlExCreateThread()
{
    return KrlExCreateThreadRealize();
} 