/**********************************************************
        执行体资源文件KrlExecutorRes.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "HalSync.h"
#include "KrlMmManage.h"
#include "KrlMmPool.h"
#include "KrlMmVmem.h"
#include "KrlExecutorManage.h"
#include "KrlExecutor.h"
#include "KrlExecutorRes.h"

DefinedExecutorData(ERM, ERMData);

private void ERMInit(ERM* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ELockInit(&init->Lock);
    ListInit(&init->Lists);
    ListInit(&init->ERHead);
    ListInit(&init->ERCacheHead);
    init->ERCacheMin = ER_CACHE_MIN;
    init->ERCacheMax = ER_CACHE_MAX;
    return;
}

private void ExecutorResInit(ExecutorRes* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->Lists);
    ELockInit(&init->Lock);
    RefCountInit(&init->Count);
    return;
}

private ExecutorRes* NewExecutorRes()
{
    ExecutorRes* res = NULL;
    res = (ExecutorRes*)KrlMmNew(sizeof(ExecutorRes));
    IF_NULL_RETURN_NULL(res);
    ExecutorResInit(res);
    return res;
}

private Bool DelExecutorRes(ExecutorRes* res)
{
    IF_NULL_RETURN_FALSE(res);
	
	return KrlMmDel((void*)res);
}

public ExecutorRes* KrlExGetExecutorRes()
{
    ExecutorRes* res = NULL;
	ERM* resmgr = NULL;
    
    resmgr = KrlExGetERMDataAddr();
    IF_NULL_RETURN_NULL(resmgr);

	KrlExLocked(&resmgr->Lock);
	if(0 < resmgr->ERCacheNR)
	{
		if(ListIsEmptyCareful(&resmgr->ERCacheHead) == FALSE)
		{
			res = ListFirstOne(&resmgr->ERCacheHead, ExecutorRes, Lists);
			ListDel(&res->Lists);
			resmgr->ERCacheNR--;
			
			ExecutorResInit(res);
			ListAdd(&res->Lists, &resmgr->ERHead);
			resmgr->ERNR++;
			RefCountInc(&res->Count);
			//res = res;
			goto out; 
		}
	}

	res = NewExecutorRes();
	if(NULL == res)
	{
		res = NULL;
		goto out;
	}

	ListAdd(&res->Lists, &resmgr->ERHead);
	resmgr->ERNR++;
	RefCountInc(&res->Count);

out:
	KrlExUnLock(&resmgr->Lock);	
	return res;
}

public Bool KrlExecutorResInit()
{
    ERM* erm = NULL;
    erm = KrlExGetERMDataAddr();
    IF_NULL_RETURN_FALSE(erm);
    ERMInit(erm);
    return TRUE;
}