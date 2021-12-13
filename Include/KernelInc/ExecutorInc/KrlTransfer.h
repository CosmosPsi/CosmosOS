/**********************************************************
        调度转移头文件KrlTransfer.h
***********************************************************
                彭东
**********************************************************/
#ifndef _KRLTRANSFERHEAD
#define _KRLTRANSFERHEAD

// TODO
#define TRANSFER_NR_MAX (64)

// 需要调度
#define NEED_TRANSFER_STATUS (1)
// 不需要调度
#define NONEED_TRANSFER_STATUS (0)

// 全局调度节点
typedef struct TRANSFERNODE{}TransferNode;

// 调度转移关系
typedef struct TRANSFER
{
    List Lists;                 // 列表
	UInt Status;                // 状态
	UInt Flags;                 // 标志位
    RBTree Node;                // 红黑树节点
    TransferNode* ParentNode;   // 父节点
    Thread* Thread;             // 线程
}Transfer;

// 转移节点
typedef struct TRANSFERNODE
{
    List Lists;                // 列表
    ELock Lock;                // 锁
	UInt Status;               // 状态
	UInt Flags;                // 标志位
    UInt TransferNR;           // TODO
    List TransferLists;        // 转移列表
    RBRoot Root;               // 根节点
    Transfer* CurrTransfer;    // 当前转移
    Transfer* CPUIdleTransfer; // CPU空闲转移
    void* Priv;                // 私有信息
    void* Ext;                 // 扩展信息
}TransferNode;

// 调度转移管理
typedef struct TRANSFERMANAGE
{
    ELock Lock;                                     // 锁
    UInt Status;                                    // 状态
	UInt Flags;                                     // 标志位
    TransferNode DefaultTransferNode;               // 默认转移节点
    TransferNode* TransferNodeArr[TRANSFER_NR_MAX]; // 转移节点数组
}TransferManage;

// 返回默认调度转移管理器
KLINE TransferManage* KrlTrGetTransferManageDataAddr()
{
    return &TransferManageData;
}

// 返回默认转移节点
KLINE TransferNode* KrlTrGetDefaultTransferNodeAddr()
{
    TransferManage* tmd = NULL;
    tmd = KrlTrGetTransferManageDataAddr();
    IF_NULL_RETURN_NULL(tmd);
    return &tmd->DefaultTransferNode;
}

// 返回调度转移线程
// @param transfer 调度转移
// @return 线程 
KLINE Thread* KrlTrGetThreadForTransfer(Transfer* transfer)
{
    IF_NULL_RETURN_NULL(transfer);
    IF_EQT_DEAD(NULL, transfer->Thread, "Transfer Thread is NULL\n");
    return transfer->Thread;
}

// 调度转移初始化
// @param init 实例
private void TransferInit(Transfer* init);

// 调度转移节点初始化
// @param init 实例
private void TransferNodeInit(TransferNode* init);

// 调度转移管理器初始化
// @param init 实例
private void TransferManageInit(TransferManage* init);

private Bool KrlTransferDelRealizeCore(TransferNode* node, Transfer* transfer);
private Bool KrlTransferAddRealizeCore(TransferNode* node, Transfer* transfer);
private Bool KrlTransferDelDefaultRealize(Transfer* transfer);
private Bool KrlTransferDelRealize(TransferNode* node, Transfer* transfer);
private Bool KrlTransferAddDefaultRealize(Transfer* transfer);
private Bool KrlTransferAddRealize(TransferNode* node, Transfer* transfer);
public Bool KrlTransferDelDefault(Transfer* transfer);
public Bool KrlTransferDel(Transfer* transfer);
public Bool KrlTransferAddDefault(Transfer* transfer);
public Bool KrlTransferAdd(TransferNode* node, Transfer* transfer);
public Bool TransferInitForThread(Transfer* init, Thread* thread);
public TransferNode* KrlExGetCurrentCPUTransferNode();
public Bool KrlExSetCurrentTransfer(Transfer* transfer);
public Transfer* KrlExGetCurrentTransfer();
public Transfer* KrlExGetCPUIdleTransfer(TransferNode* node);
public Transfer* KrlExGetCurrentCPUIdleTransfer();
public Bool KrlExSetCurrentCPUIdleTransfer(Transfer* transfer);
public Bool KrlExSetNeedTransferStatus();
private Transfer* PickTransferOnTransferNode(TransferNode* node);
public Transfer* KrlExGetNextRunTransferOnTransferNode(TransferNode* node);
private void KrlTransferCurrentToNext(Transfer* curr, Transfer* next);
private void KrlTransferRealizeCore();
private void KrlTransferRealize();
public void KrlTransfer();
public Bool KrlTransferInit();


#endif