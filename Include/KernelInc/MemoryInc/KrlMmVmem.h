/**********************************************************
        虚拟内存头文件KrlMmVmem.c
***********************************************************
                彭东
**********************************************************/
#ifndef _KRLMMVMEMHEAD
#define _KRLMMVMEMHEAD

#include "RBTree.h"
#include "Atomic.h"

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
    void* PMSADBox;
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

#endif
