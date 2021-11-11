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

}TContext;


#endif