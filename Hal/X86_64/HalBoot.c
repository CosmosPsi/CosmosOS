/**********************************************************
		底层引导初始化文件HalBoot.c
***********************************************************
				彭东
**********************************************************/
#include "BaseType.h"
#include "HalBoot.h"
#include "HalFirmware.h"
#include "HalCPU.h"
#include "HalMMU.h"
#include "HalCache.h"
#include "HalIPC.h"
#include "HalInterrupt.h"
#include "HalInterface.h"
#include "KrlInit.h"

public MachStartInfo* HalGetMachStartInfoAddr()
{
    return (MachStartInfo*)HalPAddrToVAddr((Addr)MSI_PADR);
}

public Addr HalBootAllocMem(Size size)
{
    MachStartInfo* msinfo = NULL;
    Addr ret = NULL; 
    if(0 == size)
    {
        return NULL;
    }
    msinfo = HalExPGetMachStartInfoAddr();
    IF_NULL_DEAD(msinfo);
    
    ret = (Addr)msinfo->BrkCurrAddr;
    if(((U64)(ret + (Addr)size)) >= msinfo->BrkEndAddr)
    {
        return NULL;
    }
    msinfo->BrkCurrAddr += (U64)size;
    return HalPAddrToVAddr(ret);
}

public E820Map* HalBootGetNextE820()
{
    MachStartInfo* msinfo = NULL;
    E820Map* ret = NULL; 
    E820Map* start = NULL; 
    
    msinfo = HalExPGetMachStartInfoAddr();
    IF_NULL_DEAD(msinfo);

    start = (E820Map*)HalPAddrToVAddr((Addr)msinfo->E820PAddr);
    if(NULL == start ||  msinfo->E820Curr >= msinfo->E820NR)
    {
        return NULL;
    }
    
    ret = &start[msinfo->E820Curr];
    msinfo->E820Curr++;
    return ret;
}

private Bool HalBootInit()
{
    HalFirmwareInit();    
	HalCPUInit();
	HalMMUInit();
	HalCacheInit();
    HalInterruptInit();
	HalIPCInit();
	return TRUE;
}

public void HalBootStart()
{
	HalBootInit();
	KrlCoreInit();
	return;
}