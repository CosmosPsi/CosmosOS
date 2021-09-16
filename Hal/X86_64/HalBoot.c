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
#include "KrlInit.h"

public MachStartInfo* HalGetMachStartInfoAddr()
{
    return (MachStartInfo*)HalPAddrToVAddr((Addr)MSI_PADR);
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