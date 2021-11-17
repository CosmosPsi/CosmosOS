/**********************************************************
        调度转移文件KrlTransfer.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "RBTree.h"
#include "HalSync.h"
#include "HalCPU.h"
#include "KrlMmManage.h"
#include "KrlMmPool.h"
#include "KrlMmVmem.h"
#include "KrlExecutorManage.h"
#include "KrlExecutor.h"
#include "KrlExecutorRes.h"
#include "KrlThread.h"
#include "KrlTransfer.h"
#include "KrlLog.h"

DefinedExecutorData(TransferManage, TransferManageData);
private void TransferInit(Transfer* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->Lists);
    RBTreeInit(&init->Node);
    return;
}

private void TransferNodeInit(TransferNode* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->Lists);
    ELockInit(&init->Lock);
    ListInit(&init->TransferLists);
    RBRootInit(&init->Root);
    return;
}

private void TransferManageInit(TransferManage* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ELockInit(&init->Lock);
    TransferNodeInit(&init->DefaultTransferNode);
    init->TransferNodeArr[0] = &init->DefaultTransferNode;
    return;
}

private Bool KrlTransferDelRealizeCore(TransferNode* node, Transfer* transfer)
{
    IF_NEQ_RETURN(node, transfer->ParentNode, FALSE);
    IF_LTN_RETURN(node->TransferNR, 1, FALSE);

    KrlExLocked(&node->Lock);
    ListDel(&transfer->Lists);
    transfer->ParentNode = NULL;
    node->TransferNR--;
    KrlExUnLock(&node->Lock);
    return TRUE;
}

private Bool KrlTransferAddRealizeCore(TransferNode* node, Transfer* transfer)
{
    KrlExLocked(&node->Lock);
    ListAdd(&transfer->Lists, &node->TransferLists);
    transfer->ParentNode = node;
    node->TransferNR++;
    KrlExUnLock(&node->Lock);
    return TRUE;
}

private Bool KrlTransferDelDefaultRealize(Transfer* transfer)
{
    TransferNode* node = NULL;

    IF_NULL_RETURN_FALSE(transfer);

    node = KrlTrGetDefaultTransferNodeAddr();
    IF_NEQ_RETURN(node, transfer->ParentNode, FALSE);

    return KrlTransferDel(transfer);
}

private Bool KrlTransferDelRealize(TransferNode* node, Transfer* transfer)
{
    IF_NULL_RETURN_FALSE(node);
    IF_NULL_RETURN_FALSE(transfer);
    return KrlTransferDelRealizeCore(node, transfer);
}

private Bool KrlTransferAddDefaultRealize(Transfer* transfer)
{
    TransferNode* node = NULL;
    IF_NULL_RETURN_FALSE(transfer);
    node = KrlTrGetDefaultTransferNodeAddr();
    IF_NULL_RETURN_FALSE(node);
    return KrlTransferAdd(node, transfer);
}

private Bool KrlTransferAddRealize(TransferNode* node, Transfer* transfer)
{
    IF_NULL_RETURN_FALSE(node);
    IF_NULL_RETURN_FALSE(transfer);
    return KrlTransferAddRealizeCore(node, transfer);
}

public Bool KrlTransferDelDefault(Transfer* transfer)
{
    IF_NULL_RETURN_FALSE(transfer);
    return KrlTransferDelDefaultRealize(transfer);
}

public Bool KrlTransferDel(Transfer* transfer)
{
    IF_NULL_RETURN_FALSE(transfer->ParentNode);
    IF_NULL_RETURN_FALSE(transfer);
    return KrlTransferDelRealize(transfer->ParentNode, transfer);
}

public Bool KrlTransferAddDefault(Transfer* transfer)
{
    IF_NULL_RETURN_FALSE(transfer);
    return KrlTransferAddDefaultRealize(transfer);
}

public Bool KrlTransferAdd(TransferNode* node, Transfer* transfer)
{
    IF_NULL_RETURN_FALSE(node);
    IF_NULL_RETURN_FALSE(transfer);
    return KrlTransferAddRealize(node, transfer);
}

public Bool TransferInitForThread(Transfer* init, Thread* thread)
{
    IF_NULL_RETURN_FALSE(init);
    IF_NULL_RETURN_FALSE(thread);
    TransferInit(init);
    init->Thread = thread;
    return TRUE;
}

public TransferNode* KrlExGetCurrentCPUTransferNode()
{
    TransferManage* tmd = NULL;
    TransferNode* node = NULL;
    UInt cpu = 0;
    tmd = KrlTrGetTransferManageDataAddr();
    IF_NULL_RETURN_NULL(tmd);
    cpu = HalCPUID();
    node = tmd->TransferNodeArr[cpu];
    return node;
}

public Transfer* KrlExGetCurrentTransfer()
{
    TransferNode* currnode = NULL;
    currnode = KrlExGetCurrentCPUTransferNode();
    IF_EQT_DEAD(currnode, NULL, "Current CPU TransferNode is NULL\n");
    IF_EQT_DEAD(currnode->CurrTransfer, NULL, "Current Transfer is NULL\n");
    return currnode->CurrTransfer;
}

public Bool KrlTransferInit()
{
    TransferManage* tmd = NULL;

    tmd = KrlTrGetTransferManageDataAddr();
    IF_NULL_RETURN_FALSE(tmd);
    
    TransferManageInit(tmd);
    return TRUE;
}
