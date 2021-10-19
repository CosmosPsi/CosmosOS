/**********************************************************
        物理内存分配头文件KrlMmAlloc.h
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
private SInt ForPmsadNrRetOrder(UInt msadnr);
private PABHList* ForPmsadNrRetPABListOnMArea(MNode* node, MArea* area, UInt msadnr);
private PABHList* ForPmsadNrRetAllocPABListOnMArea(MNode* node, MArea* area, UInt msadnr);
private PMSAD* PickPMSADsOnPABHList(PABHList* abhlist);
private Bool PutsPMSADsOnPABHList(PABHList* abhlist, PMSAD* msad, PMSAD* msadend, UInt order);
private PMSAD* OperationAfterAllocPMSADs(PABHList* abhlist, PMSAD* start, PMSAD* end);
private PMSAD* AllocPMSADsOnPABHList(MNode* node, MArea* area, PABHList* abhlist, PABHList* allocbhlist, UInt msadnr);
private PMSAD* KrlMmAllocPMSADsRealizeCore(GMemManage* gmm, MNode* node, MArea* area, UInt msadnr, U64 flags);
private PMSAD* KrlMmAllocPMSADsRealize(UInt nodeid, UInt areaid, UInt msadnr, U64 flags);
public U64 KrlMmGetPMSADsLen(PMSAD* msad);
public U64 KrlMmGetPMSADsSize(PMSAD* msad);
public PMSAD* KrlMmGetPMSADsEnd(PMSAD* msad);
public Addr KrlMmGetPMSADsRangeVStart(PMSAD* msad);
public Addr KrlMmGetPMSADsRangeVEnd(PMSAD* msad);
public PMSAD* KrlMmAllocPMSADs(UInt nodeid, UInt areaid, UInt msadnr, U64 flags);
public PMSAD* KrlMmAllocKernPMSADs(UInt msadnr);
public PMSAD* KrlMmAllocUserPMSADs(UInt msadnr);
private UInt OperationBeforeFreePMSADs(PABHList* abhlist, PMSAD* start, PMSAD* end);
private Bool CheckOnePMSADBlock(PABHList* abhlist, PMSAD* mblockstart, PMSAD* mblockend);
private Bool ClearPMSADTowBlockFlags(PABHList* abhlist, PMSAD* _1mblocks, PMSAD* _1mblocke, PMSAD* _2mblocks, PMSAD* _2mblocke);
private UInt PMSADTowBlockIsOk(PABHList* abhlist, PMSAD* _1mblocks, PMSAD* _1mblocke, PMSAD* _2mblocks, PMSAD* _2mblocke);
private UInt FindContinuousPMSADsBlock(PABHList* abhlist, PMSAD** msadstart, PMSAD** msadend);
private Bool FreePMSADsOnPABHList(MNode* node, MArea* area, PABHList* abhlist, PABHList* freebhlist, PMSAD* msad, UInt msadnr);
private Bool KrlMmFreePMSADsRealizeCore(GMemManage* gmm, MNode* node, MArea* area, PMSAD* msad, U64 flags);
private Bool KrlMmFreePMSADsRealize(PMSAD* msad, U64 flags);
public Bool KrlMmFreePMSADs(PMSAD* msad, U64 flags);
public Bool KrlMmFreeKernPMSADs(PMSAD* msad);
public Bool KrlMmFreeUserPMSADs(PMSAD* msad);

#endif
