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
