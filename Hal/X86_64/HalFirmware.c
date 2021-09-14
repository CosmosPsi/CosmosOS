/**********************************************************
        固件文件HalFirmware.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "HalFirmware.h"
#include "HalSync.h"
#include "HalCPU.h"
#include "KrlLog.h"


const char* CosmosVersion = "Cosmos\n内核版本:00.01\n彭东 @ 构建于 "__DATE__
                             " "__TIME__
                             "\n";

private void DefGraphInit()
{
    DefGraph* kghp = &kdftgh;
    machbstart_t* kmbsp = &kmachbsp;
    HalMemset(kghp, 0, sizeof(DefGraph));
    SPinLockInit(&kghp->Lock);
    kghp->Mode = kmbsp->mb_ghparm.mode;
    kghp->X = kmbsp->mb_ghparm.X;
    kghp->Y = kmbsp->mb_ghparm.Y;
    kghp->FramPhyAddr = HalPAddrToVAddr((Addr)kmbsp->mb_ghparm.FramPhyAddr);
    kghp->FvrmPhyAddr = HalPAddrToVAddr((Addr)kmbsp->mb_fvrmphyadr);
    kghp->FvrmSZ = kmbsp->mb_fvrmsz;
    kghp->OnePixBits = kmbsp->mb_ghparm.gh_onepixbits;
    kghp->OnePixByte = kmbsp->mb_ghparm.gh_onepixbits / 8;
    kghp->VBEModeNR = kmbsp->mb_ghparm.gh_vbemodenr;
    kghp->Bank = kmbsp->mb_ghparm.Bank;
    kghp->CurrDispBank = kmbsp->mb_ghparm.gh_curdipbnk;
    kghp->NextBank = kmbsp->mb_ghparm.FvrmPhyAddr;
    kghp->BankSZ = kmbsp->mb_ghparm.Banksz;
    kghp->FontAddr = HalPAddrToVAddr((Addr)kmbsp->mb_bfontpadr);
    kghp->FontSZ = kmbsp->mb_bfontsz;
    kghp->FontHigh = 16;
    kghp->LineSZ = 16 + 4;
    kghp->DefFontPX = BGRA(0xff, 0xff, 0xff);
    return;
}

private Pixl SetDefFontPx(Pixl px)
{
    DefGraph* kghp = &kdftgh;
    Pixl bkpx = kghp->DefFontPX;
    kghp->DefFontPX = px;
    return bkpx;
}

private void SetNextCharsXY(U64 x, U64 y)
{
    DefGraph* kghp = &kdftgh;
    kghp->NextCharsX = x;
    kghp->NextCharsY = y;
    return;
}

private void SetCharsDXWFlush(U64 chardxw, U64 flush)
{
    kdftgh.CharDefXW = chardxw;
    kdftgh.Flush = flush;
    return;
}

public void HalShowVersion()
{
    Pixl bkpx = SetDefFontPx(BGRA(0xff, 0, 0));
    KrlPrint(CosmosVersion);
    KrlPrint("系统存在进程:%d个 系统空闲内存大小:%dMB\n", (UInt)osschedcls.scls_threadnr, (UInt)((memmgrob.mo_freepages << 12) >> 20));
    KrlPrint("系统处理器工作模式:%d位 系统物理内存大小:%dMB\n", (UInt)kmachbsp.mb_cpumode, (UInt)(kmachbsp.mb_memsz >> 20));
    SetDefFontPx(bkpx);
    return;
}

private void FillRect(Pixl pix, UInt x, UInt y, UInt tx, UInt ty)
{
    for (UInt i = y; i < y + ty; i++)
    {
        for (UInt j = x; j < x + tx; j++)
        {
            WritePixColor(&kdftgh, (U32)j, (U32)i, pix);
        }
    }
    return;
}

private void FillBitMAPFile(Char* bmfname)
{
    DefGraph* kghp = &kdftgh;
    machbstart_t* kmbsp = &kmachbsp;
    U64 fadr = 0, fsz = 0;
    get_file_rvadrandsz(bmfname, kmbsp, &fadr, &fsz);
    if (0 == fadr || 0 == fsz)
    {
        system_error("not bitmap file err\n");
    }
    BMDBGR* bmdp;
    U64 img = fadr + sizeof(BMFHead) + sizeof(BitMInfo);
    bmdp = (BMDBGR* )((UInt)img);
    Pixl pix;
    int k = 0, l = 0;
    for (int j = 768; j >= 0; j--, l++)
    {
        for (int i = 0; i < 1024; i++)
        {
            pix = BGRA(bmdp[k].bmd_r, bmdp[k].bmd_g, bmdp[k].bmd_b);
            WritePixColor(kghp, i, j, pix);
            k++;
        }
    }
    FlushVideoRam(kghp);
    return;
}

private void SlouSreen(UInt hy)
{
    UInt x, y, ofx, ofy;
    DefGraph* kghp = &kdftgh;
    BGAGetXYOffset(kghp, &x, &y);
    BGAGetVWH(kghp, &ofx, &ofy);
    y += hy;
    ofy += hy;
    BGASetVWH(kghp, ofx, ofy);
    BGASetXYOffset(kghp, x, y);
    return;
}

private void HalBackGround()
{
    DefGraph* kghp = &kdftgh;
    machbstart_t* kmbsp = &kmachbsp;
    U64 fadr = 0, fsz = 0;
    get_file_rvadrandsz("desktop1.bmp", kmbsp, &fadr, &fsz);
    if (0 == fadr || 0 == fsz)
    {
        system_error("Cosmos background fail");
    }
    BMDBGR* bmdp;
    U64 img = fadr + sizeof(BMFHead) + sizeof(BitMInfo);
    bmdp = (BMDBGR*)((UInt)img);
    Pixl pix;
    int k = 0, l = 0;
    for (int j = 768; j >= 0; j--, l++)
    {
        for (int i = 0; i < 1024; i++)
        {
            pix = BGRA(bmdp[k].bmd_r, bmdp[k].bmd_g, bmdp[k].bmd_b);
            DRXWPixColor(kghp, i, j, pix);
            k++;
        }
    }
    HalShowVersion();
    return;
}

public void HalLogo()
{
    DefGraph* kghp = &kdftgh;
    machbstart_t* kmbsp = &kmachbsp;
    U64 fadr = 0, fsz = 0;
    get_file_rvadrandsz("logo.bmp", kmbsp, &fadr, &fsz);
    if (0 == fadr || 0 == fsz)
    {
        system_error("logoerr");
    }
    BMDBGR* bmdp;
    U64 img = fadr + sizeof(BMFHead) + sizeof(BitMInfo);
    bmdp = (BMDBGR* )((UInt)img);
    Pixl pix;
    int k = 0, l = 0;
    for (int j = 617; j >= 153; j--, l++)
    {
        for (int i = 402; i < 622; i++)
        {
            pix = BGRA(bmdp[k].bmd_r, bmdp[k].bmd_g, bmdp[k].bmd_b);
            DRXWPixColor(kghp, i, j, pix);
            k++;
        }
        if (l > 205)
        {
            die(0x80);
        }
    }
    return;
}

public void BootVideoDeviceInit()
{
    DefGraph* kghp = &kdftgh;

    DefGraphInit();
    BGAInit();
    VBEInit();
    FillGraph(kghp, BGRA(0, 0, 0));
    SetCharsDXWFlush(0, 0);
    HalBackGround();
    KrlLog(0, "图形显示初始化成功\n");
    // die(0x400);
    return;
}

private void BGAInit()
{
    DefGraph* kghp = &kdftgh;
    if (kghp->gh_mode != BGAMODE)
    {
        return;
    }
    kghp->OpFunc.GOpRead = BGARead;
    kghp->OpFunc.GOpWrite = BGAWrite;
    kghp->OpFunc.GOpIoCtrl = BGAIoCtrl;
    kghp->OpFunc.GOpFlush = BGAFlush;
    kghp->OpFunc.GOpSetBank = BGASetBank;
    kghp->OpFunc.GOpReadPix = BGAReadPix;
    kghp->OpFunc.GOpWritePix = BGAWritePix;
    kghp->OpFunc.GOpDXReadPix = BGADXReadPix;
    kghp->OpFunc.GOpDXWritePix = BGADXWritePix;
    kghp->OpFunc.GOpSetXY = BGASetXY;
    kghp->OpFunc.GOpSetVWH = BGASetVWH;
    kghp->OpFunc.GOpSetXYOffset = BGASetXYOffset;
    kghp->OpFunc.GOpGetXY = BGAGetXY;
    kghp->OpFunc.GOpGetVWH = BGAGetVWH;
    kghp->OpFunc.GOpGetXYOffset = BGAGetXYOffset;
    return;
}

private void VBEInit()
{
    DefGraph* kghp = &kdftgh;
    if (kghp->gh_mode != VBEMODE)
    {
        return;
    }
    kghp->OpFunc.GOpRead = VBERead;
    kghp->OpFunc.GOpWrite = VBEWrite;
    kghp->OpFunc.GOpIoCtrl = VBEIoCtrl;
    kghp->OpFunc.GOpFlush = VBEFlush;
    kghp->OpFunc.GOpSetBank = VBESetBank;
    kghp->OpFunc.GOpReadpix = VBEReadPix;
    kghp->OpFunc.GOpWritePix = VBEWritePix;
    kghp->OpFunc.GOpDXReadPix = VBDXReadPix;
    kghp->OpFunc.GOpDXWritePix = VBEDXWritePix;
    kghp->OpFunc.GOpSetXY = VBESetXY;
    kghp->OpFunc.GOpSetVWH = VBESetVWH;
    kghp->OpFunc.GOpSetXYOffset = VBESetXYOffset;
    kghp->OpFunc.GOpGetXY = VBEGetXY;
    kghp->OpFunc.GOpGetVWH = VBEGetVWH;
    kghp->OpFunc.GOpGetXYOffSet = VBEGetXYOffset;
    return;
}

private void FillGraph(DefGraph* kghp, Pixl pix)
{
    for (U64 y = 0; y < kghp->Y; y++)
    {
        for (U64 x = 0; x < kghp->X; x++)
        {
            WritePixColor(kghp, (U32)x, (U32)y, pix);
        }
    }
    FlushVideoRam(kghp);
    return;
}

private void WritePixColor(DefGraph* kghp, U32 x, U32 y, Pixl pix)
{
    kghp->OpFunc.GOpWritePix(kghp, pix, x, y);
    return;
}

private void DRXWPixColor(DefGraph* kghp, U32 x, U32 y, Pixl pix)
{

    kghp->OpFunc.GOpDXWritePix(kghp, pix, x, y);
    return;
}

private void FlushVideoRam(DefGraph* kghp)
{
    kghp->OpFunc.GOpFlush(kghp);
    return;
}

private U32 Utf8ToUnicode(UTF8* utf, int* retuib)
{
    U8 uhd = utf->UByte1, ubyt = 0;
    U32 ucode = 0, tmpuc = 0;
    if (0x80 > uhd) //0xbf&&uhd<=0xbf
    {
        ucode = utf->UByte1 & 0x7f;
        *retuib = 1;
        return ucode;
    }
    if (0xc0 <= uhd && uhd <= 0xdf) //0xdf
    {
        ubyt = utf->UByte1 & 0x1f;
        tmpuc |= ubyt;
        ubyt = utf->UByte2 & 0x3f;
        ucode = (tmpuc << 6) | ubyt;
        *retuib = 2;
        return ucode;
    }
    if (0xe0 <= uhd && uhd <= 0xef) //0xef
    {
        ubyt = utf->UByte1 & 0x0f;
        tmpuc |= ubyt;
        ubyt = utf->UByte2 & 0x3f;
        tmpuc <<= 6;
        tmpuc |= ubyt;
        ubyt = utf->UByte3 & 0x3f;
        ucode = (tmpuc << 6) | ubyt;
        *retuib = 3;
        return ucode;
    }
    if (0xf0 <= uhd && uhd <= 0xf7) //0xf7
    {
        ubyt = utf->UByte1 & 0x7;
        tmpuc |= ubyt;
        ubyt = utf->UByte2 & 0x3f;
        tmpuc <<= 6;
        tmpuc |= ubyt;
        ubyt = utf->UByte3 & 0x3f;
        tmpuc <<= 6;
        tmpuc |= ubyt;
        ubyt = utf->UByte4 & 0x3f;
        ucode = (tmpuc << 6) | ubyt;
        *retuib = 4;
        return ucode;
    }
    if (0xf8 <= uhd && uhd <= 0xfb) //0xfb
    {
        ubyt = utf->UByte1 & 0x3;
        tmpuc |= ubyt;
        ubyt = utf->UByte2 & 0x3f;
        tmpuc <<= 6;
        tmpuc |= ubyt;
        ubyt = utf->UByte3 & 0x3f;
        tmpuc <<= 6;
        tmpuc |= ubyt;
        ubyt = utf->UByte4 & 0x3f;
        tmpuc <<= 6;
        tmpuc |= ubyt;
        ubyt = utf->UByte5 & 0x3f;
        ucode = (tmpuc << 6) | ubyt;
        *retuib = 5;
        return ucode;
    }
    if (0xfc <= uhd && uhd <= 0xfd) //0xfd
    {
        ubyt = utf->UByte1 & 0x1;
        tmpuc |= ubyt;
        ubyt = utf->UByte2 & 0x3f;
        tmpuc <<= 6;
        tmpuc |= ubyt;
        ubyt = utf->UByte3 & 0x3f;
        tmpuc <<= 6;
        tmpuc |= ubyt;
        ubyt = utf->UByte4 & 0x3f;
        tmpuc <<= 6;
        tmpuc |= ubyt;
        ubyt = utf->UByte5 & 0x3f;
        tmpuc <<= 6;
        tmpuc |= ubyt;
        ubyt = utf->UByte6 & 0x3f;
        ucode = (tmpuc << 6) | ubyt;
        *retuib = 6;
        return ucode;
    }
    *retuib = 0;
    return 0;
}

private U8* GetCharsInfo(DefGraph* kghp, U32 unicode, int* retchwx, int* retchhx, int* retlinb)
{
    FontData* fntp = (FontData* )((UInt)kghp->FontAddr);
    *retchwx = fntp[unicode].FontWidth;
    *retchhx = fntp[unicode].FontHigh;
    *retlinb = fntp[unicode].FontByteSZ;

    return (U8*)(&(fntp[unicode].FontCharMap[0]));
}

public void PutPixOneChar(DefGraph* kghp, U32 unicode, Pixl pix)
{
    int y = 0, chwx = 0, chhx = 0, linb = 0;
    U8* charyp = NULL;
    if (unicode == 10)
    {
        kghp->NextCharsX = 0;
        if ((kghp->NextCharsY + kghp->LineSZ) > (kghp->Y - kghp->FontHigh))
        {
            kghp->NextCharsY = 0;
            HalBackGround();
            goto ret;
        }
        kghp->NextCharsY += kghp->LineSZ;
        goto ret;
    }
    charyp = GetCharsInfo(kghp, unicode, &chwx, &chhx, &linb);
    if (charyp == NULL)
    {
        goto ret;
    }
    if ((kghp->NextCharsX + chwx) > (kghp->X - chwx))
    {
        kghp->NextCharsX = 0;
        if ((kghp->NextCharsY + kghp->LineSZ) > (kghp->Y - kghp->FontHigh))
        {
            kghp->NextCharsY = 0;
        }
        else
        {
            kghp->NextCharsY += kghp->LineSZ;
        }
    }
    U32 wpx = (U32)kghp->NextCharsX, wpy = (U32)kghp->NextCharsY;

    for (int hx = 0; hx < chhx; hx++)
    {
        for (int btyi = 0, x = 0; btyi < linb; btyi++)
        {
            U8 bitmap = charyp[hx * linb + btyi];
            if (bitmap == 0)
            {
                x += 8;
                continue;
            }
            for (int bi = 7; bi >= 0; bi--)
            {
                if (x >= chwx)
                {
                    break;
                }
                if (((bitmap >> bi) & 1) == 1)
                {
                    WritePixColor(kghp, wpx + x, wpy + y, pix);
                }
                x++;
            }
        }
        y++;
    }
    kghp->NextCharsX += (U64)chwx;
    return;
}

public void DRXPutPixOneChar(DefGraph* kghp, U32 unicode, Pixl pix)
{
    int y = 0, chwx = 0, chhx = 0, linb = 0;
    U8* charyp = NULL;
    if (unicode == 10)
    {
        kghp->NextCharsX = 0;
        if ((kghp->NextCharsY + kghp->LineSZ) > (kghp->Y - kghp->FontHigh))
        {
            kghp->NextCharsY = 0;
            HalBackGround();
            goto ret;
        }
        kghp->NextCharsY += kghp->LineSZ;
        goto ret;
    }
    charyp = GetCharsInfo(kghp, unicode, &chwx, &chhx, &linb);
    if (charyp == NULL)
    {
        goto ret;
    }
    if ((kghp->NextCharsX + chwx) > (kghp->X - chwx))
    {
        kghp->NextCharsX = 0;
        if ((kghp->NextCharsY + kghp->LineSZ) > (kghp->Y - kghp->FontHigh))
        {
            kghp->NextCharsY = 0;
        }
        else
        {
            kghp->NextCharsY += kghp->LineSZ;
        }
    }
    U32 wpx = (U32)kghp->NextCharsX, wpy = (U32)kghp->NextCharsY;

    for (int hx = 0; hx < chhx; hx++)
    {
        for (int btyi = 0, x = 0; btyi < linb; btyi++)
        {
            U8 bitmap = charyp[hx * linb + btyi];
            if (bitmap == 0)
            {
                x += 8;
                continue;
            }
            for (int bi = 7; bi >= 0; bi--)
            {
                if (x >= chwx)
                {
                    break;
                }
                if (((bitmap >> bi) & 1) == 1)
                {
                    DRXWPixColor(kghp, wpx + x, wpy + y, pix);
                }
                x++;
            }
        }
        y++;
    }
    kghp->NextCharsX += (U64)chwx;

    return;
}

public void HalDefStringWrite(Char* buf)
{
    int chinx = 0;
    int chretnext = 0;
    U32 uc = 0;
    DefGraph* kghp = &kdftgh;
    Pixl pix = kghp->DefFontPX;
    IF_NULL_RETURN(buf);
    while (buf[chinx] != 0)
    {
        uc = Utf8ToUnicode((UTF8* )(&buf[chinx]), &chretnext);
        chinx += chretnext;
        if (kghp->gh_chardxw == 1)
        {
            PutPixOneChar(kghp, uc, pix);
        }
        else
        {
            DRXPutPixOneChar(kghp, uc, pix);
        }
    }
    if (kghp->gh_flush == 1)
    {
        FlushVideoRam(kghp);
    }
    return;
}

private void BGAWriteReg(U16 index, U16 data)
{
    out_u16(VBE_DISPI_IOPORT_INDEX, index);
    out_u16(VBE_DISPI_IOPORT_DATA, data);
    return;
}

private U16 BGAReadReg(U16 index)
{
    out_u16(VBE_DISPI_IOPORT_INDEX, index);
    return in_u16(VBE_DISPI_IOPORT_DATA);
}

private size_t BGARead(void* ghpdev, void* outp, size_t rdsz)
{
    //DefGraph* kghp=(DefGraph*)ghpdev;
    return rdsz;
}

private size_t BGAWrite(void* ghpdev, void* inp, size_t wesz)
{
    //DefGraph* kghp=(DefGraph*)ghpdev;
    return wesz;
}

private SInt BGAIoCtrl(void* ghpdev, void* outp, UInt iocode)
{
    return -1;
}

private U64* GetVRamAddrInBank(void* ghpdev)
{
    DefGraph* kghp = (DefGraph* )ghpdev;
    U64* d = (U64* )((UInt)(kghp->FramPhyAddr + (kghp->X * kghp->Y * kghp->FvrmPhyAddr * kghp->FvrmPhyAddr)));

    return d;
}

private void BGADispNextBank(void* ghpdev)
{
    DefGraph* kghp = (DefGraph* )ghpdev;
    U16 h = (U16)(kghp->Y * kghp->NextBank + 1);
    U16 ofy = (U16)(kghp->Y *  (kghp->FvrmPhyAddr));
    BGAWriteReg(VBE_DISPI_INDEX_VIRT_HEIGHT, h);
    BGAWriteReg(VBE_DISPI_INDEX_Y_OFFSET, ofy);
    kghp->gh_curdipbnk = kghp->FvrmPhyAddr;
    kghp->FvrmPhyAddr++;
    if (kghp->FvrmPhyAddr > kghp->Bank)
    {
        kghp->FvrmPhyAddr = 0;
    }
    return;
}

private void BGAFlush(void* ghpdev)
{
    DefGraph* kghp = (DefGraph* )ghpdev;

    U64* s = (U64* )((UInt)kghp->FvrmPhyAddr);
    U64* d = GetVRamAddrInBank(kghp); //(U64*)kghp->FramPhyAddr;
    U64 i = 0, j = 0;
    U64 e = kghp->X * kghp->Y * kghp->OnePixByte;
    //U64 k=e/8;
    for (; i < e; i += 8)
    {
        d[j] = s[j];
        j++;
    }
    BGADispNextBank(kghp);
    return;
}

private SInt BGASetBank(void* ghpdev, SInt bnr)
{
    return -1;
}

private Pixl BGAReadPix(void* ghpdev, UInt x, UInt y)
{
    return 0;
}

private void BGAWritePix(void* ghpdev, Pixl pix, UInt x, UInt y)
{
    DefGraph* kghp = (DefGraph* )ghpdev;
    U8* p24bas;
    if (kghp->gh_onepixbits == 24)
    {
        U64 p24adr = (x + (y * kghp->X)) * 3;
        p24bas = (U8* )((UInt)(p24adr + kghp->FvrmPhyAddr));
        p24bas[0] = (U8)(pix);
        p24bas[1] = (U8)(pix >> 8);
        p24bas[2] = (U8)(pix >> 16);
        return;
    }
    U32* phybas = (U32* )((UInt)kghp->FvrmPhyAddr);
    phybas[x + (y * kghp->X)] = pix;
    return;
}

private Pixl BGADXReadPix(void* ghpdev, UInt x, UInt y)
{
    return 0;
}

private void BGADXWritePix(void* ghpdev, Pixl pix, UInt x, UInt y)
{
    DefGraph* kghp = (DefGraph* )ghpdev;
    U8* p24bas;
    if (kghp->gh_onepixbits == 24)
    {
        U64 p24adr = (x + (y * kghp->X)) * 3 * kghp->gh_curdipbnk;
        p24bas = (U8* )((UInt)(p24adr + kghp->FramPhyAddr));
        p24bas[0] = (U8)(pix);
        p24bas[1] = (U8)(pix >> 8);
        p24bas[2] = (U8)(pix >> 16);
        return;
    }
    U32* phybas = (U32* )((UInt)(kghp->FramPhyAddr + (kghp->X * kghp->Y * kghp->FvrmPhyAddr * kghp->gh_curdipbnk)));
    phybas[x + (y * kghp->X)] = pix;
    return;
}

private SInt BGASetXY(void* ghpdev, UInt x, UInt y)
{

    BGAWriteReg(VBE_DISPI_INDEX_XRES, (U16)(x));
    BGAWriteReg(VBE_DISPI_INDEX_YRES, (U16)(y));

    return 0;
}

private SInt BGASetVWH(void* ghpdev, UInt vwt, UInt vhi)
{

    BGAWriteReg(VBE_DISPI_INDEX_VIRT_WIDTH, (U16)vwt);
    BGAWriteReg(VBE_DISPI_INDEX_VIRT_HEIGHT, (U16)vhi);
    return 0;
}

private SInt BGASetXYOffset(void* ghpdev, UInt xoff, UInt yoff)
{
    BGAWriteReg(VBE_DISPI_INDEX_X_OFFSET, (U16)(xoff));
    BGAWriteReg(VBE_DISPI_INDEX_Y_OFFSET, (U16)(yoff));
    return 0;
}

private SInt BGAGetXY(void* ghpdev, UInt* rx, UInt* ry)
{
    if (rx == NULL || ry == NULL)
    {
        return -1;
    }
    U16 retx, rety;
    retx = BGAReadReg(VBE_DISPI_INDEX_XRES);
    rety = BGAReadReg(VBE_DISPI_INDEX_YRES);
    *rx = (UInt)retx;
    *ry = (UInt)rety;
    return 0;
}

private SInt BGAGetVWH(void* ghpdev, UInt* rvwt, UInt* rvhi)
{
    if (rvwt == NULL || rvhi == NULL)
    {
        return -1;
    }
    U16 retwt, rethi;
    retwt = BGAReadReg(VBE_DISPI_INDEX_VIRT_WIDTH);
    rethi = BGAReadReg(VBE_DISPI_INDEX_VIRT_HEIGHT);
    *rvwt = (UInt)retwt;
    *rvhi = (UInt)rethi;
    return 0;
}

private SInt BGAGetXYOffset(void* ghpdev, UInt* rxoff, UInt* ryoff)
{
    if (rxoff == NULL || ryoff == NULL)
    {
        return -1;
    }
    U16 retxoff, retyoff;
    retxoff = BGAReadReg(VBE_DISPI_INDEX_X_OFFSET);
    retyoff = BGAReadReg(VBE_DISPI_INDEX_Y_OFFSET);
    *rxoff = (UInt)retxoff;
    *ryoff = (UInt)retyoff;
    return 0;
}

private size_t VBERead(void* ghpdev, void* outp, size_t rdsz)
{
    return rdsz;
}

private size_t VBEWrite(void* ghpdev, void* inp, size_t wesz)
{
    return wesz;
}

private SInt VBEIoCtrl(void* ghpdev, void* outp, UInt iocode)
{
    return -1;
}

private void VBEFlush(void* ghpdev)
{
    DefGraph* kghp = (DefGraph* )ghpdev;

    U64* s = (U64* )((UInt)kghp->FvrmPhyAddr);
    U64* d = (U64* )((UInt)kghp->FramPhyAddr);
    U64 i = 0, j = 0;
    U64 e = kghp->X * kghp->Y * kghp->FvrmPhyAddr;
    for (; i < e; i += 8)
    {
        d[j] = s[j];
        j++;
    }
    return;
}

private SInt VBESetBank(void* ghpdev, SInt bnr)
{
    return -1;
}

private Pixl VBEReadPix(void* ghpdev, UInt x, UInt y)
{
    return 0;
}

private void VBEWritePix(void* ghpdev, Pixl pix, UInt x, UInt y)
{
    DefGraph* kghp = (DefGraph* )ghpdev;
    U8* p24bas;
    if (kghp->gh_onepixbits == 24)
    {
        U64 p24adr = (x + (y * kghp->X)) * 3;
        p24bas = (U8* )((UInt)(p24adr + kghp->FvrmPhyAddr));
        p24bas[0] = (U8)(pix);
        p24bas[1] = (U8)(pix >> 8);
        p24bas[2] = (U8)(pix >> 16);
        return;
    }
    U32* phybas = (U32* )((UInt)kghp->FvrmPhyAddr);
    phybas[x + (y * kghp->X)] = pix;
    return;
}

private Pixl VBDXReadPix(void* ghpdev, UInt x, UInt y)
{

    return 0;
}

private void VBEDXWritePix(void* ghpdev, Pixl pix, UInt x, UInt y)
{
    DefGraph* kghp = (DefGraph* )ghpdev;
    U8* p24bas;
    if (kghp->gh_onepixbits == 24)
    {
        U64 p24adr = (x + (y*  kghp->X))*  3;
        p24bas = (U8* )((UInt)(p24adr + kghp->FramPhyAddr));
        p24bas[0] = (U8)(pix);
        p24bas[1] = (U8)(pix >> 8);
        p24bas[2] = (U8)(pix >> 16);
        return;
    }
    U32* phybas = (U32* )((UInt)kghp->FramPhyAddr);
    phybas[x + (y*  kghp->X)] = pix;
    return;
}

private SInt VBESetXY(void* ghpdev, UInt x, UInt y)
{
    return -1;
}

private SInt VBESetVWH(void* ghpdev, UInt vwt, UInt vhi)
{
    return -1;
}

private SInt VBESetXYOffset(void* ghpdev, UInt xoff, UInt yoff)
{
    return -1;
}

private SInt VBEGetXY(void* ghpdev, UInt* rx, UInt* ry)
{
    return -1;
}

private SInt VBEGetVWH(void* ghpdev, UInt* rvwt, UInt* rvhi)
{
    return -1;
}

private SInt VBEGetXYOffset(void* ghpdev, UInt* rxoff, UInt* ryoff)
{

    return -1;
}

public Bool HalFirmwareInit()
{
    BootVideoDeviceInit();
    return;
}
