/**********************************************************
        物理内存管理文件KrlMmManage.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "HalSync.h"
#include "HalCPU.h"
#include "KrlMmManage.h"

private void MLockInit(MLock* init)
{
    IF_NULL_RETURN(init);
    SPinLockInit(&init->Lock.Locks);
    return;
}

private void PHYMSPaceAreaInit(PHYMSPaceArea* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    return;
}

private void PMSADInit(PMSAD* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->Lists);
    SPinLockInit(&init->Lock);
    return;
}

private void PABHListInit(PABHList* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);

    MLockInit(&init->Lock);
    ListInit(&init->FreeLists);
    ListInit(&init->AllocLists);
    ListInit(&init->OveLists);
    return;
}

private void MSPlitMerInit(MSPlitMer* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);

    MLockInit(&init->Lock);
    for(UInt i = 0; i < MSPLMER_ARR_LMAX; i++)
    {
        PABHListInit(&init->PAddrBlockArr[i]);
    }
    PABHListInit(&init->OnePAddrBlock);
    return;
}

private void MAreaInit(MArea* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->Lists);
    MLockInit(&init->Lock);
    ListInit(&init->AllocPMSADLists);
    MSPlitMerInit(&init->MSPLMerData);
    return;
}

private void MNodeInit(MNode* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->Lists);
    MLockInit(&init->Lock);
    for(UInt i = 0; i < MEMAREA_MAX; i++)
    {
        MAreaInit(&init->MAreaArr[i]);
    }
    return;
}

private void GMemManageInit(GMemManage* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    MLockInit(&init->Lock);
    return;
}

public void KrlMmLocked(MLock* lock)
{
    IF_NULL_DEAD(lock);
    HalSPinLock(&lock->Locks);
    return;
}

public void KrlMmUnLock(MLock* lock)
{
    IF_NULL_DEAD(lock);
    HalUnSPinLock(&lock->Locks);
    return;
}

public Bool KrlMmPHYMSPaceAreaInit()
{
    return TRUE;
}

public Bool KrlMmMAreaInit()
{
    return TRUE;
}

public Bool KrlMmMNodeInit()
{
    return TRUE;
}


public Bool KrlMmManageInit()
{
    return TRUE;
}