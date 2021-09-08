/**********************************************************
        MMU头文件HalMMU.h
***********************************************************
                彭东
**********************************************************/
#ifndef _HALMMUHEAD
#define _HALMMUHEAD

#define TDIRE_MAX (512)
#define SDIRE_MAX (512)
#define IDIRE_MAX (512)
#define MDIRE_MAX (512)

#define MDIRE_IV_RSHTBIT (12)
#define MDIRE_IV_BITMASK (0x1ffUL)
#define MSA_PADR_LSHTBIT (12)
#define MDIRE_PADR_LSHTBIT (12)
#define IDIRE_IV_RSHTBIT (21)
#define IDIRE_IV_BITMASK (0x1ffUL)
#define IDIRE_PADR_LSHTBIT (12)
#define SDIRE_IV_RSHTBIT (30)
#define SDIRE_IV_BITMASK (0x1ffUL)
#define SDIRE_PADR_LSHTBIT (12)
#define TDIRE_IV_RSHTBIT (39)
#define TDIRE_IV_BITMASK (0x1ffUL)

#define PML4E_P (1 << 0)
#define PML4E_RW (1 << 1)
#define PML4E_US (1 << 2)
#define PML4E_PWT (1 << 3)
#define PML4E_PCD (1 << 4)

#define PDPTE_P (1 << 0)
#define PDPTE_RW (1 << 1)
#define PDPTE_US (1 << 2)
#define PDPTE_PWT (1 << 3)
#define PDPTE_PCD (1 << 4)
#define PDPTE_PS (1 << 7)

#define PDTE_P (1 << 0)
#define PDTE_RW (1 << 1)
#define PDTE_US (1 << 2)
#define PDTE_PWT (1 << 3)
#define PDTE_PCD (1 << 4)
#define PDTE_PS (1 << 7)

#define PTE_P (1 << 0)
#define PTE_RW (1 << 1)
#define PTE_US (1 << 2)
#define PTE_PWT (1 << 3)
#define PTE_PCD (1 << 4)
#define PTE_PAT (1 << 7)
#define PDPTEPHYADR_IP_LSHTBIT (12)
#define PDTEPHYADR_IP_LSHTBIT (12)
#define PTEPHYADR_IP_LSHTBIT (12)
#define PFMPHYADR_IP_LSHTBIT (12)
#define PTE_HAVE_MASK (~0xfff)
#define PDE_HAVE_MASK (~0xfff)
#define PDPTE_HAVE_MASK (~0xfff)
#define PML4E_HAVE_MASK (~0xfff)

//MSA目录项标志位
typedef struct MDIREFLAGS
{
        U64 MPBit : 1;    //0
        U64 MRWBit : 1;   //1
        U64 MUSBit : 1;   //2
        U64 MPWTBit : 1;  //3
        U64 MPCDBit : 1;  //4
        U64 MABit : 1;    //5
        U64 MDBit : 1;    //6
        U64 MPATBit : 1;  //7
        U64 MGBit : 1;    //8
        U64 MIG1Bit : 3;  //9\10\11
        U64 MMSABit : 40; //12
        U64 MIG2Bit : 11; //52
        U64 MXDBit : 1;   //63
} __attribute__((packed)) MDireFlags;

//MSA目录项
typedef struct MDIRE
{
        union
        {
                MDireFlags MFlags;
                U64 MEntry;
        } __attribute__((packed));

} __attribute__((packed)) MDire;

//中级目录项标志位
typedef struct IDIREFLAGS
{
        U64 IPBit : 1;    //0
        U64 IRWBit : 1;   //1
        U64 IUSBit : 1;   //2
        U64 IPWTBit : 1;  //3
        U64 IPCDBit : 1;  //4
        U64 IABit : 1;    //5
        U64 IIG1Bit : 1;   //6
        U64 IPSBit : 1;    //74kb==0
        U64 IIG2Bit : 4;   //8\9\10\11
        U64 IMDIRBit : 40; //12
        U64 IIG3Bit : 11;  //52
        U64 IXDBit : 1;    //63
} __attribute__((packed)) IDireFlags;

//中级目录项
typedef struct IDIRE
{
        union
        {
                IDireFlags IFlags;
                U64 IEntry;
        } __attribute__((packed));

} __attribute__((packed)) IDire;

//次级目录项标志位
typedef struct SDIREFLAGS
{
        U64 SPBit : 1;    //0
        U64 SRWBit : 1;   //1
        U64 SUSBit : 1;   //2
        U64 SPWTBit : 1;  //3
        U64 SPCDBit : 1;  //4
        U64 SABit : 1;    //5
        U64 SIG1Bit : 1;   //6
        U64 SPSBit : 1;    //74kb==0
        U64 SIG2Bit : 4;   //8\9\10\11
        U64 SIDIRBit : 40; //12
        U64 SIG3Bit : 11;  //52
        U64 SXDBit : 1;    //63
} __attribute__((packed)) SDireFlags;

//次级目录项
typedef struct SDIRE
{
        union
        {
                SDireFlags SFlags;
                U64 SEntry;
        } __attribute__((packed));

} __attribute__((packed)) SDire;

//顶级目录项标志位
typedef struct TDIREFLAGS
{
        U64 TPBit : 1;    //0
        U64 TRWBit : 1;   //1
        U64 TUSBit : 1;   //2
        U64 TPWTBit : 1;  //3
        U64 TPCDBit : 1;  //4
        U64 TABit : 1;    //5
        U64 TIG1Bit : 1;   //6
        U64 TRV1Bit : 1;    //74kb==0
        U64 TIG2Bit : 4;   //8\9\10\11
        U64 TSDIRBit : 40; //12
        U64 TIG3Bit : 11;  //52
        U64 TXDBit : 1;    //63
} __attribute__((packed)) TDireFlags;

//顶级目录项
typedef struct TDIRE
{
        union
        {
                TDireFlags TFlags;
                U64 TEntry;
        } __attribute__((packed));

} __attribute__((packed)) TDire;

//MSA目录
typedef struct MDIREARR
{
        MDire MDEArr[MDIRE_MAX];
} __attribute__((packed)) MDireArr;

//中级目录
typedef struct IDIREARR
{
        IDire IDEArr[IDIRE_MAX];
} __attribute__((packed)) IDireArr;

//次级目录
typedef struct SDIREARR
{
        SDire SDEArr[SDIRE_MAX];
} __attribute__((packed)) SDireArr;

//顶级目录
typedef struct TDIREARR
{
        TDire TDEArr[TDIRE_MAX];
} __attribute__((packed)) TDireArr;

//CR3寄存器标志位
typedef struct CR3FLAGS
{
        U64 Pcid : 12;  //0
        U64 Plm4a : 40; //12
        U64 Rv : 11;    //52
        U64 Tbc : 1;    //63
} __attribute__((packed)) CR3Flags;

//CR3寄存器
typedef struct CR3
{
        union
        {
                CR3Flags Flags;
                U64 Entry;
        } __attribute__((packed));

} __attribute__((packed)) CR3;

typedef struct MMU
{
        SPinLock Lock;
        U64 Status;
        U64 Flags;
        TDireArr *TDireArrPtr;
        CR3 Cr3;
        List TDirHead;
        List SDirHead;
        List IDirHead;
        List MDirHead;
        UInt TDirMsaNR;
        UInt SDirMsaNR;
        UInt IDirMsaNR;
        UInt MDirMsaNR;
} MMU;


public void CR3Init(CR3* init);
public void MMUInit(MMU* init);
private UInt TDireIndex(Addr vaddr);
private UInt SDireIndex(Addr vaddr);
private UInt IDireIndex(Addr vaddr);
private UInt MDireIndex(Addr vaddr);
private void TDireArrInit(TDireArr* init);
private void SDireArrInit(SDireArr* init);
private void IDireArrInit(IDireArr* init);
private void MDireArrInit(MDireArr* init);
private Bool SDireArrIsAllZero(SDireArr* sdirearr);
private Bool SDireIsHave(TDire* tdire);
private Bool SDireIsPresence(TDire* tdire);
private Addr SDireRetPAddr(TDire* tdire);
private Addr SDireRetVAddr(TDire* tdire);
private SDireArr* TDireRetSDireArr(TDire* tdire);
private Bool IDireArrIsAllZero(IDireArr* idirearr);
private Bool IDireIsHave(SDire* sdire);
private Bool IDireIsPresence(SDire* sdire);
private Addr IDireRetPAddr(SDire* sdire);
private Addr IDireRetVAddr(SDire* sdire);
private IDireArr* SDireRetIDireArr(SDire* sdire);
private Bool MDireArrIsAllZero(MDireArr* mdirearr);
private Bool MDireIsHave(IDire* idire);
private Bool MDireIsPresence(IDire* idire);
private Addr MDireRetPAddr(IDire* idire);
private Addr MDireRetVAddr(IDire* idire);
private MDireArr* IDireRetMDireArr(IDire* idire);
private Bool MMUPMSADIsHave(MDire* mdire);
private Bool MMUPMSADIsPresence(MDire* mdire);
private Addr MMUPMSADRetPAddr(MDire* mdire);
private PMSAD* MMUNewTDireArr(MMU* mmulocked);
private Bool MMUDelTDireArr(MMU* mmulocked, TDireArr* tdirearr, PMSAD* msad);
private PMSAD* MMUNewSDireArr(MMU* mmulocked);
private Bool MMUDelSDireArr(MMU* mmulocked, SDireArr* sdirearr, PMSAD* msad);
private PMSAD* MMUNewIDireArr(MMU* mmulocked);
private Bool MMUDelIDireArr(MMU* mmulocked, IDireArr* idirearr, PMSAD* msad);
private PMSAD* MMUNewMDireArr(MMU* mmulocked);
private Bool MMUDelMDireArr(MMU* mmulocked, MDireArr* mdirearr, PMSAD* msad);
private Addr MMUUnTranslationPMSAD(MMU* mmulocked, MDireArr* mdirearr, Addr vaddr);
private Bool MMUTranslationPMSAD(MMU* mmulocked, MDireArr* mdirearr, Addr vaddr, Addr paddr, U64 flags);
private Bool MMUUnTranslationMDire(MMU* mmulocked, IDireArr* idirearr, PMSAD* msad, Addr vaddr);
private MDireArr* MMUTranslationMDire(MMU* mmulocked, IDireArr* idirearr, Addr vaddr, U64 flags, PMSAD** outmsad);
private Bool MMUUnTranslationIDire(MMU* mmulocked, SDireArr* sdirearr, PMSAD* msad, Addr vaddr);
private IDireArr* MMUTranslationIDire(MMU* mmulocked, SDireArr* sdirearr, Addr vaddr, U64 flags, PMSAD** outmsad);
private Bool MMUUnTranslationSDire(MMU* mmulocked, TDireArr* tdirearr, PMSAD* msad, Addr vaddr);
private SDireArr* MMUTranslationSDire(MMU* mmulocked, TDireArr* tdirearr, Addr vaddr, U64 flags, PMSAD** outmsad);
private Addr MMUFindPMSADAddress(MDireArr* mdirearr, Addr vaddr);
private MDireArr* MMUFindMDireArr(IDireArr* idirearr, Addr vaddr);
private IDireArr* MMUFindIDireArr(SDireArr* sdirearr, Addr vaddr);
private SDireArr* MMUFindSDireArr(TDireArr* tdirearr, Addr vaddr);
public void HalMMULoad(MMU* mmu);
private void HalMMUInnerRefresh(MMU* mmulocked);
public void HalMMURefresh(MMU* mmu);
private Addr HalUnMMUTranslationCore(MMU* mmu, Addr vaddr);
public Addr HalUnMMUTranslation(MMU* mmu, Addr vaddr);
private Bool HalMMUTranslationCore(MMU* mmu, Addr vaddr, Addr paddr, U64 flags);
public Bool HalMMUTranslation(MMU* mmu, Addr vaddr, Addr paddr, U64 flags);
private Bool MMUCleanMDireArrAllPMSAD(MMU* mmulocked);
private Bool MMUCleanIDireArrAllPMSAD(MMU* mmulocked);
private Bool MMUCleanSDireArrAllPMSAD(MMU* mmulocked);
private Bool MMUCleanTDireArrAllPMSAD(MMU* mmulocked);
public Bool HalMMUClean();
public Bool HalMMUInitExecutorTDireArr(MMU* mmu);
public Bool HalMMUEnable();
public Bool HalMMUDisable();
public Bool HalMMUInit();
#endif
