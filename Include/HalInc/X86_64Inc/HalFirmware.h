/**********************************************************
        固件头文件HalFirmware.h
***********************************************************
                彭东
**********************************************************/

#ifndef _HALFIRMWAREHEAD
#define _HALFIRMWAREHEAD

typedef struct VBEINFO
{
        char VBESignature[4];
        U16 VBEVersion;
        U32 OEMStringPtr;
        U32 Capabilities;
        U32 VideoModePtr;
        U16 TotalMemory;
        U16 OEMSoftWareRev;
        U32 OEMVendorNamePtr;
        U32 OEMProductnamePtr;
        U32 OEMProductRevPtr;
        U8 Reserved[222];
        U8 OEMData[256];
}__attribute__((packed)) VBEInfo;

typedef struct VBEOMINFO
{
    U16 ModeAttributes;
    U8  WinAAttributes;
    U8  WinBAttributes;
    U16 WinGranularity;
    U16 WinSize;
    U16 WinASegment;
    U16 WinBSegment;
    U32 WinFuncPtr;
    U16 BytesPerScanLine;
    U16 XResolution;
    U16 YResolution;
    U8  XCharSize;
    U8  YCharSize;
    U8  NumberOfPlanes;
    U8  BitsPerPixel;
    U8  NumberOfBanks;
    U8  MemoryModel;
    U8  BankSize;
    U8  NumberOfImagePages;
    U8  Reserved;
    U8  RedMaskSize;
    U8  RedFieldPosition;
    U8  GreenMaskSize;
    U8  GreenFieldPosition;
    U8  BlueMaskSize;
    U8  BlueFieldPosition;
    U8  RsvdMaskSize;
    U8  RsvdFieldPosition;
    U8  DirectColorModeInfo;
    U32 PhysBasePtr;
    U32 Reserved1;
    U16 Reserved2;
    U16 LinBytesPerScanLine;
    U8  BnkNumberOfImagePages;
    U8  LinNumberOfImagePages;
    U8  LinRedMaskSize;
    U8  LinRedFieldPosition;
    U8  LinGreenMaskSize;
    U8  LinGreenFieldPosition;
    U8  LinBlueMaskSize;
    U8  LinBlueFieldPosition;
    U8  LinRsvdMaskSize;
    U8  LinRsvdFieldPosition;
    U32 MaxPixelClock;
    U8  Reserved3[189];
}__attribute__((packed)) VBEOMInfo;


#define BGRA(r,g,b) ((0|(r<<16)|(g<<8)|b))
typedef U32 Pixl;

#define VBEMODE 1
#define GPUMODE 2
#define BGAMODE 3
typedef struct GRAPH
{
    U32 Mode;
    U32 X;
    U32 y;
    U32 FramPHYAddr;
    U32 OnePixBits;
    U32 VBEModeNR;
    U32 VIFPHYAddr;
    U32 VMIFPHYAdrr;
    U32 Bank;
    U32 CurrDispBank;
    U32 NextBank;
    U32 BankSZ;
    U32 LogoPHYAddrStart;
    U32 LogoPHYAddrEnd;
    U32 FontAddr;
    U32 FontSectAddr;
    U32 FontSectNR;
    U32 FontHigh;
    U32 NextCharsX;
    U32 NextCharsY;
    U32 LineSZ;
    vbeinfo_t vbeinfo;
    vbeominfo_t vminfo;
}__attribute__((packed)) Graph;

typedef struct BMFHEAD
{
    U16 BFTag;   //0x4d42
    U32 BFSize;
    U16 BFResd1;
    U16 BFResd2;
    U32 BFOff;
}__attribute__((packed)) BMFHead;

typedef struct BITMINFO
{
    U32 BISize;
    s32_t BIWidth;
    s32_t BIHigh;
    U16 BIPlanes;
    U16 BIBCount;
    U32 BIComp;
    U32 BISZImg;
    s32_t BIXPelsper;
    s32_t BIYPelsper;
    U32 BIClrUserd;
    U32 BIClrImport;
}__attribute__((packed)) BitMInfo;

typedef struct BMDBGR
{
    U8 Blue;
    U8 Green;
    U8 Red;
}__attribute__((packed)) BMDBGR;

typedef struct UTF8
{
    U8 UByte1;
    U8 UByte2;
    U8 UByte3;
    U8 UByte4;
    U8 UByte5;
    U8 UByte6;
}__attribute__((packed)) UTF8;

typedef struct FONTFHEAD
{
    U8  fh_mgic[4];
    U32 fh_fsize;
    U8  fh_sectnr;
    U8  fh_fhght;
    U16 fh_wcpflg;
    U16 fh_nchars;
    U8  fh_rev[2];
}__attribute__((packed)) FontFHead;

typedef struct FTSECTIF
{
    U16 Fistuc;
    U16 Lastuc;
    U32 Off;
}__attribute__((packed)) FTSectIF;

typedef struct UFTINDX
{
    U32 UICharOff:26;
    U32 UICharWidth:6;
}__attribute__((packed)) UTFIndx;

#define MAX_CHARBITBY 32

typedef struct FONTDATA
{
    U8 FontByteSZ;
    U8 FontHigh;
    U8 FontWidth;
    U8 FontCharMap[MAX_CHARBITBY];
}__attribute__((packed)) FontData;

//typedef drvstus_t (*dftghp_ops_t)(void* ghpdevp,void* inp,void* outp,UInt x,UInt y,UInt iocode);

typedef struct GRAPHOPS
{
    Size (*GOpRead)(void* dev,void* out,Size rdsz);
    Size (*GOpWrite)(void* dev,void* in,Size wesz);
    SInt (*GOpIoCtrl)(void* dev,void* out,UInt iocode);
    void   (*GOpFlush)(void* dev);
    SInt (*GOpSetBank)(void* dev, SInt bnr);
    Pixl (*GOpReadpix)(void* dev,UInt x,UInt y);
    void   (*GOpWritePix)(void* dev,Pixl pix,UInt x,UInt y);
    Pixl (*GOpDXReadPix)(void* dev,UInt x,UInt y);
    void   (*GOpDXWritePix)(void* dev,Pixl pix,UInt x,UInt y);
    SInt (*GOpSetXY)(void* dev,UInt x,UInt y);
    SInt (*GOpSetVWH)(void* dev,UInt vwt,UInt vhi);
    SInt (*GOpSetXYOffset)(void* dev,UInt xoff,UInt yoff);
    SInt (*GOpGetXY)(void* dev,UInt* rx,UInt* ry);
    SInt (*GOpGetVWH)(void* dev,UInt* rvwt,UInt* rvhi);
    SInt (*GOpGetXYOffset)(void* dev,UInt* rxoff,UInt* ryoff);
}GraphOPS;

typedef struct DEFGRAPH
{
    SPinLock Lock;
    U64 Mode;
    U64 X;
    U64 Y;
    U64 FramPhyAddr;
    U64 FvrmPhyAddr;
    U64 FvrmSZ;
    U64 OnePixBits;
    U64 OnePixByte;
    U64 VBEModeNR;
    U64 Bank;
    U64 CurrDispBank;
    U64 NextBank;
    U64 BankSZ;
    U64 FontAddr;
    U64 FontSZ;
    U64 FontHigh;
    U64 NextCharsX;
    U64 NextCharsY;
    U64 LineSZ;
    Pixl DefFontPX;
    U64 CharDefXW;
    U64 Flush;
    U64 FramNR;
    U64 FSHData;
    GraphOPS OpFunc;
}DefGraph;

#define VBE_DISPI_IOPORT_INDEX (0x01CE)
#define VBE_DISPI_IOPORT_DATA (0x01CF)
#define VBE_DISPI_INDEX_ID (0)
#define VBE_DISPI_INDEX_XRES (1)
#define VBE_DISPI_INDEX_YRES (2)
#define VBE_DISPI_INDEX_BPP (3)
#define VBE_DISPI_INDEX_ENABLE (4)
#define VBE_DISPI_INDEX_BANK (5)
#define VBE_DISPI_INDEX_VIRT_WIDTH (6)
#define VBE_DISPI_INDEX_VIRT_HEIGHT (7)
#define VBE_DISPI_INDEX_X_OFFSET (8)
#define VBE_DISPI_INDEX_Y_OFFSET (9)
#define BGA_DEV_ID0 (0xB0C0) //- setting X and Y resolution and bit depth (8 BPP only), banked mode
#define BGA_DEV_ID1 (0xB0C1) //- virtual width and height, X and Y offset0
#define BGA_DEV_ID2 (0xB0C2) //- 15, 16, 24 and 32 BPP modes, support for linear frame buffer, support for retaining memory contents on mode switching
#define BGA_DEV_ID3 (0xB0C3) //- support for getting capabilities, support for using 8 bit DAC
#define BGA_DEV_ID4 (0xB0C4) //- VRAM increased to 8 MB
#define BGA_DEV_ID5 (0xB0C5) //- VRAM increased to 16 MB? [TODO: verify and check for other changes]
#define VBE_DISPI_BPP_4 (0x04)
#define VBE_DISPI_BPP_8 (0x08)
#define VBE_DISPI_BPP_15 (0x0F)
#define VBE_DISPI_BPP_16 (0x10)
#define VBE_DISPI_BPP_24 (0x18)
#define VBE_DISPI_BPP_32 (0x20)
#define VBE_DISPI_DISABLED (0x00)
#define VBE_DISPI_ENABLED (0x01)
#define VBE_DISPI_LFB_ENABLED (0x40)

private void DefGraphInit();
private Pixl SetDefFontPx(Pixl px);
private void SetNextCharsXY(U64 x, U64 y);
private void SetCharsDXWFlush(U64 chardxw, U64 flush);
public void HalShowVersion();
private void FillRect(Pixl pix, UInt x, UInt y, UInt tx, UInt ty);
private void FillBitMAPFile(Char* bmfname);
private void SlouSreen(UInt hy);
private void HalBackGround();
public void HalLogo();
public void BootVideoDeviceInit();
private void BGAInit();
private void VBEInit();
private void FillGraph(DefGraph* kghp, Pixl pix);
private void WritePixColor(DefGraph* kghp, U32 x, U32 y, Pixl pix);
private void DRXWPixColor(DefGraph* kghp, U32 x, U32 y, Pixl pix);
private void FlushVideoRam(DefGraph* kghp);
private U32 Utf8ToUnicode(UTF8* utf, int* retuib);
private U8* GetCharsInfo(DefGraph* kghp, U32 unicode, int* retchwx, int* retchhx, int* retlinb);
public void PutPixOneChar(DefGraph* kghp, U32 unicode, Pixl pix);
public void DRXPutPixOneChar(DefGraph* kghp, U32 unicode, Pixl pix);
public void HalDefStringWrite(Char* buf);
private void BGAWriteReg(U16 index, U16 data);
private U16 BGAReadReg(U16 index);
private Size BGARead(void* ghpdev, void* outp, Size rdsz);
private Size BGAWrite(void* ghpdev, void* inp, Size wesz);
private SInt BGAIoCtrl(void* ghpdev, void* outp, UInt iocode);
private U64* GetVRamAddrInBank(void* ghpdev);
private void BGADispNextBank(void* ghpdev);
private void BGAFlush(void* ghpdev);
private SInt BGASetBank(void* ghpdev, SInt bnr);
private Pixl BGAReadPix(void* ghpdev, UInt x, UInt y);
private void BGAWritePix(void* ghpdev, Pixl pix, UInt x, UInt y);
private Pixl BGADXReadPix(void* ghpdev, UInt x, UInt y);
private void BGADXWritePix(void* ghpdev, Pixl pix, UInt x, UInt y);
private SInt BGASetXY(void* ghpdev, UInt x, UInt y);
private SInt BGASetVWH(void* ghpdev, UInt vwt, UInt vhi);
private SInt BGASetXYOffset(void* ghpdev, UInt xoff, UInt yoff);
private SInt BGAGetXY(void* ghpdev, UInt* rx, UInt* ry);
private SInt BGAGetVWH(void* ghpdev, UInt* rvwt, UInt* rvhi);
private SInt BGAGetXYOffset(void* ghpdev, UInt* rxoff, UInt* ryoff);
private Size VBERead(void* ghpdev, void* outp, Size rdsz);
private Size VBEWrite(void* ghpdev, void* inp, Size wesz);
private SInt VBEIoCtrl(void* ghpdev, void* outp, UInt iocode);
private void VBEFlush(void* ghpdev);
private SInt VBESetBank(void* ghpdev, SInt bnr);
private Pixl VBEReadPix(void* ghpdev, UInt x, UInt y);
private void VBEWritePix(void* ghpdev, Pixl pix, UInt x, UInt y);
private Pixl VBDXReadPix(void* ghpdev, UInt x, UInt y);
private void VBEDXWritePix(void* ghpdev, Pixl pix, UInt x, UInt y);
private SInt VBESetXY(void* ghpdev, UInt x, UInt y);
private SInt VBESetVWH(void* ghpdev, UInt vwt, UInt vhi);
private SInt VBESetXYOffset(void* ghpdev, UInt xoff, UInt yoff);
private SInt VBEGetXY(void* ghpdev, UInt* rx, UInt* ry);
private SInt VBEGetVWH(void* ghpdev, UInt* rvwt, UInt* rvhi);
private SInt VBEGetXYOffset(void* ghpdev, UInt* rxoff, UInt* ryoff);
public Bool HalFirmwareInit();
#endif 
