/**********************************************************
        物理内存管理文件KrlMmManage.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "HalSync.h"
#include "HalCPU.h"
#include "HalBoot.h"
#include "HalInterface.h"
#include "KrlMmManage.h"

DefinedMEMData(GMemManage, GMemManageData);

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

private Bool NewOnePHYMSPaceArea(E820Map* e820, PHYMSPaceArea* area)
{
    U32 type = 0, subtype = 0;
    IF_NULL_RETURN_FALSE(e820);
    IF_NULL_RETURN_FALSE(area);
    
    PHYMSPaceAreaInit(area);
    
    switch (e820->Type)
    {
    case RAM_USABLE:
        type = PMSA_T_OSAPUSERRAM;
        subtype = RAM_USABLE;
        break;
    case RAM_RESERV:
        type = PMSA_T_RESERVRAM;
        subtype = RAM_RESERV;
        break;
    case RAM_ACPIREC:
        type = PMSA_T_HWUSERRAM;
        subtype = RAM_ACPIREC;
        break;
    case RAM_ACPINVS:
        type = PMSA_T_HWUSERRAM;
        subtype = RAM_ACPINVS;
        break;
    case RAM_AREACON:
        type = PMSA_T_BUGRAM;
        subtype = RAM_AREACON;
        break;
    default:
        break;
    }
    if (0 == type)
    {
        return FALSE;
    }

    area->Type = type;
    area->SubType = subtype;
    area->Flags = PMSA_F_X86_64;
    area->Start = e820->Addr;
    area->Size = e820->Size;
    area->End = e820->Addr + e820->Size - 1;
    return TRUE;
}

private void PHYMSPaceAreaSwap(PHYMSPaceArea *s, PHYMSPaceArea *d)
{
    PHYMSPaceArea tmp;
    PHYMSPaceAreaInit(&tmp);
    HalMemCopy(s, &tmp, sizeof(PHYMSPaceArea));
    HalMemCopy(d, s, sizeof(PHYMSPaceArea));
    HalMemCopy(&tmp, d, sizeof(PHYMSPaceArea));
    return;
}

private void PHYMSPaceAreaSort(PHYMSPaceArea* area, U64 nr)
{
    U64 i, j, k = nr - 1;
    for(j = 0; j < k; j++)
    {
        for(i = 0; i < k - j; i++)
        {
            if(area[i].Start > area[i + 1].Start)
            {
                PHYMSPaceAreaSwap(&area[i], &area[i + 1]);
            }
        }
    }
    return;
}

public Bool KrlMmPHYMSPaceAreaInit()
{
    PHYMSPaceArea* area = NULL;
    E820Map* e820 = NULL;
    MachStartInfo* msinfo = NULL;
    U64 areanr= 0;
    msinfo = HalExPGetMachStartInfoAddr();
    IF_NULL_DEAD(msinfo);
    area = HalExPBootAllocMem((Size)(msinfo->E820NR * sizeof(PHYMSPaceArea)));
    IF_NULL_RETURN_FALSE(area);
    for ( ;e820 != NULL; areanr++)
    {
        e820 = HalExPBootGetNextE820();
        NewOnePHYMSPaceArea(e820, &area[areanr]);
    }
    PHYMSPaceAreaSort(area, areanr);
    msinfo->PMSPaceAreaPAddr = HalVAddrToPAddr((Addr)area);
    msinfo->PMSPaceAreaNR = areanr;
    msinfo->PMSPaceAreaCurr = 0;
    msinfo->PMSPaceAreaSZ = areanr * (U64)(sizeof(PHYMSPaceArea));
    return TRUE;
}

public Bool KrlMmMAreaInit()
{
    return TRUE;
}

private Bool DefaultMNodeInit()
{
    MNode* node = NULL;
    MachStartInfo* msinfo = NULL;
    
    msinfo = HalExPGetMachStartInfoAddr();
    IF_NULL_DEAD(msinfo);
    
    node = (MNode*)HalExPBootAllocMem((Size)(sizeof(MNode)));
    IF_NULL_DEAD(node);
    
    MNodeInit(node);
    node->MAreaArr[MA_TYPE_HWAD].Type = MA_TYPE_HWAD;
    node->MAreaArr[MA_TYPE_HWAD].LogicStart = MA_HWAD_LSTART;
    node->MAreaArr[MA_TYPE_HWAD].LogicEnd = MA_HWAD_LEND;
    node->MAreaArr[MA_TYPE_HWAD].LogicSZ = MA_HWAD_LSZ;
    
    node->MAreaArr[MA_TYPE_KRNL].Type = MA_TYPE_KRNL;
    node->MAreaArr[MA_TYPE_KRNL].LogicStart = MA_KRNL_LSTART;
    node->MAreaArr[MA_TYPE_KRNL].LogicEnd = MA_KRNL_LEND;
    node->MAreaArr[MA_TYPE_KRNL].LogicSZ = MA_KRNL_LSZ;

    node->MAreaArr[MA_TYPE_PROC].Type = MA_TYPE_PROC;
    node->MAreaArr[MA_TYPE_PROC].LogicStart = MA_PROC_LSTART;
    node->MAreaArr[MA_TYPE_PROC].LogicEnd = MA_PROC_LEND;
    node->MAreaArr[MA_TYPE_PROC].LogicSZ = MA_PROC_LSZ;
    
    node->MAreaArr[MA_TYPE_SHAR].Type = MA_TYPE_SHAR;
    return TRUE;
}

public Bool KrlMmMNodeInit()
{
    DefaultMNodeInit();
    return TRUE;
}


public Bool KrlMmManageInit()
{
    GMemManageInit(&GMemManageData);
    KrlMmPHYMSPaceAreaInit();
    KrlMmMNodeInit();
    return TRUE;
}