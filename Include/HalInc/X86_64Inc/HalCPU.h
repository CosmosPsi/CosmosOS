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

#define DefinedCPUArchData(vartype,varname) __attribute__((section(".cpu.core.arch.data"))) vartype varname

#define DEVCLASSMAX 256
#define GDTMAX 10
#define IDTMAX 256
#define STACKMAX 0x400


#define K_CS_IDX	0x08
#define K_DS_IDX	0x10
#define U_CS_IDX	0x1b
#define U_DS_IDX	0x23
#define K_TAR_IDX	0x28
#define UMOD_EFLAGS	0x1202



/* GDT */
/* 描述符索引 */
#define	PRIVILEGE_KRNL	0
#define	PRIVILEGE_TASK	1
#define	PRIVILEGE_USER	3

#define	INDEX_DUMMY			0
#define	INDEX_FLAT_C		1
#define	INDEX_FLAT_RW		2
/* 选择子 */
#define	SELECTOR_DUMMY		0		
#define	SELECTOR_FLAT_C		0x08			
#define	SELECTOR_FLAT_RW	0x10		 


#define	SELECTOR_KERNEL_CS	SELECTOR_FLAT_C
#define	SELECTOR_KERNEL_DS	SELECTOR_FLAT_RW

//TR寄存器的数据，X64TSS必须设置在GDTable中第6个段描述符的位置
#define X64TSS_INDEX (0x30)

/* 描述符类型值说明 */
#define DA_64 			0x2000
#define	DA_32			0x4000	/* 32 位段				*/
#define	DA_LIMIT_4K		0x8000	/* 段界限粒度为 4K 字节			*/
#define	DA_DPL0			0x00	/* DPL = 0				*/
#define	DA_DPL1			0x20	/* DPL = 1				*/
#define	DA_DPL2			0x40	/* DPL = 2				*/
#define	DA_DPL3			0x60	/* DPL = 3				*/
/* 存储段描述符类型值说明 */
#define	DA_DR			0x90	/* 存在的只读数据段类型值		*/
#define	DA_DRW			0x92	/* 存在的可读写数据段属性值		*/
#define	DA_DRWA			0x93	/* 存在的已访问可读写数据段类型值	*/
#define	DA_C			0x98	/* 存在的只执行代码段属性值		*/
#define	DA_CR			0x9A	/* 存在的可执行可读代码段属性值		*/
#define	DA_CCO			0x9C	/* 存在的只执行一致代码段属性值		*/
#define	DA_CCOR			0x9E	/* 存在的可执行可读一致代码段属性值	*/
/* 系统段描述符类型值说明 */
#define	DA_LDT			0x82	/* 局部描述符表段类型值			*/
#define	DA_TaskGate		0x85	/* 任务门类型值				*/
#define	DA_386TSS		0x89	/* 可用 386 任务状态段类型值		*/
#define	DA_386CGate		0x8C	/* 386 调用门类型值			*/
#define	DA_386IGate		0x8E	/* 386 中断门类型值			*/
#define	DA_386TGate		0x8F	/* 386 陷阱门类型值			*/

/* 中断向量 */
#define	INT_VECTOR_DIVIDE		0x0
#define	INT_VECTOR_DEBUG		0x1
#define	INT_VECTOR_NMI			0x2
#define	INT_VECTOR_BREAKPOINT		0x3
#define	INT_VECTOR_OVERFLOW		0x4
#define	INT_VECTOR_BOUNDS		0x5
#define	INT_VECTOR_INVAL_OP		0x6
#define	INT_VECTOR_COPROC_NOT		0x7
#define	INT_VECTOR_DOUBLE_FAULT	0x8
#define	INT_VECTOR_COPROC_SEG		0x9
#define	INT_VECTOR_INVAL_TSS		0xA
#define	INT_VECTOR_SEG_NOT		0xB
#define	INT_VECTOR_STACK_FAULT		0xC
#define	INT_VECTOR_PROTECTION		0xD
#define	INT_VECTOR_PAGE_FAULT		0xE
#define	INT_VECTOR_COPROC_ERR		0x10
#define	INT_VECTOR_ALIGN_CHEK		0x11
#define	INT_VECTOR_MACHI_CHEK		0x12
#define	INT_VECTOR_SIMD_FAULT		0x13

/* 中断向量 */
#define	INT_VECTOR_IRQ0			0x20
#define	INT_VECTOR_IRQ8			0x28

#define INT_VECTOR_APIC_IPI_SCHEDUL     0xe0
#define	INT_VECTOR_APIC_SVR		0xef
#define INT_VECTOR_APIC_TIMER		0xf0
#define INT_VECTOR_APIC_THERMAL		0xf1
#define INT_VECTOR_APIC_PERFORM		0xf2
#define INT_VECTOR_APIC_LINTO		0xf3
#define INT_VECTOR_APIC_LINTI		0xf4
#define INT_VECTOR_APIC_ERROR		0xf5

#define INT_VECTOR_SYSCALL		0xFF

typedef struct SEGMENTDESC		/* 共 8 个字节 */
{
	U16	LimitLow;		/* Limit */
	U16	BaseLow;		/* Base */
	U8	BaseMiddle;		/* Base */
	U8	Attr1;			/* P(1) DPL(2) DT(1) TYPE(4) */
	U8	LimitHighAttr2;	/* G(1) D(1) 0(1) AVL(1) LimitHigh(4) */
	U8	BaseHigh;		/* Base */
}__attribute__((packed)) SegmentDesc;
/* 门描述符 */
typedef struct GATEDESC
{
	U16	OffsetLow;	/* Offset Low */
	U16	Selector;	/* Selector */
	U8	DCount;		/* 该字段只在调用门描述符中有效。如果在利用
				   调用门调用子程序时引起特权级的转换和堆栈
				   的改变，需要将外层堆栈中的参数复制到内层
				   堆栈。该双字计数字段就是用于说明这种情况
				   发生时，要复制的双字参数的数量。*/
	U8	Attr;		/* P(1) DPL(2) DT(1) TYPE(4) */
	U16	OffsetHigh;	/* Offset High */
	U32 OffsetHighH;
	U32	OffsetResv;
}__attribute__((packed)) GateDesc;

typedef struct GDTPTR
{
	U16 GDTLen;
	U64 GDTBass;
}__attribute__((packed)) GDTPTR;

typedef struct IDTPTR
{
	U16 IDTLen;
	U64 IDTBass;
}__attribute__((packed)) IDTPTR;

typedef struct X64TSS
{
	U32 Reserv0;
	U64 RSP0;
	U64 RSP1;
	U64 RSP2;
	U64 Reserv28;
	U64 IST[7];
	U64 Reserv92;
	U16 Reserv100;
	U16 IOBase;
}__attribute__((packed)) X64TSS;

typedef struct TSS
{
    X64TSS TSSArr[1];
}TSS;

typedef struct GDT
{
    SegmentDesc SegmentDescArr[GDTMAX];
}__attribute__((packed)) GDT;

typedef struct IDT
{
    GateDesc GateDescArr[IDTMAX];
}__attribute__((packed)) IDT;

typedef void (*IDTHandler)();


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
    SInt count = -1;
    __asm__ __volatile__(
        "bsrq %1,%q0 \t\n"
        "jnz 1f \t\n"
        "movq $-1,%q0 \t\n"
        "1:"
        : "=q"(count)
        : "q"(val));
    return count;
}

KLINE SInt HalSearch32RLBits(U32 val)
{
    SInt count = -1;
    __asm__ __volatile__(
        "bsrl %1,%0 \t\n"
        "jnz 1f \t\n"
        "movl $-1,%0 \t\n"
        "1:"
        : "=r"(count)
        : "r"(val));
    return count;
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

public void __attribute__((optnone)) HalDead(UInt count);
public Addr HalVAddrToPAddr(Addr kvaddr);
public Addr HalPAddrToVAddr(Addr kpaddr);
private Bool SetOneGateDesc(GateDesc* gate, U8 type, IDTHandler handler, U8 privilege);
private Bool SetOneSegmentDesc(SegmentDesc* desc, U32 base, U32 limit, U16 attribute);
private Bool SetOneX64TSSAttrInSegmentDesc(SegmentDesc* desc, U64 base, U32 limit, U16 attribute);
private void CPULoadGDT(GDTPTR* gdtptr);
private void CPULoadIDT(IDTPTR* idtptr);
private void CPULoadTr(U16 trindx);
private Bool CPUInitIDT();
private Bool CPUInitGDT();
public Bool HalCPUInit();
#endif
