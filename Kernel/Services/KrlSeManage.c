/**********************************************************
        系统服务管理文件KrlSeManage.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "KrlExecutorManage.h"
#include "KrlExecutor.h"
#include "KrlThread.h"
#include "KrlSeManage.h"

public Executor* KrlSeCreateServiceOnServiceInfo(ServiceInfo* info)
{
    Executor* executor = NULL;
    Thread* thread = NULL;
    IF_NULL_RETURN_NULL(info);

    executor = KrlExCreateExecutor();
    IF_NULL_RETURN_NULL(executor);
    thread = KrlExCreateThread();
    if(NULL == thread)
    {
        IF_NEQ_DEAD(TRUE, KrlExDestroyExecutor(executor), "KrlExDestroyExecutor Is Fail\n");
        return NULL;
    }

    if(KrlExThreadAddToExecutor(executor, thread) == FALSE)
    {
        IF_NEQ_DEAD(TRUE, KrlExDestroyThread(thread), "KrlExDestroyThread Is Fail\n");
        IF_NEQ_DEAD(TRUE, KrlExDestroyExecutor(executor), "KrlExDestroyExecutor Is Fail\n");
        return NULL;
    }

    if(KrlExThreadInitRunEnv(thread, &info->Env) == FALSE)
    {
        IF_NEQ_DEAD(TRUE, KrlExThreadDelOnExecutor(executor, thread), "KrlExThreadDelOnExecutor Is Fail\n");
        IF_NEQ_DEAD(TRUE, KrlExDestroyThread(thread), "KrlExDestroyThread Is Fail\n");
        IF_NEQ_DEAD(TRUE, KrlExDestroyExecutor(executor), "KrlExDestroyExecutor Is Fail\n");
        return NULL;
    }

    info->Id = (UInt)executor;
    return executor;
}

public Bool KrlSeStartAllRunService()
{
    ServiceInfo* serinfo = NULL;
}

public Bool KrlSeManageInit()
{

    return TRUE;
} 