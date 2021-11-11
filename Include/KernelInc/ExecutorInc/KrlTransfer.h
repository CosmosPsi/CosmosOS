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
    void* Thread;
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

private void TransferInit(Transfer* init);
private void TransferNodeInit(TransferNode* init);
private void TransferManageInit(TransferManage* init);
private Bool KrlTransferDelRealizeCore(TransferNode* node, Transfer* transfer);
private Bool KrlTransferAddRealizeCore(TransferNode* node, Transfer* transfer);
private Bool KrlTransferAddRealize(TransferNode* node, Transfer* transfer);
public Bool KrlTransferAdd(TransferNode* node, Transfer* transfer);
public Bool KrlTransferInit();


#endif