/**********************************************************
        内存池文件KrlMmPool.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "KrlMmManage.h"
#include "KrlMmAlloc.h"
#include "KrlMmPool.h"

DefinedMEMData(GMemPoolManage, GMemPoolData);
MEMDATA_SECTION PoolParam PoolParamArr[KMPOOL_MAX] = {
    {2, 32}, {2, 64}, {2, 96}, {2, 128}, 
    {2, 160}, {2, 192}, {2, 224}, {2, 256}, 
    {2, 288}, {2, 320}, {2, 352}, {2, 384}, 
    {4, 416}, {4, 448}, {4, 480}, {4, 512}, 
    {4, 544}, {4, 576}, {4, 608}, {4, 640}, 
    {4, 672}, {4, 704}, {4, 736}, {4, 768}, 
    {4, 800}, {4, 832}, {4, 864}, {4, 896}, 
    {8, 928}, {8, 960}, {8, 992}, {8, 1024}, 
    {8, 1056}, {8, 1088}, {8, 1120}, {8, 1152}, 
    {8, 1184}, {8, 1216}, {8, 1248}, {8, 1280}, 
    {8, 1312}, {8, 1344}, {8, 1376}, {8, 1408}, 
    {8, 1440}, {8, 1472}, {8, 1504}, {8, 1536}, 
    {8, 1568}, {8, 1600}, {8, 1632}, {8, 1664}, 
    {8, 1696}, {8, 1728}, {8, 1760}, {8, 1796}, 
    {16, 1824}, {16, 1856}, {16, 1888}, {16, 1920}, 
    {16, 1952}, {16, 1984}, {16, 2016}, {16, 2048}, 
};

private void PMLHeadInit(PMLHead* init, UInt msadnr, UInt endmsadnr)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    init->AllocPMSADNR = msadnr;
    init->EndMaxPMSADNR = endmsadnr;
    ListInit(&init->Lists);
    return;
}

private void KPMSADsPoolInit(KPMSADsPool* init)
{
    IF_NULL_RETURN(init);
    INIT_OBJOFPTR_ZERO(init);
    ListInit(&init->Lists);
    MLockInit(&init->Lock);
    for(UInt i = 0; i < PMLH_MAX; i++)
    {
        PMLHeadInit(&init->PMLHeadArr[i], (1 << i), ((1 << (i + 1)) - 1));    
    }
    return;
}

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
    KPMSADsPoolInit(&init->PMSADsPool);
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

private PMSAD* ForAddrDelAndRetPMSADOnKPMSADsPool(KPMSADsPool* pool, void* addr)
{
    PMLHead* head = NULL;
    List* lists = NULL;
    PMSAD* msad = NULL;

    IF_NULL_RETURN_NULL(pool);
    IF_NULL_RETURN_NULL(addr);
    for(UInt i = 0; i < PMLH_MAX; i++)
    {
        head = &pool->PMLHeadArr[i];
        ListForEach(lists, &head->Lists)
        {
            msad = ListEntry(lists, PMSAD, Lists);
            if(addr == (void*)PMSADRetVAddr(msad))
            {
                ListDel(&msad->Lists);
                head->PmsadNR--;
                return msad;
            }
        }
    }
    return NULL;
}

private KMemPool* ForAddrRetKMemPoolOnGMemPoolManage(GMemPoolManage* gmpm, void* addr)
{
    List* lists = NULL;
    PMSAD* msad = NULL;
    KMemPool* pool = NULL;
    Addr rangestart = NULL; 
    Addr rangeend = NULL; 
    IF_NULL_RETURN_NULL(gmpm);
    if(NULL != gmpm->KMemPoolCache)
    {
        pool = gmpm->KMemPoolCache;
        if((pool->VAddrStart <= (Addr)addr) && ((Addr)addr < pool->VAddrEnd))
        {
            ListForEach(lists, &pool->PMSADsLists)
            {
                msad = ListEntry(lists, PMSAD, Lists);
                rangestart = KrlMmGetPMSADsRangeVStart(msad);
                rangeend = KrlMmGetPMSADsRangeVEnd(msad);
                if((rangestart <= (Addr)addr) && ((Addr)addr < rangeend))
                {
                    return pool;
                }
            }
        }
        
    }
    for(UInt i = 0; i < KMPOOL_MAX; i++)
    {
        if(NULL != gmpm->KMemPoolArr[i])
        {
            pool = gmpm->KMemPoolArr[i];
            if ((pool->VAddrStart <= (Addr)addr) && ((Addr)addr < pool->VAddrEnd))
            {
                ListForEach(lists, &pool->PMSADsLists)
                {
                    msad = ListEntry(lists, PMSAD, Lists);
                    rangestart = KrlMmGetPMSADsRangeVStart(msad);
                    rangeend = KrlMmGetPMSADsRangeVEnd(msad);
                    if((rangestart <= (Addr)addr) && ((Addr)addr < rangeend))
                    {
                        return pool;
                    }
                }
            }
        }
    }
    return NULL;
}

private PMLHead* ForMsadNrRetPMLHeadOnGMemPoolManage(GMemPoolManage* gmpm, UInt msadnr)
{
    PMLHead* head = NULL;
    KPMSADsPool* pool = NULL;
    IF_NULL_RETURN_NULL(gmpm);
    pool = &gmpm->PMSADsPool;
    for(UInt i = 0; i < PMLH_MAX; i++)
    {
        head = &pool->PMLHeadArr[i];
        if((msadnr <= head->AllocPMSADNR))
        {
            return head;
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

private Bool PutsPOEntitiesOnKMemPool(KMemPool* pool, POEntities* entities)
{
    IF_NULL_RETURN_FALSE(pool);
    IF_NULL_RETURN_FALSE(entities);
    POEntitiesInit(entities);
    ListAdd(&entities->Lists, &pool->ObjLists);
    pool->FreeObjNR++;
    return TRUE;
}

private UInt POEntitiesArrInitOnMemSPace(KMemPool* pool, Addr start, Addr end)
{
    POEntities* entstart = NULL;
    POEntities* entend = NULL;
    UInt i = 0;

    entstart = (POEntities*)start;
    entend = (POEntities*)end;
    for(; entstart < entend; i++)
    {
        POEntitiesInit(entstart);
        ListAdd(&entstart->Lists, &pool->ObjLists);
        pool->ObjNR++;
        pool->FreeObjNR++;
        entstart = (POEntities*)(((UInt)entstart) + ((UInt)pool->Size));
    }
    IF_EQT_RETURN(0, i, 0);
    if(start < pool->VAddrStart)
    {
        pool->VAddrStart = start;
    }
    if(end > pool->VAddrEnd)
    {
        pool->VAddrEnd = end;
    }
    return i;
}

private Bool ExtendKMemPoolCapacity(GMemPoolManage* gmpm, KMemPool* pool)
{
    PMSAD* msad = NULL;
    Addr start = NULL; 
    Addr end = NULL;
    Bool rets = FALSE;
    UInt nr = 0; 
    IF_NULL_RETURN_FALSE(gmpm);
    IF_NULL_RETURN_FALSE(pool);
    msad = KrlMmAllocKernPMSADs(pool->AllocPMSADNR);
    IF_NULL_RETURN_FALSE(msad);

    start = PMSADRetVAddr(msad);
    end = start + (Addr)(KrlMmGetPMSADsSize(msad) - 1);
    SetPMSADKMPool(msad);

    nr = POEntitiesArrInitOnMemSPace(pool, start, end);
    if(1 > nr)
    {
        rets = KrlMmFreeKernPMSADs(msad);
        IF_EQT_DEAD(FALSE, rets, "KrlMmFreeKernPMSADs is Fail\n");
        return FALSE;
    }
    return TRUE;
}

private void* NewPOEntitiesOnKMemPool(GMemPoolManage* gmpm, KMemPool* pool, Size size)
{
    POEntities* entities = NULL;
    Bool rets = FALSE;
    IF_NULL_RETURN_NULL(gmpm);
    IF_NULL_RETURN_NULL(pool);
    IF_LTN_RETURN(pool->Size, size, NULL);
    IF_GTN_RETURN(pool->Size, (size + PER_POE_INCSIZE), NULL);
    entities = PickPOEntitiesOnKMemPool(pool);
    if(NULL != entities)
    {
        INIT_OBJOFPTR_ZERO(entities);
        return (void*)entities;
    }
    rets = ExtendKMemPoolCapacity(gmpm, pool);
    IF_NEQ_RETURN(TRUE, rets, NULL);
    return (void*)PickPOEntitiesOnKMemPool(pool);
}

private void* NewPMSADsOnKPMSADsPool(GMemPoolManage* gmpm, KPMSADsPool* pool, PMLHead* head, UInt msadnr)
{
    PMSAD* msad = NULL;
    IF_NULL_RETURN_NULL(gmpm);
    IF_NULL_RETURN_NULL(pool);
    IF_NULL_RETURN_NULL(head);
    IF_ZERO_RETURN_NULL(msadnr);
    msad = KrlMmAllocKernPMSADs(msadnr);
    IF_NULL_RETURN_NULL(msad);
    ListAdd(&msad->Lists, &head->Lists);
    head->PmsadNR++;
    pool->PmsadsNR++;
    return (void*)PMSADRetVAddr(msad);
}

private void* KrlMmNewPMSADsRealizeCore(GMemPoolManage* gmpm, Size size)
{
    void* addr = NULL;
    KPMSADsPool* pool = NULL;
    PMLHead* head = NULL;
    UInt msadnr = 0;
    IF_NULL_RETURN_NULL(gmpm);
    msadnr = (MSAD_ALIGN(size) >> MSAD_PADR_SLBITS);
    pool = &gmpm->PMSADsPool;
    head = ForMsadNrRetPMLHeadOnGMemPoolManage(gmpm, msadnr);
    IF_NULL_RETURN_NULL(head);
    
    KrlMmLocked(&pool->Lock);
    addr = NewPMSADsOnKPMSADsPool(gmpm, pool, head, msadnr);
    KrlMmUnLock(&pool->Lock);
    return addr;
}

private void* KrlMmNewPOEntitiesRealizeCore(GMemPoolManage* gmpm, Size size)
{
    void* addr = NULL;
    KMemPool* pool = NULL;
    IF_NULL_RETURN_NULL(gmpm);

    pool = ForSizeRetKMemPoolOnGMemPoolManage(gmpm, size);
    IF_NULL_RETURN_NULL(pool);
    
    KrlMmLocked(&pool->Lock);
    addr = NewPOEntitiesOnKMemPool(gmpm, pool, size);
    KrlMmUnLock(&pool->Lock);
    return addr;
}

private void* KrlMmNewPMSADsRealize(Size size)
{
    void* addr = NULL;
    GMemPoolManage* gmpm = NULL;
    
    gmpm = KrlMmGetGMemPoolAddr();
    IF_NULL_RETURN_NULL(gmpm);

    KrlMmLocked(&gmpm->Lock);
    addr = KrlMmNewPMSADsRealizeCore(gmpm, size);
    KrlMmUnLock(&gmpm->Lock);
    return addr;
}

private void* KrlMmNewPOEntitiesRealize(Size size)
{
    void* addr = NULL;
    GMemPoolManage* gmpm = NULL;
    
    gmpm = KrlMmGetGMemPoolAddr();
    IF_NULL_RETURN_NULL(gmpm);

    KrlMmLocked(&gmpm->Lock);
    addr = KrlMmNewPOEntitiesRealizeCore(gmpm, size);
    KrlMmUnLock(&gmpm->Lock);
    return addr;
}

private Bool DelPOEntitiesOnKMemPool(GMemPoolManage* gmpm, KMemPool* pool, void* addr)
{
    Bool rets = FALSE;
    IF_NULL_RETURN_FALSE(gmpm);
    IF_NULL_RETURN_FALSE(pool);
    IF_NULL_RETURN_FALSE(addr);

    rets = PutsPOEntitiesOnKMemPool(pool, (POEntities*)addr);
    return rets;
}

private Bool DelPMSADsOnKPMSADsPool(KPMSADsPool* pool, PMLHead* head, PMSAD* msad)
{
    Bool rets = FALSE;
    List* lists = NULL;
    PMSAD* tmpmsad = NULL;

    IF_NULL_RETURN_FALSE(pool);
    IF_NULL_RETURN_FALSE(head);
    IF_NULL_RETURN_FALSE(msad);
    KrlMmLocked(&pool->Lock);
    ListForEach(lists, &head->Lists)
    {
        tmpmsad = ListEntry(lists, PMSAD, Lists);
        if(tmpmsad == msad)
        {
            ListDel(&msad->Lists);
            head->PmsadNR--;
            rets = KrlMmFreeKernPMSADs(msad);
            IF_NEQ_DEAD(TRUE, rets, "KrlMmFreeKernPMSADs Called Fail\n");
            if(pool->PmsadsNR > 0)
            {
                pool->PmsadsNR--;
            }
            KrlMmUnLock(&pool->Lock);
            return rets;
        }
    }
    KrlMmUnLock(&pool->Lock);
    return FALSE;
}

private Bool KrlMmDelPMSADsRealizeCore(GMemPoolManage* gmpm, PMSAD* msad, void* addr)
{
    Bool rets = FALSE;
    UInt msadnr = 0;
    PMLHead* head = NULL;
    IF_NULL_RETURN_FALSE(gmpm);
    IF_NULL_RETURN_FALSE(msad);

    msadnr = (UInt)KrlMmGetPMSADsLen(msad);
    IF_ZERO_RETURN_FALSE(msadnr);
    IF_NEQ_RETURN(FALSE, PMSADIsKMPool(msad), FALSE);
    IF_NEQ_DEAD(addr, (void*)PMSADRetVAddr(msad), "addr is not EQ PMSAD Addr\n");
    head = ForMsadNrRetPMLHeadOnGMemPoolManage(gmpm, msadnr);
    IF_NULL_RETURN_FALSE(head);
    return DelPMSADsOnKPMSADsPool(&gmpm->PMSADsPool, head, msad);
}

private Bool KrlMmDelPOEntitiesRealizeCore(GMemPoolManage* gmpm, void* addr)
{
    Bool rets = FALSE;
    KMemPool* pool = NULL;
    IF_NULL_RETURN_FALSE(gmpm);

    pool = ForAddrRetKMemPoolOnGMemPoolManage(gmpm, addr);
    IF_NULL_RETURN_FALSE(pool);
    
    KrlMmLocked(&pool->Lock);
    rets = DelPOEntitiesOnKMemPool(gmpm, pool, addr);
    KrlMmUnLock(&pool->Lock);
    return rets;
}

private Bool KrlMmDelPOEntitiesRealize(void* addr)
{
    Bool rets = FALSE;
    GMemPoolManage* gmpm = NULL;
    PMSAD* msad = NULL;

    gmpm = KrlMmGetGMemPoolAddr();
    IF_NULL_RETURN_FALSE(gmpm);

    KrlMmLocked(&gmpm->Lock);
    
    msad = KrlMmGetPMSADOnVaddr((Addr)addr);
    if(msad != NULL)
    {
        if(PMSADIsKMPool(msad) != TRUE)
        {
            rets = KrlMmDelPMSADsRealizeCore(gmpm, msad, addr);
            KrlMmUnLock(&gmpm->Lock);
            return rets;
        }
        else
        {
            rets = KrlMmDelPOEntitiesRealizeCore(gmpm, addr);
            KrlMmUnLock(&gmpm->Lock);
            return rets;
        }
    }

    rets = KrlMmDelPOEntitiesRealizeCore(gmpm, addr);
    KrlMmUnLock(&gmpm->Lock);
    return rets;
}

private UInt CreateNewKMemPoolInit(KMemPool* pool, PMSAD* msad, UInt msadnr, Addr start, Addr end, Size size)
{
    POEntities* entstart = NULL;
    POEntities* entend = NULL;
    KMemPool* tmp = 0;
    UInt i = 0;
    tmp = pool + 1;
    entstart = (POEntities*)tmp;
    entend = (POEntities*)end;
    
    for(; entstart < entend; i++)
    {
        POEntitiesInit(entstart);
        ListAdd(&entstart->Lists, &pool->ObjLists);
        pool->ObjNR++;
        pool->FreeObjNR++;
        entstart = (POEntities*)(((UInt)entstart) + ((UInt)pool->Size));
    }

    IF_EQT_RETURN(0, i, 0);

    pool->Size = size;
    pool->AllocPMSADNR = msadnr;
    pool->VAddrStart = start;
    pool->VAddrEnd = end;
    SetPMSADKMPool(msad);
    ListAdd(&msad->Lists, &pool->PMSADsLists);
    return i;
}

private KMemPool* KrlMmCreateKMemPoolRealize(GMemPoolManage* gmpm, UInt msadnr, Size size)
{
    KMemPool* pool = NULL;
    PMSAD* msad = NULL;
    Bool rets = FALSE;
    IF_NULL_RETURN_NULL(gmpm);
    msad = KrlMmAllocKernPMSADs(msadnr);
    IF_NULL_RETURN_NULL(msad);
    pool = (KMemPool*)PMSADRetVAddr(msad);
    IF_NULL_RETURN_NULL(pool);
    KMemPoolInit(pool);
    if(1 > CreateNewKMemPoolInit(pool, msad, msadnr, (Addr)pool, ((Addr)pool + (Addr)KrlMmGetPMSADsSize(msad) - 1), size))
    {
        rets = KrlMmFreeKernPMSADs(msad);
        IF_EQT_DEAD(FALSE, rets, "KrlMmFreeKernPMSADs is Fail\n");
        return NULL;
    }
    return pool;
}

public KMemPool* KrlMmCreateKMemPool(UInt msadnr, Size size)
{
    GMemPoolManage* gmpm;
    IF_LTNONE_RETRUN_NULL(msadnr);
    IF_LTNONE_RETRUN_NULL(size);
    IF_GTN_RETURN(size, 2048, NULL);
    gmpm = KrlMmGetGMemPoolAddr();
    IF_NULL_RETURN_NULL(gmpm);
    return KrlMmCreateKMemPoolRealize(gmpm, msadnr, size);
}

public Bool KrlMmDelPOEntities(void* addr)
{
    IF_NULL_RETURN_FALSE(addr);
    return KrlMmDelPOEntitiesRealize(addr);
}

public Bool KrlMmDel(void* addr)
{
    return KrlMmDelPOEntities(addr);
}

public void* KrlMmNewPOEntities(Size size)
{
    IF_LTNONE_RETRUN_NULL(size);
    if(size > 2048);
    {
        return KrlMmNewPMSADsRealize(size);
    }
    return KrlMmNewPOEntitiesRealize(size);
}

public void* KrlMmNew(Size size)
{
    return KrlMmNewPOEntities(size);
}

public Bool KrlMmPoolInit()
{
    GMemPoolManage* gmpm = NULL;
    KMemPool* pool = NULL;
    gmpm = KrlMmGetGMemPoolAddr();
    IF_NULL_RETURN_FALSE(gmpm);
    GMemPoolManageInit(gmpm);
    for(UInt i = 0; i < KMPOOL_MAX; i++)
    {
        pool = KrlMmCreateKMemPool(PoolParamArr[i].AllocPMSADs, PoolParamArr[i].POESize);
        if(NULL != pool)
        {
            gmpm->KMemPoolArr[i] = pool;
            gmpm->KMemPoolNR++;
        }
    }   
    return TRUE;
}