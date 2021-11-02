/**********************************************************
        虚拟内存头文件KrlMmVmem.c
***********************************************************
                彭东
**********************************************************/
#ifndef _KRLMMVMEMHEAD
#define _KRLMMVMEMHEAD

#include "RBTree.h"
#include "Atomic.h"

#define VPB_CACHE_MAX (0x1000)
#define VPB_CACHE_MIN (0x40)
#define VADSIZE_ALIGN(x) ALIGN(x, (typeof(x))0x1000)
#define VMAP_MIN_SIZE (MSAD_SIZE)

#define ACCESS_PERMISSION_FAIL (1)
#define ACCESS_NOTMAP_FAIL (2)

// Virtual Memory PMSAD(Page) Box Manager
typedef struct VBM 
{
	List Lists;
	MLock Lock;
	UInt Status;
	UInt Flags;
	UInt VPBNR;
	List VPBHead;
	UInt VPBCacheNR;
	UInt VPBCacheMax;
	UInt VPBCacheMin;
	List VPBCacheHead;
	void* Ext;
}VBM;

// Virtual Memory PMSAD(Page) Box
typedef struct VPB 
{
	List Lists;
	MLock Lock;
	UInt Status;
	UInt Flags;
    RefCount Count;
    UInt Type;
	UInt PmsadNR;
	List PmsadLists;
	VBM* ParentVBM;
	void* FileNode;
	void* ObjExecer;
	void* Ext;
}VPB;

// Virtual Address Descriptors
typedef struct VAD
{
    List Lists;
    RBTree TNode;
	MLock Lock;
    U32   MapType;
    U64   Flags;
    U64   Access;
    Addr  Start;
    Addr  End;
    VPB*  PMSADBox;
    VAM*  ParentVAM;
    void* InnerOBJ; 
}VAD;

// Virtual Address Manager
typedef struct VAM
{
    List Lists;
    RBRoot TRoot;
    List VADLists;
	MLock Lock;
	UInt Status;
	UInt Flags;
    UInt VADNr;
	VMS* ParentVms;
	VAD* StartVAD;
	VAD* EndVAD;
	VAD* CurrVAD;
	VAD* KMapVAD;
	VAD* HWVAD;
	VAD* KOldVAD;
	VAD* HeapVAD;
	VAD* StackVAD;
	Addr IsAllocStart;
	Addr IsAllocEnd;
	void* Priv;
	void* Ext;
}VAM;

// Virtual Memory Space
typedef struct VMS
{
    List Lists;
	MLock Lock;
	UInt Status;
	UInt Flags;
    RefCount Count;
	void* ParentExec;
	MMU Mmu;
	VAM VAManager;
	Addr TextStart;
	Addr TextEnd;
	Addr DataStart;
	Addr DataEnd;
	Addr BssStart;
	Addr BssEnd;
	Addr BrkStart;
	Addr BrkEnd;
}VMS;

DefinedMEMData(VBM, VPBoxManager);
DefinedMEMData(VMS, RootVMemSPace);

KLINE VBM* KrlMmGetVPBoxManagerAddr()
{
    return &VPBoxManager;
}

KLINE VBM* KrlMmGetRootVMemSPaceAddr()
{
    return &RootVMemSPace;
}

KLINE void KrlVPBCountInc(VPB* vpb)
{
    IF_NULL_RETURN(vpb);
    RefCountInc(&vpb->Count);
    return;
}

KLINE void KrlVPBCountDec(VPB* vpb)
{
    IF_NULL_RETURN(vpb);
    RefCountDec(&vpb->Count);
    return;
}

private void VBMInit(VBM* init);
private void VPBInit(VPB* init);
private void VADInit(VAD* init);
private void VAMInit(VAM* init);
private void VMSInit(VMS* init);
private VPB* NewVPB();
private Bool DelVPB(VPB* vpb);
private VAD* NewVAD();
private Bool DelVAD(VAD* vad);
public Bool KrlVMemPutVPB(VPB* box);
public VPB* KrlVMemGetVPB();
private VAD* FindVADForVMFree(VAM* vam, Addr start, Size size);
private VAD* VADIsOkForVMAlloc(VAM *vam, VAD* vad, Addr start, Size size, U64 access, UInt type);
private VAD* FindVADForVMAlloc(VAM* vam, Addr start, Size size, U64 access, UInt type);
private UInt VADRBRePlace(RBTree* srcrb, RBTree* reprb);
private UInt VADRBDelAfter(RBTree* delrb);
private UInt VADRBPathCMP(RBTree* srcrb, RBTree* cmprb);
private Bool VADReMoveVAM(VAM* vam, VAD* vad);
private Addr VADInsertVAM(VAM* vam, VAD* currvad, VAD* newvad);
private void SetEndAndCurVADForVMFree(VAM* vam, VAD* vad);
private Bool DelUserPMSADsForVMemUnMapping(VMS* vms, VPB* box, PMSAD* msad, Addr phyadr);
private PMSAD* NewUserPMSADsForVMemMapping(VMS* vms, VPB* box);
private Bool KrlVMemUnMappingRealize(VMS* vms, VAD* vad, Addr start, Addr end);
public Bool KrlVMemUnMapping(VMS* vms, VAD* vad, Addr start, Size size);
private Bool KrlVMemFreeRealizeCore(VMS* vms, VAM* vam, Addr start, Size size);
private Addr KrlVMemAllocRealizeCore(VMS* vms, VAM* vam, Addr start, Size size, U64 access, UInt type);
private Bool KrlVMemFreeRealize(VMS* vms, Addr start, Size size);
private Addr KrlVMemAllocRealize(VMS* vms, Addr start, Size size, U64 access, UInt type);
public Bool KrlVMemFree(VMS* vms, Addr start, Size size);
public Addr KrlVMemAlloc(VMS* vms, Addr start, Size size, U64 access, UInt type);
public VMS* KrlMmGetCurrVMS();
private VPB* ForMappingGetVPBOnVAD(VAD* vad);
private VAD* ForMappingFindVADOnVAM(VAM* vam, Addr vaddr);
private Bool KrlVMemHandPermissionFail(VMS* vms, VAM* vam, Addr vaddr);
private Addr KrlVMemMappingRealizeCore(VMS* vms, VAD* vad, VPB* box, Addr start, Addr end, U64 flags);
private Addr KrlVMemMappingRealize(VMS* vms, VAD* vad, VPB* box, Addr start, Size size, U64 flags);
public Addr KrlVMemMapping(VMS* vms, VAD* vad, Addr start, Size size, U64 flags);
private Bool KrlVMemHandMappingFail(VMS* vms, VAM* vam, Addr vaddr);
private Bool KrlVMemHandAddrAccessFailRealizeCore(VMS* vms, Addr vaddr, UInt errcode);
private Bool KrlVMemHandAddrAccessFailRealize(Addr vaddr, UInt errcode);
public Bool KrlMmVMemInit();
#endif
