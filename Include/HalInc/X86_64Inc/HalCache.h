/**********************************************************
        Cache控制头文件HalCache.h
***********************************************************
                彭东
**********************************************************/
#ifndef _HALCACHEHEAD
#define _HALCACHEHEAD
private void CacheWbinvd();
private void CacheInvd();
private void CacheCLFlush(void* addr);
private void CacheEnable();
private void CacheDisable();
public Bool HalCacheEnable();
public Bool HalCacheDisable();
public Bool HalCacheRefresh();
public Bool HalCacheRefreshLine(Addr vaddr);
public Bool HalCacheInvalid();
public Bool HalCacheInvalidLine(Addr vaddr);
public Bool HalCacheInit();
#endif