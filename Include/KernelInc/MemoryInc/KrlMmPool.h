/**********************************************************
        内存池头文件KrlMmPool.h
***********************************************************
                彭东
**********************************************************/

#ifndef _KRLMMPOOLHEAD
#define _KRLMMPOOLHEAD
#define MSCLST_MAX (5)
#define KMPOOL_MAX (64)
#define PER_POE_INCSIZE (32)
#define KUC_NEWFLG (1)
#define KUC_DELFLG (2)
#define KUC_DSYFLG (3)

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

typedef struct GMEMPOOLMANAGE
{
    List Lists;
	MLock Lock;
	UInt Status;
	UInt Flags;
    UInt KMemPoolNR;
    KMemPool KMemPoolMain;
    KMemPool* KMemPoolCache;
    KMemPool* KMemPoolArr[KMPOOL_MAX];
}GMemPoolManage;

KLINE GMemPoolManage* KrlMmGetGMemPoolAddr()
{
    return &GMemPoolData;
}

private void POEntitiesInit(POEntities* init);
private void KMemPoolInit(KMemPool* init);
private void GMemPoolManageInit(GMemPoolManage* init);
private KMemPool* ForSizeRetKMemPoolOnGMemPoolManage(GMemPoolManage* gmpm, Size size);
private POEntities* PickPOEntitiesOnKMemPool(KMemPool* pool);

public Bool KrlMmPoolInit();
#endif