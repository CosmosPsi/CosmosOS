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
    UInt  Flags;
    UInt  Id;
    TRunEnv Env;
    Char* Name;
}ServiceInfo;

#define SERVICE_IDLE_FLAG (1)
#define SETRUNENV(flags, executor, start, cpumode) 0,flags,executor,start,cpumode,0,0

#define KERNELTRUNENV(start) SETRUNENV(0, NULL, start, KERNEL_CPU_MODE)
#define USERTRUNENV(start) SETRUNENV(0, NULL, start, USER_CPU_MODE)

#define DEFINE_SERVICE(typename, flags, env, sename) \
__attribute__((section(".service.data"))) ServiceInfo typename = {0, flags, 0, {env}, "##sename##"}

#define DEFINE_SYSIDLE_SERVICE(start, sename) DEFINE_SERVICE(ServiceObj##sename, SERVICE_IDLE_FLAG, KERNELTRUNENV(start), sename)
#define DEFINE_KERNEL_SERVICE(start, sename) DEFINE_SERVICE(ServiceObj##sename, 0, KERNELTRUNENV(start), sename)
#define DEFINE_USER_SERVICE(start, sename) DEFINE_SERVICE(ServiceObj##sename, 0, USERTRUNENV(start), sename)
public Executor* KrlSeCreateServiceOnServiceInfo(ServiceInfo* info);
public Executor* KrlSeCreateSystemIdleService(ServiceInfo* info);
public Bool KrlSeStartAllRunService();
public Bool KrlSeManageInit();
#endif