/**********************************************************
		底层引导初始化头文件HalBoot.h
***********************************************************
				彭东
**********************************************************/

#ifndef _HALBOOTHEAD
#define _HALBOOTHEAD

#define KRNL_INRAM_START 0x30000000
#define LINK_VIRT_ADDR 0x30008000
#define LINK_LINE_ADDR 0x30008000
#define KERNEL_VIRT_ADDR 0x30008000
#define KERNEL_PHYI_ADDR 0x30008000
#define PAGE_TLB_DIR 0x30004000
#define PAGE_TLB_SIZE 4096
#define INIT_HEAD_STACK_ADDR 0x34000000

#define CPU_VECTOR_PHYADR 0x30000000
#define CPU_VECTOR_VIRADR 0


#define PTE_SECT_AP (3<<10)
#define PTE_SECT_DOMAIN (0<<5)
#define PTE_SECT_NOCW (0<<2)
#define PTE_SECT_BIT (2)

#define PLFM_ADRSPCE_NR 29

#define INTSRC_MAX 32

#define KRNL_MAP_VIRTADDRESS_SIZE 0x400000000
#define KRNL_VIRTUAL_ADDRESS_START 0xffff800000000000
#define KRNL_VIRTUAL_ADDRESS_END 0xffffffffffffffff
#define USER_VIRTUAL_ADDRESS_START 0
#define USER_VIRTUAL_ADDRESS_END 0x00007fffffffffff
#define KRNL_MAP_PHYADDRESS_START 0
#define KRNL_MAP_PHYADDRESS_END 0x400000000
#define KRNL_MAP_PHYADDRESS_SIZE 0x400000000
#define KRNL_MAP_VIRTADDRESS_START KRNL_VIRTUAL_ADDRESS_START
#define KRNL_MAP_VIRTADDRESS_END (KRNL_MAP_VIRTADDRESS_START+KRNL_MAP_VIRTADDRESS_SIZE)
#define KRNL_ADDR_ERROR 0xf800000000000

#define MSI_PADR ((MachStartInfio*)(0x100000))

#define BFH_RW_R 1
#define BFH_RW_W 2

#define BFH_BUF_SZ 0x1000
#define BFH_ONERW_SZ 0x1000
#define BFH_RWONE_OK 1
#define BFH_RWONE_ER 2
#define BFH_RWALL_OK 3

#define FDSC_NMAX 192
#define FDSC_SZMAX 256
#define KIMAGE_ENDMAGIC 0xaaffaaffaaffaaff
#define KIMAGE_RVMAGIC 0xffaaffaaffaaffaa

#define KIMAGEINFO_OFF (0x1000)
#define RAM_USABLE 1
#define RAM_RESERV 2
#define RAM_ACPIREC 3
#define RAM_ACPINVS 4
#define RAM_AREACON 5

#define MSI_MAGIC (U64)((((U64)'C')<<56)|(((U64)'o')<<48)|(((U64)'s')<<40)|(((U64)'m')<<32)|(((U64)'o')<<24)|(((U64)'s')<<16)|(((U64)'O')<<8)|((U64)'S'))

//Machine start-up information
typedef struct MACHSTARTINFO
{
    U64   Magic;          //CosmosOS//0
    U64   CheckSum;//8
    U64   KrlInitStack;//16
    U64   KrkInitStackSZ;//24
    U64   KrlImgFilePAddr;
    U64   KrlImgFileSZ;
    U64   krlCoreFilePAddr;
    U64   krlCoreFileSZ;
    U64   krlCoreFileVec;
    U64   krlCoreFileRunMode;
    U64   FontPAddr;
    U64   FontSZ;
    U64   FramVRamPAddr;
    U64   FramVRamSZ;
    U64   CPUMode;
    U64   MemSZ;
    U64   E820PAddr;
    U64   E820NR;
    U64   E820SZ;
    U64   E820Curr;
    U64   PMSPaceAreaPAddr;
    U64   PMSPaceAreaNR;
    U64   PMSPaceAreaSZ;
    U64   PMSPaceAreaCurr;
    U64   MNodePAddr;
    U64   MNodeNR;
    U64   MNodeSZ;
    U64   MNodeCurr;
    U64   TDireArrPAddr;
    U64   SubDireLen;
    U64   MAPPhyMemSZ;
    U64   EndPhyAddr;
    U64   BrkStartAddr;
    U64   BrkCurrAddr;
    U64   BrkEndAddr;
    Graph GraphParm;
}__attribute__((packed)) MachStartInfo;

typedef struct E820{
    U64 Addr;    /* start of memory segment8 */
    U64 Size;    /* size of memory segment8 */
    U32 Type;    /* type of memory segment 4*/
}__attribute__((packed)) E820Map;

typedef struct FILEDESC
{
    U64  Type;
    U64  SubType;
    U64  Status;
    U64  ID;
    U64  Offset;
    U64  End;
    U64  RealSZ;
    U64  Sum;
    char Name[FDSC_NMAX];
}FileDESC;

//Kernel Image Information
typedef struct KIMAGEINFO
{
    U64 Magic;
    U64 SelfSum;
    U64 SelfStart;
    U64 SelfEnd;
    U64 SelfSZ;
    U64 LDRBlockStart;
    U64 LDRBlockEnd;
    U64 LDRBlockSZ;
    U64 LDRBlockSum;
    U64 FileDescBlockStart;
    U64 FileDescBlockEnd;
    U64 FileDescBlockSZ;
    U64 FileDescBlockSum;
    U64 FileBlockStart;
    U64 FileBlockEnd;
    U64 FileBlockSZ;
    U64 FileBlockSum;
    U64 LDRCodeNR;
    U64 FileDescNR;
    U64 FileNR;
    U64 EndMagic;
    U64 Rescv;
}KImageInfo;
public MachStartInfo* HalGetMachStartInfoAddr();
public Addr HalBootAllocMem(Size size);
public E820Map* HalBootGetNextE820();
private Bool HalBootInit();
public void HalBootStart();
#endif
