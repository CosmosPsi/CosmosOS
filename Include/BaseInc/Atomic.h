/**********************************************************
		原子操作头文件Atomic.h
***********************************************************
		彭东
**********************************************************/
#ifndef _ATOMICHEAD
#define _ATOMICHEAD

typedef struct ATOMIC{
	volatile S32 Count;
}Atomic;

typedef struct REFCOUNT
{
	Atomic ARef;
}RefCount;


KLINE S32 AtomicRead(const Atomic *A)
{
        return (*(volatile U32 *)&(A)->Count);
}

KLINE void AtomicSet(Atomic *A, S32 Val)
{
        A->Count = Val;
}

KLINE void AtomicAdd(S32 Val, Atomic *A)
{
        __asm__ __volatile__("lock;"
                             "addl %1,%0"
                             : "+m"(A->Count)
                             : "ir"(I));
}

KLINE void AtomicSub(S32 Val, Atomic *A)
{
        __asm__ __volatile__("lock;"
                             "subl %1,%0"
                             : "+m"(A->Count)
                             : "ir"(Val));
}

KLINE S32 AtomicSubAndTest(S32 Val, Atomic *A)
{
        unsigned char c;

        __asm__ __volatile__("lock;"
                             "subl %2,%0; sete %1"
                             : "+m"(A->Count), "=qm"(c)
                             : "ir"(Val)
                             : "memory");
        return c;
}

KLINE void AtomicInc(Atomic *A)
{
        __asm__ __volatile__("lock;"
                             "incl %0"
                             : "+m"(A->Count));
}

KLINE void AtomicDec(Atomic *A)
{
        __asm__ __volatile__("lock;"
                             "decl %0"
                             : "+m"(A->Count));
}

KLINE S32 AtomicDecAndTest(Atomic *A)
{
        unsigned char c;

        __asm__ __volatile__("lock;"
                             "decl %0; sete %1"
                             : "+m"(A->Count), "=qm"(c)
                             :
                             : "memory");
        return c != 0;
}

KLINE S32 AtomicIncAndTest(Atomic *A)
{
        unsigned char c;

        __asm__ __volatile__("lock;"
                             "incl %0; sete %1"
                             : "+m"(A->Count), "=qm"(c)
                             :
                             : "memory");
        return c != 0;
}

KLINE void RefCountInit(RefCount *init)
{
        AtomicSet(&init->ARef, 0);
        return;
}

KLINE void RefCountInc(RefCount *Ref)
{
        AtomicInc(&Ref->ARef);
        return;
}

KLINE void RefCountDec(RefCount *Ref)
{
        AtomicDec(&Ref->ARef);
}

KLINE S32 RefCountRead(RefCount *Ref)
{
        return AtomicRead(&Ref->ARef);
}

#endif
