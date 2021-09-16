/**********************************************************
		硬件抽象层接口头文件HalInterface.h
***********************************************************
				彭东
**********************************************************/
#ifndef _HALINTERFACEHEAD
#define _HALINTERFACEHEAD
public PMSAD* HalExPGetKrlOnePMSAD();
public Bool HalExPPutKrlOnePMSAD(PMSAD* msad);
public MachStartInfo* HalExPGetMachStartInfoAddr();
public Addr HalExPBootAllocMem(Size size);
public E820Map* HalExPBootGetNextE820();
#endif