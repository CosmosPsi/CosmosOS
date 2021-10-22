/**********************************************************
        物理内存管理头文件KrlMmManage.h
***********************************************************
                彭东
**********************************************************/
#ifndef _KRLMMMANAGEHEAD
#define _KRLMMMANAGEHEAD

#include "HalSync.h"
#include "HalBoot.h"

#define MSAD_PADR_SLBITS (12)
#define MSAD_PAGE_MAX (8)
#define MSAD_SIZE (1 << MSAD_PADR_SLBITS)
#define MSAD_ALIGN(n) ALIGN(n, MSAD_SIZE)
#define MSAD_MASK (~(MSAD_SIZE-1))

#define MF_P_INIT (0)
#define MF_P_PRESENT (1)


#define MF_OLKTY_INIT (0)
#define MF_OLKTY_ODER (1)
#define MF_OLKTY_BAFH (2)
#define MF_OLKTY_TOBJ (3)

#define MF_LSTTY_LIST (0)
#define MF_MOCTY_FREE (0)
#define MF_MOCTY_KRNL (1)
#define MF_MOCTY_USER (2)
#define MF_MRV1_VAL (0)
#define MF_UINDX_INIT (0)
#define MF_UINDX_MAX (0xffffff)
#define MF_MARTY_INIT (0)
#define MF_MARTY_HWD (1)
#define MF_MARTY_KRL (2)
#define MF_MARTY_PRC (3)
#define MF_MARTY_SHD (4)

#define  PAF_ALLOC (1)
#define  PAF_NO_ALLOC (0)
#define  PAF_SHARED (1)
#define  PAF_NO_SHARED (0)
#define  PAF_SWAP (1)
#define  PAF_NO_SWAP (0)
#define  PAF_CACHE (1)
#define  PAF_NO_CACHE (0)
#define  PAF_KMPOOL (1)
#define  PAF_NO_KMPOOL (0)
#define  PAF_LOCK (1)
#define  PAF_NO_LOCK (0)
#define  PAF_RV2_VAL (0)
#define  PAF_INIT_PADRS (0)

#define MA_TYPE_INIT 0
#define MA_TYPE_HWAD 1
#define MA_TYPE_KRNL 2
#define MA_TYPE_PROC 3
#define MA_TYPE_SHAR 4
#define MEMAREA_MAX 5
#define MA_HWAD_LSTART 0
#define MA_HWAD_LSZ 0x2000000
#define MA_HWAD_LEND (MA_HWAD_LSTART+MA_HWAD_LSZ-1)
#define MA_KRNL_LSTART 0x2000000
#define MA_KRNL_LSZ (0x400000000-0x2000000)
#define MA_KRNL_LEND (MA_KRNL_LSTART+MA_KRNL_LSZ-1)
#define MA_PROC_LSTART 0x400000000
#define MA_PROC_LSZ (0xffffffffffffffff-0x400000000)
#define MA_PROC_LEND (MA_PROC_LSTART+MA_PROC_LSZ)

#define PABH_STUS_INIT 0
#define PABH_STUS_ONEM 1
#define PABH_STUS_DIVP 2
#define PABH_STUS_DIVM 3

#define MSPLMER_ARR_LMAX 52
#define MSPLMER_ARR_BMAX 11
#define MSPLMER_ARR_OMAX 9

#define PMSA_T_OSAPUSERRAM 1
#define PMSA_T_RESERVRAM 2
#define PMSA_T_HWUSERRAM 8
#define PMSA_T_ARACONRAM 0xf
#define PMSA_T_BUGRAM 0xff
#define PMSA_F_X86_32 (1<<0)
#define PMSA_F_X86_64 (1<<1)
#define PMSA_F_ARM_32 (1<<2)
#define PMSA_F_ARM_64 (1<<3)
#define PMSA_F_HAL_MASK 0xff

#define MEMDATA_SECTION __attribute__((section(".mem.data")))
#define DefinedMEMData(vartype,varname) MEMDATA_SECTION vartype varname

#define PMSADDIRE_MAX (1 << 18)
#define PMSADDIRE_INDEX_BITS (30)
#define PMSADDIRE_SIZE (1 << PMSADDIRE_INDEX_BITS)
#define PMSADDIRE_PMSAD_NR (PMSADDIRE_SIZE >> MSAD_PADR_SLBITS)
//PMSAD标志
typedef struct PMSADFLAGS
{
	U32 OLTypeBit:2;
	U32 InListBit:1;
	U32 OccupancyTypeBit:2;
	U32 AreaTypeBit:3;
	U32 PresentBit:2;
	U32 RefCountBit:22;
}__attribute__((packed)) PMSADFlags; 

//物理地址及其标志
typedef struct PHYADDRFLAGS
{
	U64 AllocBit:1;
	U64 SharedBit:1;
	U64 SwapBit:1;
	U64 CacheBit:1;
	U64 LockBit:1;
	U64 KMPoolBit:1;
	U64 ReservedBit:6;
	U64 PAddrBit:52;
}__attribute__((packed)) PhyAddrFlags;

//物理内存空间地址描述述
//Physical memory space address descriptor
typedef struct PMSAD
{
	List Lists;//16
	SPinLock Lock; //4
	PMSADFlags CountFlags;//4
	PhyAddrFlags PhyAddr;//8
	void* BlockLink;//8
}__attribute__((packed)) PMSAD;//16+24

typedef struct MLOCK
{
    SPinLock Locks;
    //SEM
}MLock;

//Physical memory space area
//物理内存空间区域
typedef struct PHYMSPACEAREA
{
    U32 Type;
    U32 SubType;
    U32 DevType;
    U32 Flags;
    U32 Status;
    U64 Start;
    U64 Size;
    U64 End;
    U64 ResvMemStart;
    U64 ResvMemEnd;
    void* Pri;
    void* Ext;
}PHYMSPaceArea;

//Physical Address Block Head List 
//物理地址块头链
typedef struct PABHLIST
{
	MLock Lock;
	U32  Status;
	UInt Order;
	UInt InOrderPmsadNR;
	UInt FreePmsadNR;
	UInt PmsadNR;
	UInt AllocCount;
	UInt FreeCount;
	List FreeLists;
	List AllocLists;
	List OveLists;
}PABHList;

//Memory Splitting and Merging
//内存拆分合并结构
typedef struct MSPLITMER
{
	MLock Lock;
	U32  Status;
	UInt MaxSMNR;
	UInt PhySMNR;
	UInt PreSMNR;
	UInt SPlitNR;
	UInt MerNR;
	PABHList PAddrBlockArr[MSPLMER_ARR_LMAX];
	PABHList OnePAddrBlock;
}MSPlitMer;


//0x400000000  0x40000000
//Memory Area
//物理内存区
typedef struct MAREA
{
	List Lists;
	MLock Lock;
    U64 AreaId;
	UInt Status;
	UInt Flags;
	UInt Type;
	UInt MaxPMSAD;
	UInt AllocPMSAD;
	UInt FreePMSAD;
	UInt ResvPMSAD;
	UInt HorizLine;
	Addr LogicStart;
	Addr LogicEnd;
	UInt LogicSZ;
	Addr EffectStart;
	Addr EffectEnd;
	UInt EffectSZ;
	List AllPMSADLists;
	UInt AllPMSADNR;
	MSPlitMer MSPLMerData;
	void* Priv;
	void* Ext;
	/*
	*这个结构至少占用一个页面，当然
	*也可以是多个连续的的页面，但是
	*该结构从第一个页面的首地址开始
	*存放，后面的空间用于存放实现分
	*配算法的数据结构，这样每个区可
	*方便的实现不同的分配策略，或者
	*有天你觉得我的分配算法是渣渣，
	*完全可以替换mafuncobjs_t结构
	*中的指针，指向你的函数。
	*/
}MArea;

//PMSAD 目录项
typedef struct PMSADDIRE
{
    union
    {
        PMSAD* DireStart;
        U64 Entry;
    }__attribute__((packed));
}__attribute__((packed)) PMSADDire;

//PMSAD 目录数组
typedef struct PMSADDIREARR
{
    PMSADDire PMSADEArr[PMSADDIRE_MAX];
}PMSADDireArr;


/*
Memory Node
内存节点
*/
typedef struct MNode
{
    List Lists;
	MLock Lock;
	UInt Status;
	UInt Flags;
    UInt NodeID;
    UInt CPUID;
    Addr NodeMemAddrStart;
    Addr NodeMemAddrEnd;
    PHYMSPaceArea* PMSAreaPtr;
    U64 PMSAreaNR;
    U64 NodeMemSize;
    Addr NodeMemResvAddrStart;
    Addr NodeMemResvAddrEnd;
    U64 NodeMemResvSize;
    MArea MAreaArr[MEMAREA_MAX];
    PMSADDireArr PMSADDir;
}MNode;
//Global Memory Manage
//全局内存管理
typedef struct GMEMMANAGE
{
	MLock Lock;
	U64 Status;
	U64 Flags;
	U64 MemroySZ;
	U64 MaxPMSAD;
	U64 FreePMSAD;
	U64 AllocPMSAD;
	U64 ResvPMSAD;
	PHYMSPaceArea* PAreaStart;
	U64 PAreaNR;
	MNode* MNodeStart;
	U64 MNodeNR;
	void* Priv;
	void* Ext;
}GMemManage;

DefinedMEMData(GMemManage, GMemManageData);


KLINE Addr PMSADRetPAddr(PMSAD* msad)
{
    if(NULL == msad)
    {
        return NULL;
    }
    return (msad->PhyAddr.PAddrBit << MSAD_PADR_SLBITS);
}

KLINE Addr PMSADRetVAddr(PMSAD* msad)
{
    if(NULL == msad)
    {
        return NULL;
    }	
    return HalPAddrToVAddr(PMSADRetPAddr(msad));
}

KLINE Bool PMSADIsAdjacent(PMSAD* curr, PMSAD* next)
{
    Addr curraddr = 0;
    Addr nextaddr = 0;

    if((curr + 1) != next)
    {
        return FALSE;
    }
    curraddr = PMSADRetPAddr(curr);
    nextaddr = PMSADRetPAddr(next);
    if((nextaddr - curraddr) != MSAD_SIZE)
    {
        return FALSE;
    }
    return TRUE;
}

KLINE Bool PMSADIsFree(PMSAD* msad)
{
    IF_NULL_DEAD(msad);
    if(MF_MOCTY_FREE != msad->CountFlags.OccupancyTypeBit ||
		0 != msad->CountFlags.RefCountBit || PAF_NO_ALLOC != msad->PhyAddr.AllocBit)
	{
		return FALSE;
	}
    return TRUE;
}

KLINE Bool PMSADIsFreeAlsoAdjacent(PMSAD* curr, PMSAD* next)
{
    if((PMSADIsFree(curr) == TRUE) && (PMSADIsFree(next) == TRUE))
    {
        if(PMSADIsAdjacent(curr, next) == TRUE)
        {
            return TRUE;
        }
    }
    return FALSE;
}

KLINE Bool PMSADIsEQAreaType(PMSAD* msad, UInt areatype)
{
    UInt type = 0;
    IF_NULL_DEAD(msad);
    type = (UInt)(msad->CountFlags.AreaTypeBit);
    if(type == areatype)
    {
        return TRUE;
    }
    return FALSE;
}

KLINE Bool PMSADIsMArea(PMSAD* msad, MArea* area)
{
    Addr phyaddr = 0;
    IF_NULL_RETURN_FALSE(msad);
    IF_NULL_RETURN_FALSE(area);
    if(PMSADIsEQAreaType(msad, area->Type) == FALSE)
    {
        return FALSE;
    }

    phyaddr = PMSADRetPAddr(msad);
    if(area->LogicStart <= phyaddr && phyaddr < area->LogicEnd)
    {
        return TRUE;
    }
    return FALSE;
}

KLINE Bool PMSADIsPresent(PMSAD* msad)
{
    IF_NULL_RETURN_FALSE(msad);
    if(MF_P_PRESENT == msad->CountFlags.PresentBit)
    {
        return TRUE;
    }
    return FALSE;
}

KLINE void SetPMSADPresent(PMSAD* msad)
{
    IF_NULL_DEAD(msad);
    msad->CountFlags.PresentBit = MF_P_PRESENT;
    return;
}

KLINE void ClearPMSADPresent(PMSAD* msad)
{
    IF_NULL_DEAD(msad);
    msad->CountFlags.PresentBit = MF_P_INIT;
    return;
}

KLINE void SetPMSADOLType(PMSAD* msad, U32 oltype)
{
    IF_NULL_DEAD(msad);
    if(MF_OLKTY_TOBJ < oltype)
    {
        return;
    }
    msad->CountFlags.OLTypeBit = oltype;
    return;
}

KLINE U32 RetPMSADOLType(PMSAD* msad)
{
    IF_NULL_DEAD(msad);
    return (U32)(msad->CountFlags.OLTypeBit);
}

KLINE void SetPMSADOccupancyType(PMSAD* msad, U32 occupancytype)
{
    IF_NULL_DEAD(msad);
    if(MF_MOCTY_USER < occupancytype)
    {
        return;
    }
    msad->CountFlags.OccupancyTypeBit = occupancytype;
    return;
}

KLINE void SetPMSADPAddr(PMSAD* msad, U64 paddr)
{
    U64 phyaddr = 0;
    PhyAddrFlags* pf = NULL;
    IF_NULL_DEAD(msad);

    phyaddr = paddr;
    pf = (PhyAddrFlags*)(&phyaddr);
    msad->PhyAddr.PAddrBit = pf->PAddrBit;
    return;
}

KLINE void SetPMSADAlloc(PMSAD* msad)
{
    IF_NULL_DEAD(msad);
    msad->PhyAddr.AllocBit = PAF_ALLOC;
    return;
}

KLINE void ClearPMSADAlloc(PMSAD* msad)
{
    IF_NULL_DEAD(msad);
    msad->PhyAddr.AllocBit = 0;
    return;
}

KLINE void SetPMSADShare(PMSAD* msad)
{
    IF_NULL_DEAD(msad);
    msad->PhyAddr.SharedBit = PAF_SHARED;
    return;
}

KLINE void ClearPMSADShare(PMSAD* msad)
{
    IF_NULL_DEAD(msad);
    msad->PhyAddr.SharedBit = PAF_NO_SHARED;
    return;
}

KLINE Bool PMSADIsShare(PMSAD* msad)
{
    IF_NULL_RETURN_FALSE(msad);
    if(PAF_SHARED == msad->PhyAddr.SharedBit)
    {
        return TRUE;
    }
    return FALSE;
}

KLINE void SetPMSADSwap(PMSAD* msad)
{
    IF_NULL_DEAD(msad);
    msad->PhyAddr.SwapBit = PAF_SWAP;
    return;
}

KLINE void ClearPMSADSwap(PMSAD* msad)
{
    IF_NULL_DEAD(msad);
    msad->PhyAddr.SwapBit = PAF_NO_SWAP;
    return;
}

KLINE Bool PMSADIsSwap(PMSAD* msad)
{
    IF_NULL_RETURN_FALSE(msad);
    if(PAF_SWAP == msad->PhyAddr.SwapBit)
    {
        return TRUE;
    }
    return FALSE;
}

KLINE void SetPMSADCache(PMSAD* msad)
{
    IF_NULL_DEAD(msad);
    msad->PhyAddr.CacheBit = PAF_CACHE;
    return;
}

KLINE void ClearPMSADCache(PMSAD* msad)
{
    IF_NULL_DEAD(msad);
    msad->PhyAddr.CacheBit = PAF_NO_CACHE;
    return;
}

KLINE Bool PMSADIsCache(PMSAD* msad)
{
    IF_NULL_RETURN_FALSE(msad);
    if(PAF_CACHE == msad->PhyAddr.CacheBit)
    {
        return TRUE;
    }
    return FALSE;
}

KLINE void SetPMSADLock(PMSAD* msad)
{
    IF_NULL_DEAD(msad);
    msad->PhyAddr.LockBit = PAF_LOCK;
    return;
}

KLINE void ClearPMSADLock(PMSAD* msad)
{
    IF_NULL_DEAD(msad);
    msad->PhyAddr.LockBit = PAF_NO_LOCK;
    return;
}

KLINE Bool PMSADIsLock(PMSAD* msad)
{
    IF_NULL_RETURN_FALSE(msad);
    if(PAF_LOCK == msad->PhyAddr.LockBit)
    {
        return TRUE;
    }
    return FALSE;
}

KLINE void SetPMSADKMPool(PMSAD* msad)
{
    IF_NULL_DEAD(msad);
    msad->PhyAddr.KMPoolBit = PAF_KMPOOL;
    return;
}

KLINE void ClearPMSADKMPool(PMSAD* msad)
{
    IF_NULL_DEAD(msad);
    msad->PhyAddr.KMPoolBit = PAF_NO_KMPOOL;
    return;
}

KLINE Bool PMSADIsKMPool(PMSAD* msad)
{
    IF_NULL_RETURN_FALSE(msad);
    if(PAF_KMPOOL == msad->PhyAddr.KMPoolBit)
    {
        return TRUE;
    }
    return FALSE;
}

KLINE void SetPMSADBlockLink(PMSAD* msad, void* link)
{
    IF_NULL_DEAD(msad);
    msad->BlockLink = link;
    return;
}

KLINE void ClearPMSADBlockLink(PMSAD* msad)
{
    IF_NULL_DEAD(msad);
    msad->BlockLink = NULL;
    return;
}

KLINE void* RetPMSADBlockLink(PMSAD* msad)
{
    IF_NULL_DEAD(msad);
    return msad->BlockLink;
}

KLINE void GetPMSAD(PMSAD* msad)
{
    IF_NULL_DEAD(msad);
    if(MF_UINDX_MAX <= msad->CountFlags.RefCountBit)
    {
        return;
    }
    msad->CountFlags.RefCountBit++;
    return;
}

KLINE void PutPMSAD(PMSAD* msad)
{
    IF_NULL_DEAD(msad);
    if(1 > msad->CountFlags.RefCountBit)
    {
        return;
    }
    msad->CountFlags.RefCountBit--;
    return;
}

KLINE UInt RetPMSADRefCount(PMSAD* msad)
{
    IF_NULL_DEAD(msad);
    return (UInt)msad->CountFlags.RefCountBit;
}


KLINE GMemManage* KrlMmGetGMemManageAddr()
{
    return &GMemManageData;
}

KLINE UInt PMSADDireIndex(U64 phyaddr)
{
    return (UInt)(phyaddr >> PMSADDIRE_INDEX_BITS);
}

KLINE UInt PMSADIndex(U64 phyaddr)
{
    return (UInt)((phyaddr & (PMSADDIRE_SIZE -1)) >> MSAD_PADR_SLBITS);
}

KLINE MNode* PHYAddrRetMNode(U64 phyaddr)
{
    GMemManage* gmm = NULL;
    MNode* node = NULL;
    gmm = KrlMmGetGMemManageAddr();
    for(U64 i = 0; i < gmm->MNodeNR; i++)
    {
        node = &gmm->MNodeStart[i];
        if(phyaddr < node->NodeMemAddrEnd && phyaddr > node->NodeMemAddrStart)
        {
            return node;
        }
    }
    return NULL; 
}

KLINE PMSADDire* PHYAddrRetPMSADDire(U64 phyaddr)
{
    PMSADDire* dire = NULL;
    MNode* node = NULL;
    UInt index = PMSADDIRE_MAX; 
    node = PHYAddrRetMNode(phyaddr);
    IF_NULL_RETURN_NULL(node);
    index = PMSADDireIndex(phyaddr);
    return &(node->PMSADDir.PMSADEArr[index]);
}

KLINE PMSAD* PHYAddrRetPMSAD(U64 phyaddr)
{
    PMSADDire* dire = NULL;
    dire = PHYAddrRetPMSADDire(phyaddr);
    IF_NULL_RETURN_NULL(dire);
    return &dire->DireStart[PMSADIndex(phyaddr)];
}

KLINE MNode* PMSADRetItsMNode(PMSAD* msad)
{
    U64 phyaddr = 0;
    MNode* node = NULL; 
    IF_NULL_RETURN_NULL(msad);
    phyaddr = PMSADRetPAddr(msad);
    node = PHYAddrRetMNode(phyaddr);
    return node;
}

KLINE MArea* PMSADRetItsMArea(PMSAD* msad)
{
    MNode* node = NULL;
    MArea* area = NULL;
    IF_NULL_RETURN_NULL(msad);
    node = PMSADRetMNode(msad);
    IF_NULL_RETURN_NULL(node);
    area = node->MAreaArr;
    for(UInt i = 0; i < MEMAREA_MAX; i++)
    {
        if(PMSADIsMArea(msad, &area[i]) == TRUE)
        {
            return &area[i];
        }
    }
    return NULL;
}

KLINE Bool PMSADDireIsHave(PMSADDire* dire)
{
    IF_NULL_DEAD(dire);
    if(0 < dire->Entry)
    {
        return TRUE;
    }
    return FALSE;
}

private void MLockInit(MLock* init);
private void PHYMSPaceAreaInit(PHYMSPaceArea* init);
private void PMSADInit(PMSAD* init);
private void PABHListInit(PABHList* init);
private void MSPlitMerInit(MSPlitMer* init);
private void MAreaInit(MArea* init);
private void MNodeInit(MNode* init);
private void GMemManageInit(GMemManage* init);
public Bool KrlMmUPAddGMMAllocMaxFreeNR(UInt allocnr, UInt maxnr, UInt freenr);
public Bool KrlMmUPAddMNodeAreaAllocMaxFreeNR(MNode* node, MArea* area, UInt allocnr, UInt maxnr, UInt freenr);
public Bool KrlMmUPSubGMMAllocMaxFreeNR(UInt allocnr, UInt maxnr, UInt freenr);
public Bool KrlMmUPSubMNodeAreaAllocMaxFreeNR(MNode* node, MArea* area, UInt allocnr, UInt maxnr, UInt freenr);
public void KrlMmLocked(MLock* lock);
public void KrlMmUnLock(MLock* lock);
private Bool NewOnePHYMSPaceArea(E820Map* e820, PHYMSPaceArea* area);
private void PHYMSPaceAreaSwap(PHYMSPaceArea *s, PHYMSPaceArea *d);
private void PHYMSPaceAreaSort(PHYMSPaceArea* area, U64 nr);
private Bool SetPMSADInMNodeMAreaInfo(MNode* node, PMSAD* msad);
private PMSAD* NewOnePMSAD(MNode* node, PHYMSPaceArea* area, PMSADDire* dire, U64 paddr);
private UInt PMSADInitOnPHYMSPaceArea(MNode* node, PMSADDire* dire, PHYMSPaceArea* area, U64 start, U64 end);
private PMSAD* PMSADDireIsNeedAllocMemory(U64 start, U64 end);
private Bool PMSADInitOnMNode(MNode* node);
private Bool ScanOccupancyPMSADOnAddr(Addr start, Addr end);
public Bool KrlMmScanOccupancyPMSAD();
public Bool KrlMmPMSADInit();
public Bool KrlMmPHYMSPaceAreaInit();
public Bool PMSADAddInPABHList(PABHList* pabhl, PMSAD* msad, UInt order);
private SInt RetOrderInPMSADsNR(PMSAD* start, PMSAD* end);
private PMSAD* NextOrderPMSADsAddInPABHList(MNode* node, MArea* area, PMSAD* start, PMSAD* end, UInt* summsad);
private UInt ScanOrderPMSADsAddInPABHList(MNode* node, MArea* area, PMSAD* start, PMSAD* end);
private UInt ScanContinuousAddrPMSADsLen(PMSAD* start, PMSAD* end);
private UInt ScanContinuousAddrPMSADs(MNode* node, MArea* area, PMSAD* start, PMSAD* end);
private UInt ScanContinuousFreePMSADsLen(PMSAD* start, PMSAD* end);
private UInt ScanContinuousFreePMSADs(MNode* node, MArea* area, PMSAD* start, PMSAD* end);
private UInt ScanSameAreaTypePMSADsLen(PMSAD* start, PMSAD* end, UInt areatype);
private UInt ScanSameAreaTypePMSADs(MNode* node, MArea* area, PMSAD* start, PMSAD* end);
private UInt PMSADBlockInitOnPMSADDire(MNode* node, MArea* area, PMSADDire* dire);
private Bool OneMAreaInitOnMNode(MNode* node, MArea* area);
private Bool MAreaInitOnMNode(MNode* node);
public Bool KrlMmClearPMSADUseStatus(PMSAD* msad);
public PMSAD* KrlMmGetPMSADOnVaddr(Addr vaddr);
public MNode* KrlMmGetMNode(UInt nodeid);
public MArea* KrlMmGetMArea(MNode* node, UInt areaid);
public Bool KrlMmMAreaInit();
private Bool DefaultMNodeInit();
public Bool KrlMmMNodeInit();
public Bool KrlMmManageInit();
#endif
