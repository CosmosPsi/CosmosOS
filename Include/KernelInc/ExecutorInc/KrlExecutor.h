/**********************************************************
        执行体头文件KrlExecutor.h
***********************************************************
                彭东
**********************************************************/
#ifndef _KRLEXECUTORHEAD
#define _KRLEXECUTORHEAD

#include "KrlExecutorManage.h"

// 执行体状态
// 初始状态
#define EXECUTOR_INIT_STATUS (0)
// 新建状态
#define EXECUTOR_NEW_STATUS (2)
// 运行状态
#define EXECUTOR_RUN_STATUS (4)
// 休眠状态
#define EXECUTOR_SLEEP_STATUS (8)
// 阻塞状态
#define EXECUTOR_BLOCK_STATUS (16)
// 等待状态
#define EXECUTOR_WAIT_STATUS (32)
// 死亡状态
#define EXECUTOR_DEAD_STATUS (64)
// 缓存状态
#define EXECUTOR_CACHE_STATUS (128)

// 执行体名称最大长度
#define EXNAME_CHARS_MAX (64)
// 执行体名称
typedef struct EXNAME
{
    Char *NamePtr;                  // 名称指针
    Char NameArr[EXNAME_CHARS_MAX]; // 名称字符数组
}ExName;

// 执行体资源集合
typedef struct EXRESOURCEBOX
{
    ExecutorRes *Res;
}ExResourceBox;

// 执行体隶属关系
typedef struct EXAFFILIATION
{
    UInt CPUID;                     // CPU ID
    UInt MNodeID;                   // machine node ID
    ExecutorBox *ExecutorBoxPtr;    // 执行组
    ExecutorNode *ExecutorNodePtr;  // 执行节点
    //User
    //Transfer
    //Group
}ExAffiliation;

// 线程头
typedef struct THREADHEAD
{
    UInt ThreadNR;
    List ThreadLists;
}ThreadHead;

// 线程集合
typedef struct THREADBOX
{
    ELock Lock;
    U64 Status;
    U64 Flags;
    UInt ThreadSumNR;
    ThreadHead NewHead;
    ThreadHead RunHead;
    ThreadHead SleepHead;
    ThreadHead BlockHead;
    ThreadHead WaitHead;
    ThreadHead DeadHead;
    ThreadHead CacheHead;
}ThreadBox;

// 执行体
typedef struct EXECUTOR
{
    List Lists;                // 列表
    ELock Lock;                // 锁
    U64 Status;                // 状态
    U64 Flags;                 // 标志位
    UInt Type;                 // 类型
    UInt RunStatus;            // 运行状态
    ExName Name;               // 名称
    ExAffiliation Affiliation; // 隶属关系
    ThreadBox ExThreadBox;     // 线程集合
    VMS ExVMS;                 // VMS
    ExResourceBox ResourceBox; // 资源集合
    void *Priv;                // 私有信息
    void *Ext;                 // 扩展信息
}Executor;

// 执行体名称初始化
// param@init 名称实例
private void ExNameInit(ExName *init);

// 执行体资源集合初始化
// param@init 实例
private void ExResourceBoxInit(ExResourceBox *init);

// 执行隶属关系初始化
// param@init 实例
private void ExAffiliationInit(ExAffiliation *init);

// 线程头初始化
// param@init 实例
private void ThreadHeadInit(ThreadHead *init);

// 线程集合初始化
// param@init 实例
private void ThreadBoxInit(ThreadBox *init);

// 执行体初始化
// param@init 实例
private void ExecutorInit(Executor *init);

// 新建执行体
// @return 执行体实例
private Executor *NewExecutor();

// 删除执行体
// @param executor 执行体
// @return 是否删除成功
private Bool DelExecutor(Executor *executor);

// TODO
private Executor *KrlExCreateExecutorRealizeCore(Executor *executor);

private Executor *KrlExCreateExecutorRealize();

public Executor *KrlExCreateExecutor();

private Bool KrlExDestroyExecutorRealizeCore(Executor *executor);

private Bool KrlExDestroyExecutorRealize(Executor *executor);

public Bool KrlExDestroyExecutor(Executor *executor);

public Bool KrlExSetAffiliationExNode(Executor *executor, ExecutorNode *exnode);

public Bool KrlExSetAffiliationExBox(Executor *executor, ExecutorBox *exbox);

public Executor *KrlExGetCPUIdleExecutor();

public Executor *KrlExGetCurrentRunExecutor();

public Bool KrlExSetCurrentRunExecutor(Executor *executor);

private Bool KrlExThreadAddToThreadBoxHead(Executor *executor, ThreadBox *box, ThreadHead *head, Thread *thread);

private Bool KrlExThreadDelOnThreadBoxHead(Executor *executor, ThreadBox *box, ThreadHead *head, Thread *thread);

private Bool KrlExThreadDelOnExecutorRealizeCore(Executor *executor, Thread *thread);

private Bool KrlExThreadAddToExecutorRealizeCore(Executor *executor, Thread *thread);

private Bool KrlExThreadAddToExecutorRealize(Executor *executor, Thread *thread);

private Bool KrlExThreadDelOnExecutorRealize(Executor *executor, Thread *thread);

public Bool KrlExThreadDelOnExecutor(Executor *executor, Thread *thread);

public Bool KrlExThreadAddToExecutor(Executor *executor, Thread *thread);
#endif