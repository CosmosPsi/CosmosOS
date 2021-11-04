/**********************************************************
        执行体管理文件KrlExecutorManage.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "HalSync.h"
#include "KrlExecutorManage.h"

private void ELockInit(ELock* init)
{
    IF_NULL_RETURN(init);
    SPinLockInit(&init->Locks);
    return;
}

