/**********************************************************
		同步头文件HalSync.h
***********************************************************
				彭东
**********************************************************/
#ifndef _HALSYNCHEAD
#define _HALSYNCHEAD


typedef struct SPINLOCK
{
	 volatile U32 Lock;
}SPinLock;

public void HalHalt();
public void HalCliHalt();
public void HalStiHalt();
public void HalSti();
public void HalCli();
public void HalStiFlags(CPUFlg* cpuflg);
public void HalCliFlags(CPUFlg* cpuflg);
public void HalFlagsSti(CPUFlg* cpuflg);
public void HalFlagsCli(CPUFlg* cpuflg);
public void SPinLockInit(SPinLock *init);
public void HalSPinLock(SPinLock *lock);
public void HalUnSPinLock(SPinLock *lock);
public void HalSPinLockSaveFlagsCli(SPinLock *lock, CPUFlg *cpuflg);
public void HalUnSPinLockRestoreFlagsSti(SPinLock *lock, CPUFlg *cpuflg);
#endif