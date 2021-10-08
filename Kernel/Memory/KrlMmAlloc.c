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

