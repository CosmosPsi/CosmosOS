/**********************************************************
        物理内存管理头文件KrlMmManage.h
***********************************************************
                彭东
**********************************************************/
#ifndef _KRLMMMANAGEHEAD
#define _KRLMMMANAGEHEAD

#define PAGPHYADR_SZLSHBIT (12)
#define MSAD_PAGE_MAX (8)
#define MSA_SIZE (1 << PAGPHYADR_SZLSHBIT)

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

#define  PAF_NO_ALLOC (0)
#define  PAF_ALLOC (1)
#define  PAF_NO_SHARED (0)
#define  PAF_NO_SWAP (0)
#define  PAF_NO_CACHE (0)
#define  PAF_NO_KMAP (0)
#define  PAF_NO_LOCK (0)
#define  PAF_NO_DIRTY (0)
#define  PAF_NO_BUSY (0)
#define  PAF_RV2_VAL (0)
#define  PAF_INIT_PADRS (0)

#define MA_TYPE_INIT 0
#define MA_TYPE_HWAD 1
#define MA_TYPE_KRNL 2
#define MA_TYPE_PROC 3
#define MA_TYPE_SHAR 4
#define MEMAREA_MAX 4
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

//PMSAD标志
typedef struct PMSADFLAGS
{
	U32 OLTypeBit:2;
	U32 InListBit:1;
	U32 OccupancyTypeBit:2;
	U32 AreaTypeBit:3;
	U32 RefCountBit:24;
}__attribute__((packed)) PMSADFlags; 

//物理地址及其标志
typedef struct PHYADDRFLAGS
{
	U64 AllocBit:1;
	U64 SharedBit:1;
	U64 SwapBit:1;
	U64 CacheBit:1;
	U64 KMapBit:1;
	U64 LockBit:1;
	U64 DirtyBit:1;
	U64 BusyBit:1;
	U64 ReservedBit:4;
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
    U64 NodeMemSize;
    Addr NodeMemResvAddrStart;
    Addr NodeMemResvAddrEnd;
    U64 NodeMemResvSize;
    MArea MAreaArr[MEMAREA_MAX];
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
	void* privp;
	void* extp;
}GMemManage;



KLINE Addr PMSADRetPAddr(PMSAD* msad)
{
    if(NULL == msad)
    {
        return NULL;
    }
    return (msad->PhyAddr.PAddrBit << PAGPHYADR_SZLSHBIT);
}

KLINE Addr PMSADRetVAddr(PMSAD* msad)
{
    if(NULL == msad)
    {
        return NULL;
    }	
    return HalPAddrToVAddr(PMSADRetPAddr(msad));
}

private void MLockInit(MLock* init);
private void PHYMSPaceAreaInit(PHYMSPaceArea* init);
private void PABHListInit(PABHList* init);
private void MSPlitMerInit(MSPlitMer* init);
private void MAreaInit(MArea* init);
private void MNodeInit(MNode* init);
private void GMemManageInit(GMemManage* init);
public void KrlMmLocked(MLock* lock);
public void KrlMmUnLock(MLock* lock);
public Bool KrlMmManageInit();
#endif
