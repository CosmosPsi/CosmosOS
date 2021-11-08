/**********************************************************
        执行体文件KrlExecutor.c
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

private void ExNameInit(ExName* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    return;
}

private void ExResourceBoxInit(ExResourceBox* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    return;
}

private void ExAffiliationInit(ExAffiliation* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    return;
}

private void ThreadHeadInit(ThreadHead* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->ThreadLists);
    return;
}

private void ThreadBoxInit(ThreadBox* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ELockInit(&init->Lock);
    ThreadHeadInit(&init->NewHead);
    ThreadHeadInit(&init->RunHead);
    ThreadHeadInit(&init->SleepHead);
    ThreadHeadInit(&init->BlockHead);
    ThreadHeadInit(&init->WaitHead);
    ThreadHeadInit(&init->DeadHead);
    ThreadHeadInit(&init->CacheHead);
    return;
}

private void ExecutorInit(Executor* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->Lists);
    ELockInit(&init->Lock);
    ExNameInit(&init->Name);
    ExAffiliationInit(&init->Affiliation);
    ThreadBoxInit(&init->ExThreadBox);
    VMSInit(&init->ExVMS);
    ExResourceBoxInit(&init->ResourceBox);
    return;
}