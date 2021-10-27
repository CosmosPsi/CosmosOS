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