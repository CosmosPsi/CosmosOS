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

DefinedMEMData(VBM, VPBoxManager);
DefinedMEMData(VMS, RootVMemSPace);
private void VBMInit(VBM* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->Lists);
    MLockInit(&init->Lock);
    ListInit(&init->VPBHead);
    init->VPBCacheMax = VPB_CACHE_MAX;
    init->VPBCacheMin = VPB_CACHE_MIN;
    ListInit(&init->VPBCacheHead);
    return;
}

private void VPBInit(VPB* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->Lists);
    MLockInit(&init->Lock);
    RefCountInit(&init->Count);
    ListInit(&init->PmsadLists);
    return;
}

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

public Bool KrlMmVMemInit()
{
    VBM* vboxmgr = NULL;
    VMS* rootvms = NULL;
    vboxmgr = KrlMmGetVPBoxManagerAddr();
    IF_NULL_RETURN_FALSE(vboxmgr);
    rootvms = KrlMmGetRootVMemSPaceAddr();
    IF_NULL_RETURN_FALSE(rootvms);
    VBMInit(vboxmgr);
    VMSInit(rootvms);
    return TRUE;
}