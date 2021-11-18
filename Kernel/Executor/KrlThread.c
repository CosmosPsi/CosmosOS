/**********************************************************
        线程文件KrlThread.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "RBTree.h"
#include "HalSync.h"
#include "HalCPU.h"
#include "HalMMU.h"
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

public void TRunEnvInit(TRunEnv* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
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

private Bool KrlExDelThreadKernelStack(Thread* thread)
{
    IF_NULL_RETURN_FALSE(thread);
    IF_NULL_RETURN_FALSE(thread->ThreadContext.KrlStackPmsad);
    IF_NEQ_DEAD(TRUE, KrlMmFreeKernPMSADs(thread->ThreadContext.KrlStackPmsad), "Free Kernel Stack Fail\n");
    INIT_OBJOFPTR_ZERO(&thread->ThreadContext);
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

private Bool KrlExDestroyThreadRealizeCore(Thread* thread)
{
    IF_NULL_RETURN_FALSE(thread);
    IF_NEQ_RETURN(TRUE, KrlTransferDel(&thread->ThreadTransfer), FALSE);
    IF_NEQ_RETURN(TRUE, KrlExDelThreadKernelStack(thread), FALSE);
    IF_NEQ_DEAD(TRUE, DelThread(thread), "Del Thread is Fail\n");
    return TRUE;
}

private Thread* KrlExCreateThreadRealize()
{
    return KrlExCreateThreadRealizeCore();
}

private Bool KrlExDestroyThreadRealize(Thread* thread)
{
    return KrlExDestroyThreadRealizeCore(thread);
}

public Thread* KrlExCreateThread()
{
    return KrlExCreateThreadRealize();
}

public Bool KrlExDestroyThread(Thread* thread)
{
    IF_NULL_RETURN_FALSE(thread);
    return KrlExDestroyThreadRealize(thread);
}

public Thread* KrlExGetCurrentThread()
{
    Transfer* transfer = NULL;
    transfer = KrlExGetCurrentTransfer();
    IF_NULL_RETURN_NULL(transfer);
    return KrlTrGetThreadForTransfer(transfer);
}

private Bool KrlExThreadInitKernelStackRealizeCore(Thread* thread, TRunEnv* env)
{
    Addr next = NULL;
    CPUFlg flags = 0;
    Addr userstack = 0;
    if(KERNEL_CPU_MODE == env->CPUMode)
    {
        flags = KMOD_EFLAGS;
        userstack = thread->ThreadContext.KrlStackTop;
    }
    else if(USER_CPU_MODE == env->CPUMode)
    {
        flags = UMOD_EFLAGS;
        userstack = USER_VIRTUAL_ADDRESS_END;
    }
    next = HalCPUInitContextRegisterInStack(thread->ThreadContext.KrlStackTop, 
                    thread->ThreadContext.KrlStackSize, env->RunStart, 
                        env->CPUMode, flags, userstack);
    IF_ZERO_RETURN_FALSE(next);
    thread->ThreadContext.NextStackAddr = next;
    thread->ThreadContext.NextStartAddr = env->RunStart;
    return TRUE;
}

private Bool KrlExThreadInitKernelStackRealize(Thread* thread, TRunEnv* env)
{
    IF_ZERO_RETURN_FALSE(thread->ThreadContext.KrlStackAddr);
    IF_ZERO_RETURN_FALSE(thread->ThreadContext.KrlStackTop);
    IF_ZERO_RETURN_FALSE(thread->ThreadContext.KrlStackSize);
    IF_NULL_RETURN_FALSE(thread->ThreadContext.KrlStackPmsad);
    IF_ZERO_RETURN_FALSE(env->RunStart);
    IF_ZERO_RETURN_FALSE(env->CPUMode);
    return KrlExThreadInitKernelStackRealizeCore(thread, env);
}

public Bool KrlExThreadInitKernelStack(Thread* thread, TRunEnv* env)
{
    IF_NULL_RETURN_FALSE(thread);
    IF_NULL_RETURN_FALSE(env);
    return KrlExThreadInitKernelStackRealize(thread, env);
}

private Bool KrlExThreadInitRunEnvRealizeCore(Thread* thread, TRunEnv* env)
{
    thread->ThreadContext.FunStartAddr = env->RunStart;
    thread->RunMode = env->CPUMode;
    return KrlExThreadInitKernelStack(thread, env);
}

private Bool KrlExThreadInitRunEnvRealize(Thread* thread, TRunEnv* env)
{
    IF_ZERO_RETURN_FALSE(env->RunStart);
    IF_ZERO_RETURN_FALSE(env->Flags);
    IF_ZERO_RETURN_FALSE(env->CPUMode);
    return KrlExThreadInitRunEnvRealizeCore(thread, env);
}

public Bool KrlExThreadInitRunEnv(Thread* thread, TRunEnv* env)
{
    IF_NULL_RETURN_FALSE(thread);
    IF_NULL_RETURN_FALSE(env);
    return KrlExThreadInitRunEnvRealize(thread, env); 
}

private Bool KrlExCreateThreadInitRunEnvRealizeCore(TRunEnv* env)
{
    Thread* thread = NULL;
    Bool rets = FALSE;

    thread = KrlExCreateThread();
    IF_NULL_RETURN_NULL(thread);
    rets = KrlExThreadInitRunEnv(thread, env);
    if(FALSE == rets)
    {
        KrlExDestroyThread(thread);
        return FALSE;
    }

    rets = KrlTransferAddDefault(&thread->ThreadTransfer);
    if(FALSE == rets)
    {
        KrlExDestroyThread(thread);
        return FALSE;
    }
    return TRUE;
}

private Bool KrlExCreateThreadInitRunEnvRealize(TRunEnv* env)
{
    IF_ZERO_RETURN_FALSE(env->RunStart);
    IF_ZERO_RETURN_FALSE(env->Flags);
    IF_ZERO_RETURN_FALSE(env->CPUMode);
    return KrlExCreateThreadInitRunEnvRealizeCore(env);
}

public Bool KrlExCreateThreadInitRunEnv(TRunEnv* env)
{
    IF_NULL_RETURN_NULL(env);
    return KrlExCreateThreadInitRunEnvRealize(env);
}

private void KrlExAfterThreadDoTransfer(Thread* curr, Thread* next)
{
    KrlExSetCurrentTransfer(&next->ThreadTransfer);

    //next->td_context.ctx_nexttss = &x64tss[cpuid];
    //x64tss[cpuid].rsp0 = next->td_krlstktop;
    //next->td_context.ctx_nexttss->rsp0 = next->td_krlstktop;

    HalMMULoad(&(next->Affiliation.ExecutorPtr->ExVMS.Mmu));
    if (next->RunStatus == THREAD_NEW_STATUS)
    {
        next->RunStatus = THREAD_RUN_STATUS;
        HalCPULoadContextRegisterToRun(next);
    }
    return;
}

private Bool KrlExThreadDoTransferRealizeCore(Thread* curr, Thread* next)
{
    Addr* outstack = NULL;
    Addr* instack = NULL;
    outstack = &(curr->ThreadContext.NextStackAddr);
    instack = &(next->ThreadContext.NextStackAddr);
    HalCPUSaveToNewContext(curr, next, outstack, instack, KrlExAfterThreadDoTransfer);
    return TRUE;    
}

