/**********************************************************
        虚拟内存文件KrlMmVmem.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "HalSync.h"
#include "HalMMU.h"
#include "KrlLog.h"
#include "KrlMmManage.h"
#include "KrlMmAlloc.h"
#include "KrlMmPool.h"
#include "KrlMmVmem.h"

private void VADInit(VAD* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->Lists);
    RBTreeInit(&init->TNode);
    MLockInit(&init->Lock);
    return;
}

private void VAMInit(VAM* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->Lists);
    RBRootInit(&init->TRoot);
    MLockInit(&init->Lock);
    return;
}

private void VMSInit(VMS* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->Lists);
    MLockInit(&init->Lock);
    RefCountInit(&init->Count);
    HalMMUInit(&init->Mmu);
    VAMInit(&init->VAManager);
    return;
}