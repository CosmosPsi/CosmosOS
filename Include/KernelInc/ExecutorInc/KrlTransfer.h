/**********************************************************
        调度转移头文件KrlTransfer.h
***********************************************************
                彭东
**********************************************************/
#ifndef _KRLTRANSFERHEAD
#define _KRLTRANSFERHEAD

#define TRANSFER_NR_MAX (64)

typedef struct TRANSFER Transfer;

typedef struct TRANSFERNODE
{
    List Lists;
    ELock Lock;
	UInt Status;
	UInt Flags;
    UInt TransferNR;
    List TransferLists;
    RBRoot Root;
    Transfer* CurrTransfer;
    void* Priv; 
    void* Ext;
}TransferNode;



#endif