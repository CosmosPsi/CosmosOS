extern syscalls_table
extern hal_hwint_allocator
extern hal_fault_allocator
extern hal_syscl_allocator

global HalExCDivideError
global HalExCSingleStepException
global HalExCNMI
global HalExCBreakPointException
global HalExCOverflow
global HalExCBoundsCheck
global HalExCInvalOPCode
global HalExCCoprNotAvailable
global HalExCDoubleFault
global HalExCCoprSegOverrun
global HalExCInvalTSS
global HalExCSegmentNotPresent
global HalExCStackException
global HalExCGeneralProtection
global HalExCPageFault
global HalExCCoprError
global HalExCAlignmentCheck
global HalExCMachineCheck
global HalExCSimdFault
global HalExCOSDefaultFaultHandle
global HalExIHWINT00
global HalExIHWINT01
global HalExIHWINT02
global HalExIHWINT03
global HalExIHWINT04
global HalExIHWINT05
global HalExIHWINT06
global HalExIHWINT07
global HalExIHWINT08
global HalExIHWINT09
global HalExIHWINT10
global HalExIHWINT11
global HalExIHWINT12
global HalExIHWINT13
global HalExIHWINT14
global HalExIHWINT15
global HalExIHWINT16
global HalExIHWINT17
global HalExIHWINT18
global HalExIHWINT19
global HalExIHWINT20
global HalExIHWINT21
global HalExIHWINT22
global HalExIHWINT23
global HalExIApicSVR
global HalExIApicIPISchedul
global HalExIApicTimer
global HalExIApicThermal
global HalExIApicPerformonitor
global HalExIApicLint0
global HalExIApicLint1
global HalExIApicError
global HalExSAPICall
global HalASMRetFromUserMode
%define	INT_VECTOR_IRQ0			0x20
%define	INT_VECTOR_IRQ8			0x28
%define INT_VECTOR_APIC_IPI_SCHEDUL 0xe0
%define	INT_VECTOR_APIC_SVR 0xef
%define INT_VECTOR_APIC_TIMER	0xf0
%define INT_VECTOR_APIC_THERMAL	0xf1
%define INT_VECTOR_APIC_PERFORM	0xf2
%define INT_VECTOR_APIC_LINTO	0xf3
%define INT_VECTOR_APIC_LINTI	0xf4
%define INT_VECTOR_APIC_ERROR	0xf5

%define _KERNEL_DS 0x10
%define _NOERRO_CODE 0xffffffffffffffff


%macro	SAVEALL	0

	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rsi
	push rdi
	
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	
	xor r14,r14
	
	mov r14w,ds
	push r14
	mov r14w,es
	push r14
	mov r14w,fs
	push r14
	mov r14w,gs
	push r14
%endmacro

%macro	RESTOREALL	0
	pop r14
	mov gs,r14w
	pop r14 
	mov fs,r14w
	pop r14
	mov es,r14w
	pop r14
	mov ds,r14w
	 
	
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	
	pop rdi
	pop rsi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
	
	iretq
%endmacro

%macro	SAVEALLFAULT 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rsi
	push rdi
	
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	xor r14,r14
	mov r14w,ds
	push r14
	mov r14w,es
	push r14
	mov r14w,fs
	push r14
	mov r14w,gs
	push r14
	
%endmacro

%macro	RESTOREALLFAULT	0

	pop r14
	mov gs,r14w
	pop r14 
	mov fs,r14w
	pop r14
	mov es,r14w
	pop r14
	mov ds,r14w
	
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	
	pop rdi
	pop rsi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
	
	add rsp,8
	iretq
%endmacro

%macro	SRFTFAULT 1
	push	  _NOERRO_CODE
	SAVEALLFAULT
	mov r14w,0x10
	mov ds,r14w
	mov es,r14w
	mov fs,r14w
	mov gs,r14w

	mov 	rdi,%1 ;rdi, rsi
	mov 	rsi,rsp
	call 	hal_fault_allocator
	RESTOREALLFAULT
%endmacro

%macro	SRFTFAULT_ECODE 1
	SAVEALLFAULT
	mov r14w,0x10
	mov ds,r14w
	mov es,r14w
	mov fs,r14w
	mov gs,r14w

	mov 	rdi,%1
	mov 	rsi,rsp
	call 	hal_fault_allocator
	RESTOREALLFAULT
%endmacro


%macro	HARWINT	1
	SAVEALL
	
	mov r14w,0x10
	mov ds,r14w
	mov es,r14w
	mov fs,r14w
	mov gs,r14w

	mov	rdi, %1
	mov 	rsi,rsp
	call    hal_hwint_allocator

	RESTOREALL
%endmacro



%macro  EXSAPICALL  0
	
	push rbx
	push rcx
	push rdx
	push rbp
	push rsi
	push rdi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	
	xor r14,r14
	
	mov r14w,ds
	push r14
	mov r14w,es
	push r14
	mov r14w,fs
	push r14
	mov r14w,gs
	push r14
	
	mov r14w,0x10
	mov ds,r14w
	mov es,r14w
	mov fs,r14w
	mov gs,r14w
	
	mov	rdi, rax
	mov rsi, rsp
	call hal_syscl_allocator
	
	pop r14
	mov gs,r14w
	pop r14 
	mov fs,r14w
	pop r14
	mov es,r14w
	pop r14
	mov ds,r14w
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	
	pop rdi
	pop rsi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	iretq
%endmacro
