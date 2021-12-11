/**********************************************************
        线程头文件KrlThread.h
***********************************************************
                彭东
**********************************************************/
#ifndef _KRLTHREADHEAD
#define _KRLTHREADHEAD

// 线程栈大小
#define THREAD_KERNELSTACK_SIZE (0x2000)
// TODO
#define THREAD_KERNELSTACK_PMSADNR (THREAD_KERNELSTACK_SIZE >> MSAD_PADR_SLBITS)

// 线程状态
// 初始状态
#define THREAD_INIT_STATUS (0)
// 新建状态
#define THREAD_NEW_STATUS (2)
// 运行状态
#define THREAD_RUN_STATUS (4)
// 休眠状态
#define THREAD_SLEEP_STATUS (8)
// 阻塞状态
#define THREAD_BLOCK_STATUS (16)
// 等待状态
#define THREAD_WAIT_STATUS (32)
// 死亡状态
#define THREAD_DEAD_STATUS (64)

// 线程隶属关系
typedef struct TAFFILIATION
{
    UInt CPUID;            // CPU ID
    UInt MNodeID;          // machine Node ID
    Executor* ExecutorPtr; // 执行体指针
    EWaitList WaitList;    // 等待列表
    //User
    //Transfer
    //Group
}TAffiliation;

// 线程上下文
typedef struct TCONTEXT
{
    Addr KrlStackAddr;    // 栈地址
    Addr KrlStackTop;     // 栈顶地址
    Size KrlStackSize;    // 栈大小
    PMSAD* KrlStackPmsad; // TODO
    Addr FunStartAddr;    // 运行函数地址
    Addr NextStartAddr;   // 下一个运行地址
    Addr NextStackAddr;   // 下一个栈地址
    X64TSS* Tss;          // TSS
}TContext;

// 线程
typedef struct THREAD
{
    List Lists;               // 线程列表
    ELock Lock;               // 锁
	U64 Status;               // 状态
	U64 Flags;                // 标志位
    U64 ID;                   // ID
	UInt RunStatus;           // 运行状态
	UInt RunMode;             // 运行模式
    TAffiliation Affiliation; // 隶属关系
    Transfer ThreadTransfer;  // 调度转移
    TContext ThreadContext;   // 上下文
    void* Priv;               // 前一个线程
    void* Ext;                // 后一个线程
}Thread;

// 线程运行环境变量
typedef struct TRUNENV
{
    UInt Status;              // 状态
    UInt Flags;               // 标志位
    Executor* AffiExecutor;   // 隶属关系
    Addr RunStart;            // 运行地址
    U64 CPUMode;              // CPU模式
    UInt Authority;           // 授权
    UInt Priority;            // 优先级
}TRunEnv;

// 隶属关系初始化
// @param init 实例
private void TAffiliationInit(TAffiliation* init);

// 上下文初始化
// @param init 实例
private void TContextInit(TContext* init);

// 线程初始化
// @param init 实例
private void ThreadInit(Thread* init);

// 运行环境初始化
// @param init 实例
public void TRunEnvInit(TRunEnv* init);

// 删除线程
// @param thread 线程
// @return 是否删除成功
private Bool DelThread(Thread* thread);

// 新建线程
// @return 线程
private Thread* NewThread();

// TODO
private Bool KrlExNewThreadKernelStack(Thread* thread);

private Bool KrlExDelThreadKernelStack(Thread* thread);

private Thread* KrlExCreateThreadRealizeCore();

private Bool KrlExDestroyThreadRealizeCore(Thread* thread);

private Thread* KrlExCreateThreadRealize();

private Bool KrlExDestroyThreadRealize(Thread* thread);

public Thread* KrlExCreateThread();

public Bool KrlExDestroyThread(Thread* thread);

public Thread* KrlExGetCurrentThread();

private Bool KrlExThreadInitKernelStackRealizeCore(Thread* thread, TRunEnv* env);

private Bool KrlExThreadInitKernelStackRealize(Thread* thread, TRunEnv* env);

public Bool KrlExThreadInitKernelStack(Thread* thread, TRunEnv* env);

private Bool KrlExThreadInitRunEnvRealizeCore(Thread* thread, TRunEnv* env);

private Bool KrlExThreadInitRunEnvRealize(Thread* thread, TRunEnv* env);

public Bool KrlExThreadInitRunEnv(Thread* thread, TRunEnv* env);

private Bool KrlExCreateThreadInitRunEnvRealizeCore(TRunEnv* env);

private Bool KrlExCreateThreadInitRunEnvRealize(TRunEnv* env);

public Bool KrlExCreateThreadInitRunEnv(TRunEnv* env);

private void KrlExAfterThreadDoTransfer(Thread* curr, Thread* next);

private Bool KrlExThreadDoTransferRealizeCore(Thread* curr, Thread* next);

private Bool KrlExThreadDoTransferRealize(Thread* curr, Thread* next);

public Bool KrlExThreadDoTransfer(Thread* curr, Thread* next);

private Bool KrlExThreadSetRunStatusRealizeCore(Thread* thread, UInt status);

private Bool KrlExThreadSetRunStatusRealize(Thread* thread, UInt status);

public Bool KrlExThreadSetRunStatus(Thread* thread, UInt status);

private Bool KrlExSetThreadStatusAndTransfer(Thread* thread, UInt status);

private Bool KrlExThreadWaitRealizeCore(Thread* thread);

private Bool KrlExThreadWaitRealize(Thread* thread);

public Bool KrlExThreadWait(Thread* thread);

private Bool KrlExThreadSleepRealizeCore(Thread* thread);

private Bool KrlExThreadSleepRealize(Thread* thread);

public Bool KrlExThreadSleep(Thread* thread);

private Bool KrlExThreadBlockRealizeCore(Thread* thread);

private Bool KrlExThreadBlockRealize(Thread* thread);

public Bool KrlExThreadBlock(Thread* thread);

private Bool KrlExThreadDeadRealizeCore(Thread* thread);

private Bool KrlExThreadDeadRealize(Thread* thread);

public Bool KrlExThreadDead(Thread* thread);

private Bool KrlExThreadRunRealizeCore(Thread* thread);

private Bool KrlExThreadRunRealize(Thread* thread);

public Bool KrlExThreadRun(Thread* thread);

private Bool KrlExThreadAwakenRealizeCore(Thread* thread);

private Bool KrlExThreadAwakenRealize(Thread* thread);

public Bool KrlExThreadAwaken(Thread* thread);

#endif