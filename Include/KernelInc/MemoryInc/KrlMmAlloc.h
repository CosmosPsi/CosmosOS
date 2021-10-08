/**********************************************************
        物理内存分配文件KrlMmAlloc.c
***********************************************************
                彭东
**********************************************************/

#ifndef _KRLMMALLOCHEAD
#define _KRLMMALLOCHEAD

#define KMAF_DEFAULT (0) 
#define DEFAULT_NODE_ID (0)
#define INIT_AREA_ID (MA_TYPE_INIT) 
#define HWAD_AREA_ID (MA_TYPE_HWAD)
#define KERN_AREA_ID (MA_TYPE_KRNL)
#define USER_AREA_ID (MA_TYPE_PROC) 
#define SHAR_AREA_ID (MA_TYPE_SHAR)

private PABHList* ForPmsadNrRetPABListOnMArea(MNode* node, MArea* area, UInt msadnr);
private PABHList* ForPmsadNrRetAllocPABListOnMArea(MNode* node, MArea* area, UInt msadnr);
private PMSAD* PickPMSADsOnPABHList(PABHList* abhlist);
private Bool PutsPMSADsOnPABHList(PABHList* abhlist, PMSAD* msad, UInt order);
private PMSAD* OperationAfterAllocPMSADs(PABHList* abhlist, PMSAD* start, PMSAD* end);
private PMSAD* AllocPMSADsOnPABHList(MNode* node, MArea* area, PABHList* abhlist, PABHList* allocbhlist, UInt msadnr);
private PMSAD* KrlMmAllocPMSADsRealizeCore(GMemManage* gmm, MNode* node, MArea* area, UInt msadnr, U64 flags);
private PMSAD* KrlMmAllocPMSADsRealize(UInt nodeid, UInt areaid, UInt msadnr, U64 flags);
public PMSAD* KrlMmAllocPMSADs(UInt nodeid, UInt areaid, UInt msadnr, U64 flags);
public PMSAD* KrlMmAllocKernPMSADs(UInt msadnr);
#endif
