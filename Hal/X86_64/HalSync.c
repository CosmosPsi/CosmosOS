/**********************************************************
		同步文件HalSync.c
***********************************************************
				彭东
**********************************************************/
#include "BaseType.h"
#include "HalCPU.h"
#include "HalSync.h"

public void HalHalt()
{
    HALT();
}

public void HalCliHalt()
{
    CLIHALT();
}

public void HalStiHalt()
{
    STIHALT();
}

public void HalSti()
{
    STI();
}

public void HalCli()
{
    CLI();
}


public void HalStiFlags(CPUFlg* cpuflg)
{
    HalRestoreFlagsSti(cpuflg);
}

public void HalCliFlags(CPUFlg* cpuflg)
{
    HalSaveFlagsCli(cpuflg);
}

public void HalFlagsSti(CPUFlg* cpuflg)
{
    HalSaveFlagsSti(cpuflg);
}

public void HalFlagsCli(CPUFlg* cpuflg)
{
    HalRestoreFlagsCli(cpuflg);
}

public void SPinLockInit(SPinLock *init)
{
    init->Lock = 0;
    return;
}

public void HalSPinLock(SPinLock *lock)
{
    __asm__ __volatile__(
        "1:         \n"
        "lock; xchg  %0, %1 \n"
        "cmpl   $0, %0      \n"
        "jnz    2f      \n"
        ".section .spinlock.text,"
        "\"ax\""
        "\n"                    // 重新定义一个代码段所以jnz 2f下面并不是
        "2:         \n"         //cmpl $0,%1 事实上下面的代码不会常常执行,
        "cmpl   $0, %1      \n" //这是为了不在cpu指令高速缓存中填充无用代码
        "jne    2b      \n"     //要知道那可是用每位6颗晶体管做的双极性静态
        "jmp    1b      \n"     //储存器,比内存条快几千个数量级
        ".previous      \n"
        :
        : "r"(1), "m"(*lock));
    return;
}

public void HalUnSPinLock(SPinLock *lock)
{
    __asm__ __volatile__(
        "movl   $0, %0\n"
        :
        : "m"(*lock));
    return;
}

public void HalSPinLockSaveFlagsCli(SPinLock *lock, CPUFlg *cpuflg)
{
    __asm__ __volatile__(
        "pushfq             \n\t"
        "cli                \n\t"
        "popq %0            \n\t"

        "1:                 \n\t"
        "lock; xchg  %1, %2 \n\t"
        "cmpl   $0,%1       \n\t"
        "jnz    2f          \n\t"
        ".section .spinlock.text,"
        "\"ax\""
        "\n\t"                    //重新定义一个代码段所以jnz 2f下面并不是
        "2:                 \n\t" //cmpl $0,%1 事实上下面的代码不会常常执行,
        "cmpl   $0,%2       \n\t" //这是为了不在cpu指令高速缓存中填充无用代码
        "jne    2b          \n\t"
        "jmp    1b          \n\t"
        ".previous          \n\t"
        : "=m"(*cpuflg)
        : "r"(1), "m"(*lock));
    return;
}

public void HalUnSPinLockRestoreFlagsSti(SPinLock *lock, CPUFlg *cpuflg)
{
    __asm__ __volatile__(
        "movl   $0, %0\n\t"
        "pushq %1 \n\t"
        "popfq \n\t"
        :
        : "m"(*lock), "m"(*cpuflg));
    return;
}