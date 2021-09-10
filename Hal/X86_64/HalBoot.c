/**********************************************************
		底层引导初始化文件HalBoot.c
***********************************************************
				彭东
**********************************************************/
#include "BaseType.h"
#include "HalBoot.h"
#include "HalCPU.h"
#include "HalMMU.h"
#include "HalCache.h"
#include "HalIPC.h"
#include "HalInterrupt.h"
#include "KrlInit.h"

private Bool HalBootInit()
{
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