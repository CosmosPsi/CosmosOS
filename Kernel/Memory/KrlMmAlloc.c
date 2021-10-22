/**********************************************************
        物理内存分配文件KrlMmAlloc.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "HalCPU.h"
#include "KrlMmManage.h"
#include "KrlMmAlloc.h"

private SInt ForPmsadNrRetOrder(UInt msadnr)
{
	SInt mbits = HalSearch64RLBits((U64)msadnr);
    IF_LTN_RETURN(mbits, 0, -1);

	if(msadnr & (msadnr - 1))
	{
		mbits++;//有问题不要使用
	}
	return mbits;
}


private PABHList* ForPmsadNrRetPABListOnMArea(MNode* node, MArea* area, UInt msadnr)
{
    PABHList* abhlist = NULL;
    IF_NULL_RETURN_NULL(node);
    IF_NULL_RETURN_NULL(area);
    abhlist = area->MSPLMerData.PAddrBlockArr;
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
    abhlist = area->MSPLMerData.PAddrBlockArr;
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
        abhlist->PmsadNR -= 1;
        KrlMmUnLock(&abhlist->Lock);
        return msad;
    }

    IF_NEQ_DEAD(MF_OLKTY_ODER, RetPMSADOLType(msad), "PMSAD OLTYPE NOT MF_OLKTY_ODER");

    end = (PMSAD*)RetPMSADBlockLink(msad);
    IF_NEQ_DEAD(MF_OLKTY_BAFH, RetPMSADOLType(end), "PMSAD OLTYPE NOT MF_OLKTY_BAFH");
    
    ListDel(&msad->Lists);
    abhlist->FreePmsadNR -= abhlist->InOrderPmsadNR;
    abhlist->PmsadNR -= abhlist->InOrderPmsadNR;
    KrlMmUnLock(&abhlist->Lock);
    return msad;
}

private Bool PutsPMSADsOnPABHList(PABHList* abhlist, PMSAD* msad, PMSAD* msadend, UInt order)
{
    PMSAD* end = NULL;
    IF_NULL_RETURN_FALSE(abhlist);
    IF_NULL_RETURN_FALSE(msad);
    IF_NULL_RETURN_FALSE(msadend);
    
    if(PMSADIsFree(msad) == FALSE)
    {
        return FALSE;
    }
    if(abhlist->Order != order)
    {
        return FALSE;
    }
    
    end = &msad[(1 << order) - 1];
    IF_NEQ_RETURN(end, msadend, FALSE);

    KrlMmLocked(&abhlist->Lock);
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
        rets = PutsPMSADsOnPABHList(tmp, &msad[tmp->InOrderPmsadNR], &msad[tmp->InOrderPmsadNR + tmp->InOrderPmsadNR - 1], tmp->Order);
        IF_NEQ_DEAD(FALSE, rets, "PMSADAddInPABHList rets FALSE\n");        
        tmp--;
    }
    OperationAfterAllocPMSADs(abhlist, msad, &msad[abhlist->InOrderPmsadNR]);
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
    
    msad = AllocPMSADsOnPABHList(node, area, abhlist, allocbhlist, msadnr);
    IF_NULL_RETURN_NULL(msad);
    return msad;
}

private PMSAD* KrlMmAllocPMSADsRealize(UInt nodeid, UInt areaid, UInt msadnr, U64 flags)
{
    GMemManage* gmm = NULL;
    MNode* node = NULL;
    MArea* area = NULL;
    PMSAD* msad = NULL;
    UInt msadnr = 0;
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
    msadnr = (UInt)KrlMmGetPMSADsLen(msad);
    KrlMmUPAddMNodeAreaAllocMaxFreeNR(node, area, msadnr, 0, 0);
    KrlMmUPSubMNodeAreaAllocMaxFreeNR(node, area, 0, 0, msadnr);
    KrlMmUPAddGMMAllocMaxFreeNR(msadnr, 0, 0);
    KrlMmUPSubGMMAllocMaxFreeNR(0, 0, msadnr);
    return msad;
}

public U64 KrlMmGetPMSADsLen(PMSAD* msad)
{
    PMSAD* end = NULL;
	IF_NULL_RETURN_ZERO(msad);
    IF_NEQ_RETURN(TRUE, PMSADIsFree(msad), 0);
	IF_NULL_RETURN_ZERO(RetPMSADBlockLink(msad));

    end = (PMSAD*)RetPMSADBlockLink(msad);
    IF_LTN_RETURN(end, msad, 0);

	return ((U64)(end - msad) + 1);
}

public U64 KrlMmGetPMSADsSize(PMSAD* msad)
{
    U64 msadlen = 0;
	IF_NULL_RETURN_ZERO(msad);
    IF_NEQ_RETURN(TRUE, PMSADIsFree(msad), 0);
    msadlen = KrlMmGetPMSADLen(msad);
    return (msadlen << MSAD_PADR_SLBITS);
}

public PMSAD* KrlMmGetPMSADsEnd(PMSAD* msad)
{
    PMSAD* end = NULL;
	IF_NULL_RETURN_NULL(msad);
    IF_NEQ_RETURN(TRUE, PMSADIsFree(msad), NULL);
    IF_NEQ_RETURN(MF_OLKTY_ODER, RetPMSADOLType(msad), NULL);
	IF_NULL_RETURN_NULL(RetPMSADBlockLink(msad));

    end = (PMSAD*)RetPMSADBlockLink(msad);
    IF_LTN_RETURN(end, msad, NULL);
	return end;
}

public Addr KrlMmGetPMSADsRangeVStart(PMSAD* msad)
{
	IF_NULL_DEAD(msad);
    IF_NEQ_DEAD(TRUE, PMSADIsFree(msad), "PMSAD Is Not Free\n");
    return PMSADRetVAddr(msad);
}

public Addr KrlMmGetPMSADsRangeVEnd(PMSAD* msad)
{
    PMSAD* end = NULL;
	IF_NULL_DEAD(msad);
    IF_NEQ_DEAD(TRUE, PMSADIsFree(msad), "PMSAD Is Not Free\n");
    end = KrlMmGetPMSADsEnd(msad);
    IF_NULL_DEAD(end);
    return (Addr)((PMSADRetVAddr(end) + MSAD_SIZE) - 1);
}

public PMSAD* KrlMmAllocPMSADs(UInt nodeid, UInt areaid, UInt msadnr, U64 flags)
{
    return KrlMmAllocPMSADsRealize(nodeid, areaid, msadnr, flags);
}


public PMSAD* KrlMmAllocKernPMSADs(UInt msadnr)
{
    return KrlMmAllocPMSADs(DEFAULT_NODE_ID, KERN_AREA_ID, msadnr, KMAF_DEFAULT);
}

public PMSAD* KrlMmAllocUserPMSADs(UInt msadnr)
{
    IF_NEQONE_RETRUN_NULL(msadnr);
    return KrlMmAllocPMSADs(DEFAULT_NODE_ID, USER_AREA_ID, msadnr, KMAF_DEFAULT);
}

private UInt OperationBeforeFreePMSADs(PABHList* abhlist, PMSAD* start, PMSAD* end)
{
    UInt msadnr = 0;
    IF_EQT_DEAD(NULL, abhlist, "PARM:abhlist == NULL\n");
    IF_EQT_DEAD(NULL, start, "PARM: start == NULL\n");
    IF_EQT_DEAD(NULL, end, "PARM:end == NULL\n");
    IF_EQT_DEAD(TRUE, PMSADIsFree(start), "PMSAD:start is Not Alloc\n");
    IF_EQT_DEAD(TRUE, PMSADIsFree(end), "PMSAD:end is Not Alloc\n");
    
    msadnr = (end - start) + 1;
    IF_NEQ_DEAD(msadnr, abhlist->InOrderPmsadNR, "abhlist->InOrderPmsadNR != msadnr\n");
    
    if(start == end)
    {
        IF_NEQ_DEAD(1, abhlist->InOrderPmsadNR, "abhlist->InOrderPmsadNR != 1\n");
        PutPMSAD(start);
        if(RetPMSADRefCount(start) > 0)
        {
            return 1;
        }

        KrlMmClearPMSADUseStatus(start);
        SetPMSADOLType(start, MF_OLKTY_BAFH);
        SetPMSADBlockLink(start, (void*)abhlist);
		return 2;
    }

    PutPMSAD(start);
    PutPMSAD(end);
    if(RetPMSADRefCount(start) > 0)
    {
        return 1;
    }

    KrlMmClearPMSADUseStatus(start);
    KrlMmClearPMSADUseStatus(end);

    SetPMSADOLType(start, MF_OLKTY_ODER);
    SetPMSADBlockLink(start, (void*)end);
    SetPMSADOLType(end, MF_OLKTY_BAFH);
    SetPMSADBlockLink(end, (void*)abhlist);
	return 2;
}

private Bool CheckOnePMSADBlock(PABHList* abhlist, PMSAD* mblockstart, PMSAD* mblockend)
{
    IF_NULL_RETURN_FALSE(abhlist);
    IF_NULL_RETURN_FALSE(mblockstart);
    IF_NULL_RETURN_FALSE(mblockend);
    IF_LTN_RETURN(mblockend, mblockstart, FALSE);
    IF_NEQ_RETURN(abhlist->InOrderPmsadNR, ((mblockend - mblockstart) + 1), FALSE);
    IF_NEQ_RETURN(TRUE, PMSADIsFree(mblockstart), FALSE);
    IF_NEQ_RETURN(TRUE, PMSADIsFree(mblockend), FALSE);
    return TRUE;
}

private Bool ClearPMSADTowBlockFlags(PABHList* abhlist, PMSAD* _1mblocks, PMSAD* _1mblocke, PMSAD* _2mblocks, PMSAD* _2mblocke)
{
    if(_1mblocks == _2mblocks || _1mblocke == _2mblocke)
	{
		return FALSE;
	}
    SetPMSADOLType(_1mblocke, MF_OLKTY_INIT);
    SetPMSADBlockLink(_1mblocke, NULL);
    SetPMSADOLType(_2mblocks, MF_OLKTY_INIT);
    SetPMSADBlockLink(_2mblocks, NULL);

    SetPMSADOLType(_1mblocks, MF_OLKTY_ODER);
    SetPMSADBlockLink(_1mblocks, (void*)_2mblocke);
	SetPMSADOLType(_2mblocke, MF_OLKTY_BAFH);
    SetPMSADBlockLink(_2mblocke, (void*)abhlist);
    return TRUE;
}

private UInt PMSADTowBlockIsOk(PABHList* abhlist, PMSAD* _1mblocks, PMSAD* _1mblocke, PMSAD* _2mblocks, PMSAD* _2mblocke)
{
    IF_NEQ_RETURN(TRUE, CheckOnePMSADBlock(abhlist, _1mblocks, _1mblocke), 1);
    IF_NEQ_RETURN(TRUE, CheckOnePMSADBlock(abhlist, _2mblocks, _2mblocke), 1);
    IF_EQT_RETURN(TRUE, PMSADIsAdjacent(_1mblocke, _2mblocks), 2);
    IF_EQT_RETURN(TRUE, PMSADIsAdjacent(_2mblocke, _1mblocks), 4);
    return 0;
}

private UInt FindContinuousPMSADsBlock(PABHList* abhlist, PMSAD** msadstart, PMSAD** msadend)
{
    PMSAD* start = NULL;
    PMSAD* end = NULL;
    UInt msadnr = 0;
    List* tmplst = NULL;
	PMSAD* tmpmsa = NULL;
    PMSAD* blkms = NULL;
    PMSAD* blkme = NULL;
	UInt rets = 0;
    
    start = *msadstart;
    end = *msadend;
	msadnr = (UInt)KrlMmGetPMSADsLen(start);

    KrlMmLocked(&abhlist->Lock);   
    if(1 > abhlist->FreePmsadNR)
    {
        KrlMmUnLock(&abhlist->Lock);
        return 1;
    }

    IF_EQT_DEAD(abhlist->InOrderPmsadNR, msadnr, "PMSAD len NEQ area order NR\n");
    
    ListForEach(tmplst, &abhlist->FreeLists)
    {
        tmpmsa = ListEntry(tmplst, PMSAD, Lists);
        rets = PMSADTowBlockIsOk(abhlist, start, end, tmpmsa, &tmpmsa[abhlist->InOrderPmsadNR - 1]);
		if (2 == rets || 4 == rets)
		{
			blkms = tmpmsa;
			blkme = &tmpmsa[abhlist->InOrderPmsadNR - 1];
			ListDel(&tmpmsa->Lists);
			abhlist->FreePmsadNR -= abhlist->InOrderPmsadNR;
            abhlist->PmsadNR -= abhlist->InOrderPmsadNR;
            KrlMmUnLock(&abhlist->Lock);
			goto step1;
		}
    }

step1:

    KrlMmUnLock(&abhlist->Lock);
	if(0 == rets || 1 == rets)
	{
		return 1;
	}

    if(2 == rets)
	{
		if(ClearPMSADTowBlockFlags(abhlist + 1, start, end, blkms, blkme) == TRUE)
		{
			*msadstart = start;
			*msadend = blkme;
			return 2;
		}
		return 0;
	}

	if(4 == rets)
	{
		if(ClearPMSADTowBlockFlags(abhlist + 1, blkms, blkme, start, end) == TRUE)
		{
			*msadstart = blkms;
			*msadend = end;
			return 2;
		}
		return 0;
	}
    return 0;
}

private Bool FreePMSADsOnPABHList(MNode* node, MArea* area, PABHList* abhlist, PABHList* freebhlist, PMSAD* msad, UInt msadnr)
{
    PABHList* start = NULL;
    PABHList* end = NULL;
    PABHList* tmp = NULL;
	PMSAD* mblockstart = NULL;
    PMSAD* mblockend = NULL;
    Bool rets = FALSE;
    UInt oks = 0;

    IF_NULL_RETURN_FALSE(node);
    IF_NULL_RETURN_FALSE(area);
    IF_NULL_RETURN_FALSE(abhlist);
    IF_NULL_RETURN_FALSE(freebhlist);

    IF_GTN_RETURN(abhlist, freebhlist, FALSE);
    start = abhlist;
    end = freebhlist;

    mblockstart = msad;
    mblockend = &msad[msadnr - 1];
	for(tmp = start; tmp < end; tmp++)
	{
		oks = FindContinuousPMSADsBlock(tmp, &mblockstart, &mblockend);
		if(1 == oks)
		{
			break;
		}
		IF_EQT_DEAD(0, oks, "oks is zero\n");
	}
    IF_NEQ_DEAD(tmp->InOrderPmsadNR, KrlMmGetPMSADsLen(mblockstart), "tmp->InOrderPmsadNR != mblockstart len\n");
    return PutsPMSADsOnPABHList(tmp, mblockstart, mblockend, tmp->Order);
}

private Bool KrlMmFreePMSADsRealizeCore(GMemManage* gmm, MNode* node, MArea* area, PMSAD* msad, U64 flags)
{
    Bool rets = FALSE;
    PABHList* abhlist = NULL;
    PABHList* freebhlist = NULL;
    UInt msadnr = 0;
    UInt opt = 0;
    IF_NULL_RETURN_FALSE(gmm);
    IF_NULL_RETURN_FALSE(node);
    IF_NULL_RETURN_FALSE(area);
    IF_NULL_RETURN_FALSE(msad);

    msadnr = (UInt)KrlMmGetPMSADsLen(msad);
    freebhlist = &area->MSPLMerData.PAddrBlockArr[MSPLMER_ARR_LMAX - 1];
    abhlist = ForPmsadNrRetPABListOnMArea(node, area, msadnr);
    IF_NULL_RETURN_FALSE(abhlist);
    opt = OperationBeforeFreePMSADs(abhlist, msad, KrlMmGetPMSADsEnd(msad));
    if(2 == opt)
	{
        rets = FreePMSADsOnPABHList(node, area,abhlist, freebhlist, msad, msadnr);
        if(TRUE == rets)
		{
			return rets;
		}
		return FALSE;
	}
	if(1 == opt)
	{
		return TRUE;
	}
	if(0 == opt)
	{
		return FALSE;
	}
    return FALSE;
}

private Bool KrlMmFreePMSADsRealize(PMSAD* msad, U64 flags)
{
    Bool rets = FALSE;
    UInt msadnr = 0;
    GMemManage* gmm = NULL;
    MNode* node = NULL;
    MArea* area = NULL;
    IF_NULL_RETURN_FALSE(msad);

    gmm = KrlMmGetGMemManageAddr();
    IF_NULL_RETURN_FALSE(gmm);

    node = PMSADRetItsMNode(msad);
    IF_NULL_RETURN_FALSE(node);
    
    area = PMSADRetItsMArea(msad);
    IF_NULL_RETURN_FALSE(area);
    
    msadnr = (UInt)KrlMmGetPMSADsLen(msad);

    KrlMmLocked(&node->Lock);
    KrlMmLocked(&area->Lock);
    rets = KrlMmFreePMSADsRealizeCore(gmm, node, area, msad, flags);
    KrlMmUnLock(&area->Lock);
    KrlMmUnLock(&node->Lock);
    KrlMmUPAddMNodeAreaAllocMaxFreeNR(node, area, 0, 0, msadnr);
    KrlMmUPSubMNodeAreaAllocMaxFreeNR(node, area, msadnr, 0, 0);
    KrlMmUPAddGMMAllocMaxFreeNR(0, 0, msadnr);
    KrlMmUPSubGMMAllocMaxFreeNR(msadnr, 0, 0);
    return rets;
}

public Bool KrlMmFreePMSADs(PMSAD* msad, U64 flags)
{
    return KrlMmFreePMSADsRealize(msad, flags);
}

public Bool KrlMmFreeKernPMSADs(PMSAD* msad)
{
    return KrlMmFreePMSADs(msad, KMAF_DEFAULT);
}

public Bool KrlMmFreeUserPMSADs(PMSAD* msad)
{
    IF_NULL_RETURN_FALSE(msad);
    return KrlMmFreePMSADs(msad, KMAF_DEFAULT);
}