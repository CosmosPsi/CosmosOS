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

public Bool KrlSeManageInit();

#endif