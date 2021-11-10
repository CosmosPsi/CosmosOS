/**********************************************************
        执行体资源头文件KrlExecutorRes.h
***********************************************************
                彭东
**********************************************************/
#ifndef _KRLEXECUTORRESHEAD
#define _KRLEXECUTORRESHEAD

#define EXRES_NR_MAX (64)
#define ER_CACHE_MAX (0x200)
#define ER_CACHE_MIN (0x40)
// Executor Resource Manager
typedef struct ERM 
{
	List Lists;
	ELock Lock;
	UInt Status;
	UInt Flags;
	UInt ERNR;
	List ERHead;
	UInt ERCacheNR;
	UInt ERCacheMax;
	UInt ERCacheMin;
	List ERCacheHead;
	void* Ext;
}ERM;

typedef struct EXECUTORRES
{
    List Lists;
    ELock Lock;
	U64 Status;
	U64 Flags;
    UInt CurrResIndex;
    UInt NextResIndex;
    void* ResHandArr[EXRES_NR_MAX];
}ExecutorRes;

KLINE ERM* KrlExGetERMDataAddr()
{
    return &ERMData;
}

private void ERMInit(ERM* init);
private void ExecutorResInit(ExecutorRes* init);
public Bool KrlExecutorResInit();

#endif