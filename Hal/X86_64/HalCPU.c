/**********************************************************
        CPU相关的文件HalCPU.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "HalCPU.h"

public Addr HalVAddrToPAddr(Addr kvaddr)
{
    if (kvaddr < KRNL_MAP_VIRTADDRESS_START || kvaddr > KRNL_MAP_VIRTADDRESS_END)
    {
        // system_error("virAddro_phyadr err\n");
        return KRNL_ADDR_ERROR;
    }
    return kvaddr - KRNL_MAP_VIRTADDRESS_START;
}

public Addr HalPAddrToVAddr(Addr kpaddr)
{
    if (kpaddr >= KRNL_MAP_PHYADDRESS_END)
    {
        // system_error("phyAddro_viradr err\n");
        return KRNL_ADDR_ERROR;
    }
    return kpaddr + KRNL_MAP_VIRTADDRESS_START;
}