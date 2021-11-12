/**********************************************************
        线程文件KrlThread.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "RBTree.h"
#include "HalSync.h"
#include "KrlMmManage.h"
#include "KrlMmPool.h"
#include "KrlMmVmem.h"
#include "KrlExecutorManage.h"
#include "KrlExecutor.h"
#include "KrlExecutorRes.h"
#include "KrlTransfer.h"
#include "KrlThread.h"

private void TAffiliationInit(TAffiliation* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    return;
}

private void TContextInit(TContext* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    return;
}

private void ThreadInit(Thread* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->Lists);
    ELockInit(&init->Lock);
    TAffiliationInit(&init->Affiliation);
    TransferInitForThread(&init->ThreadTransfer, init);
    TContextInit(&init->ThreadContext);
    return;
}

private Bool DelThread(Thread* thread)
{
    IF_NULL_RETURN_FALSE(thread);
    return KrlMmDel((void*)thread);
}

private Thread* NewThread()
{
    Thread* thread = NULL;
    thread = (Thread*)KrlMmNew(sizeof(Thread));
    IF_NULL_RETURN_NULL(thread);
    ThreadInit(thread);
    return thread;
}