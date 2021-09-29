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
public Bool MemMSPlitMerOnPerPMSADBlockTest(MNode* node, MArea* area, PABHList* pabhl, PMSAD* start);
public Bool MemMSPlitMerOnPerPABHListTest(MNode* node, MArea* area, MSPlitMer* splitmer, PABHList* pabhl);
public Bool MemMSPlitMerTest(MNode* node, MArea* area, MSPlitMer* splitmer);
public Bool MemMAreaOnPerMAreaTest(MNode* node, MArea* area);
public Bool MemMAreaOnMNodeTest(MNode* node);
public Bool MemMAreaTest();
public Bool MemTestUnit();

#endif