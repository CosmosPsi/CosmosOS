/**********************************************************
        Cache控制文件HalCache.c
***********************************************************
                彭东
**********************************************************/
#include "BaseType.h"
#include "HalCache.h"
private void CacheWbinvd()
{
	__asm__ __volatile__("wbinvd": : :"memory");
    return;
}

private void CacheInvd()
{
	__asm__ __volatile__("invd": : :"memory");
    return;    
}

private void CacheCLFlush(void* addr)
{
	__asm__ __volatile__("clflush %0" : "+m" (*(volatile char*)addr));
    return;
}

private void CacheEnable()
{
    __asm__ __volatile__(
        "movl %%cr0, %%eax \n\t"
        "btrl $29, %%eax \n\t"       //CR0.NW=0
        "btrl $30, %%eax \n\t"       //CR0.CD=0  CACHE
        "movl %%eax, %%cr0 \n\t"
        :
        :
        :"eax", "memory"
    )
    return;
}

private void CacheDisable()
{
    __asm__ __volatile__(
        "movl %%cr0, %%eax \n\t"
        "btsl $29, %%eax \n\t"       //CR0.NW=0
        "btsl $30, %%eax \n\t"       //CR0.CD=0  CACHE
        "movl %%eax, %%cr0 \n\t"
        :
        :
        :"eax", "memory"
    )
    return;
}


public Bool HalCacheEnable()
{
    CacheEnable();
    return TRUE;
}

public Bool HalCacheDisable()
{
    CacheWbinvd();
    CacheDisable();
    return TRUE;
}

public Bool HalCacheRefresh()
{
    CacheWbinvd();
    return TRUE;
}

public Bool HalCacheRefreshLine(Addr vaddr)
{
    CacheCLFlush((void*)vaddr);
    return TRUE;
}

public Bool HalCacheInvalid()
{
    CacheInvd();
    return TRUE;
}

public Bool HalCacheInvalidLine(Addr vaddr)
{
    return HalCacheRefreshLine(vaddr);
}

public Bool HalCacheInit()
{
    return HalCacheEnable();
}