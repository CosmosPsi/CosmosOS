/**********************************************************
        内存测试单元文件MemTestUnit.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "HalSync.h"
#include "HalCPU.h"
#include "HalBoot.h"
#include "HalInterface.h"
#include "KrlLog.h"
#include "KrlMmManage.h"
#include "MemTestUnit.h"


public Bool MemGMemManageTest()
{
    GMemManage* gmm = NULL;
    gmm = KrlMmGetGMemManageAddr();
    IF_NULL_DEAD(gmm);
    IF_GTN_DEAD(1, gmm->FreePMSAD, "GMemManage:FreePMSAD < 1\n");
    IF_GTN_DEAD(1, gmm->MaxPMSAD, "GMemManage:MaxPMSAD < 1\n");
    IF_NEQ_DEAD(gmm->MemroySZ, (gmm->MaxPMSAD << MSAD_PADR_SLBITS), "GMemManage:MemorySZ data error\n");
    IF_GTN_DEAD(1, gmm->PAreaNR,"GMemManage:PAreaNR < 1\n");
    IF_EQT_DEAD(NULL, gmm->PAreaStart, "GMemManage:PAreaStart = NULL\n");
    IF_GTN_DEAD(1, gmm->MNodeNR,"GMemManage:MNodeNR < 1\n");
    IF_EQT_DEAD(NULL, gmm->MNodeStart, "GMemManage:MNodeStart = NULL\n");
    return TRUE;
}

public Bool MemTestUnit()
{
    return TRUE;
}