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

#endif
