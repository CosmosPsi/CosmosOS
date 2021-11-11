/**********************************************************
        调度转移文件KrlTransfer.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "RBTree.h"
#include "HalSync.h"
#include "KrlMmManage.h"
#include "KrlMmPool.h"
#include "KrlMmVmem.h"
#include "KrlExecutorManage.h"
#include "KrlExecutor.h"
#include "KrlExecutorRes.h"
#include "KrlTransfer.h"

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
    return;
}

private Bool KrlTransferAddRealizeCore(TransferNode* node, Transfer* transfer)
{
    KrlExLocked(&node->Lock);
    ListAdd(&transfer->Lists, &node->TransferLists);
    node->TransferNR++;
    KrlExUnLock(&node->Lock);
    return TRUE;
}

private Bool KrlTransferAddRealize(TransferNode* node, Transfer* transfer)
{
    IF_NULL_RETURN_FALSE(node);
    IF_NULL_RETURN_FALSE(transfer);
    return KrlTransferAddRealizeCore(node, transfer);
}

public Bool KrlTransferAdd(TransferNode* node, Transfer* transfer)
{
    IF_NULL_RETURN_FALSE(node);
    IF_NULL_RETURN_FALSE(transfer);
    return KrlTransferAddRealize(node, transfer);
}

public Bool KrlTransferInit()
{
    TransferManage* tmd = NULL;

    tmd = KrlTrGetTransferManageDataAddr();
    IF_NULL_RETURN_FALSE(tmd);
    
    TransferManageInit(tmd);
    return TRUE;
}
