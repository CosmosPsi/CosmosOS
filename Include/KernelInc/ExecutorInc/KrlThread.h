/**********************************************************
        线程头文件KrlThread.h
***********************************************************
                彭东
**********************************************************/
#ifndef _KRLTHREADHEAD
#define _KRLTHREADHEAD

typedef struct TAFFILIATION
{
    UInt CPUID;
    UInt MNodeID;
    Executor* ExecutorPtr;
    //User
    //Transfer
    //Group
}TAffiliation;

typedef struct TCONTEXT
{
    Addr KrlStackAddr;
    Size KrlStackSize;
    Addr FunStartAddr;
    Addr NextStartAddr;
    Addr NextStackAddr;
}TContext;

typedef struct THREAD
{
    List Lists;
    ELock Lock;
	U64 Status;
	U64 Flags;
    U64 ID;
	UInt RunStatus;
	UInt RunMode;
    TAffiliation Affiliation;
    Transfer ThreadTransfer;
    TContext ThreadContext;
    void* Priv;
    void* Ext;
}Thread;

private void TAffiliationInit(TAffiliation* init);
private void TContextInit(TContext* init);

#endif