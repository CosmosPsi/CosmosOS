/**********************************************************
        hal层中断处理头文件halInterrupt.h
***********************************************************
                彭东
**********************************************************/
#ifndef _HALINTERRUPTHEAD
#define _HALINTERRUPTHEAD

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

#endif