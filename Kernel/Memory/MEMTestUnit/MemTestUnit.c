/**********************************************************
        内存测试单元文件MemTestUnit.c
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
#include "MemTestUnit.h"


public Bool MemGMemManageTest()
{
    GMemManage* gmm = NULL;
    gmm = KrlMmGetGMemManageAddr();
    IF_NULL_DEAD(gmm);
    IF_GTN_DEAD(1, gmm->FreePMSAD, "GMemManage:FreePMSAD < 1\n");
    IF_GTN_DEAD(1, gmm->MaxPMSAD, "GMemManage:MaxPMSAD < 1\n");
    IF_NEQ_DEAD(gmm->MemroySZ, (gmm->MaxPMSAD << MSAD_PADR_SLBITS), "GMemManage:MemorySZ data error\n");
    IF_GTN_DEAD(1, gmm->PAreaNR,"GMemManage:PAreaNR < 1\n");
    IF_EQT_DEAD(NULL, gmm->PAreaStart, "GMemManage:PAreaStart = NULL\n");
    IF_GTN_DEAD(1, gmm->MNodeNR,"GMemManage:MNodeNR < 1\n");
    IF_EQT_DEAD(NULL, gmm->MNodeStart, "GMemManage:MNodeStart = NULL\n");
    return TRUE;
}

public Bool MemPHYMSPaceAreaTest()
{
    MachStartInfo* msinfo = NULL;
    GMemManage* gmm = NULL;
    PHYMSPaceArea* parea = NULL;
    E820Map* e820 = NULL;
    UInt status = 0;
    msinfo = HalExPGetMachStartInfoAddr();
    gmm = KrlMmGetGMemManageAddr();
    IF_NULL_DEAD(msinfo);
    IF_NULL_DEAD(gmm);
    
    e820 = (E820Map*)HalPAddrToVAddr((Addr)msinfo->E820PAddr);
    IF_EQT_DEAD(NULL, e820, "MachStartInfo:E820PAddr = NULL\n");
    IF_GTN_DEAD(1, msinfo->E820NR, "MachStartInfo:E820NR < 1\n");
    
    parea = gmm->PAreaStart;
    IF_GTN_DEAD(1, gmm->PAreaNR,"GMemManage:PAreaNR < 1\n");
    IF_EQT_DEAD(NULL, parea, "GMemManage:PAreaStart = NULL\n");
    for(U64 i = 0; i < gmm->PAreaNR; i++)
    {
        status = 0;
        for(U64 e = 0; e < msinfo->E820NR; e++)
        {
            if((parea[i].SubType == e820[e].Type) && (parea[i].Start == e820[e].Addr) &&
                    (parea[i].End == (e820[e].Size + e820[e].Addr - 1)) && 
                    (parea[i].Size == e820[e].Size))
            {
                status++;
            }
        }
        IF_NEQ_DEAD(1, status, "No have E820Map EQ\n");
        IF_NEQ_DEAD(PMSA_F_X86_64, parea[i].Flags, "PHYMSPaceArea:Flags != PMSA_F_X86_64\n");
    }
    
    return TRUE;
}

public Bool MemMNodeTest()
{
    GMemManage* gmm = NULL;
    MNode* node = NULL;

    gmm = KrlMmGetGMemManageAddr();
    IF_NULL_DEAD(gmm);

    node = gmm->MNodeStart;
    IF_EQT_DEAD(NULL, node, "GMemManage:MNodeStart = NULL\n");
    IF_GTN_DEAD(1, gmm->MNodeNR, "GMemManage:MNodeNR < 1\n");

    for(U64 i = 0; i < gmm->MNodeNR; i++)
    {
        IF_EQT_DEAD(0, node[i].NodeMemAddrEnd, "MNode: NodeMemAddrEnd = 0");
    }
    return TRUE;
}

public Bool MemPMSADOnPMSADDireTest(MNode* node, PMSADDire* dire, U64 start, U64 end)
{
    PMSAD* msad = NULL;
    MNode* cnode = NULL;
    U64 paddr = 0;
    IF_EQT_DEAD(NULL, node, "PRAM node = NULL\n");
    IF_EQT_DEAD(NULL, dire, "PRAM dire = NULL\n");
    msad = dire->DireStart;
    IF_NULL_RETURN_FALSE(msad);
    for(UInt i = 0; i < PMSADDIRE_PMSAD_NR; i++)
    {
        
        if(PMSADIsPresent(&msad[i]) == TRUE)
        {
            paddr = PMSADRetPAddr(&msad[i]);
            cnode = PMSADRetItsMNode(&msad[i]);
            IF_NEQ_DEAD(cnode, node, "PMSAD doesn't node\n");
            IF_GTN_DEAD(paddr, end, "PMSAD addr Greater than PMSADDire end\n");
            IF_LTN_DEAD(paddr, start, "PMSAD addr Less than PMSADDire start\n");
        }
    }
    return TRUE;
}


public Bool MemPMSADOnMNodeTest(MNode* node)
{
    U64 addrstart = 0;
    U64 addrend = 0;
    PMSADDire* dire = NULL;
    IF_EQT_DEAD(NULL, node, "PRAM node = NULL\n");
    dire = node->PMSADDir.PMSADEArr;
    for(UInt i = 0; i < PMSADDIRE_MAX; i++)
    {
        addrstart = i * PMSADDIRE_SIZE;
        addrend = addrstart + PMSADDIRE_SIZE;
        MemPMSADOnPMSADDireTest(node, &dire[i], addrstart, addrend);
    }
    return TRUE;
}

public Bool MemPMSADTest()
{
    GMemManage* gmm = NULL;
    MNode* node = NULL;

    gmm = KrlMmGetGMemManageAddr();
    IF_NULL_DEAD(gmm);

    node = gmm->MNodeStart;
    IF_EQT_DEAD(NULL, node, "GMemManage:MNodeStart = NULL\n");
    IF_GTN_DEAD(1, gmm->MNodeNR, "GMemManage:MNodeNR < 1\n");
    for(U64 i = 0; i < gmm->MNodeNR; i++)
    {
        MemPMSADOnMNodeTest(&node[i]);        
    }
    return TRUE;
}


private UInt PMSADBlockIsContinuousAddr(PMSAD* start, PMSAD* end)
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

public Bool MemMSPlitMerOnPerPMSADBlockTest(MNode* node, MArea* area, PABHList* pabhl, PMSAD* start)
{
    PMSAD* end = NULL;
    PMSAD* msad = NULL;
    Addr paddr = 0;
    UInt ordernr = 0;
    UInt msadnr = 0;
    IF_EQT_DEAD(NULL, node, "PRAM node = NULL\n");
    IF_EQT_DEAD(NULL, area, "PRAM area = NULL\n");
    IF_EQT_DEAD(NULL, pabhl, "PRAM pabhl = NULL\n");
    IF_EQT_DEAD(NULL, start, "PRAM splitmer = NULL\n");
    IF_NEQ_DEAD(MF_OLKTY_ODER, start->CountFlags.OLTypeBit, "PMSAD:CountFlags.OLTypeBit != MF_OLKTY_ODER\n");
    
    end = (PMSAD*)start->BlockLink;
    msad = start;
    ordernr = (end - start) + 1;
 
    msadnr = PMSADBlockIsContinuousAddr(start, end);
    IF_NEQ_DEAD(msadnr, pabhl->InOrderPmsadNR, "PMSADBlock is not continuous\n");

    while(msad <= end)
    {
        paddr = PMSADRetPAddr(msad);
        IF_GTN_DEAD(paddr, area->LogicEnd, "PMSAD PAddr doesn't area\n");
        IF_LTN_DEAD(paddr, area->LogicStart, "PMSAD PAddr doesn't area\n");
        IF_NEQ_DEAD(TRUE, PMSADIsPresent(msad), "PMSAD Not Present\n");
        IF_NEQ_DEAD(TRUE, PMSADIsFree(msad), "PMSAD Not Free\n");
        IF_NEQ_DEAD(node, PMSADRetItsMNode(msad), "PMSAD doesn't node\n");
        IF_NEQ_DEAD(area, PMSADRetItsMArea(msad), "PMSAD doesn't area\n");
        IF_NEQ_DEAD(ordernr, pabhl->InOrderPmsadNR, "PMSAD number NEQ\n");
    }
    return TRUE;
}

public Bool MemMSPlitMerOnPerPABHListTest(MNode* node, MArea* area, MSPlitMer* splitmer, PABHList* pabhl)
{
    Bool rets = FALSE;
    List* pos = NULL;
    PMSAD* msad = NULL;
    IF_EQT_DEAD(NULL, node, "PRAM node = NULL\n");
    IF_EQT_DEAD(NULL, area, "PRAM area = NULL\n");
    IF_EQT_DEAD(NULL, splitmer, "PRAM splitmer = NULL\n");
    IF_EQT_DEAD(NULL, pabhl, "PRAM pabhl = NULL\n");
    if(0 < pabhl->PmsadNR)
    {
        rets = ListIsEmptyCareful(&pabhl->FreeLists);
        IF_EQT_DEAD(TRUE, rets, "PABHList:FreeLists == Empty\n");
        ListForEach(pos, &pabhl->FreeLists)
        {
            msad = ListEntry(pos, PMSAD, Lists);
            MemMSPlitMerOnPerPMSADBlockTest(node, area, pabhl, msad);
        }
    }
}

public Bool MemMSPlitMerTest(MNode* node, MArea* area, MSPlitMer* splitmer)
{
    PABHList* pabhl = NULL;
    IF_EQT_DEAD(NULL, node, "PRAM node = NULL\n");
    IF_EQT_DEAD(NULL, area, "PRAM area = NULL\n");
    IF_EQT_DEAD(NULL, splitmer, "PRAM splitmer = NULL\n");
    pabhl = splitmer->PAddrBlockArr;
    for(UInt i = 0; i < MSPLMER_ARR_LMAX; i++)
    {
        MemMSPlitMerOnPerPABHListTest(node, area, splitmer, &pabhl[i]);            
    }
    return TRUE;
}

public Bool MemMAreaOnPerMAreaTest(MNode* node, MArea* area)
{
    IF_EQT_DEAD(NULL, node, "PRAM node = NULL\n");
    IF_EQT_DEAD(NULL, area, "PRAM area = NULL\n");
    if(0 < area->MaxPMSAD)
    {
        MemMSPlitMerTest(node, area, &area->MSPLMerData);
    }
    return;
}

public Bool MemMAreaOnMNodeTest(MNode* node)
{
    MArea* area = NULL;
    IF_EQT_DEAD(NULL, node, "PRAM node = NULL\n");
    area = &node->MAreaArr;
    for(UInt i = 0; i < MEMAREA_MAX; i++)
    {
        MemMAreaOnPerMAreaTest(node, &area[i]);    
    }
    return TRUE;
}

public Bool MemMAreaTest()
{
    GMemManage* gmm = NULL;
    MNode* node = NULL;

    gmm = KrlMmGetGMemManageAddr();
    IF_NULL_DEAD(gmm);

    node = gmm->MNodeStart;
    IF_EQT_DEAD(NULL, node, "GMemManage:MNodeStart = NULL\n");
    IF_GTN_DEAD(1, gmm->MNodeNR, "GMemManage:MNodeNR < 1\n");
    for(U64 i = 0; i < gmm->MNodeNR; i++)
    {
        MemMAreaOnMNodeTest(&node[i]);        
    }
    return TRUE;
}

public Bool MemTestUnit()
{
    MemGMemManageTest();
    MemPHYMSPaceAreaTest();
    MemMNodeTest();
    MemPMSADTest();
    MemMAreaTest();
    return TRUE;
}