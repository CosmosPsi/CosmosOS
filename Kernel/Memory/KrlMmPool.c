/**********************************************************
        内存池文件KrlMmPool.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "KrlMmManage.h"
#include "KrlMmPool.h"
#include "KrlMmAlloc.h"

DefinedMEMData(GMemPoolManage, GMemPoolData);

private void POEntitiesInit(POEntities* init)
{
    IF_NULL_RETURN_NULL(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->Lists);
    return;
}

private void KMemPoolInit(KMemPool* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->Lists);
    ListInit(&init->ObjLists);
    ListInit(&init->PMSADsLists);
    MLockInit(&init->Lock);
    return;
}

private void GMemPoolManageInit(GMemPoolManage* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->Lists);
    MLockInit(&init->Lock);
    KMemPoolInit(&init->KMemPoolMain);
    return;
}

private KMemPool* ForSizeRetKMemPoolOnGMemPoolManage(GMemPoolManage* gmpm, Size size)
{
    IF_NULL_RETURN_NULL(gmpm);
    if(NULL != gmpm->KMemPoolCache)
    {
        if((gmpm->KMemPoolCache->Size > size) && (gmpm->KMemPoolCache->Size < (size + PER_POE_INCSIZE)))
        {
            return gmpm->KMemPoolCache;
        }
    }
    for(UInt i = 0; i < KMPOOL_MAX; i++)
    {
        if(NULL != gmpm->KMemPoolArr[i])
        {
            if((gmpm->KMemPoolArr[i]->Size > size) && (gmpm->KMemPoolCache->Size < (size + PER_POE_INCSIZE)))
            {
                gmpm->KMemPoolCache = &gmpm->KMemPoolArr[i];
                return &gmpm->KMemPoolArr[i];
            }
        }
    }
    return NULL;
}

private POEntities* PickPOEntitiesOnKMemPool(KMemPool* pool)
{
    POEntities* entities = NULL;
    IF_NULL_RETURN_NULL(pool);
    IF_LTNONE_RETRUN_NULL(pool->FreeObjNR);
    IF_LTNONE_RETRUN_NULL(pool->ObjNR);
    IF_EQT_RETURN(TRUE, ListIsEmptyCareful(&pool->ObjLists), NULL);
    entities = ListFirstOne(&pool->ObjLists, POEntities, Lists);
    ListDel(&entities->Lists);
    pool->FreeObjNR--;
    return entities;
}

public Bool KrlMmPoolInit()
{
    GMemPoolManageInit(&GMemPoolData);
    return TRUE;
}