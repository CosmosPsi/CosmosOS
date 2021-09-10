/**********************************************************
        hal层中断处理头文件halInterrupt.h
***********************************************************
                彭东
**********************************************************/
#ifndef _HALINTERRUPTHEAD
#define _HALINTERRUPTHEAD

#define IERSD_MAX (256) 
#define DefinedCPUInterData(vartype,varname) __attribute__((section(".cpu.core.interrupt.data"))) vartype varname

typedef DrvStus (*IODCallBack)(UInt inr, void* device, void* kistacktop);

//Interrupt exception request service descriptor
typedef struct IERSD
{
    SPinLock  lock;
    U32       Flags;
    U32       Status;
    UInt      Prity;        //中断优先级
    UInt      InterNR;        //中断号
    UInt      Deep;         //中断嵌套深度
    U64       Count;         //中断计数
    List      IODLists;
    UInt      IODNR;
    List      IODThreadLists;   //中断线程链表头
    UInt      IODThreadNR;    //中断线程个数
    void*     OneIODThread;    //只有一个中断线程时直接用指针
    void*     RBTreeRoot;   //如果中断线程太多则按优先级组成红黑树
    void*     MSGMPool;     //可能的中断消息池
    void*     Priv;
    void*     Ext;
}IERSD;

//Interrupt object descriptor
typedef struct IOD
{
    List    Lists;
    List    InDeviceLists;
    U32     Flags;
    IERSD*  Parent;
    void*   Device;
    UInt    Count;
    IODCallBack CallBackFunc;
}IOD;

typedef struct IERSDT
{
    IERSD IERSDArr[IERSD_MAX];
}IERSDT;


void HalExCDivideError();
void HalExCSingleStepException();
void HalExCNMI();
void HalExCBreakPointException();
void HalExCOverflow();
void HalExCBoundsCheck();
void HalExCInvalOPCode();
void HalExCCoprNotAvailable();
void HalExCDoubleFault();
void HalExCCoprSegOverrun();
void HalExCInvalTSS();
void HalExCSegmentNotPresent();
void HalExCStackException();
void HalExCGeneralProtection();
void HalExCPageFault();
void HalExCCoprError();
void HalExCAlignmentCheck();
void HalExCMachineCheck();
void HalExCSimdFault();
void HalExCOSDefaultFaultHandle();
void HalExIHWINT00();
void HalExIHWINT01();
void HalExIHWINT02();
void HalExIHWINT03();
void HalExIHWINT04();
void HalExIHWINT05();
void HalExIHWINT06();
void HalExIHWINT07();
void HalExIHWINT08();
void HalExIHWINT09();
void HalExIHWINT10();
void HalExIHWINT11();
void HalExIHWINT12();
void HalExIHWINT13();
void HalExIHWINT14();
void HalExIHWINT15();
void HalExIHWINT16();
void HalExIHWINT17();
void HalExIHWINT18();
void HalExIHWINT19();
void HalExIHWINT20();
void HalExIHWINT21();
void HalExIHWINT22();
void HalExIHWINT23();
void HalExIApicSVR();
void HalExIApicIPISchedul();
void HalExIApicTimer();
void HalExIApicThermal();
void HalExIApicPerformonitor();
void HalExIApicLint0();
void HalExIApicLint1();
void HalExIApicError();
void HalExSAPICall();
void HalASMRetFromUserMode();

private Bool IERSDInit(IERSD* init, U32 flags, U32 status, UInt prity, UInt internr);
private Bool IERSDTInit();
private Bool IODInit(IOD* init, U32 flags, IERSD* parent, void* device, IODCallBack callbackfunc);
public void HalInterDistributor(UInt internr, void* kistacktop);
public void HalFaultDistributor(UInt faultnr, void* kfstacktop);
public void HalOSAPIDistributor(UInt apinr, void* kistacktop);
public Bool HalInterruptInit();
#endif
