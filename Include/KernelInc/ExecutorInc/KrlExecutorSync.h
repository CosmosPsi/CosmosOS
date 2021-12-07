/**********************************************************
        执行体同步头文件KrlExecutorSync.h
***********************************************************
                彭东
**********************************************************/
#ifndef _KRLEXECUTORSYNCHEAD
#define _KRLEXECUTORSYNCHEAD

typedef struct EWAITLIST
{
    UInt WaitNR;
    List Lists;
}EWaitList;

typedef struct ESYNC
{
    SPinLock Lock;
    RefCount LockCount;
    EWaitList WaitList;
}ESync;

#endif
