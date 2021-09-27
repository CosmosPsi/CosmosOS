/******************************************************************
*		ld自动化链接脚本文件OSLinker.S			  					*
*				彭东 											  *
******************************************************************/

#include "OSLinker.h" 
OUTPUT_ARCH(OSONARCH)
OUTPUT_FORMAT(OSONFORMAT)
ENTRY(OSSTARTPOINT)

SECTIONS
{
        
	. = LINEAR_ADDRESS;
	StartKernel = . + VIRTUAL_ADDRESS;
    BeginStartText = .;
        .start.text : ALIGN(4) { *(.start.text) }
	EndStartText = .;
	
	BeginStartData = .;
        .start.data : ALIGN(0x4) { *(.start.data) }
	EndStartData = .;

	. +=VIRTUAL_ADDRESS;
	BeginEntry64Text = .;
        .entry64.text ALIGN(4) : AT(ADDR(.entry64.text)-VIRTUAL_ADDRESS){ *(.entry64.text) }
	EndEntry64Text = .;
    
    BeginCPUCoreArchData = .;
        .cpu.core.arch.data ALIGN(4) : AT(ADDR(.cpu.core.arch.data)-VIRTUAL_ADDRESS){ *(.cpu.core.arch.data) }
	EndCPUCoreArchData = .;

    BeginCPUCoreInterruptData = .;
        .cpu.core.interrupt.data ALIGN(4) : AT(ADDR(.cpu.core.interrupt.data)-VIRTUAL_ADDRESS){ *(.cpu.core.interrupt.data) }
	EndCPUCoreInterruptData = .;

    BeginHWINTText = .;
	.hwint.text ALIGN(4) : AT(ADDR(.hwint.text)-VIRTUAL_ADDRESS){ *(.hwint.text) }
	EndHWINTText = .;
	
	BeginEIRSText = .;
	.eirs.text ALIGN(4) : AT(ADDR(.eirs.text)-VIRTUAL_ADDRESS) { *(.eirs.text) }
	EndEIRSText = .;
	
	BeginSYSCallText = .;
	.syscall.text ALIGN(4) : AT(ADDR(.syscall.text)-VIRTUAL_ADDRESS) {*(.syscall.text) }
	EndSYSCallText = .;
	
	BeginSchedText = .;
	.sched.text ALIGN(4) : AT(ADDR(.sched.text)-VIRTUAL_ADDRESS) { *(.sched.text) }
	EndSchedText = .;
	
	BeginFsText = .;
	.fs.text ALIGN(4) : AT(ADDR(.fs.text)-VIRTUAL_ADDRESS) { *(.fs.text) }
	EndFsText = .;

	BeginDriverText = .;
	.driver.text ALIGN(4) : AT(ADDR(.driver.text)-VIRTUAL_ADDRESS) { *(.driver.text)}
	EndDriverText = .;
	
	BeginText = .;
        .text  ALIGN(4) : AT(ADDR(.text)-VIRTUAL_ADDRESS) { *(.text) }
	EndText = .;
	
	BeginSPinLockText = .;
	.spinlock.text ALIGN(4) : AT(ADDR(.spinlock.text)-VIRTUAL_ADDRESS) { *(.spinlock.text) }
	EndSPinLockText = .;
	
	BeginDriverEntryText = .;
	.drventry.text ALIGN(4) : AT(ADDR(.drventry.text)-VIRTUAL_ADDRESS) { *(.drventry.text)}
	EndDriverEntryText = .;

    BeginMEMData = .;
        .mem.data ALIGN(4) : AT(ADDR(.mem.data)-VIRTUAL_ADDRESS) { *(.mem.data) }
 	EndMEMData = .;

	BeginData = .;
        .data ALIGN(4) : AT(ADDR(.data)-VIRTUAL_ADDRESS) { *(.data) *(.bss) }
 	EndData = .;
	
	BeginRoData = .;
	.rodata ALIGN(4) : AT(ADDR(.rodata)-VIRTUAL_ADDRESS) { *(.rodata) *(.rodata.*) }
	EndRRoData = .;

	BeginKStrTab = .;
	.kstrtab ALIGN(4) : AT(ADDR(.kstrtab)-VIRTUAL_ADDRESS) { *(.kstrtab) }
	EndKStrTab = .;

	BeginAPBootText = .;
	.apboot.text ALIGN(4) : AT(ADDR(.apboot.text)-VIRTUAL_ADDRESS) { *(.apboot.text) }
	EndAPBootText = .;	
	
	BeginAPBootData = .;
	.apboot.data ALIGN(4) : AT(ADDR(.apboot.data)-VIRTUAL_ADDRESS) { *(.apboot.data) }
	EndAPBootData = .;

	BeginBss = .;
      .bss ALIGN(4) : AT(ADDR(.bss)-VIRTUAL_ADDRESS) { *(.bss) }
	EndBss = .;
	EndKernel = .;
}