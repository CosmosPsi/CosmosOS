/**********************************************************
        内存池头文件KrlMmPool.h
***********************************************************
                彭东
**********************************************************/

#ifndef _KRLMMPOOLHEAD
#define _KRLMMPOOLHEAD
#define MSCLST_MAX (5)
#define KMPOOL_MAX (64)
#define PMLH_MAX (32)
#define PER_POE_INCSIZE (32)
#define KUC_NEWFLG (1)
#define KUC_DELFLG (2)
#define KUC_DSYFLG (3)

typedef struct POOLPARM
{
    UInt AllocPMSADs;
    UInt POESize;
}PoolParam;

typedef struct PMLHEAD
{
    UInt PmsadNR;
    UInt AllocPMSADNR;
    UInt EndMaxPMSADNR;
    List Lists;
}PMLHead;


typedef struct POENTITIES
{
    List Lists;
    UInt Status;
	void* Start;
}POEntities;

typedef struct KMEMPOOL
{
    List Lists;
	MLock Lock;
    UInt Status;
	UInt Flags;
    UInt Size;
    UInt ObjNR;
    UInt FreeObjNR;
    UInt AllocPMSADNR;
    Addr VAddrStart;
    Addr VAddrEnd;
    List ObjLists;
    List PMSADsLists;
    void* Priv;
	void* Ext;
}KMemPool;

typedef struct KPMSADSPOOL
{
    List Lists;
	MLock Lock;
    UInt Status;
	UInt Flags;
    UInt PmsadsNR;
    PMLHead* PMLHeadCache;
    PMLHead PMLHeadArr[PMLH_MAX];
}KPMSADsPool;

typedef struct GMEMPOOLMANAGE
{
    List Lists;
	MLock Lock;
	UInt Status;
	UInt Flags;
    UInt KMemPoolNR;
    KPMSADsPool PMSADsPool;
    KMemPool* KMemPoolCache;
    KMemPool* KMemPoolArr[KMPOOL_MAX];
}GMemPoolManage;

KLINE GMemPoolManage* KrlMmGetGMemPoolAddr()
{
    return &GMemPoolData;
}

private void PMLHeadInit(PMLHead* init, UInt msadnr, UInt endmsadnr);
private void KPMSADsPoolInit(KPMSADsPool* init);
private void POEntitiesInit(POEntities* init);
private void KMemPoolInit(KMemPool* init);
private void GMemPoolManageInit(GMemPoolManage* init);
private KMemPool* ForSizeRetKMemPoolOnGMemPoolManage(GMemPoolManage* gmpm, Size size);
private PMSAD* ForAddrDelAndRetPMSADOnKPMSADsPool(KPMSADsPool* pool, void* addr);
private KMemPool* ForAddrRetKMemPoolOnGMemPoolManage(GMemPoolManage* gmpm, void* addr);
private PMLHead* ForMsadNrRetPMLHeadOnGMemPoolManage(GMemPoolManage* gmpm, UInt msadnr);
private POEntities* PickPOEntitiesOnKMemPool(KMemPool* pool);
private Bool PutsPOEntitiesOnKMemPool(KMemPool* pool, POEntities* entities);
private UInt POEntitiesArrInitOnMemSPace(KMemPool* pool, Addr start, Addr end);
private Bool ExtendKMemPoolCapacity(GMemPoolManage* gmpm, KMemPool* pool);
private void* NewPOEntitiesOnKMemPool(GMemPoolManage* gmpm, KMemPool* pool, Size size);
private void* NewPMSADsOnKPMSADsPool(GMemPoolManage* gmpm, KPMSADsPool* pool, PMLHead* head, UInt msadnr);
private void* KrlMmNewPMSADsRealizeCore(GMemPoolManage* gmpm, Size size);
private void* KrlMmNewPOEntitiesRealizeCore(GMemPoolManage* gmpm, Size size);
private void* KrlMmNewPMSADsRealize(Size size);
private void* KrlMmNewPOEntitiesRealize(Size size);
private Bool DelPOEntitiesOnKMemPool(GMemPoolManage* gmpm, KMemPool* pool, void* addr);
private Bool DelPMSADsOnKPMSADsPool(KPMSADsPool* pool, PMLHead* head, PMSAD* msad);
private Bool KrlMmDelPMSADsRealizeCore(GMemPoolManage* gmpm, PMSAD* msad, void* addr);
private Bool KrlMmDelPOEntitiesRealizeCore(GMemPoolManage* gmpm, void* addr);
private Bool KrlMmDelPOEntitiesRealize(void* addr);
private UInt CreateNewKMemPoolInit(KMemPool* pool, PMSAD* msad, UInt msadnr, Addr start, Addr end, Size size);
private KMemPool* KrlMmCreateKMemPoolRealize(GMemPoolManage* gmpm, UInt msadnr, Size size);
public KMemPool* KrlMmCreateKMemPool(UInt msadnr, Size size);
public Bool KrlMmDelPOEntities(void* addr);
public Bool KrlMmDel(void* addr);
public void* KrlMmNewPOEntities(Size size);
public void* KrlMmNew(Size size);
public Bool KrlMmPoolInit();
#endif