/**********************************************************
        执行体管理头文件KrlExecutorManage.h
***********************************************************
                彭东
**********************************************************/
#ifndef _KRLEXECUTORMANAGEHEAD
#define _KRLEXECUTORMANAGEHEAD
#include "HalSync.h"
#include "KrlExecutor.h"

// 最大执行体个数
#define EXECUTORNODE_MAX (64)
// 执行体只读数据段
#define EXECUTORDATA_SECTION __attribute__((section(".executor.data")))
// 定义全局执行体变量
#define DefinedExecutorData(vartype,varname) EXECUTORDATA_SECTION vartype varname

// 执行体锁
typedef struct ELOCK
{
    SPinLock Locks;
    // SEM
}ELock;

// 执行体头部信息
typedef struct EXECUTORHEAD
{
     UInt ExecutorNR;     // TODO
     List ExecutorLists;  // 执行列表
}ExecutorHead;

// 执行体集合
typedef struct EXECUTORBOX
{
    ELock Lock;             // 锁
	U64 Status;             // 状态
	U64 Flags;              // 标志位
    UInt ExecutorSumNR;     // TODO
    ExecutorHead NewHead;   // 新建状态执行体头部
    ExecutorHead RunHead;   // 运行状态执行体头部
    ExecutorHead SleepHead; // 休眠状态执行体头部
    ExecutorHead BlockHead; // 阻塞状态执行体头部
    ExecutorHead WaitHead;  // 等待状态执行体头部
    ExecutorHead DeadHead;  // 死亡状态执行体头部
    ExecutorHead CacheHead; // 缓存状态执行体头部
}ExecutorBox;

// 执行体节点
typedef struct EXECUTORNODE
{
    ELock Lock;                 // 锁
	U64 Status;                 // 状态
	U64 Flags;                  // 标志位
    UInt ExecutorNR;            // TODO
    Executor* CurrRunExecutor;  // 正在运行执行体
    Executor* CPUIdleExecutor;  // CPU常驻执行体
    ExecutorBox ExecutorBoxor;  // 执行体集合
}ExecutorNode;

// 执行体管理器
typedef struct GEXECUTORMANAGE
{
    ELock Lock;                                         // 锁
	U64 Status;                                         // 状态
	U64 Flags;                                          // 标志位
    ExecutorNode* DefaultExecutorNode;                  // 默认执行节点
    ExecutorNode* ExecutorNodePtrArr[EXECUTORNODE_MAX]; // 执行体数组
}GExecutorManage;

// 定义全局执行体管理器
DefinedExecutorData(GExecutorManage, GExecutorManageData);
// 定义全局默认执行体
DefinedExecutorData(ExecutorNode, DefaultExecutorNode);

// 返回全局执行体管理器
KLINE GExecutorManage* KrlExGetGExecutorManageDataAddr()
{
    return &GExecutorManageData;
}

// 返回全局默认执行体
KLINE ExecutorNode* KrlExGetDefaultExecutorNodeAddr()
{
    return &DefaultExecutorNode;
}

// 锁初始化
// @param init 实例
private void ELockInit(ELock* init);

// 执行体头初始化
// @param init 实例
private void ExecutorHeadInit(ExecutorHead* init);

// 执行体集合初始化
// @param init 实例
private void ExecutorBoxInit(ExecutorBox* init);

// 执行体节点初始化
// @param init 实例
private void ExecutorNodeInit(ExecutorNode* init);

// 执行体管理器初始化
// @param init 实例
private void GExecutorManageInit(GExecutorManage* init);

// 加锁
// @param lock 锁实例
public void KrlExLocked(ELock* lock);

// 解锁
// @param lock 锁实例
public void KrlExUnLock(ELock* lock);

// TODO
private Bool KrlExAddExecutorToExecutorHead(GExecutorManage* gexem, ExecutorNode* exnode, ExecutorBox* box, ExecutorHead* exhead, Executor* executor);

private Bool KrlExDefaultAddExecutorRealizeCore(GExecutorManage* gexem, ExecutorNode* exnode, Executor* executor);

private Bool KrlExDefaultAddExecutorRealize(Executor* executor);

public Bool KrlExDefaultAddExecutor(Executor* executor);

private Bool KrlExAddCPUIdleExecutorRealizeCore(GExecutorManage* gexem, ExecutorNode* exnode, Executor* executor);

private Bool KrlExAddCPUIdleExecutorRealize(Executor* executor);

public Bool KrlExAddCPUIdleExecutor(Executor* executor);

public Bool KrlExecutorManageInit();
#endif
