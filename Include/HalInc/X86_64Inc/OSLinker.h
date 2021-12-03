/******************************************************************
*		ld自动化链接脚本头文件OSLinker.h			  					*
*				彭东 											  *
******************************************************************/
#ifndef _OSLINKERHEAD
#define _OSLINKERHEAD

#define LINEAR_ADDRESS 0x2000000
#define VIRTUAL_ADDRESS 0xffff800000000000
#define OSONARCH i386:x86-64
#define OSONFORMAT elf64-x86-64
#define OSSTARTPOINT InitEntryStart

extern long StartKernel;
extern long BeginStartText;
extern long	EndStartText;
extern long	BeginStartData;
extern long	EndStartData;
extern long	BeginEntry64Text;
extern long	EndEntry64Text;
extern long BeginBootData;
extern long	EndBootData;
extern long BeginCPUCoreArchData;
extern long	EndCPUCoreArchData;
extern long BeginCPUCoreInterruptData;
extern long	EndCPUCoreInterruptData;
extern long BeginHWINTText;
extern long	EndHWINTText;
extern long	BeginEIRSText;
extern long	EndEIRSText;
extern long	BeginSYSCallText;
extern long	EndSYSCallText;
extern long	BeginSchedText;
extern long	EndSchedText;
extern long	BeginFsText;
extern long	EndFsText;
extern long	BeginDriverText;
extern long	EndDriverText;
extern long	BeginText;
extern long	EndText;
extern long	BeginSPinLockText;
extern long	EndSPinLockText;
extern long	BeginDriverEntryText;
extern long	EndDriverEntryText;
extern long BeginMEMData;
extern long EndMEMData;
extern long BeginExecutorData;
extern long EndExecutorData;
extern long BeginData;
extern long	EndData;
extern long	BeginRoData;
extern long	EndRRoData;
extern long	BeginKStrTab;
extern long	EndKStrTab;
extern long	BeginAPBootText;
extern long	EndAPBootText;
extern long	BeginAPBootData;
extern long	EndAPBootData;
extern long BeginServiceData;
extern long	EndServiceData;
extern long	BeginBss;
extern long	EndBss;
extern long	EndKernel;

#endif
