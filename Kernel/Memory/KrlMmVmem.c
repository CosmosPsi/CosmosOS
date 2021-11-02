/**********************************************************
        虚拟内存文件KrlMmVmem.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "HalBoot.h"
#include "HalCPU.h"
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
    ListInit(&init->VADLists);
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

private VPB* NewVPB()
{
    VPB* vpb = NULL;
    vpb = (VPB*)KrlMmNew(sizeof(VPB));
    IF_NULL_RETURN_NULL(vpb);
    VPBInit(vpb);
    return vpb;
}

private Bool DelVPB(VPB* vpb)
{
    IF_NULL_RETURN_FALSE(vpb);
	
	return KrlMmDel((void*)vpb);
}

private VAD* NewVAD()
{
	VAD* vad = NULL;
	vad = (VAD*)KrlMmNew(sizeof(VAD));
	IF_NULL_RETURN_NULL(vad);
	VADInit(vad);
	return vad;
}

private Bool DelVAD(VAD* vad)
{
    IF_NULL_RETURN_FALSE(vad);
	
	return KrlMmDel((void*)vad);
}

public Bool KrlVMemPutVPB(VPB* box)
{
    VBM* boxmgr = NULL;
	Bool rets = FALSE;

    IF_NULL_RETURN_FALSE(box);
    boxmgr = KrlMmGetVPBoxManagerAddr();
    IF_NULL_RETURN_FALSE(boxmgr);

	KrlMmLocked(&boxmgr->Lock);
	
	RefCountDec(&box->Count);
	if(RefCountRead(&box->Count) >= 1)
	{
		rets = TRUE;
		goto out;
	}
	
	if(boxmgr->VPBCacheNR >= boxmgr->VPBCacheMax)
	{
		ListDel(&box->Lists);
		if(DelVPB(box) == FALSE)
		{
			rets = FALSE;
			goto out;
		}
		else
		{
			boxmgr->VPBNR--;
			rets = TRUE;
			goto out;
		}
	}

	ListMove(&box->Lists, &boxmgr->VPBCacheHead);
	boxmgr->VPBCacheNR++;
	boxmgr->VPBNR--;
	
	rets = TRUE;
out:
	KrlMmUnLock(&boxmgr->Lock);
	return rets;
}

public VPB* KrlVMemGetVPB()
{
    VPB* box = NULL;
	VBM* boxmgr = NULL;
    
    boxmgr = KrlMmGetVPBoxManagerAddr();
    IF_NULL_RETURN_NULL(boxmgr);

	KrlMmLocked(&boxmgr->Lock);
	if(0 < boxmgr->VPBCacheNR)
	{
		if(ListIsEmptyCareful(&boxmgr->VPBCacheHead) == FALSE)
		{
			box = ListFirstOne(&boxmgr->VPBCacheHead, VPB, Lists);
			ListDel(&box->Lists);
			boxmgr->VPBCacheNR--;
			
			VPBInit(box);
			ListAdd(&box->Lists, &boxmgr->VPBHead);
			boxmgr->VPBNR++;
			RefCountInc(&box->Count);
			box->ParentVBM = boxmgr;
			//box = box;
			goto out; 
		}
	}

	box =NewVPB();
	if(NULL == box)
	{
		box = NULL;
		goto out;
	}

	ListAdd(&box->Lists, &boxmgr->VPBHead);
	boxmgr->VPBNR++;
	RefCountInc(&box->Count);
	box->ParentVBM = boxmgr;

out:
	KrlMmUnLock(&boxmgr->Lock);	
	return box;
}

private VAD* FindVADForVMFree(VAM* vam, Addr start, Size size)
{
	VAD* curr = NULL;
	VAD* vad = NULL;
    Addr newend = NULL;
	List* list = NULL;
	RBTree* srcrb = NULL;
    
    newend = start + (Addr)size;
    curr = vam->CurrVAD;
	if(NULL != curr)
	{
		if((curr->Start) <= start && (newend <= curr->End))
		{
			return curr;
		}
	}
	
    if(NULL == vam->TRoot.Node)
	{
		return NULL;
	}

	srcrb = vam->TRoot.Node;
	while(NULL != srcrb)
	{
	    vad = RBTreeEntry(srcrb, VAD, TNode);
		if((vad->Start <= start) && (newend <= vad->End))
		{
			return vad;
		}
		if((vad->Start > start) && (vad->End > newend) && (newend < vad->Start))
		{
			srcrb = srcrb->Left;
		}
		else
		{
			srcrb = srcrb->Right;
		}
	}
    
	return NULL;
}

private VAD* VADIsOkForVMAlloc(VAM *vam, VAD* vad, Addr start, Size size, U64 access, UInt type)
{
	VAD* nextvad = NULL;
	Addr newend = NULL;
    
    nextvad = start + (Addr)size;
    if((access == vad->Access) && (type == vad->MapType))
	{
        if(ListIsLast(&vad->Lists, &vam->VADLists) == FALSE)
        {
            nextvad = ListNextEntry(vad, VAD, Lists);
            if(NULL == start)
            {
                if((vad->End + (Addr)size) <= nextvad->Start)
                {
                    return vad;
                }
            }
            else
            {
                if((vad->End <= start) && (newend <= nextvad->Start))
                {
                    return vad;
                }
            }
        }
        else
        {
            if(NULL == start)
            {
                if((vad->End + (Addr)size) < vam->IsAllocEnd)
                {
                    return vad;
                }
            }
            else
            {
                if((vad->End <= start) && (newend < vam->IsAllocEnd))
                {
                    return vad;
                }
            }
        }
    }
	return NULL;
}

private VAD* FindVADForVMAlloc(VAM* vam, Addr start, Size size, U64 access, UInt type)
{
    VAD* vadcurrent = NULL;
    VAD* curr = NULL;
	Addr newend = NULL;
	List* list = NULL;

    curr = vam->CurrVAD;
    newend = start + size;

    IF_LTN_RETURN(size, MSAD_SIZE, NULL);
    IF_LTN_RETURN(vam->IsAllocEnd, newend, NULL);

	if(NULL != curr)
	{
		vadcurrent = VADIsOkForVMAlloc(vam, curr, start, size, access, type);
		if(NULL != vadcurrent)
		{
            return vadcurrent;
		}
	}

    ListForEach(list, &vam->VADLists)
	{
		curr = ListEntry(list, VAD, Lists);
		vadcurrent = VADIsOkForVMAlloc(vam, curr, start, size, access, type);
		if (NULL != vadcurrent)
		{
			return vadcurrent;
		}
	}

    return NULL;
}

private UInt VADRBRePlace(RBTree* srcrb, RBTree* reprb)
{
    VAD* srcvad = NULL;
    VAD* repvad = NULL;
    if(NULL == srcrb || NULL == reprb)
    {
        return 1;
    }
    srcvad = RBTreeEntry(srcrb, VAD, TNode);
    repvad = RBTreeEntry(reprb, VAD, TNode);
    HalMemCopy((void*)repvad, (void*)srcvad, sizeof(VAD));
    //srcrb->tr_flags.tr_hight = reprb->tr_flags.tr_hight;
    return 0; 
}

private UInt VADRBDelAfter(RBTree* delrb)
{
    VAD* vad = NULL;
    if(NULL == delrb)
    {
        return 1;
    }
    vad = RBTreeEntry(delrb, VAD, TNode);
    if(DelVAD(vad) == FALSE)
    {
        return 2;
    }
    return 0;
}

private UInt VADRBPathCMP(RBTree* srcrb, RBTree* cmprb)
{
    VAD* srcvad = NULL;
    VAD* cmpvad = NULL;
    IF_EQT_RETURN(srcrb, NULL, RBERR);
    IF_EQT_RETURN(cmprb, NULL, RBERR);

    srcvad = RBTreeEntry(srcrb, VAD, TNode);
    cmpvad = RBTreeEntry(cmprb, VAD, TNode);

    if((cmpvad->Start < srcvad->Start) && 
            (cmpvad->End < srcvad->End) && (cmpvad->End <= srcvad->Start))
    {
        return RBLEFT;
    }
    else if((cmpvad->Start > srcvad->Start) && 
            (cmpvad->End > srcvad->End) && (cmpvad->Start >= srcvad->End))
    {
        return RBRIGHT;
    }
    return RBERR;
}

private Bool VADReMoveVAM(VAM* vam, VAD* vad)
{
    RBTree* rbtree = NULL;
    RBTree* tmprbtree = NULL;
    IF_NULL_RETURN_FALSE(vam);
    IF_NULL_RETURN_FALSE(vad);
    tmprbtree = &vad->TNode;
    ListDel(&vad->Lists);
    rbtree = RBTreeDelete(&vam->TRoot, &vad->TNode, VADRBRePlace, VADRBDelAfter);
    IF_NEQ_DEAD(tmprbtree, rbtree, "RBTreeDelete call fail\n");
    vam->VADNr--;
    return TRUE;
}

private Addr VADInsertVAM(VAM* vam, VAD* currvad, VAD* newvad)
{
    RBTree* rbtree = NULL;
    IF_NULL_RETURN_NULL(vam);
    IF_NULL_RETURN_NULL(currvad);
    IF_NULL_RETURN_NULL(newvad);
    newvad->ParentVAM = vam;
	
    rbtree = RBTreeInsert(&vam->TRoot, &newvad->TNode, VADRBPathCMP);
    IF_NULL_RETURN_NULL(rbtree);
    IF_NEQ_RETURN(newvad, RBTreeEntry(rbtree, VAD, TNode), NULL);

    vam->CurrVAD = newvad;
	ListAdd(&newvad->Lists, &currvad->Lists);
	if(ListIsLast(&newvad->Lists, &vam->VADLists) == TRUE)
	{
		vam->EndVAD = newvad;
	}
    vam->VADNr++;
    return newvad->Start;
}

private void SetEndAndCurVADForVMFree(VAM* vam, VAD* vad)
{
	VAD* prevvad = NULL;
    VAD* nextvad = NULL;
	if(ListIsLast(&vad->Lists, &vam->VADLists) == TRUE)
	{
		if(ListIsFirst(&vad->Lists, &vam->VADLists) == FALSE)
		{
			prevvad = ListPrevEntry(vad, VAD, Lists);
			vam->EndVAD = prevvad;
			vam->CurrVAD = prevvad;
		}
		else
		{
			vam->EndVAD = NULL;
			vam->CurrVAD = NULL;
		}
	}
	else
	{
		nextvad = ListNextEntry(vad, VAD, Lists);
		vam->CurrVAD = nextvad;
	}
	return;
}

private Bool DelUserPMSADsForVMemUnMapping(VMS* vms, VPB* box, PMSAD* msad, Addr phyadr)
{
	Bool rets = FALSE;
	PMSAD* tmpmsad = NULL;
    PMSAD* delmsad = NULL;
	List* pos = NULL;

    IF_NULL_RETURN_FALSE(vms);
    IF_NULL_RETURN_FALSE(box);
    IF_ZERO_RETURN_FALSE(phyadr);

    KrlMmLocked(&box->Lock);

	if(NULL != msad)
	{
		if(PMSADRetPAddr(msad) == phyadr)
		{
			delmsad = msad;
			ListDel(&msad->Lists);
			box->PmsadNR--;
			rets = TRUE;
			goto out;
		}
	}

	ListForEach(pos, &box->PmsadLists)
	{
		tmpmsad = ListEntry(pos, PMSAD, Lists);
		if (PMSADRetPAddr(tmpmsad) == phyadr)
		{
			delmsad = tmpmsad;
			ListDel(&tmpmsad->Lists);
			box->PmsadNR--;
			rets = TRUE;
			goto out;
		}
	}

	delmsad = NULL;
	rets = FALSE;

out:
    KrlMmUnLock(&box->Lock);

	if(NULL != delmsad)
	{
        IF_NEQ_DEAD(TRUE, KrlMmFreeUserPMSADs(delmsad), "Free User PMSAD Fail\n");
	}
	return rets;
}

private PMSAD* NewUserPMSADsForVMemMapping(VMS* vms, VPB* box)
{
	PMSAD* msad = NULL;
    IF_NULL_RETURN_NULL(vms);
    IF_NULL_RETURN_NULL(box);

    msad = KrlMmAllocUserPMSADs(1);
	IF_NULL_RETURN_NULL(msad);

    KrlMmLocked(&box->Lock);
    ListAdd(&msad->Lists, &box->PmsadLists);
    box->PmsadNR++;
    KrlMmUnLock(&box->Lock);
	return msad;
}

private Bool KrlVMemUnMappingRealize(VMS* vms, VAD* vad, Addr start, Addr end)
{
    Addr phyadr;
	Bool rets = TRUE;
	MMU* mmu = NULL;
    VPB* box = NULL;
    PMSAD* msad = NULL;
    mmu = &vms->Mmu;
	box = vad->PMSADBox;
    IF_NULL_RETURN_FALSE(box);

	for(Addr vadr = start; vadr < end; vadr += VMAP_MIN_SIZE)
	{
		phyadr = HalUnMMUTranslation(mmu, vadr);
		if(NULL != phyadr)
		{
			rets = DelUserPMSADsForVMemUnMapping(vms, box, PHYAddrRetPMSAD((U64)phyadr), phyadr);
            IF_NEQ_DEAD(TRUE, rets, "Call DelUserPMSADsForVMemUnMapping Fail\n");
		}
	}

	return rets;
}

public Bool KrlVMemUnMapping(VMS* vms, VAD* vad, Addr start, Size size)
{
	Addr end = NULL;
    IF_NULL_RETURN_FALSE(vms);
    IF_NULL_RETURN_FALSE(vad);

	end = start + (Addr)size;

	return KrlVMemUnMappingRealize(vms, vad, start, end);
}

private Bool KrlVMemFreeRealizeCore(VMS* vms, VAM* vam, Addr start, Size size)
{
    Bool rets = FALSE;
	VAD* newvad = NULL; 
    VAD* delvad = NULL;
    IF_NULL_RETURN_FALSE(vam);
    KrlMmLocked(&vam->Lock);
    delvad = FindVADForVMFree(vam, start, size);
    if(NULL == delvad)
	{
		rets = FALSE;
		goto out;
	}
    if((delvad->Start == start) && ((start + (Addr)size) == delvad->End))
    {
        KrlVMemUnMapping(vms, delvad, start, size);
        SetEndAndCurVADForVMFree(vam, delvad);
        VADReMoveVAM(vam, delvad);
        rets = TRUE;
		goto out;
    }

    if((delvad->Start == start) && (delvad->End > (start + (Addr)size)))
	{
		delvad->Start = start + (Addr)size;
        KrlVMemUnMapping(vms, delvad, start, size);
		rets = TRUE;
		goto out;
	}

    if((delvad->Start < start) && (delvad->End == (start + (Addr)size)))
	{
		delvad->End = start;
        KrlVMemUnMapping(vms, delvad, start, size);
		rets = TRUE;
		goto out;
	}

    if ((delvad->Start < start) && (delvad->End > (start + (Addr)size)))
	{
		newvad = NewVAD();
		if(NULL == newvad)
		{
			rets = FALSE;
			goto out;
		}

		newvad->End = delvad->End;
		newvad->Start = start + (Addr)size;
		newvad->Access = delvad->Access;
		newvad->MapType = delvad->MapType;
		newvad->ParentVAM = vam;
		delvad->End = start;

		KrlVPBCountInc(delvad->PMSADBox);
		newvad->PMSADBox = delvad->PMSADBox;
        KrlVMemUnMapping(vms, delvad, start, size);
        VADInsertVAM(vam, delvad, newvad);
		rets = TRUE;
		goto out;
	}

    rets = FALSE;

out:
    KrlMmUnLock(&vam->Lock);
    return rets;
}

private Addr KrlVMemAllocRealizeCore(VMS* vms, VAM* vam, Addr start, Size size, U64 access, UInt type)
{
    Addr vaddr = NULL;
    VAD* currvad = NULL;
    VAD* newvad = NULL;

    IF_NULL_RETURN_NULL(vam);
    KrlMmLocked(&vam->Lock);
    currvad = FindVADForVMAlloc(vam, start, size, access, type);
	if(NULL == currvad)
	{
		vaddr = NULL;
		goto out;
	}

	if(((NULL == start) || (start == currvad->End)))
	{
		vaddr = currvad->End;
		currvad->End += (Addr)size;
		vam->CurrVAD = currvad;
		goto out;
	}
    
    newvad = NewVAD();
    if(NULL == newvad)
    {
        vaddr = NULL;
        goto out;
    }

    if(NULL == start)
	{
		newvad->Start = currvad->End;
	}
	else
	{
		newvad->Start = start;
	}

	newvad->End = newvad->Start + (Addr)size;
	newvad->Access = access;
	newvad->MapType = type;
	
	vaddr = VADInsertVAM(vam, currvad, newvad);
    if(NULL == vaddr)
    {
        IF_NEQ_DEAD(TRUE, DelVAD(newvad), "Del VAD fail\n");
    }

out:
    KrlMmUnLock(&vam->Lock);
    return vaddr;
}

private Bool KrlVMemFreeRealize(VMS* vms, Addr start, Size size)
{
    Bool rets = FALSE;
    IF_NULL_RETURN_NULL(vms);
    KrlMmLocked(&vms->Lock);
    rets = KrlVMemFreeRealizeCore(vms, &vms->VAManager, start, VADSIZE_ALIGN(size));
    KrlMmUnLock(&vms->Lock);
    return rets;
}

private Addr KrlVMemAllocRealize(VMS* vms, Addr start, Size size, U64 access, UInt type)
{
    Addr vaddr = NULL;
    IF_NULL_RETURN_NULL(vms);
    KrlMmLocked(&vms->Lock);
    vaddr = KrlVMemAllocRealizeCore(vms, &vms->VAManager, start, VADSIZE_ALIGN(size), access, type);
    KrlMmUnLock(&vms->Lock);
    return vaddr;
}

public Bool KrlVMemFree(VMS* vms, Addr start, Size size)
{
    IF_NULL_RETURN_FALSE(vms);
    IF_NULL_RETURN_FALSE(size);
    IF_LTNONE_RETRUN_FALSE(size);

	return KrlVMemFreeRealize(vms, start, size);
}

public Addr KrlVMemAlloc(VMS* vms, Addr start, Size size, U64 access, UInt type)
{
    IF_NULL_RETURN_NULL(vms);
    if(NULL != start)
    {
        IF_NEQ_RETURN(0, (start & (MSAD_SIZE - 1)), NULL);
        IF_LTN_RETURN(start, MSAD_SIZE, NULL);
        IF_LTN_RETURN(USER_VIRTUAL_ADDRESS_END, (start + size), NULL);
    }
    return KrlVMemAllocRealize(vms, start, size, access, type);
}

public VMS* KrlMmGetCurrVMS()
{
    return KrlMmGetRootVMemSPaceAddr();
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