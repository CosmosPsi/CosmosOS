/**********************************************************
        系统服务管理文件KrlSeManage.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "HalCPU.h"
#include "OSLinker.h"
#include "KrlMmManage.h"
#include "KrlExecutorManage.h"
#include "KrlExecutor.h"
#include "KrlThread.h"
#include "KrlTransfer.h"
#include "KrlSeManage.h"

public Executor* KrlSeCreateServiceOnServiceInfo(ServiceInfo* info)
{
    Executor* executor = NULL;
    Thread* thread = NULL;
    IF_NULL_RETURN_NULL(info);
    if(NULL == info->Env.AffiExecutor)
    {
        executor = KrlExCreateExecutor();
        IF_NULL_RETURN_NULL(executor);
        info->Env.AffiExecutor = executor;
    }
    
    if(KrlExCreateThreadInitRunEnv(&info->Env) == FALSE)
    {
        IF_NEQ_DEAD(TRUE, KrlExDestroyExecutor(executor), "KrlExDestroyExecutor Is Fail\n");
        return NULL;
    }

    info->Id = (UInt)info->Env.AffiExecutor;
    return (Executor*)(info->Env.AffiExecutor);
}

public Executor* KrlSeCreateSystemIdleService(ServiceInfo* info)
{
    Executor* executor = NULL;
    Thread* thread = NULL;
    IF_NULL_RETURN_NULL(info);
    IF_NEQ_RETURN(SERVICE_IDLE_FLAG, info->flags, NULL);

    executor = KrlExCreateExecutor();
    IF_NULL_RETURN_NULL(executor);

    thread = KrlExCreateThread();
    if(NULL == thread)
    {
        IF_NEQ_DEAD(TRUE, KrlExDestroyExecutor(executor), "KrlExDestroyExecutor Is Fail\n");
        return NULL;
    }

    if(KrlExThreadInitRunEnv(thread, &info->Env) == FALSE)
    {
        IF_NEQ_DEAD(TRUE, KrlExDestroyThread(thread), "KrlExDestroyThread Is Fail\n");
        IF_NEQ_DEAD(TRUE, KrlExDestroyExecutor(executor), "KrlExDestroyExecutor Is Fail\n");
        return NULL;
    }

    KrlExThreadSetRunStatus(thread, THREAD_NEW_STATUS);

    if(KrlExThreadAddToExecutor(executor, thread) == FALSE)
    {
        IF_NEQ_DEAD(TRUE, KrlExDestroyThread(thread), "KrlExDestroyThread Is Fail\n");
        IF_NEQ_DEAD(TRUE, KrlExDestroyExecutor(executor), "KrlExDestroyExecutor Is Fail\n");
        return NULL;
    }

    KrlExAddCPUIdleExecutor(executor);
    KrlExSetCurrentCPUIdleTransfer(&thread->ThreadTransfer);

    info->Id = (UInt)executor;
    return executor;
}

public Bool KrlSeStartAllRunService()
{
    ServiceInfo* serinfostart = NULL;
    ServiceInfo* serinfoend = NULL;
    serinfostart = (ServiceInfo*)(&BeginServiceData);
    serinfoend = (ServiceInfo*)(&EndServiceData);
    IF_NULL_RETURN_FALSE(serinfostart);
    IF_NULL_RETURN_FALSE(serinfoend);
    IF_LTN_RETURN((UInt)serinfoend, (UInt)serinfostart, FALSE);
    for(;serinfostart < serinfoend; serinfostart++)
    {
        if(serinfostart->flags == SERVICE_IDLE_FLAG)
        {
            IF_NEQ_DEAD(TRUE, KrlSeCreateSystemIdleService(serinfostart), "KrlSeCreateSystemIdleService Is Fail\n");
            return FALSE;
        }
        else
        {
            IF_NEQ_DEAD(TRUE, KrlSeCreateServiceOnServiceInfo(serinfostart), "KrlSeCreateServiceOnServiceInfo Is Fail\n");
            return FALSE;
        }
    }
    return TRUE;
}

public Bool KrlSeManageInit()
{
    KrlSeStartAllRunService();
    return TRUE;
} 