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

public Bool MemPHYMSPaceAreaTest()
{
    MachStartInfo* msinfo = NULL;
    GMemManage* gmm = NULL;
    PHYMSPaceArea* parea = NULL;
    E820Map* e820 = NULL;
    UInt status = 0;
    msinfo = HalExPGetMachStartInfoAddr();
    gmm = KrlMmGetGMemManageAddr();
    IF_NULL_DEAD(msinfo);
    IF_NULL_DEAD(gmm);
    
    e820 = (E820Map*)HalPAddrToVAddr((Addr)msinfo->E820PAddr);
    IF_EQT_DEAD(NULL, e820, "MachStartInfo:E820PAddr = NULL\n");
    IF_GTN_DEAD(1, msinfo->E820NR, "MachStartInfo:E820NR < 1\n");
    
    parea = gmm->PAreaStart;
    IF_GTN_DEAD(1, gmm->PAreaNR,"GMemManage:PAreaNR < 1\n");
    IF_EQT_DEAD(NULL, parea, "GMemManage:PAreaStart = NULL\n");
    for(U64 i = 0; i < gmm->PAreaNR; i++)
    {
        status = 0;
        for(U64 e = 0; e < msinfo->E820NR; e++)
        {
            if((parea[i].SubType == e820[e].Type) && (parea[i].Start == e820[e].Addr) &&
                    (parea[i].End == (e820[e].Size + e820[e].Addr - 1)) && 
                    (parea[i].Size == e820[e].Size))
            {
                status++;
            }
        }
        IF_NEQ_DEAD(1, status, "No have E820Map EQ\n");
        IF_NEQ_DEAD(PMSA_F_X86_64, parea[i].Flags, "PHYMSPaceArea:Flags != PMSA_F_X86_64\n");
    }
    
    return TRUE;
}

public Bool MemMNodeTest()
{
    GMemManage* gmm = NULL;
    MNode* node = NULL;
    
    gmm = KrlMmGetGMemManageAddr();
    IF_NULL_DEAD(gmm);

    node = gmm->MNodeStart;
    IF_EQT_DEAD(NULL, node, "GMemManage:MNodeStart = NULL\n");
    IF_GTN_DEAD(1, gmm->MNodeNR, "GMemManage:MNodeNR < 1\n");

    for(U64 i = 0; i < gmm->MNodeNR; i++)
    {
        IF_EQT_DEAD(0, node[i].NodeMemAddrEnd, "MNode: NodeMemAddrEnd = 0");
    }
    return TRUE;
}

public Bool MemTestUnit()
{
    return TRUE;
}