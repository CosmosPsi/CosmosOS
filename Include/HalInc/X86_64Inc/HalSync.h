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

void HalHalt();
void HalCliHalt();
void HalStiHalt();
void HalSti();
void HalCli();
void HalStiFlags(CPUFlg* cpuflg);
void HalCliFlags(CPUFlg* cpuflg);
void HalFlagsSti(CPUFlg* cpuflg);
void HalFlagsCli(CPUFlg* cpuflg);
void SPinLockInit(SPinLock *init);
void HalSPinLock(SPinLock *lock);
void HalUnSPinLock(SPinLock *lock);
void HalSPinLockSaveFlagsCli(SPinLock *lock, CPUFlg *cpuflg);
void HalUnSPinLockRestoreFlagsSti(SPinLock *lock, CPUFlg *cpuflg);
#endif