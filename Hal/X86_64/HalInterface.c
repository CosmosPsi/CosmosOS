
/**********************************************************
		硬件抽象层接口文件HalInterface.c
***********************************************************
				彭东
**********************************************************/
#include "BaseType.h"
#include "KrlMmManage.h"
#include "HalBoot.h"
#include "HalInterface.h"
public PMSAD* HalExPGetKrlOnePMSAD()
{
    return NULL;
}

public Bool HalExPPutKrlOnePMSAD(PMSAD* msad)
{
    return FALSE;
}

public MachStartInfo* HalExPGetMachStartInfoAddr()
{
    return HalGetMachStartInfoAddr();
}

public Addr HalExPBootAllocMem(Size size)
{
    return HalBootAllocMem(size);
}

public E820Map* HalExPBootGetNextE820()
{
    return HalBootGetNextE820();
}