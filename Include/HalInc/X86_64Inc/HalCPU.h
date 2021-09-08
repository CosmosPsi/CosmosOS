/**********************************************************
		CPU头文件HalCPU.h
***********************************************************
				彭东
**********************************************************/
#ifndef _HALCPUHEAD
#define _HALCPUHEAD

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

#define NORETURN __attribute__((noreturn))
#define SYSRCALL __attribute__((regparm(3)))
#define HINTCALL __attribute__((regparm(2)))
#define FAUTCALL __attribute__((regparm(2)))

#define REGCALL __attribute__((regparm(3))) //让GCC使用(EAX, EDX, ECX)寄存器传递参数

#define SAVECPUFLAGS(x) __asm__ __volatile__("pushfl ; popl %0" : "=g"(x) : "memory")
#define RESTORECPUFLAGS(x) __asm__ __volatile__("pushl %0 ; popfl" : "g"(x) : "memory")
#define CLI() __asm__ __volatile__("cli" : : : "memory")
#define STI() __asm__ __volatile__("sti" : : : "memory")

#define HALT() __asm__ __volatile__("hlt": : : "memory")
#define STIHALT() __asm__ __volatile__("sti; hlt": : : "memory")
#define CLIHALT() __asm__ __volatile__("cli; hlt": : : "memory")

KLINE void HalOutU8(const U16 port, const U8 val)
{

    __asm__ __volatile__("outb  %1, %0\n"
                         :
                         : "dN"(port), "a"(val));
}

KLINE void HalOutU8NOP(const U16 port, const U8 val)
{

    __asm__ __volatile__("outb  %1, %0\n\t"
                         "nop \n\t"
                         "nop \n\t"
                         "nop \n\t"
                         "nop \n\t"
                         :
                         : "dN"(port), "a"(val));
}

KLINE U8 HalInU8(const U16 port)
{
    U8 tmp;
    __asm__ __volatile__("inb %1, %0\n"
                         : "=a"(tmp)
                         : "dN"(port));
    return tmp;
}

KLINE void HalOutU16(const U16 port, const U16 val)
{

    __asm__ __volatile__("outw  %1, %0\n"
                         :
                         : "dN"(port), "a"(val));
}

KLINE U16 HalInU16(const U16 port)
{
    U16 tmp;

    __asm__ __volatile__("inw %1, %0\n"
                         : "=a"(tmp)
                         : "dN"(port));
    return tmp;
};

KLINE void HalOutU32(const U16 port, const U32 val)
{

    __asm__ __volatile__("outl  %1, %0\n"
                         :
                         : "dN"(port), "a"(val));
}

KLINE U32 HalInU32(const U16 port)
{
    U32 tmp;

    __asm__ __volatile__("inl %1, %0\n"
                         : "=a"(tmp)
                         : "dN"(port));
    return tmp;
}

KLINE U64 HalReadMsr(const U32 reg)
{
    U32 eax, edx;

    __asm__ __volatile__(
        "rdmsr"
        : "=a"(eax), "=d"(edx)
        : "c"(reg));

    return (((U64)edx) << 32) | (U64)eax;
}

KLINE void HalWriteMsr(const U32 reg, const U64 val)
{
    __asm__ __volatile__(
        "wrmsr"
        :
        : "a"((U32)val), "d"((U32)(val >> 32)), "c"(reg));
}

KLINE void HalMemCopy(void* src, void* dest, UInt count)
{

    U8 *ss = src, *sd = dest;
    for (UInt i = 0; i < count; i++)
    {
        sd[i] = ss[i];
    }
    return;
}

KLINE SInt HalM2MCopy(void* sadr, void* dadr, SInt len)
{
    if (NULL == sadr || NULL == dadr || 1 > len)
    {
        return 0;
    }
    U8 *s = (U8 *)sadr, *d = (U8 *)dadr;
    if (s < d)
    {
        for (SInt i = (len - 1); i >= 0; i--)
        {
            d[i] = s[i];
        }
        return len;
    }
    if (s > d)
    {
        for (SInt j = 0; j < len; j++)
        {
            d[j] = s[j];
        }
        return len;
    }
    if (s == d)
    {
        return len;
    }
    return 0;
}

KLINE void HalMemSet(void *s, U8 c, UInt count)
{
    U8 *st = s;
    for (UInt i = 0; i < count; i++)
    {
        st[i] = c;
    }
    return;
}

KLINE void HalSaveFlagsCli(CPUFlg *flags)
{
    __asm__ __volatile__(
        "pushfq \t\n"
        "cli    \t\n"
        "popq %0 \t\n"

        : "=m"(*flags)
        :
        : "memory");
    return;
}

KLINE void HalRestoreFlagsSti(CPUFlg *flagsres)
{
    __asm__ __volatile__(
        "pushq %0 \t\n"
        "popfq \t\n"
        :
        : "m"(*flagsres)
        : "memory");
    return;
}

KLINE void HalRestoreFlagsCli(CPUFlg *flags)
{
    __asm__ __volatile__(
        "pushq %0 \t\n"
        "popfq \t\n"
        : "=m"(*flags)
        :
        : "memory");
    return;
}

KLINE void HalSaveFlagsSti(CPUFlg *flags)
{
    __asm__ __volatile__(
        "pushfq \t\n"
        "popq %0\t\n"
        "sti\t\n"
        :
        : "m"(*flags)
        : "memory");
    return;
}

KLINE U64 HalReadX86RDPMC(const int ctrsel)
{
    U32 eax, edx;

    __asm__ __volatile__(
        "rdpmc"
        : "=a"(eax), "=d"(edx)
        : "c"(ctrsel));

    return (((U64)edx) << 32) | (U64)eax;
}

KLINE U64 HalReadX86RDTSC(void)
{
    U32 leax, ledx;

    __asm__ __volatile__(
        "rdtsc"
        : "=a"(leax), "=d"(ledx));

    return (((U64)ledx) << 32) | (U64)leax;
}

KLINE SInt HalSearch64RLBits(U64 val)
{
    SInt retbitnr = -1;
    __asm__ __volatile__(
        "bsrq %1,%q0 \t\n"
        : "+r"(retbitnr)
        : "rm"(val));
    return retbitnr + 1;
}

KLINE SInt HalSearch32RLBits(U32 val)
{
    SInt retbitnr = -1;
    __asm__ __volatile__(
        "bsrl %1,%0 \t\n"
        : "+r"(retbitnr)
        : "rm"(val));
    return retbitnr + 1;
}

KLINE U32 HalReadKESP()
{
    U32 esp;

    __asm__ __volatile__(
        "movl %%esp,%0"
        : "=g"(esp)
        :
        : "memory");

    return esp;
}

KLINE U64 HalReadRSP()
{
    U64 rsp;

    __asm__ __volatile__(
        "movq %%rsp,%0"
        : "=g"(rsp)
        :
        : "memory");

    return rsp;
}


KLINE void HalSetCR3(U64 val)
{
    __asm__ __volatile__(

        "movq %0,%%cr3 \n\t"
        :
        : "r"(val)
        : "memory"
    );
    return;
}

KLINE UInt HalReadCR2()
{
    UInt regtmp = 0;
    __asm__ __volatile__(
        "movq %%cr2,%0\n\t"
        : "=r"(regtmp)
        :
        : "memory");
    return regtmp;
}

KLINE UInt HalReadCR3()
{
    UInt regtmp = 0;
    __asm__ __volatile__(
        "movq %%cr3,%0\n\t"
        : "=r"(regtmp)
        :
        : "memory");
    return regtmp;
}

KLINE void HalWriteCR3(UInt val)
{
    __asm__ __volatile__(
        "movq %0,%%cr3 \n\t"
        :
        : "r"(val)
        : "memory" //, "edx"
    );
    return;
}

public Addr HalVAddrToPAddr(Addr kvaddr);
public Addr HalPAddrToVAddr(Addr kpaddr);
public Bool HalCPUInit();
#endif
