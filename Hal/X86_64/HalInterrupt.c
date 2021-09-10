/**********************************************************
        hal层中断处理文件halInterrupt.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "HalInterrupt.h"

DefinedCPUInterData(IERSDT, IERSDTable);

private Bool IERSDInit(IERSD* init, U32 flags, U32 status, UInt prity, UInt internr)
{
    if(NULL == init)
    {
        return FALSE;
    }
    HalSPinLockInit(&init->Lock);
    init->Flags = flags;
    init->Status = status;
    init->Prity = prity;
    init->InterNR = internr;
    init->Deep = 0;
    init->Count = 0;
    ListInit(&init->IODLists);
    init->IODNR = 0;
    ListInit(&init->IODThreadLists);
    init->IODThreadNR = 0;
    init->OneIODThread = NULL;
    init->RBTreeRoot = NULL;
    init->MSGMPool = NULL;
    init->Priv = NULL;
    init->Ext = NULL;
    return TRUE;
}

private Bool IERSDTInit()
{
    for(UInt i = 0; i < IERSD_MAX; i++)
    {
       IERSDInit(&IERSDTable.IERSDArr[i], 0, 0, 0, 0);
    }
    return TRUE;
}

private Bool IODInit(IOD* init, U32 flags, IERSD* parent, void* device, IODCallBack callbackfunc)
{
    if(NULL == init)
    {
        return;
    }
    ListInit(&init->Lists);
    ListInit(&init->InDeviceLists);
    init->Flags = flags;
    init->Parent = parent;
    init->Device = device;
    init->Count = 0;
    init->CallBackFunc = callbackfunc;
    return TRUE;
}


public void HalInterDistributor(UInt internr, void* kistacktop)
{
    return;
}

public void HalFaultDistributor(UInt faultnr, void* kfstacktop)
{
    return;
}

public void HalOSAPIDistributor(UInt apinr, void* kistacktop)
{
    return;
}

public Bool HalInterruptInit()
{
    IERSDTInit();
    return TRUE;        
}