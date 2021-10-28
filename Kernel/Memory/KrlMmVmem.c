/**********************************************************
        虚拟内存文件KrlMmVmem.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "HalBoot.h"
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
	
	return KrlMmDel((void *)vad);
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

	if (NULL != curr)
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

private Addr KrlVMemAllocRealize(VMS* vms, Addr start, Size size, U64 access, UInt type)
{
    Addr vaddr = NULL;
    IF_NULL_RETURN_NULL(vms);
    KrlMmLocked(&vms->Lock);
    vaddr = KrlVMemAllocRealizeCore(vms, &vms->VAManager, start, VADSIZE_ALIGN(size), access, type);
    KrlMmUnLock(&vms->Lock);
    return vaddr;
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