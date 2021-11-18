/**********************************************************
        调度转移头文件KrlTransfer.h
***********************************************************
                彭东
**********************************************************/
#ifndef _KRLTRANSFERHEAD
#define _KRLTRANSFERHEAD

#define TRANSFER_NR_MAX (64)

typedef struct TRANSFERNODE{}TransferNode;

typedef struct TRANSFER
{
    List Lists;
	UInt Status;
	UInt Flags;
    RBTree Node;
    TransferNode* ParentNode;
    Thread* Thread;
}Transfer;

typedef struct TRANSFERNODE
{
    List Lists;
    ELock Lock;
	UInt Status;
	UInt Flags;
    UInt TransferNR;
    List TransferLists;
    RBRoot Root;
    Transfer* CurrTransfer;
    Transfer* CPUIdleTransfer;
    void* Priv; 
    void* Ext;
}TransferNode;

typedef struct TRANSFERMANAGE
{
    ELock Lock;
    UInt Status;
	UInt Flags;
    TransferNode DefaultTransferNode;
    TransferNode* TransferNodeArr[TRANSFER_NR_MAX];
}TransferManage;

KLINE TransferManage* KrlTrGetTransferManageDataAddr()
{
    return &TransferManageData;
}

KLINE TransferNode* KrlTrGetDefaultTransferNodeAddr()
{
    TransferManage* tmd = NULL;
    tmd = KrlTrGetTransferManageDataAddr();
    IF_NULL_RETURN_NULL(tmd);
    return &tmd->DefaultTransferNode;
}

KLINE Thread* KrlTrGetThreadForTransfer(Transfer* transfer)
{
    IF_NULL_RETURN_NULL(transfer);
    IF_EQT_DEAD(NULL, transfer->Thread, "Transfer Thread is NULL\n");
    return transfer->Thread;
}

private void TransferInit(Transfer* init);
private void TransferNodeInit(TransferNode* init);
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
public Transfer* KrlExGetCurrentTransfer();
public Transfer* KrlExGetCPUIdleTransfer(TransferNode* node);
public Transfer* KrlExGetCurrentCPUIdleTransfer();
public Bool KrlTransferInit();


#endif