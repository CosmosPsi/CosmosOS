/**********************************************************
        CPU相关的文件HalCPU.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "HalCPU.h"
#include "HalInterrupt.h"

DefinedCPUArchData(GDT, GDTable);
DefinedCPUArchData(IDT, IDTable);
DefinedCPUArchData(TSS, TSSTable);
DefinedCPUArchData(GDTPTR, GDTablePtr);
DefinedCPUArchData(IDTPTR, IDTablePtr);

public void __attribute__((optnone)) HalDead(UInt count)
{
    if(0 == count)
    {
        HALT();
    }

    for(UInt i = 0; i < count; i++)
    {
        for(UInt j = 0; j < count; j++)
        {
            for(UInt k = 0; k < count; k++)
            {
                for(UInt l = 0; l < count; l++)
                {
                    for(UInt m = 0; m < count; m++)
                    {
                        for(UInt n = 0; n < count; n++)
                        {
                            ;
                        }
                    }
                }
            }
        }
    }
    return;
}

public Addr HalVAddrToPAddr(Addr kvaddr)
{
    if (kvaddr < KRNL_MAP_VIRTADDRESS_START || kvaddr > KRNL_MAP_VIRTADDRESS_END)
    {
        // system_error("virAddro_phyadr err\n");
        return KRNL_ADDR_ERROR;
    }
    return kvaddr - KRNL_MAP_VIRTADDRESS_START;
}

public Addr HalPAddrToVAddr(Addr kpaddr)
{
    if (kpaddr >= KRNL_MAP_PHYADDRESS_END)
    {
        // system_error("phyAddro_viradr err\n");
        return KRNL_ADDR_ERROR;
    }
    return kpaddr + KRNL_MAP_VIRTADDRESS_START;
}

//gate = &IDTable.GateDescArr[vector];
private Bool SetOneGateDesc(GateDesc* gate, U8 type, IDTHandler handler, U8 privilege)
{
    U64 base = (U64)handler;
    gate->OffsetLow = base & 0xFFFF;
    gate->Selector = SELECTOR_KERNEL_CS;
    gate->DCount = 0;
    gate->Attr = (U8)(type | (privilege << 5));
    gate->OffsetHigh = (U16)((base >> 16) & 0xFFFF);
    gate->OffsetHighH = (U32)((base >> 32) & 0xffffffff);
    gate->OffsetResv = 0;
    return TRUE;
}

private Bool SetOneSegmentDesc(SegmentDesc* desc, U32 base, U32 limit, U16 attribute)
{
    desc->LimitLow = limit & 0x0FFFF;                                                   // 段界限 1(2 字节)
    desc->BaseLow = base & 0x0FFFF;                                                     // 段基址 1(2 字节)
    desc->BaseMiddle = (base >> 16) & 0x0FF;                                               // 段基址 2(1 字节)
    desc->Attr1 = (U8)(attribute & 0xFF);                                              // 属性 1
    desc->LimitHighAttr2 = (U8)(((limit >> 16) & 0x0F) | ((attribute >> 8) & 0xF0)); // 段界限 2 + 属性 2
    desc->BaseHigh = (U8)((base >> 24) & 0x0FF);                                      // 段基址 3\(1 字节)
    return;
}

private Bool SetOneX64TSSAttrInSegmentDesc(SegmentDesc* desc, U64 base, U32 limit, U16 attribute)
{
    U32* x64tssbasehight = (U32*)(desc + 1);

    desc->LimitLow = limit & 0x0FFFF;                                                   // 段界限 1(2 字节)
    desc->BaseLow = base & 0x0FFFF;                                                     // 段基址 1(2 字节)
    desc->BaseMiddle = (base >> 16) & 0x0FF;                                               // 段基址 2(1 字节)
    desc->Attr1 = (U8)(attribute & 0xFF);                                              // 属性 1
    desc->LimitHighAttr2 = (U8)(((limit >> 16) & 0x0F) | ((attribute >> 8) & 0xF0)); // 段界限 2 + 属性 2
    desc->BaseHigh = (U8)((base >> 24) & 0x0FF);

    *x64tssbasehight = (U32)((base >> 32) & 0xffffffff);
    *(x64tssbasehight + 1) = 0;
    return TRUE;
}

private void CPULoadGDT(GDTPTR* gdtptr)
{

    __asm__ __volatile__(

        "cli \n\t"
        "pushq %%rax \n\t"
        //"movq %0,%%rax \n\t"
        "lgdt (%0) \n\t"

        "movabsq $1f,%%rax \n\t"
        //"pushq   $0  \n\t"             $x64_igdt_reg//# fake return address to stop unwinder
        "pushq   $8 \n\t"
        "pushq   %%rax    \n\t"
        "lretq \n\t"
        "1:\n\t"
        "movw $0x10,%%ax\n\t"
        "movw %%ax,%%ds\n\t"
        "movw %%ax,%%es\n\t"
        "movw %%ax,%%ss\n\t"
        "movw %%ax,%%fs\n\t"
        "movw %%ax,%%gs\n\t"
        "popq %%rax \n\t"
        :
        : "r"(gdtptr)
        : "rax", "memory");
    return;
}

private void CPULoadIDT(IDTPTR* idtptr)
{
    __asm__ __volatile__(
        "lidt (%0) \n\t"

        :
        : "r"(idtptr)
        : "memory");
    return;
}

private void CPULoadTr(U16 trindx)
{
    __asm__ __volatile__(
        "ltr %0 \n\t"
        :
        : "r"(trindx)
        : "memory");
    return;    
}

private Bool CPUInitTSS()
{
    TSS* tss = NULL;
    tss = HalGetTSSTableAddr();
    IF_NULL_RETURN_FALSE(tss);
    INIT_OBJOFPTR_ZERO(tss);
    return TRUE;
}

private Bool CPUInitIDT()
{
    for(UInt i = 0; i < IDTMAX; i++)
    {
        SetOneGateDesc(&IDTable.GateDescArr[i], DA_386IGate, HalExCOSDefaultFaultHandle, PRIVILEGE_KRNL);
    }
    
    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_DIVIDE], DA_386IGate, HalExCDivideError, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_DEBUG], DA_386IGate, HalExCSingleStepException, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_NMI], DA_386IGate, HalExCNMI, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_BREAKPOINT], DA_386IGate, HalExCBreakPointException, PRIVILEGE_USER);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_OVERFLOW], DA_386IGate, HalExCOverflow, PRIVILEGE_USER);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_BOUNDS], DA_386IGate, HalExCBoundsCheck, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_INVAL_OP], DA_386IGate, HalExCInvalOPCode, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_COPROC_NOT], DA_386IGate, HalExCCoprNotAvailable, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_DOUBLE_FAULT], DA_386IGate, HalExCDoubleFault, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_COPROC_SEG], DA_386IGate, HalExCCoprSegOverrun, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_INVAL_TSS], DA_386IGate, HalExCInvalTSS, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_SEG_NOT], DA_386IGate, HalExCSegmentNotPresent, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_STACK_FAULT], DA_386IGate, HalExCStackException, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_PROTECTION], DA_386IGate, HalExCGeneralProtection, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_PAGE_FAULT], DA_386IGate, HalExCPageFault, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_COPROC_ERR], DA_386IGate, HalExCCoprError, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_ALIGN_CHEK], DA_386IGate, HalExCAlignmentCheck, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_MACHI_CHEK], DA_386IGate, HalExCMachineCheck, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_SIMD_FAULT], DA_386IGate, HalExCSimdFault, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ0 + 0], DA_386IGate, HalExIHWINT00, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ0 + 1], DA_386IGate, HalExIHWINT01, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ0 + 2], DA_386IGate, HalExIHWINT02, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ0 + 3], DA_386IGate, HalExIHWINT03, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ0 + 4], DA_386IGate, HalExIHWINT04, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ0 + 5], DA_386IGate, HalExIHWINT05, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ0 + 6], DA_386IGate, HalExIHWINT06, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ0 + 7], DA_386IGate, HalExIHWINT07, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ8 + 0], DA_386IGate, HalExIHWINT08, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ8 + 1], DA_386IGate, HalExIHWINT09, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ8 + 2], DA_386IGate, HalExIHWINT10, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ8 + 3], DA_386IGate, HalExIHWINT11, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ8 + 4], DA_386IGate, HalExIHWINT12, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ8 + 5], DA_386IGate, HalExIHWINT13, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ8 + 6], DA_386IGate, HalExIHWINT14, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ8 + 7], DA_386IGate, HalExIHWINT15, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ8 + 8], DA_386IGate, HalExIHWINT16, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ8 + 9], DA_386IGate, HalExIHWINT17, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ8 + 10], DA_386IGate, HalExIHWINT18, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ8 + 11], DA_386IGate, HalExIHWINT19, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ8 + 12], DA_386IGate, HalExIHWINT20, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ8 + 13], DA_386IGate, HalExIHWINT21, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ8 + 14], DA_386IGate, HalExIHWINT22, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_IRQ8 + 15], DA_386IGate, HalExIHWINT23, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_APIC_IPI_SCHEDUL], DA_386IGate, HalExIApicIPISchedul, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_APIC_SVR], DA_386IGate, HalExIApicSVR, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_APIC_TIMER], DA_386IGate, HalExIApicTimer, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_APIC_THERMAL], DA_386IGate, HalExIApicThermal, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_APIC_PERFORM], DA_386IGate, HalExIApicPerformonitor, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_APIC_LINTO], DA_386IGate, HalExIApicLint0, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_APIC_LINTI], DA_386IGate, HalExIApicLint1, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_APIC_ERROR], DA_386IGate, HalExIApicError, PRIVILEGE_KRNL);

    SetOneGateDesc(&IDTable.GateDescArr[INT_VECTOR_SYSCALL], DA_386IGate, HalExSAPICall, PRIVILEGE_USER);

    IDTablePtr.IDTBass = (U64)(&IDTable.GateDescArr[0]);
    IDTablePtr.IDTLen = sizeof(IDTable.GateDescArr) - 1;
    CPULoadIDT(&IDTablePtr);
    return TRUE;
}

private Bool CPUInitGDT()
{
    SetOneSegmentDesc(&GDTable.SegmentDescArr[0], 0, 0, 0);
    SetOneSegmentDesc(&GDTable.SegmentDescArr[1], 0, 0, DA_CR | DA_64 | 0);
    SetOneSegmentDesc(&GDTable.SegmentDescArr[2], 0, 0, DA_DRW | DA_64 | 0);
    SetOneSegmentDesc(&GDTable.SegmentDescArr[3], 0, 0, DA_CR | DA_64 | DA_DPL3 | 0);
    SetOneSegmentDesc(&GDTable.SegmentDescArr[4], 0, 0, DA_DRW | DA_64 | DA_DPL3 | 0);
    SetOneX64TSSAttrInSegmentDesc(&GDTable.SegmentDescArr[6], (U64)(&TSSTable.TSSArr[0]), sizeof(TSSTable.TSSArr[0]) - 1, DA_386TSS);

    GDTablePtr.GDTBass = (U64)(GDTable.SegmentDescArr);
    GDTablePtr.GDTLen = sizeof(GDTable.SegmentDescArr) - 1;

    CPULoadGDT(&GDTablePtr);
    CPULoadTr(X64TSS_INDEX);
    return TRUE;
}

public UInt HalCPUID()
{
    return 0;
}

public void HalCPUSaveToNewContext(void* curr, void* next, Addr* outstack, Addr* instack, void* callbackfun)
{
    __asm__ __volatile__(
        "pushfq \n\t"
        "cli \n\t"
        "pushq %%rax\n\t"
        "pushq %%rbx\n\t"
        "pushq %%rcx\n\t"
        "pushq %%rdx\n\t"
        "pushq %%rbp\n\t"
        "pushq %%rsi\n\t"
        "pushq %%rdi\n\t"
        "pushq %%r8\n\t"
        "pushq %%r9\n\t"
        "pushq %%r10\n\t"
        "pushq %%r11\n\t"
        "pushq %%r12\n\t"
        "pushq %%r13\n\t"
        "pushq %%r14\n\t"
        "pushq %%r15\n\t"

        "movq %%rsp,[%[PREV_RSP]] \n\t"
        "movq [%[NEXT_RSP]],%%rsp \n\t"
        "callq %[CALLBACK]\n\t"

        "popq %%r15\n\t"
        "popq %%r14\n\t"
        "popq %%r13\n\t"
        "popq %%r12\n\t"
        "popq %%r11\n\t"
        "popq %%r10\n\t"
        "popq %%r9\n\t"
        "popq %%r8\n\t"
        "popq %%rdi\n\t"
        "popq %%rsi\n\t"
        "popq %%rbp\n\t"
        "popq %%rdx\n\t"
        "popq %%rcx\n\t"
        "popq %%rbx\n\t"
        "popq %%rax\n\t"
        "popfq \n\t"
        : [PREV_RSP] "=r"(outstack)
        : [NEXT_RSP] "r"(instack), "D"(curr), "S"(next), [CALLBACK] "r"(callbackfun)
        : "memory");

    return;
}

public void HalCPULoadContextRegisterToRun(Addr stack)
{
    IF_NULL_RETURN(stack);
    __asm__ __volatile__(
        "movq %[NEXT_RSP],%%rsp\n\t"
        "popq %%r14\n\t"
        "movw %%r14w,%%gs\n\t"
        "popq %%r14\n\t"
        "movw %%r14w,%%fs\n\t"
        "popq %%r14\n\t"
        "movw %%r14w,%%es\n\t"
        "popq %%r14\n\t"
        "movw %%r14w,%%ds\n\t"
        "popq %%r15\n\t"
        "popq %%r14\n\t"
        "popq %%r13\n\t"
        "popq %%r12\n\t"
        "popq %%r11\n\t"
        "popq %%r10\n\t"
        "popq %%r9\n\t"
        "popq %%r8\n\t"
        "popq %%rdi\n\t"
        "popq %%rsi\n\t"
        "popq %%rbp\n\t"
        "popq %%rdx\n\t"
        "popq %%rcx\n\t"
        "popq %%rbx\n\t"
        "popq %%rax\n\t"
        "iretq\n\t"

        :
        : [NEXT_RSP] "r"(stack)
        : "memory");
    return;
}

public Addr HalCPUInitContextRegisterInStack(Addr stacktop, Size size, Addr start, UInt mode, CPUFlg cpuflags, Addr userstack)
{
    StackREGContext* c = NULL;
    IF_ZERO_RETURN_NULL(stacktop);
    IF_ZERO_RETURN_NULL(size);
    IF_ZERO_RETURN_NULL(start);
    IF_ZERO_RETURN_NULL(mode);
    IF_LTN_RETURN(size, sizeof(StackREGContext), NULL);

    c = (StackREGContext*)(stacktop - sizeof(StackREGContext));
    INIT_OBJOFPTR_ZERO(c);

    if(USER_CPU_MODE == mode)
    {
        c->RIP = (UInt)start;
        c->CS = U_CS_IDX;
        c->RFLAGS = (UInt)cpuflags;
        c->RSP = (UInt)userstack;
        c->SS = U_DS_IDX;
        c->DS = U_DS_IDX;
        c->ES = U_DS_IDX;
        c->FS = U_DS_IDX;
        c->GS = U_DS_IDX;
        return (Addr)c;
    }
    else if(KERNEL_CPU_MODE == mode)
    {
        c->RIP = (UInt)start;
        c->CS = K_CS_IDX;
        c->RFLAGS = (UInt)cpuflags;
        c->RSP = (UInt)stacktop;
        c->SS = 0;
        c->DS = K_DS_IDX;
        c->ES = K_DS_IDX;
        c->FS = K_DS_IDX;
        c->GS = K_DS_IDX;
        return (Addr)c;
    }
    return NULL;
}

public Bool HalCPUInit()
{
    CPUInitGDT();
    CPUInitIDT();
    CPUInitTSS();
    return TRUE;
}