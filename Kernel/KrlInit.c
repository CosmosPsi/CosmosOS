/****************************************************************
        Kernel层初始化头文件KrlInit.h
*****************************************************************
                彭东 
****************************************************************/
#include "BaseType.h"
#include "List.h"
#include "KrlMmManage.h"
#include "KrlMmPool.h"
#include "KrlMmVmem.h"
#include "KrlExecutorManage.h"
#include "KrlTransfer.h"
#include "KrlSeManage.h"
#include "KrlInit.h"

public void KrlCoreInit()
{
    IF_NEQ_DEAD(TRUE, KrlMmManageInit(), "KrlMmManageInit Is Fail\n");
    IF_NEQ_DEAD(TRUE, KrlMmPoolInit(), "KrlMmPoolInit Is Fail\n");
    IF_NEQ_DEAD(TRUE, KrlMmVMemInit(), "KrlMmVMemInit Is Fail\n");
    IF_NEQ_DEAD(TRUE, KrlExecutorManageInit(), "KrlExecutorManageInit Is Fail\n");
    IF_NEQ_DEAD(TRUE, KrlTransferInit(), "KrlTransferInit Is Fail\n");
    IF_NEQ_DEAD(TRUE, KrlSeManageInit(), "KrlSeManageInit Is Fail\n");
    return;
}