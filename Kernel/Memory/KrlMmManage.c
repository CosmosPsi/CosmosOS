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
#include "KrlLog.h"
#include "KrlMmManage.h"

DefinedMEMData(GMemManage, GMemManageData);

private void MLockInit(MLock* init)
{
    IF_NULL_RETURN(init);
    SPinLockInit(&init->Locks);
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
    ListInit(&init->AllPMSADLists);
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
        init->MAreaArr[i].AreaId = (U64)i;
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

public Bool KrlMmUPAddGMMAllocMaxFreeNR(UInt allocnr, UInt maxnr, UInt freenr)
{
    GMemManage* gmm = NULL;
    gmm = KrlMmGetGMemManageAddr();
    IF_NULL_RETURN_FALSE(gmm);
    
    KrlMmLocked(&gmm->Lock);
    gmm->AllocPMSAD += (U64)allocnr;
    gmm->MaxPMSAD += (U64)maxnr;
    gmm->FreePMSAD += (U64)freenr;
    gmm->MemroySZ = (U64)(gmm->MaxPMSAD << MSAD_PADR_SLBITS);
    KrlMmUnLock(&gmm->Lock);
    return TRUE;
}

public Bool KrlMmUPAddMNodeAreaAllocMaxFreeNR(MNode* node, MArea* area, UInt allocnr, UInt maxnr, UInt freenr)
{
    IF_NULL_RETURN_FALSE(node);
    IF_NULL_RETURN_FALSE(area);
    KrlMmLocked(&area->Lock);
    area->AllocPMSAD += allocnr;
    area->MaxPMSAD += maxnr;
    area->FreePMSAD += freenr;
    KrlMmUnLock(&area->Lock);
    return TRUE;
}

public Bool KrlMmUPSubGMMAllocMaxFreeNR(UInt allocnr, UInt maxnr, UInt freenr)
{
    GMemManage* gmm = NULL;
    gmm = KrlMmGetGMemManageAddr();
    IF_NULL_RETURN_FALSE(gmm);
    
    KrlMmLocked(&gmm->Lock);
    if((U64)allocnr <= gmm->AllocPMSAD)
    {
        gmm->AllocPMSAD -= allocnr;
    }
    if((U64)maxnr <= gmm->MaxPMSAD)
    {
        gmm->MaxPMSAD -= (U64)maxnr;
    }
    if((U64)freenr <= gmm->FreePMSAD)
    {
        gmm->FreePMSAD -= (U64)freenr;
    }
    gmm->MemroySZ = (U64)(gmm->MaxPMSAD << MSAD_PADR_SLBITS);
    KrlMmUnLock(&gmm->Lock);
    return TRUE;
}

public Bool KrlMmUPSubMNodeAreaAllocMaxFreeNR(MNode* node, MArea* area, UInt allocnr, UInt maxnr, UInt freenr)
{
    IF_NULL_RETURN_FALSE(node);
    IF_NULL_RETURN_FALSE(area);
    KrlMmLocked(&area->Lock);
    if(allocnr <= area->AllocPMSAD)
    {
        area->AllocPMSAD -= allocnr;
    }
    if(maxnr <= area->MaxPMSAD)
    {
        area->MaxPMSAD -= maxnr;
    }
    if(freenr <= area->FreePMSAD)
    {
        area->FreePMSAD -= freenr;
    }
    KrlMmUnLock(&area->Lock);
    return TRUE;
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

private Bool SetPMSADInMNodeMAreaInfo(MNode* node, PMSAD* msad)
{
    Addr paddr = 0;   
    IF_NULL_RETURN_FALSE(node);
    IF_NULL_RETURN_FALSE(msad);

    paddr = PMSADRetPAddr(msad);

	for(U64 i = 0; i < MEMAREA_MAX; i++)
    {
        if((node->MAreaArr[i].LogicStart <= paddr) && ((paddr + MSAD_SIZE) < node->MAreaArr[i].LogicEnd))
        {
            switch (node->MAreaArr[i].Type)
            {
            case MA_TYPE_HWAD:
            {
                msad->CountFlags.AreaTypeBit = MF_MARTY_HWD;
                return TRUE;
            }
            case MA_TYPE_KRNL:
            {
                msad->CountFlags.AreaTypeBit = MF_MARTY_KRL;
                return TRUE;
            }
            case MA_TYPE_PROC:
            {
                msad->CountFlags.AreaTypeBit = MF_MARTY_PRC;
                return TRUE;
            }
            case MA_TYPE_SHAR:
            {
                msad->CountFlags.AreaTypeBit = MF_MARTY_SHD;
                return TRUE;
            }
            default:
            {
                return FALSE;
            }
            }
        }   
	}
    return FALSE;
}

private PMSAD* NewOnePMSAD(MNode* node, PHYMSPaceArea* area, PMSADDire* dire, U64 paddr)
{
    PMSAD* msadstart = NULL;
    MArea* marea = NULL;
    UInt index = 0;
    IF_NULL_RETURN_NULL(node);
    IF_NULL_RETURN_NULL(area);
    IF_NULL_RETURN_NULL(dire);

    msadstart = dire->DireStart;
    IF_NULL_RETURN_NULL(msadstart);
    
    index = PMSADIndex(paddr);
    
    PMSADInit(&msadstart[index]);

    SetPMSADPAddr(&msadstart[index], paddr);
    SetPMSADPresent(&msadstart[index]);
    
    SetPMSADInMNodeMAreaInfo(node, &msadstart[index]);
    
    marea = PMSADRetItsMArea(&msadstart[index]);
    IF_NULL_DEAD(marea);
    marea->AllPMSADNR++;
    marea->MaxPMSAD++;
    node->NodeMemSize += MSAD_SIZE;
    KrlMmUPAddGMMAllocMaxFreeNR(0, 1, 0);
    return &msadstart[index];
}

private UInt PMSADInitOnPHYMSPaceArea(MNode* node, PMSADDire* dire, PHYMSPaceArea* area, U64 start, U64 end)
{
    UInt count = 0;
    PMSAD* pstart = NULL;
    IF_NULL_RETURN_ZERO(node);
    IF_NULL_RETURN_ZERO(dire);
    IF_NULL_RETURN_ZERO(area);
    for(U64 paddr = area->Start; (paddr + (MSAD_SIZE - 1)) < area->End; paddr += MSAD_SIZE)
    {
        if((start <= paddr) && (paddr < end))
        {
            pstart = NewOnePMSAD(node, area, dire, paddr);
            if(NULL != pstart)
            {
                count++;
            }
        }
    }
    return count;
}

private PMSAD* PMSADDireIsNeedAllocMemory(U64 start, U64 end)
{
    PHYMSPaceArea* pmsarea = NULL;
    GMemManage* gmm = NULL;
    
    gmm = KrlMmGetGMemManageAddr();
    IF_NULL_RETURN_NULL(gmm);

    pmsarea = gmm->PAreaStart;
    for(U64 i = 0; i < gmm->PAreaNR; i++)
    {
        if(PMSA_T_OSAPUSERRAM == pmsarea[i].Type)
        {
            for(U64 paddr = pmsarea[i].Start; (paddr + (MSAD_SIZE - 1)) < pmsarea[i].End; paddr += MSAD_SIZE)
            {
                if((start <= paddr) && (paddr < end))
                {
                    return HalExPBootAllocMem((Size)((PMSADDIRE_SIZE >> MSAD_PADR_SLBITS) * sizeof(PMSAD)));
                }
            }
        }
    }
    return NULL;
}

private Bool PMSADInitOnMNode(MNode* node)
{
    PHYMSPaceArea* pmsarea = NULL;
    GMemManage* gmm = NULL;
    PMSADDire* dire = NULL;
    PMSAD* msad = NULL;
    U64 addrstart = 0;
    U64 addrend = 0;
    UInt msadnr = 0;

    IF_NULL_RETURN_FALSE(node);
    gmm = KrlMmGetGMemManageAddr();
    IF_NULL_DEAD(gmm);
     
    pmsarea = gmm->PAreaStart;
    for(U64 d = 0; d < PMSADDIRE_MAX; d++)
    {
        dire = &node->PMSADDir.PMSADEArr[d];
        addrstart = d * PMSADDIRE_SIZE;
        addrend = addrstart + PMSADDIRE_SIZE;

        msad = PMSADDireIsNeedAllocMemory(addrstart, addrend);
        if(NULL == msad)
        {
            continue;
        }

        dire->DireStart = msad;
        
        for(U64 i = 0; i < gmm->PAreaNR; i++)
        {
            if(PMSA_T_OSAPUSERRAM == pmsarea[i].Type)
            {
                msadnr += PMSADInitOnPHYMSPaceArea(node, dire, &pmsarea[i], addrstart, addrend);
            }
        }
    }
    return TRUE;
}

private Bool ScanOccupancyPMSADOnAddr(Addr start, Addr end)
{
    Addr tmpstart = 0;
    Addr tmpend = 0;
    MNode* node = NULL;
    PMSADDire* dire = NULL;
    PMSAD* msad = NULL;

    tmpstart = (start & MSAD_MASK);
    tmpend = MSAD_ALIGN(end);

    for(Addr paddr = tmpstart; paddr < tmpend; paddr += MSAD_SIZE)
    {
        msad = PHYAddrRetPMSAD(paddr);
        IF_NULL_RETURN_FALSE(msad);
        if(PMSADIsFree(msad) == TRUE)
        {
            SetPMSADOccupancyType(msad, MF_MOCTY_KRNL);
            GetPMSAD(msad);
            SetPMSADAllocBit(msad);
        }

    }
    return TRUE;
}

public Bool KrlMmScanOccupancyPMSAD()
{
    MachStartInfo* msinfo = NULL;
    msinfo = HalExPGetMachStartInfoAddr();
    IF_NULL_RETURN_FALSE(msinfo);

    ScanOccupancyPMSADOnAddr(0, 0x1000 - 1);
    ScanOccupancyPMSADOnAddr((Addr)(msinfo->KrlInitStack & MSAD_MASK), (Addr)msinfo->KrlInitStack);
    ScanOccupancyPMSADOnAddr((Addr)(msinfo->krlCoreFilePAddr), (Addr)(msinfo->krlCoreFilePAddr + msinfo->krlCoreFileSZ));
    // ScanOccupancyPMSADOnAddr((Addr)(msinfo->KrlImgFilePAddr), (Addr)(msinfo->KrlImgFilePAddr + msinfo->KrlImgFileSZ));
    ScanOccupancyPMSADOnAddr((Addr)msinfo->BrkCurrAddr, (Addr)msinfo->BrkCurrAddr);
    return TRUE;
}

public Bool KrlMmPMSADInit()
{
    PHYMSPaceArea* pmsarea = NULL;
    GMemManage* gmm = NULL;
    MNode* node = NULL;
    UInt msadnr = 0;

    gmm = KrlMmGetGMemManageAddr();
    IF_NULL_DEAD(gmm);
    
    node = gmm->MNodeStart;
    for(U64 i = 0; i < gmm->MNodeNR; i++)
    {
        PMSADInitOnMNode(&node[i]);
    }
    
    return TRUE;
}

public Bool KrlMmPHYMSPaceAreaInit()
{
    PHYMSPaceArea* area = NULL;
    E820Map* e820 = NULL;
    GMemManage* gmm = NULL;
    MachStartInfo* msinfo = NULL;
    U64 areanr= 0;

    msinfo = HalExPGetMachStartInfoAddr();
    IF_NULL_DEAD(msinfo);
    
    gmm = KrlMmGetGMemManageAddr();
    IF_NULL_DEAD(gmm);

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
    gmm->PAreaStart = area;
    gmm->PAreaNR = areanr; 
    return TRUE;
}

public Bool PMSADAddInPABHList(PABHList* pabhl, PMSAD* msad, UInt order)
{
    PMSAD* msadend = NULL;
    IF_NULL_RETURN_FALSE(pabhl);
    IF_NULL_RETURN_FALSE(msad);
    
    if(PMSADIsFree(msad) == FALSE)
    {
        return FALSE;
    }
    if(pabhl->Order != order)
    {
        return FALSE;
    }
    
    KrlMmLocked(&pabhl->Lock);
    
    msadend = &msad[(1 << order) - 1];
    ListAdd(&msad->Lists, &pabhl->FreeLists);
    SetPMSADOLType(msad, MF_OLKTY_ODER);
    SetPMSADBlockLink(msad, (void*)msadend);
    SetPMSADOLType(msadend, MF_OLKTY_BAFH);
    SetPMSADBlockLink(msad, (void*)pabhl);
    pabhl->FreePmsadNR += (1 << order);
    pabhl->PmsadNR += (1 << order);
    KrlMmUnLock(&pabhl->Lock);
    return TRUE;
}

private SInt RetOrderInPMSADsNR(PMSAD* start, PMSAD* end)
{
    UInt msadnr = 0;
    SInt order = -1;
    if(NULL == start || NULL == end)
    {
        return -1;
    }
    if(start > end)
    {
        return -1;
    }

    msadnr = (end - start) + 1;
    
    for(UInt i = 0; i < MSPLMER_ARR_LMAX; i++)
    {
        if((1 << i) <= msadnr)
        {
            order++;
        }
    }
    return order;
} 

private PMSAD* NextOrderPMSADsAddInPABHList(MNode* node, MArea* area, PMSAD* start, PMSAD* end, UInt* summsad)
{
    SInt msadnr = 0;
    SInt order = -1;
    PABHList* pabhlist = NULL;
    PMSAD* nextmsad = NULL;

    pabhlist = area->MSPLMerData.PAddrBlockArr;
    msadnr = (end - start) + 1;

    order = RetOrderInPMSADsNR(start, end);
    if(0 > order)
    {
        KrlErrorCrashDead("RetOrderInPMSADsNR is Fail\n");
        return NULL;
    }

    if(PMSADAddInPABHList(&pabhlist[order], start, (UInt)order) == FALSE)
    {
        KrlErrorCrashDead("PMSADAddInPABHList is Fail\n");
        return NULL;
    }
    *summsad = 1 << order;
    nextmsad = &start[(msadnr - (1 << order)) + 1];

    area->FreePMSAD += (UInt)(1 << order);
    KrlMmUPAddGMMAllocMaxFreeNR(0, 0, (UInt)(1 << order));
    return nextmsad;
}

private UInt ScanOrderPMSADsAddInPABHList(MNode* node, MArea* area, PMSAD* start, PMSAD* end)
{
    PMSAD* msad = NULL;
    UInt count = 0;
    UInt sum = 0;
    msad = start;
    while(msad <= end)
    {
        msad = NextOrderPMSADsAddInPABHList(node, area, msad, end, &count);
        if(NULL == msad)
        {
            KrlErrorCrashDead("NextOrderPMSADsAddInPABHList is Fail\n");
        }
        sum += count;
    }

    return sum;
}

private UInt ScanContinuousAddrPMSADsLen(PMSAD* start, PMSAD* end)
{
    PMSAD* msad = NULL;
    UInt count = 1;
    if(start == end)
    {
        return 1;
    }
    msad = start + 1;
    while(msad <= end)
    {
        if(PMSADIsAdjacent(msad - 1, msad) == FALSE)
        {
            return count;
        }
        count++;
        msad++;
    }
    return count;
}

private UInt ScanContinuousAddrPMSADs(MNode* node, MArea* area, PMSAD* start, PMSAD* end)
{
    PMSAD* msad = NULL;
    UInt count = 0;
    UInt sum = 0;
    msad = start;
    while(msad <= end)
    {
        if(MA_TYPE_PROC == area->Type)
        {
            sum += ScanOrderPMSADsAddInPABHList(node, area, msad, msad);
            msad++;
        }
        else
        {
            count = ScanContinuousAddrPMSADsLen(msad, end);
            if(0 < count)
            {
                sum += ScanOrderPMSADsAddInPABHList(node, area, msad, end);
                msad += count;
            }
            else
            {
                msad++;
            }
        }
    }
    return sum;
}

private UInt ScanContinuousFreePMSADsLen(PMSAD* start, PMSAD* end)
{
    PMSAD* msad = NULL;
    UInt count = 0;
    msad = start;
    while(msad <= end)
    {
        if(PMSADIsFree(msad) == FALSE)
        {
            return count;
        }
        count++;
        msad++;
    }
    return count;
}

private UInt ScanContinuousFreePMSADs(MNode* node, MArea* area, PMSAD* start, PMSAD* end)
{
    PMSAD* msad = NULL;
    UInt count = 0;
    UInt sum = 0;
    msad = start;
    while(msad <= end)
    {
        count = ScanContinuousFreePMSADsLen(msad, end);
        if(0 < count)
        {
            sum += ScanContinuousAddrPMSADs(node, area, msad, &msad[count - 1]);
            msad += count;            
        }
        else
        {
            msad++;
        }
    }
    return sum;
}

private UInt ScanSameAreaTypePMSADsLen(PMSAD* start, PMSAD* end, UInt areatype)
{
    PMSAD* msad = NULL;
    UInt count = 0;
    msad = start;
    while(msad <= end)
    {
        if(PMSADIsEQAreaType(msad, areatype) == FALSE)
        {
            return count;
        }
        count++;
        msad++;
    }
    return count;
}

private UInt ScanSameAreaTypePMSADs(MNode* node, MArea* area, PMSAD* start, PMSAD* end)
{
    PMSAD* msad = NULL;
    UInt count = 0;
    UInt sum = 0;
    UInt areatype = 0;
    
    areatype = area->Type;
    msad = start;

    while(msad <= end)
    {
        count = ScanSameAreaTypePMSADsLen(msad, end, areatype);
        if(0 < count)
        {
            sum += ScanContinuousFreePMSADs(node, area, msad, &msad[count - 1]);
            msad += count;            
        }
        else
        {
            msad++;
        }
    }

    return sum;
}

private UInt PMSADBlockInitOnPMSADDire(MNode* node, MArea* area, PMSADDire* dire)
{
    PMSAD* start = NULL;
    PMSAD* end = NULL;

    IF_NULL_RETURN_ZERO(node);
    IF_NULL_RETURN_ZERO(area);
    IF_NULL_RETURN_ZERO(dire);
    IF_NULL_RETURN_ZERO(dire->DireStart);

    start = dire->DireStart;
    end = &start[(PMSADDIRE_PMSAD_NR - 1)];
    return ScanSameAreaTypePMSADs(node, area, start, end);
}

private Bool OneMAreaInitOnMNode(MNode* node, MArea* area)
{
    PMSADDire* dire = NULL;
    IF_NULL_RETURN_FALSE(node);
    IF_NULL_RETURN_FALSE(area);

    dire = node->PMSADDir.PMSADEArr;
    for(UInt i = 0; i < PMSADDIRE_MAX; i++)
    {
       PMSADBlockInitOnPMSADDire(node, area, &dire[i]);
    }
    return TRUE;
}

private Bool MAreaInitOnMNode(MNode* node)
{
    MArea* area = NULL;
    IF_NULL_RETURN_FALSE(node);
    area = node->MAreaArr;
    for(UInt i = 0; i < MEMAREA_MAX; i++)
    {
        OneMAreaInitOnMNode(node, &area[i]);
    }
    return TRUE;
}

public Bool KrlMmClearPMSADUseStatus(PMSAD* msad)
{
    IF_NULL_RETURN_FALSE(msad);
    ClearPMSADAlloc(msad);
    ClearPMSADShare(msad);
    ClearPMSADSwap(msad);
    ClearPMSADLock(msad);
    ClearPMSADKMPool(msad);
    return TRUE;
}

public PMSAD* KrlMmGetPMSADOnVaddr(Addr vaddr)
{
    Addr paddr = NULL;
    if((vaddr & (MSAD_SIZE - 1)) != 0)
    {
        return NULL;
    }
    paddr = HalVAddrToPAddr(vaddr);
    IF_NULL_RETURN_NULL(paddr);
    return PHYAddrRetPMSAD((U64)paddr);
}

public MNode* KrlMmGetMNode(UInt nodeid)
{
    GMemManage* gmm = NULL;
    MNode* node = NULL;
    gmm = KrlMmGetGMemManageAddr();
    IF_NULL_RETURN_NULL(gmm);

    node = gmm->MNodeStart;
    IF_NULL_RETURN_NULL(node);
    
    for(UInt i = 0; i < gmm->MNodeNR; i++)
    {
        if(node[i].NodeID == nodeid)
        {
            return &node[i];    
        }
    }
    return NULL;
}

public MArea* KrlMmGetMArea(MNode* node, UInt areaid)
{
    IF_NULL_RETURN_NULL(node);
    
    for(UInt i = 0; i < MEMAREA_MAX; i++)
    {
        if(node->MAreaArr[i].AreaId == (U64)areaid)
        {
            return &node->MAreaArr[i];    
        }
    }
    return NULL;
}

public Bool KrlMmMAreaInit()
{
    MNode* node = NULL;
    MachStartInfo* msinfo = NULL;
    GMemManage* gmm = NULL;
    gmm = KrlMmGetGMemManageAddr();
    IF_NULL_DEAD(gmm);

    node = gmm->MNodeStart;
    IF_NULL_RETURN_FALSE(node);
    
    for(UInt i = 0; i < gmm->MNodeNR; i++)
    {
        MAreaInitOnMNode(&node[i]);    
    }
    return TRUE;
}

private Bool DefaultMNodeInit()
{
    MNode* node = NULL;
    MachStartInfo* msinfo = NULL;
    GMemManage* gmm = NULL;

    msinfo = HalExPGetMachStartInfoAddr();
    IF_NULL_DEAD(msinfo);
    
    gmm = KrlMmGetGMemManageAddr();
    IF_NULL_DEAD(gmm);

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

    node->PMSAreaPtr = gmm->PAreaStart;
    node->PMSAreaNR = gmm->PAreaNR;

    gmm->MNodeStart = node;
    gmm->MNodeNR = 1;

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
    KrlMmPMSADInit();
    KrlMmScanOccupancyPMSAD();
    KrlMmMAreaInit();
    return TRUE;
}