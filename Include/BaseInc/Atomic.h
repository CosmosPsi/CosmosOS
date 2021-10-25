/**********************************************************
		原子操作头文件Atomic.h
***********************************************************
		彭东
**********************************************************/
#ifndef _ATOMICHEAD
#define _ATOMICHEAD

// 原子整数
// 使用 volatile 来避免缓存导致的数据不一致
typedef struct ATOMIC
{
    volatile S32 Count;
} Atomic;

// 引用计数
typedef struct REFCOUNT
{
    Atomic ARef;
} RefCount;

// 读取
// @param A 原子整数实例
KLINE S32 AtomicRead(const Atomic* A)
{
    return (*(volatile U32* )&(A)->Count);
}

// 设置
// @param A 原子整数实例
// @param val 整数
KLINE void AtomicSet(Atomic* A, S32 Val)
{
    A->Count = Val;
}

// 做加法运算
// @param val 加数
// @param A 原子整数实例
KLINE void AtomicAdd(S32 Val, Atomic* A)
{
    __asm__ __volatile__("lock;"
                         "addl %1,%0"
                         : "+m"(A->Count)
                         : "ir"(I));
}

// 做减法运算
// @param val 减数
// @param A 原子整数实例
KLINE void AtomicSub(S32 Val, Atomic* A)
{
    __asm__ __volatile__("lock;"
                         "subl %1,%0"
                         : "+m"(A->Count)
                         : "ir"(Val));
}

// 做减法运算
// @param val 减数
// @param A 原子整数实例
KLINE S32 AtomicSubAndTest(S32 Val, Atomic* A)
{
    unsigned char c;
    __asm__ __volatile__("lock;"
                         "subl %2,%0; sete %1"
                         : "+m"(A->Count), "=qm"(c)
                         : "ir"(Val)
                         : "memory");
    return c;
}

// 原子整数++
// @param A 原子整数实例
KLINE void AtomicInc(Atomic* A)
{
    __asm__ __volatile__("lock;"
                         "incl %0"
                         : "+m"(A->Count));
}

// 原子整数--
// @param A 原子整数实例
KLINE void AtomicDec(Atomic* A)
{
    __asm__ __volatile__("lock;"
                         "decl %0"
                         : "+m"(A->Count));
}

// 原子整数--后测试
// @param A 原子整数实例
KLINE S32 AtomicDecAndTest(Atomic* A)
{
    unsigned char c;
    __asm__ __volatile__("lock;"
                         "decl %0; sete %1"
                         : "+m"(A->Count), "=qm"(c)
                         :
                         : "memory");
    return c != 0;
}

// 原子整数++后测试
// @param A 原子整数实例
KLINE S32 AtomicIncAndTest(Atomic* A)
{
    unsigned char c;
    __asm__ __volatile__("lock;"
                         "incl %0; sete %1"
                         : "+m"(A->Count), "=qm"(c)
                         :
                         : "memory");
    return c != 0;
}

// 引用计数初始化
// @param init 引用计数实例
KLINE void RefCountInit(RefCount* init)
{
    AtomicSet(&init->ARef, 0);
    return;
}

// 引用计数++
// @param Ref 引用计数实例
KLINE void RefCountInc(RefCount* Ref)
{
    AtomicInc(&Ref->ARef);
    return;
}

// 引用计数--
// @param Ref 引用计数实例
KLINE void RefCountDec(RefCount* Ref)
{
    AtomicDec(&Ref->ARef);
}

// 读取引用个数
// @param Ref 引用计数实例
KLINE S32 RefCountRead(RefCount* Ref)
{
    return AtomicRead(&Ref->ARef);
}

#endif
