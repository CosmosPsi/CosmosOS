/**********************************************************
        线程头文件KrlThread.h
***********************************************************
                彭东
**********************************************************/
#ifndef _KRLTHREADHEAD
#define _KRLTHREADHEAD

#define THREAD_KERNELSTACK_SIZE (0x2000)
#define THREAD_KERNELSTACK_PMSADNR (THREAD_KERNELSTACK_SIZE >> MSAD_PADR_SLBITS)

typedef struct TAFFILIATION
{
    UInt CPUID;
    UInt MNodeID;
    Executor* ExecutorPtr;
    //User
    //Transfer
    //Group
}TAffiliation;

typedef struct TCONTEXT
{
    Addr KrlStackAddr;
    Addr KrlStackTop;
    Size KrlStackSize;
    PMSAD* KrlStackPmsad;
    Addr FunStartAddr;
    Addr NextStartAddr;
    Addr NextStackAddr;
}TContext;

typedef struct THREAD
{
    List Lists;
    ELock Lock;
	U64 Status;
	U64 Flags;
    U64 ID;
	UInt RunStatus;
	UInt RunMode;
    TAffiliation Affiliation;
    Transfer ThreadTransfer;
    TContext ThreadContext;
    void* Priv;
    void* Ext;
}Thread;

typedef struct TRUNENV
{
    UInt Status;
    UInt Flags;
    Addr RunStart;
    U64 CPUMode;
    UInt Authority;
    UInt Priority;
}TRunEnv;

private void TAffiliationInit(TAffiliation* init);
private void TContextInit(TContext* init);
private void ThreadInit(Thread* init);
public void TRunEnvInit(TRunEnv* init);
private Bool DelThread(Thread* thread);
private Thread* NewThread();
private Bool KrlExNewThreadKernelStack(Thread* thread);
private Bool KrlExDelThreadKernelStack(Thread* thread);
private Thread* KrlExCreateThreadRealizeCore();
private Bool KrlExDestroyThreadRealizeCore(Thread* thread);
private Thread* KrlExCreateThreadRealize();
private Bool KrlExDestroyThreadRealize(Thread* thread);
public Thread* KrlExCreateThread();
public Thread* KrlExGetCurrentThread();
private Bool KrlExThreadInitKernelStackRealizeCore(Thread* thread, TRunEnv* env);
private Bool KrlExThreadInitKernelStackRealize(Thread* thread, TRunEnv* env);
public Bool KrlExThreadInitKernelStack(Thread* thread, TRunEnv* env);
private Bool KrlExThreadInitRunEnvRealizeCore(Thread* thread, TRunEnv* env);
private Bool KrlExThreadInitRunEnvRealize(Thread* thread, TRunEnv* env);
public Bool KrlExThreadInitRunEnv(Thread* thread, TRunEnv* env);

#endif