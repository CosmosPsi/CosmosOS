/**********************************************************
        基本数据类型文件BaseType.h
***********************************************************
                彭东
**********************************************************/
#ifndef _BASETYPEHEAD
#define _BASETYPEHEAD
//无符号整数类型
typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;
typedef unsigned long U64;
//有符号整数类型
typedef char S8;
typedef short S16;
typedef int S32;
typedef long S64;
//通用有/无符号整数类型
typedef long SInt;
typedef unsigned long UInt;
//CPU标志寄存器类型
typedef U64 CPUFlg;
//地址类型
typedef unsigned long Addr;
//布尔类型
typedef SInt Bool;
//字符串类型
typedef const char* String;
//字符类型
typedef char Char;
//句柄类型
typedef SInt Hand;
//缓冲区地址类型
typedef void* Buff;
//大小类型
typedef unsigned long Size;

typedef SInt DrvStus;
#define KLINE static inline
#define public
#define private	static
#define EXTERN extern
#define KEXTERN extern
#define NULL	(0UL)
#define TRUE    (1UL)
#define	FALSE	(0UL)

#define ALIGN(x, a)     (((x) + (a) - 1) & ~((a) - 1))

#define IF_NULL_RETURN(ptr) if(NULL == ptr){return;}
#define IF_NULL_DEAD(ptr) if(NULL == ptr){KrlErrorCrashDead("OBJ PTR IS NULL!!");}

#define INIT_OBJOFPTR_ZERO(ptr) HalMemSet((void*)ptr, 0, sizeof(typeof(*ptr)))

#endif