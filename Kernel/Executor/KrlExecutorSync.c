/**********************************************************
        执行体同步文件KrlExecutorSync.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "Atomic.h"
#include "List.h"
#include "HalSync.h"
#include "KrlExecutorSync.h"

public void EWaitListInit(EWaitList* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->Lists);
    return;
}