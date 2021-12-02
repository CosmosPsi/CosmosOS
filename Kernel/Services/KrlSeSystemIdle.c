/**********************************************************
        系统空转服务文件KrlSeSystemIdle.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "HalCPU.h"
#include "KrlMmManage.h"
#include "KrlExecutorManage.h"
#include "KrlExecutor.h"
#include "KrlTransfer.h"
#include "KrlThread.h"
#include "KrlSeManage.h"
#include "KrlSeSystemIdle.h"

public void SystemIdleMain()
{
    while(TRUE)
    {
        KrlTransfer();
    }
    return;
}

DEFINE_SERVICE(SystemIdle, SETRUNENV(SystemIdleMain, KERNEL_CPU_MODE), "SystemIdleService");