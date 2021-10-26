/**********************************************************
        虚拟内存头文件KrlMmVmem.c
***********************************************************
                彭东
**********************************************************/
#ifndef _KRLMMVMEMHEAD
#define _KRLMMVMEMHEAD

#include "RBTree.h"
#include "Atomic.h"


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

private void VADInit(VAD* init);
private void VAMInit(VAM* init);
private void VMSInit(VMS* init);
#endif
