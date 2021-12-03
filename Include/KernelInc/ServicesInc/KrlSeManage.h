/**********************************************************
        系统服务管理头文件KrlSeManage.h
***********************************************************
                彭东
**********************************************************/
#ifndef _KRLSEMANAGEHEAD
#define _KRLSEMANAGEHEAD

typedef struct SERVICEINFO
{
    UInt  Status;
    UInt  flags;
    UInt  Id;
    TRunEnv Env;
    Char* Name;
}ServiceInfo;

#define SERVICE_IDLE_FLAG (1)
#define SETRUNENV(flags, executor, start, cpumode) 0,flags,executor,start,cpumode,0,0
#define DEFINE_SERVICE(typename, flags, env, sename) \
__attribute__((section(".service.data"))) ServiceInfo typename = {0, flags, 0, {env}, sename}

public Executor* KrlSeCreateServiceOnServiceInfo(ServiceInfo* info);
public Executor* KrlSeCreateSystemIdleService(ServiceInfo* info);
public Bool KrlSeStartAllRunService();
public Bool KrlSeManageInit();
#endif