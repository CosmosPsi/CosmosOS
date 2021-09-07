/**********************************************************
        物理内存管理文件KrlMmManage.h
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


#endif