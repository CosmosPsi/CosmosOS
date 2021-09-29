/**********************************************************
        内存测试单元头文件MemTestUnit.h
***********************************************************
                彭东
**********************************************************/
#ifndef _MEMTESTUNITHEAD
#define _MEMTESTUNITHEAD

public Bool MemGMemManageTest();
public Bool MemPHYMSPaceAreaTest();
public Bool MemMNodeTest();
public Bool MemPMSADOnPMSADDireTest(MNode* node, PMSADDire* dire, U64 start, U64 end);
public Bool MemPMSADOnMNodeTest(MNode* node);
public Bool MemPMSADTest();
private UInt PMSADBlockIsContinuousAddr(PMSAD* start, PMSAD* end);
public Bool MemTestUnit();

#endif