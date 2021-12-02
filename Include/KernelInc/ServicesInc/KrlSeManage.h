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
#define SETRUNENV(start, cpumode) start##,##cpumode,0,0
#define DEFINE_SERVICE(typename, env, sename) \
__attribute__((section(".service.data"))) ServiceInfo typename = {0,0,0, {0,0,env}, sename}

public Bool KrlSeCreateServiceOnServiceInfo(ServiceInfo* info);
public Bool KrlSeManageInit();
#endif