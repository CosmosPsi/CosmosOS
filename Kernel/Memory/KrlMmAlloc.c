/**********************************************************
        物理内存分配文件KrlMmAlloc.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "KrlMmManage.h"
#include "KrlMmAlloc.h"

private PABHList* ForPmsadNrRetPABListOnMArea(MNode* node, MArea* area, UInt msadnr)
{
    PABHList* abhlist = NULL;
    IF_NULL_RETURN_NULL(node);
    IF_NULL_RETURN_NULL(area);
    abhlist = area->MSPLMerData.PAddrBlockAr;
    for(UInt i = 0; i < MSPLMER_ARR_LMAX; i++)
    {
        if(abhlist->InOrderPmsadNR >= msadnr)
        {
            return abhlist;
        }
    }
    return NULL;
}

private PABHList* ForPmsadNrRetAllocPABListOnMArea(MNode* node, MArea* area, UInt msadnr)
{
    PABHList* abhlist = NULL;
    IF_NULL_RETURN_NULL(node);
    IF_NULL_RETURN_NULL(area);
    abhlist = area->MSPLMerData.PAddrBlockAr;
    for(UInt i = 0; i < MSPLMER_ARR_LMAX; i++)
    {
        if((abhlist->InOrderPmsadNR >= msadnr) && (abhlist->FreePmsadNR >= msadnr) && 
                (ListIsEmptyCareful(&abhlist->FreeLists) == FALSE))
        {
            return abhlist;
        }
    }
    return NULL;
}

private PMSAD* PickPMSADsOnPABHList(PABHList* abhlist)
{
    PMSAD* msad = NULL;
    PMSAD* end = NULL;
    
    IF_NULL_RETURN_NULL(abhlist);
    IF_LTN_RETURN(abhlist->FreePmsadNR, abhlist->InOrderPmsadNR, NULL);
    KrlMmLocked(&abhlist->Lock);
    
    if(ListIsEmptyCareful(&abhlist->FreeLists) == TRUE)
    {
        KrlMmUnLock(&abhlist->Lock);
        return NULL;
    }
    
    msad = ListFirstOne(&abhlist->FreeLists, PMSAD, Lists); 
    if(1 == abhlist->InOrderPmsadNR)
    {
        IF_NEQ_DEAD(MF_OLKTY_BAFH, RetPMSADOLType(msad), "PMSAD OLTYPE NOT MF_OLKTY_BAFH");
        ListDel(&msad->Lists);
        abhlist->FreePmsadNR -= 1;
        KrlMmUnLock(&abhlist->Lock);
        return msad;
    }

    IF_NEQ_DEAD(MF_OLKTY_ODER, RetPMSADOLType(msad), "PMSAD OLTYPE NOT MF_OLKTY_ODER");

    end = (PMSAD*)RetPMSADBlockLink(msad);
    IF_NEQ_DEAD(MF_OLKTY_BAFH, RetPMSADOLType(end), "PMSAD OLTYPE NOT MF_OLKTY_BAFH");
    
    ListDel(&msad->Lists);
    abhlist->FreePmsadNR -= abhlist->InOrderPmsadNR;
    KrlMmUnLock(&abhlist->Lock);
    return msad;
}

private Bool PutsPMSADsOnPABHList(PABHList* abhlist, PMSAD* msad, UInt order)
{
    PMSAD* msadend = NULL;
    IF_NULL_RETURN_FALSE(abhlist);
    IF_NULL_RETURN_FALSE(msad);
    
    if(PMSADIsFree(msad) == FALSE)
    {
        return FALSE;
    }
    if(abhlist->Order != order)
    {
        return FALSE;
    }
    
    KrlMmLocked(&abhlist->Lock);
    
    msadend = &msad[(1 << order) - 1];
    ListAdd(&msad->Lists, &abhlist->FreeLists);
    SetPMSADOLType(msad, MF_OLKTY_ODER);
    SetPMSADBlockLink(msad, (void*)msadend);
    SetPMSADOLType(msadend, MF_OLKTY_BAFH);
    SetPMSADBlockLink(msad, (void*)abhlist);
    abhlist->FreePmsadNR += (1 << order);
    abhlist->PmsadNR += (1 << order);
    KrlMmUnLock(&abhlist->Lock);
    return TRUE;
}

private PMSAD* OperationAfterAllocPMSADs(PABHList* abhlist, PMSAD* start, PMSAD* end)
{
    UInt msadnr = 0;
    IF_EQT_DEAD(NULL, abhlist, "PARM:abhlist == NULL\n");
    IF_EQT_DEAD(NULL, start, "PARM: start == NULL\n");
    IF_EQT_DEAD(NULL, end, "PARM:end == NULL\n");
    IF_EQT_DEAD(FALSE, PMSADIsFree(start), "PMSAD:start is Not Free\n");
    IF_EQT_DEAD(FALSE, PMSADIsFree(end), "PMSAD:end is Not Free\n");
    
    msadnr = (end - start) + 1;
    IF_NEQ_DEAD(msadnr, abhlist->InOrderPmsadNR, "abhlist->InOrderPmsadNR != msadnr\n");
    
    if(start == end)
    {
        IF_NEQ_DEAD(1, abhlist->InOrderPmsadNR, "abhlist->InOrderPmsadNR != 1\n");
        GetPMSAD(start);
        SetPMSADAlloc(start);
        SetPMSADOLType(start, MF_OLKTY_ODER);
        SetPMSADBlockLink(start, (void*)end);
		return start;
    }
    GetPMSAD(start);
    SetPMSADAlloc(start);

    GetPMSAD(end);
    SetPMSADAlloc(end);

    SetPMSADOLType(start, MF_OLKTY_ODER);
    SetPMSADBlockLink(start, (void*)end);
	
	return start;
}

private PMSAD* AllocPMSADsOnPABHList(MNode* node, MArea* area, PABHList* abhlist, PABHList* allocbhlist, UInt msadnr)
{
    PABHList* start = NULL;
    PABHList* end = NULL;
    PABHList* tmp = NULL;
    PMSAD* msad = NULL;
    Bool rets = FALSE;

    IF_NULL_RETURN_NULL(node);
    IF_NULL_RETURN_NULL(area);
    IF_NULL_RETURN_NULL(abhlist);
    IF_NULL_RETURN_NULL(allocbhlist);

    IF_LTN_RETURN(msadnr, abhlist->InOrderPmsadNR, NULL);
    IF_GTN_RETURN(msadnr, allocbhlist->InOrderPmsadNR, NULL);

    IF_GTN_RETURN(abhlist, allocbhlist, NULL);
    start = abhlist;
    end = allocbhlist;
    
    if(start == end)
    {
        msad = PickPMSADsOnPABHList(allocbhlist);
        IF_NULL_RETURN_NULL(msad);
        SetPMSADAlloc(msad);
        GetPMSAD(msad);
        return msad;
    }

    msad = PickPMSADsOnPABHList(allocbhlist);
    IF_NULL_RETURN_NULL(msad);

    tmp = end - 1;
    while(tmp >= start)
    {
        rets = PutsPMSADsOnPABHList(tmp, &msad[tmp->InOrderPmsadNR], tmp->Order);
        IF_NEQ_DEAD(FALSE, rets, "PMSADAddInPABHList rets FALSE\n");        
        tmp--;
    }
    OperationAfterAllocPMSADs(abhlist， msad， &msad[abhlist->InOrderPmsadNR]);
    return msad;
}

private PMSAD* KrlMmAllocPMSADsRealizeCore(GMemManage* gmm, MNode* node, MArea* area, UInt msadnr, U64 flags)
{
    PABHList* abhlist = NULL;
    PABHList* allocbhlist = NULL;
    PMSAD* msad = NULL;
    IF_NULL_RETURN_NULL(gmm);
    IF_NULL_RETURN_NULL(node);
    IF_NULL_RETURN_NULL(area);
    IF_ZERO_RETURN_NULL(msadnr);

    IF_LTN_RETURN(gmm->FreePMSAD, msadnr, NULL);
    IF_LTN_RETURN(area->FreePMSAD, msadnr, NULL);

    abhlist = ForPmsadNrRetPABListOnMArea(node, area, msadnr);
    IF_NULL_RETURN_NULL(abhlist);

    allocbhlist = ForPmsadNrRetAllocPABListOnMArea(node, area, msadnr);
    IF_NULL_RETURN_NULL(allocbhlist);
    
    msad = AllocPMSADsOnPABHList(node, area, abhlist, msadnr);
    IF_NULL_RETURN_NULL(msad);
    return msad;
}

private PMSAD* KrlMmAllocPMSADsRealize(UInt nodeid, UInt areaid, UInt msadnr, U64 flags)
{
    GMemManage* gmm = NULL;
    MNode* node = NULL;
    MArea* area = NULL;
    PMSAD* msad = NULL;

    gmm = KrlMmGetGMemManageAddr();
    IF_NULL_RETURN_NULL(gmm);

    node = KrlMmGetMNode(nodeid);
    IF_NULL_RETURN_NULL(node);
    
    area = KrlMmGetMArea(node, areaid);
    IF_NULL_RETURN_NULL(area);

    KrlMmLocked(&node->Lock);
    KrlMmLocked(&area->Lock);
    msad = KrlMmAllocPMSADsRealizeCore(gmm, node, area, msadnr, flags);
    KrlMmUnLock(&area->Lock);
    KrlMmUnLock(&node->Lock);
    return NULL;
}

public PMSAD* KrlMmAllocPMSADs(UInt nodeid, UInt areaid, UInt msadnr, U64 flags)
{
    return KrlMmAllocPMSADsRealize(nodeid, areaid, msadnr, flags);
}


public PMSAD* KrlMmAllocKernPMSADs(UInt msadnr)
{
    return KrlMmAllocPMSADs(DEFAULT_NODE_ID, KERN_AREA_ID, msadnr, KMAF_DEFAULT);
}