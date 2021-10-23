/**********************************************************
        基本数据类型文件BaseType.h
***********************************************************
                彭东
**********************************************************/
#ifndef _BASETYPEHEAD
#define _BASETYPEHEAD

// 无符号整数类型
typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;
typedef unsigned long U64;

// 有符号整数类型
typedef char S8;
typedef short S16;
typedef int S32;
typedef long S64;

// 通用有/无符号整数类型
typedef long SInt;
typedef unsigned long UInt;

// CPU标志寄存器类型
typedef U64 CPUFlg;

// 地址类型
typedef unsigned long Addr;

// 布尔类型
typedef SInt Bool;

// 字符串类型
typedef const char* String;

// 字符类型
typedef char Char;

// 句柄类型
typedef SInt Hand;

// 缓冲区地址类型
typedef void* Buff;

// 大小类型
typedef unsigned long Size;

// 驱动状态
typedef SInt DrvStus;

// 静态，内联缩写
#define KLINE static inline

// public 啥也不做，标记而已
#define public

// 通过 static 关键字来达到文件私有
#define private	static

// 外部的
#define EXTERN extern

// 外部的
#define KEXTERN extern

// NULL = 0，注意是 usigned long
#define NULL	(0UL)

// TRUE = 1
#define TRUE    (1UL)

// FALSE = 0
#define	FALSE	(0UL)

// 对齐，可参考：https://en.wikipedia.org/wiki/Data_structure_alignment
#define ALIGN(x, a)     (((x) + (a) - 1) & ~((a) - 1))

// 注意：do {} while(0) 是一种宏书写技巧，将代码包括起来并只执行一次
// 判断为 NULL 返回
#define IF_NULL_RETURN(ptr) do{if(NULL == ptr){return;}}while(0)

// 判断为 NULL 返回 false
#define IF_NULL_RETURN_FALSE(ptr) do{if(NULL == ptr){return FALSE;}}while(0)

// 判断为 NULL 返回 NULL
#define IF_NULL_RETURN_NULL(ptr) do{if(NULL == ptr){return NULL;}}while(0)

// 判断为 0 返回 NULL
#define IF_ZERO_RETURN_NULL(x) do{if(0 == x){return NULL;}}while(0)

// 判断为 0 返回 FALSE
#define IF_ZERO_RETURN_FALSE(x) do{if(0 == x){return FALSE;}}while(0)

// 判断为 NULL 返回 0
#define IF_NULL_RETURN_ZERO(ptr) do{if(NULL == ptr){return 0;}}while(0)

// 判断为 NULL，直接死机
#define IF_NULL_DEAD(ptr) do{if(NULL == ptr){KrlErrorCrashDead("OBJ PTR IS NULL!!");}}while(0)

// 将对象内存清 0
#define INIT_OBJOFPTR_ZERO(ptr) do{HalMemSet((void*)ptr, 0, sizeof(typeof(*ptr)));}while(0)

// 测试失败打印
#define TEST_FAIL_STRING(fmt, ...) fmt,##__VA_ARGS__

// 测试失败，打印并死机
#define TEST_FAIL_DEAD_ESTMFUNC(str) KrlErrorCrashDead(TEST_FAIL_STRING("%s,%d,%s,%s",__FILE__,__LINE__,__FUNCTION__,str))

// 是否为测试指令
#define IF_TEST_OP(cmpsrc, tester, op, excstmfunc) do{if(cmpsrc op tester) {excstmfunc;}}while(0)

// 是否为退出指令
#define IF_RET_OP(cmpsrc, tester, op, rets) do{if(cmpsrc op tester) {return rets;}}while(0)

// 判断失败，直接死机
// <= 判断
#define IF_LTN_DEAD(cmp, test, str) IF_TEST_OP(cmp, test, <, TEST_FAIL_DEAD_ESTMFUNC(str))
// >= 判断
#define IF_GTN_DEAD(cmp, test, str) IF_TEST_OP(cmp, test, >, TEST_FAIL_DEAD_ESTMFUNC(str))
// == 判断
#define IF_EQT_DEAD(cmp, test, str) IF_TEST_OP(cmp, test, ==, TEST_FAIL_DEAD_ESTMFUNC(str))
// != 判断
#define IF_NEQ_DEAD(cmp, test, str) IF_TEST_OP(cmp, test, !=, TEST_FAIL_DEAD_ESTMFUNC(str))

// 判断失败直接返回
// 如果 <= 返回
#define IF_LTN_RETURN(cmp, test, rets) IF_RET_OP(cmp, test, <, rets)
// 如果 >= 返回
#define IF_GTN_RETURN(cmp, test, rets) IF_RET_OP(cmp, test, >, rets)
// 如果 == 返回
#define IF_EQT_RETURN(cmp, test, rets) IF_RET_OP(cmp, test, ==, rets)
// 如果 != 返回
#define IF_NEQ_RETURN(cmp, test, rets) IF_RET_OP(cmp, test, !=, rets)

// 如果 <= 0，返回 NULL
#define IF_LTNZERO_RETRUN_NULL(test) IF_LTN_RETURN(test, 0, NULL)
// 如果 <= 1，返回 NULL
#define IF_LTNONE_RETRUN_NULL(test) IF_LTN_RETURN(test, 1, NULL)
// 如果 >= 0，返回 NULL
#define IF_GTNZERO_RETRUN_NULL(test) IF_GTN_RETURN(test, 0, NULL)
// 如果 >= 1，返回 NULL
#define IF_GTNONE_RETRUN_NULL(test) IF_GTN_RETURN(test, 1, NULL)
// 如果 != 0，返回 NULL
#define IF_NEQZERO_RETRUN_NULL(test) IF_NEQ_RETURN(test, 0, NULL)
// 如果 != 1，返回 NULL
#define IF_NEQONE_RETRUN_NULL(test) IF_NEQ_RETURN(test, 1, NULL)

// 如果 <= 0，返回 FALSE
#define IF_LTNZERO_RETRUN_FALSE(test) IF_LTN_RETURN(test, 0, FALSE)
// 如果 <= 1，返回 FALSE
#define IF_LTNONE_RETRUN_FALSE(test) IF_LTN_RETURN(test, 1, FALSE)
// 如果 >= 0，返回 FALSE
#define IF_GTNZERO_RETRUN_FALSE(test) IF_GTN_RETURN(test, 0, FALSE)
// 如果 >= 1，返回 FALSE
#define IF_GTNONE_RETRUN_FALSE(test) IF_GTN_RETURN(test, 1, FALSE)
// 如果 != 0，返回 FALSE
#define IF_NEQZERO_RETRUN_FALSE(test) IF_NEQ_RETURN(test, 0, FALSE)
// 如果 != 1，返回 FALSE
#define IF_NEQONE_RETRUN_FALSE(test) IF_NEQ_RETURN(test, 1, FALSE)

#endif