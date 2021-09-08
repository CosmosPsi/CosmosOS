/**********************************************************
		内存mmu文件HalMMU.c
***********************************************************
				彭东
**********************************************************/
#include "BaseType.h"
#include "List.h"
#include "HalSync.h"
#include "HalCPU.h"
#include "HalMMU.h"
#include "HalInterface.h"
#include "KrlMmManage.h"

public void CR3Init(CR3* init)
{
    if(NULL == init)
    {
        return;
    }
    init->Entry = 0;
    return;
}

public void MMUInit(MMU* init)
{
    if(NULL == init)
    {
        return;
    }

    SPinlockInit(&init->Lock);
	init->Status = 0;
	init->Flags = 0;
	init->TDireArrPtr = NULL;
	CR3Init(&init->Cr3);
	ListInit(&init->TDirHead);	
	ListInit(&init->SDirHead);	
	ListInit(&init->IDirHead);	
	ListInit(&init->MDirHead);
	init->TDirMsaNR = 0;	
	init->SDirMsaNR = 0;	
	init->IDirMsaNR = 0;	
	init->MDirMsaNR = 0;
	return;
}

private UInt TDireIndex(Addr vaddr)
{
	return (UInt)((vaddr >> TDIRE_IV_RSHTBIT) & TDIRE_IV_BITMASK);
}

private UInt SDireIndex(Addr vaddr)
{
	return (UInt)((vaddr >> SDIRE_IV_RSHTBIT) & SDIRE_IV_BITMASK);
}

private UInt IDireIndex(Addr vaddr)
{
	return (UInt)((vaddr >> IDIRE_IV_RSHTBIT) & IDIRE_IV_BITMASK);
}

private UInt MDireIndex(Addr vaddr)
{
	return (UInt)((vaddr >> MDIRE_IV_RSHTBIT) & MDIRE_IV_BITMASK);
}


private void TDireArrInit(TDireArr* init)
{
    if(NULL == init)
    {
        return;
    }
    HalMemSet((void*)init, 0, sizeof(TDireArr));
    return;
}

private void SDireArrInit(SDireArr* init)
{
    if(NULL == init)
    {
        return;
    }
    HalMemSet((void*)init, 0, sizeof(SDireArr));
    return;
}

private void IDireArrInit(IDireArr* init)
{
    if(NULL == init)
    {
        return;
    }
    HalMemSet((void*)init, 0, sizeof(IDireArr));
    return;
}

private void MDireArrInit(MDireArr* init)
{
    if(NULL == init)
    {
        return;
    }
    HalMemSet((void*)init, 0, sizeof(MDireArr));
    return;
}

private Bool SDireArrIsAllZero(SDireArr* sdirearr)
{
    for(UInt i = 0; i < SDIRE_MAX; i++)
    {
        if(0 != sdirearr->SDEArr[i].SEntry)
        {
            return FALSE;
        }
    }
    return TRUE;
}

private Bool SDireIsHave(TDire* tdire)
{
    if(0 < tdire->TFlags.TSDIRBit)
    {
        return TRUE;
    }
    return FALSE;
}

private Bool SDireIsPresence(TDire* tdire)
{
    if(1 == tdire->TFlags.TPBit)
    {
        return TRUE;
    }
    return FALSE;
}

private Addr SDireRetPAddr(TDire* tdire)
{
    return (Addr)(tdire->TFlags.TSDIRBit << SDIRE_PADR_LSHTBIT);
}

private Addr SDireRetVAddr(TDire* tdire)
{
    return HalPAddrToVAddr(SDireRetPAddr(tdire));
}

private SDireArr* TDireRetSDireArr(TDire* tdire)
{
    return (SDireArr*)(SDireRetVAddr(tdire));
}

private Bool IDireArrIsAllZero(IDireArr* idirearr)
{
    for(UInt i = 0; i < IDIRE_MAX; i++)
    {
        if(0 != idirearr->IDEArr[i].IEntry)
        {
            return FALSE;
        }
    }
    return TRUE;
}

private Bool IDireIsHave(SDire* sdire)
{
    if(0 < sdire->SFlags.SIDIRBit)
    {
        return TRUE;
    }
    return FALSE;
}

private Bool IDireIsPresence(SDire* sdire)
{
    if(1 == sdire->SFlags.SPBit)
    {
        return TRUE;
    }
    return FALSE;
}

private Addr IDireRetPAddr(SDire* sdire)
{
    return (Addr)(sdire->SFlags.SIDIRBit << IDIRE_PADR_LSHTBIT);
}

private Addr IDireRetVAddr(SDire* sdire)
{
    return HalPAddrToVAddr(IDireRetPAddr(sdire));
}

private IDireArr* SDireRetIDireArr(SDire* sdire)
{
    return (IDireArr*)(IDireRetVAddr(sdire));
}

private Bool MDireArrIsAllZero(MDireArr* mdirearr)
{
    for(UInt i = 0; i < MDIRE_MAX; i++)
    {
        if(0 != mdirearr->MDEArr[i].MEntry)
        {
            return FALSE;
        }
    }
    return TRUE;
}

private Bool MDireIsHave(IDire* idire)
{
    if(0 < idire->IFlags.IMDIRBit)
    {
        return TRUE;
    }
    return FALSE;
}

private Bool MDireIsPresence(IDire* idire)
{
    if(1 == idire->IFlags.IPBit)
    {
        return TRUE;
    }
    return FALSE;
}

private Addr MDireRetPAddr(IDire* idire)
{
    return (Addr)(idire->IFlags.IMDIRBit << MDIRE_PADR_LSHTBIT);
}

private Addr MDireRetVAddr(IDire* idire)
{
    return HalPAddrToVAddr(MDireRetPAddr(idire));
}

private MDireArr* IDireRetMDireArr(IDire* idire)
{
    return (MDireArr*)(MDireRetVAddr(idire));
}

private Bool MMUPMSADIsHave(MDire* mdire)
{
    if(0 < mdire->MFlags.MMSABit)
    {
        return TRUE;
    }
    return FALSE;
}

private Bool MMUPMSADIsPresence(MDire* mdire)
{
    if(1 == mdire->MFlags.MPBit)
    {
        return TRUE;
    }
    return FALSE;
}

private Addr MMUPMSADRetPAddr(MDire* mdire)
{
    return (Addr)(mdire->MFlags.MMSABit << MSA_PADR_LSHTBIT);
}

private PMSAD* MMUNewTDireArr(MMU* mmulocked)
{
    TDireArr* tdirearr = NULL;
    PMSAD* msad = NULL;
    if(NULL == mmulocked)
    {
        return NULL;
    }
    
	msad = HalExPGetKrlOnePMSAD();
	if(NULL == msad)
	{
		return NULL;
	}

	tdirearr = (TDireArr*)PMSADRetVaddr(msa);

    TDireArrInit(tdirearr);

    ListAdd(&msad->Lists, &mmulocked->TDirHead);
    mmulocked->TDirMsaNR++;
	mmulocked->TDireArrPtr = tdirearr;

    return msad;
}

private Bool MMUDelTDireArr(MMU* mmulocked, TDireArr* tdirearr, PMSAD* msad)
{
	List* pos;
	PMSAD* tmpmsad;
	Addr tblphyadr;
	if(NULL == mmulocked || NULL == tdirearr)
	{
		return FALSE;
	}

	tblphyadr = HalVAddrToPAddr((Addr)tdirearr);

	if(NULL != msad)
	{
		if(PMSADRetPAddr(msad) == tblphyadr)
		{
			ListDel(&msad->Lists);
			if(HalExPPutKrlOnePMSAD(msad) == FALSE)
			{
				system_error("mmu_del_tdirearr err\n");
				return FALSE;
			}
			mmulocked->TDirMsaNR--;
			return TRUE;
		}
	}
	ListForEach(pos, &mmulocked->TDirHead)
	{
		tmpmsad = ListEntry(pos, PMSAD, Lists);
		if(PMSADRetPAddr(tmpmsad) == tblphyadr)
		{
			ListDel(&tmpmsad->Lists);
			if(HalExPPutKrlOnePMSAD(tmpmsad) == FALSE)
			{
				system_error("mmu_del_tdirearr err\n");
				return FALSE;
			}
			mmulocked->TDirMsaNR--;
			return TRUE;
		}
	}
	return FALSE;
}

private PMSAD* MMUNewSDireArr(MMU* mmulocked)
{
    SDireArr* sdirearr = NULL;
    PMSAD* msad = NULL;
    if(NULL == mmulocked)
    {
        return NULL;
    }
    
	msad = HalExPGetKrlOnePMSAD();
	if(NULL == msad)
	{
		return NULL;
	}

	sdirearr = (SDireArr*)PMSADRetVAddr(msad);

    SDireArrInit(sdirearr);

    ListAdd(&msad->Lists, &mmulocked->SDirHead);
    mmulocked->SDirMsaNR++;
    return msad;
}

private Bool MMUDelSDireArr(MMU* mmulocked, SDireArr* sdirearr, PMSAD* msad)
{
	List* pos;
	PMSAD* tmpmsad;
	Addr tblphyadr;
	if(NULL == mmulocked || NULL == sdirearr)
	{
		return FALSE;
	}

	tblphyadr = HalVAddrToPAddr((Addr)sdirearr);

	if(NULL != msad)
	{
		if(PMSADRetPAddr(msad) == tblphyadr)
		{
			ListDel(&msad->Lists);
			if(HalExPPutKrlOnePMSAD(msad) == FALSE)
			{
				system_error("mmu_del_tdirearr err\n");
				return FALSE;
			}
			mmulocked->SDirMsaNR--;
			return TRUE;
		}
	}
	ListForEach(pos, &mmulocked->mud_sdirhead)
	{
		tmpmsad = ListEntry(pos, PMSAD, Lists);
		if(PMSADRetPAddr(tmpmsa) == tblphyadr)
		{
			list_del(&tmpmsad->Lists);
			if(HalExPPutKrlOnePMSAD(tmpmsad) == FALSE)
			{
				system_error("mmu_del_tdirearr err\n");
				return FALSE;
			}
			mmulocked->SDirMsaNR--;
			return TRUE;
		}
	}
	return FALSE;
}

private PMSAD* MMUNewIDireArr(MMU* mmulocked)
{
    IDireArr* idirearr = NULL;
    PMSAD* msad = NULL;
    
    if(NULL == mmulocked)
    {
        return NULL;
    }
    
	msad = HalExPGetKrlOnePMSAD();
	if(NULL == msad)
	{
		return NULL;
	}

	idirearr = (IDireArr*)PMSADRetVAddr(msad);

    IDireArrInit(idirearr);

    ListAdd(&msad->Lists, &mmulocked->IDirHead);
    mmulocked->IDirMsaNR++;
    
    return msad;
}

private Bool MMUDelIDireArr(MMU* mmulocked, IDireArr* idirearr, PMSAD* msad)
{
	List* pos;
	PMSAD* tmpmsad;
	Addr tblphyadr;
	
    if(NULL == mmulocked || NULL == idirearr)
	{
		return FALSE;
	}

	tblphyadr = HalVAddrToPAddr((Addr)idirearr);

	if(NULL != msad)
	{
		if(PMSADRetPAddr(msad) == tblphyadr)
		{
			ListDel(&msad->Lists);
			if(HalExPPutKrlOnePMSAD(msad) == FALSE)
			{
				system_error("mmu_del_tdirearr err\n");
				return FALSE;
			}
			mmulocked->IDirMsaNR--;
			return TRUE;
		}
	}
	ListForEach(pos, &mmulocked->IDirHead)
	{
		tmpmsad = ListEntry(pos, PMSAD, Lists);
		if(PMSADRetPAddr(tmpmsad) == tblphyadr)
		{
			ListDel(&tmpmsad->Lists);
			if(HalExPPutKrlOnePMSAD(tmpmsad) == FALSE)
			{
				system_error("mmu_del_tdirearr err\n");
				return FALSE;
			}
			mmulocked->IDirMsaNR--;
			return TRUE;
		}
	}
	return FALSE;
}

private PMSAD* MMUNewMDireArr(MMU* mmulocked)
{
    MDireArr* mdirearr = NULL;
    PMSAD* msad = NULL;
    
    if(NULL == mmulocked)
    {
        return NULL;
    }
    
	msad = HalExPGetKrlOnePMSAD();
	if(NULL == msad)
	{
		return NULL;
	}

	mdirearr = (MDireArr*)PMSADRetVAddr(msad);
	MDireArrInit(mdirearr);
    
	ListAdd(&msad->Lists, &mmulocked->MDirHead);
    mmulocked->MDirMsaNR++;

    return msad;
}

private Bool MMUDelMDireArr(MMU* mmulocked, MDireArr* mdirearr, PMSAD* msad)
{
	List* pos;
	PMSAD* tmpmsad;
	Addr tblphyadr;

	if(NULL == mmulocked || NULL == mdirearr)
	{
		return FALSE;
	}

	tblphyadr = HalVAddrToPAddr((Addr)mdirearr);

	if(NULL != msad)
	{
		if(PMSADRetPAddr(msad) == tblphyadr)
		{
			ListDel(&msad->Lists);
			if(HalExPPutKrlOnePMSAD(msad) == FALSE)
			{
				system_error("mmu_del_tdirearr err\n");
				return FALSE;
			}
			mmulocked->MDirMsaNR--;
			return TRUE;
		}
	}
	ListForEach(pos, &mmulocked->MDirHead)
	{
		tmpmsad = ListEntry(pos, PMSAD, Lists);
		if(PMSADRetPAddr(tmpmsad) == tblphyadr)
		{
			ListDel(&tmpmsad->Lists);
			if(HalExPPutKrlOnePMSAD(tmpmsad) == FALSE)
			{
				system_error("mmu_del_tdirearr err\n");
				return FALSE;
			}
			mmulocked->MDirMsaNR--;
			return TRUE;
		}
	}
	return FALSE;
}

private Addr MMUUnTranslationPMSAD(MMU* mmulocked, MDireArr* mdirearr, Addr vaddr)
{
	UInt mindex;
	MDire mdire;
	Addr retadr;
	if(NULL == mmulocked || NULL == mdirearr)
	{
		return NULL;
	}

	mindex = MDireIndex(vaddr);
	
	mdire = mdirearr->MDEArr[mindex];
	if(MMUPMSADIsHave(&mdire) == FALSE)
	{
		return NULL;
	}

	retadr = MMUPMSADRetPAddr(&mdire);
	
	mdirearr->MDEArr[mindex].MEntry = 0;
	return retadr; 
}

private Bool MMUTranslationPMSAD(MMU* mmulocked, MDireArr* mdirearr, Addr vaddr, Addr paddr, U64 flags)
{
	UInt mindex;	
	if(NULL == mmulocked || NULL == mdirearr)
	{
		return FALSE;
	}

	mindex = MDireIndex(vaddr);
	mdirearr->MDEArr[mindex].MEntry = (((U64)paddr) | flags);
	
	return TRUE;
}

private Bool MMUUnTranslationMDire(MMU* mmulocked, IDireArr* idirearr, PMSAD* msad, Addr vaddr)
{
	UInt iindex;
	IDire idire;
	MDireArr* mdirearr = NULL;
	if(NULL == mmulocked || NULL == idirearr)
	{
		return FALSE;
	}

	iindex = IDireIndex(vaddr);
	
	idire = idirearr->IDEArr[iindex];
	if(MDireIsHave(&idire) == FALSE)
	{
		return TRUE;
	}

	mdirearr = IDireRetMDireArr(&idire);
	if(MDireArrIsAllZero(mdirearr) == FALSE)
	{
		return TRUE;
	}

	if(MMUDelMDireArr(mmulocked, mdirearr, msad) == FALSE)
	{
		return FALSE;
	}

	idirearr->IDEArr[iindex].IEntry = 0;
	
	return TRUE; 
}

private MDireArr* MMUTranslationMDire(MMU* mmulocked, IDireArr* idirearr, Addr vaddr, U64 flags, PMSAD** outmsad)
{
	UInt iindex;
	IDire idire;
	Addr dire;
	PMSAD* msad = NULL;
	MDireArr* mdirearr = NULL;
	if(NULL == mmulocked || NULL == idirearr || NULL == outmsad)
	{
		return NULL;
	}
	
	iindex = IDireIndex(vaddr);
	
	idire = idirearr->IDEArr[iindex];
	if(MDireIsHave(&idire) == TRUE)
	{
		mdirearr = IDireRetMDireArr(&idire);
		*outmsad = NULL;
		return mdirearr;
	}

	msa = MMUNewMDireArr(mmulocked);
	if(NULL == msad)
	{
		*outmsad = NULL;
		return NULL;
	}

	dire = PMSADRetPAddr(msad);
	mdirearr = (MDireArr*)(HalPAddrToVAddr(dire));
	idirearr->IDEArr[iindex].IEntry = (((U64)dire) | flags);

	*outmsad = msad;
	
	return mdirearr;
}

private Bool MMUUnTranslationIDire(MMU* mmulocked, SDireArr* sdirearr, PMSAD* msad, Addr vaddr)
{
	UInt sindex;
	SDire sdire;
	IDireArr* idirearr = NULL;
	if(NULL == mmulocked || NULL == sdirearr)
	{
		return FALSE;
	}

	sindex = SDireIndex(vaddr);
	
	sdire = sdirearr->SDEArr[sindex];
	if(IDireIsHave(&sdire) == FALSE)
	{
		return TRUE;
	}

	idirearr = SDireRetIDireArr(&sdire);
	if(IDireArrIsAllZero(idirearr) == FALSE)
	{
		return TRUE;
	}

	if(MMUDelIDireArr(mmulocked, idirearr, msad) == FALSE)
	{
		return FALSE;
	}

	sdirearr->SDEArr[sindex].SEntry = 0;
	
	return TRUE; 
}

private IDireArr* MMUTranslationIDire(MMU* mmulocked, SDireArr* sdirearr, Addr vaddr, U64 flags, PMSAD** outmsad)
{
	UInt sindex;
	SDire sdire;
	Addr dire;	
	PMSAD* msad = NULL;	
	IDireArr* idirearr = NULL;
	if(NULL == mmulocked || NULL == sdirearr || NULL == outmsad)
	{
		return NULL;
	}
	
	sindex = SDireIndex(vaddr);
	
	sdire = sdirearr->SDEArr[sindex];
	if(IDireIsHave(&sdire) == TRUE)
	{
		idirearr = SDireRetIDireArr(&sdire);
		*outmsad = NULL;
		return idirearr;
	}

	msad = MMUNewIDireArr(mmulocked);
	if(NULL == msad)
	{
		*outmsad = NULL;
		return NULL;
	}

	dire = PMSADRetPAddr(msad);
	idirearr = (IDireArr*)(HalPAddrToVAddr(dire));
	sdirearr->SDEArr[sindex].SEntry = (((U64)dire) | flags);

	*outmsad = msad;
	
	return idirearr;
}

private Bool MMUUnTranslationSDire(MMU* mmulocked, TDireArr* tdirearr, PMSAD* msad, Addr vaddr)
{
	UInt tindex;
	TDire tdire;
	SDireArr* sdirearr = NULL;

	if(NULL == mmulocked || NULL == tdirearr)
	{
		return FALSE;
	}

	tindex = TDireIndex(vaddr);
	
	tdire = tdirearr->tde_arr[tindex];
	if(SDireIsHave(&tdire) == FALSE)
	{
		return TRUE;
	}

	sdirearr = TDireRetSDireArr(&tdire);
	if(SDireArrIsAllZero(sdirearr) == FALSE)
	{
		return TRUE;
	}

	if(MMUDelSDireArr(mmulocked, sdirearr, msad) == FALSE)
	{
		return FALSE;
	}

	tdirearr->TDEArr[tindex].TEntry = 0;
	
	return TRUE; 
}

private SDireArr* MMUTranslationSDire(MMU* mmulocked, TDireArr* tdirearr, Addr vaddr, U64 flags, PMSAD** outmsad)
{
	UInt tindex;
	TDire tdire;
	Addr dire;
	SDireArr* sdirearr = NULL;
	PMSAD* msad = NULL;
	if(NULL == mmulocked || NULL == tdirearr || NULL == outmsad)
	{
		return NULL;
	}
	
	tindex = TDireIndex(vaddr);
	
	tdire = tdirearr->TDEArr[tindex];
	if(SDireIsHave(&tdire) == TRUE)
	{
		sdirearr = TDireRetSDireArr(&tdire);
		*outmsad = NULL;
		return sdirearr;
	}

	msad = MMUNewSDireArr(mmulocked);
	if(NULL == msad)
	{
		*outmsad = NULL;
		return NULL;
	}

	dire = PMSADRetPAddr(msad);
	sdirearr = (SDireArr*)(HalPAddrToVAddr(dire));
	tdirearr->TDEArr[tindex].TEntry = (((U64)dire) | flags);

	*outmsad = msad;
	
	return sdirearr;
}

private Addr MMUFindPMSADAddress(MDireArr* mdirearr, Addr vaddr)
{
	UInt mindex;
	MDire dire;
	if(NULL == mdirearr)
	{
		return NULL;
	}

	mindex = MDireIndex(vaddr);

	dire = mdirearr->MDEArr[mindex];

	if(MMUPMSADIsHave(&dire) == FALSE)
	{
		return NULL;
	}

	return MMUPMSADRetPAddr(&dire);
}


private MDireArr* MMUFindMDireArr(IDireArr* idirearr, Addr vaddr)
{
	UInt iindex;
	IDire dire;
	if(NULL == idirearr)
	{
		return NULL;
	}

	iindex = IDireIndex(vaddr);

	dire = idirearr->IDEArr[iindex];

	if(MDireIsHave(&dire) == FALSE)
	{
		return NULL;
	}

	return IDireRetMDireArr(&dire);
}


private IDireArr* MMUFindIDireArr(SDireArr* sdirearr, Addr vaddr)
{
	UInt sindex;
	SDire dire;
	if(NULL == sdirearr)
	{
		return NULL;
	}

	sindex = SDireIndex(vaddr);

	dire = sdirearr->SDEArr[sindex];

	if(IDireIsHave(&dire) == FALSE)
	{
		return NULL;
	}

	return SDireRetIDireArr(&dire);
}

private SDireArr* MMUFindSDireArr(TDireArr* tdirearr, Addr vaddr)
{
	UInt tindex;
	TDire dire;
	if(NULL == tdirearr)
	{
		return NULL;
	}

	tindex = TDireIndex(vaddr);

	dire = tdirearr->TDEArr[tindex];

	if(SDireIsHave(&dire) == FALSE)
	{
		return NULL;
	}

	return TDireRetSDireArr(&dire);
}

public void HalMMULoad(MMU* mmu)
{
	if(NULL == mmu)
	{
		return;
	}
	
	HalSPinLock(&mmu->Lock);
	if(NULL == mmu->TDireArrPtr || 0 != (((U64)(mmu->TDireArrPtr)) & 0xfff))
	{
		goto out;
	}

	mmu->CR3.Entry = HalVAddrToPAddr((Addr)mmu->TDireArrPtr);
	HalWriteCR3((UInt)(mmu->CR3.Entry));

out:
    HalUnSPinLock(&mmu->Lock);	
	return;
}

private void HalMMUInnerRefresh(MMU* mmulocked)
{
	CR3 cr3;
	if(NULL == mmulocked)
	{
		cr3.Entry = (U64)HalReadCR3();
		HalWriteCR3(cr3.Entry);
		return;
	}
	if(NULL == mmulocked->TDireArrPtr || 0 != (((U64)(mmulocked->TDireArrPtr)) & 0xfff))
	{
		return;
	}

	mmulocked->CR3.Entry = HalVAddrToPAddr((Addr)mmulocked->TDireArrPtr);
	HalWriteCR3((UInt)(mmulocked->CR3.Entry));
	return;
}



public void HalMMURefresh(MMU* mmu)
{
	CR3 cr3;
	if(NULL == mmu)
	{
		cr3.Entry = (U64)HalReadCR3();
		HalWriteCR3(cr3.Entry);
		return;
	}
	HalMMULoad(mmu);
	return;
}

private Addr HalUnMMUTranslationCore(MMU* mmu, Addr vaddr)
{
	Addr retadr;
	SDireArr* sdirearr;
	IDireArr* idirearr;
	MDireArr* mdirearr;
	HalSPinLock(&mmu->Lock);
	sdirearr = MMUFindSDireArr(mmu->TDireArrPtr, vaddr);
	if(NULL == sdirearr)
	{
		retadr = NULL;
		goto OUTLabel;
	}

	idirearr = MMUFindIDireArr(sdirearr, vaddr);
	if(NULL == idirearr)
	{
		retadr = NULL;
		goto ERRLabelSDireArr;
	}

	mdirearr = MMUFindMDireArr(idirearr, vaddr);
	if(NULL == mdirearr)
	{
		retadr = NULL;
		goto ERRLabelIDireArr; 
	}
	
	retadr = MMUUnTranslationPMSAD(mmu, mdirearr, vaddr);

	MMUUnTranslationMDire(mmu, idirearr, NULL, vaddr);
ERRLabelIDireArr:
	MMUUnTranslationIDire(mmu, sdirearr, NULL, vaddr);
ERRLabelSDireArr:
	MMUUnTranslationSDire(mmu, mmu->TDireArrPtr, NULL, vaddr);
OUtLabel:
	HalMMUInnerRefresh(mmu);	
	HalUnSPinLock(&mmu->Lock);
	return retadr;
}

public Addr HalUnMMUTranslation(MMU* mmu, Addr vaddr)
{
	if(NULL == mmu)
	{
		return EPARAM;
	}
	return HalUnMMUTranslationCore(mmu, vaddr);
}

private Bool HalMMUTranslationCore(MMU* mmu, Addr vaddr, Addr paddr, U64 flags)
{
	Bool rets = FALSE;
	TDireArr* tdirearr = NULL;
	SDireArr* sdirearr = NULL;
	IDireArr* idirearr = NULL;
	MDireArr* mdirearr = NULL;
	PMSAD* smsad = NULL;
	PMSAD* imsad = NULL;
	PMSAD* mmsad = NULL;

	HalSPinLock(&mmu->Lock);
	
	tdirearr = mmu->TDireArrPtr;
	if(NULL == tdirearr)
	{
		rets = FALSE;
		goto OUTLabel;
	}

	sdirearr = MMUTranslationSDire(mmu, tdirearr, vaddr, flags, &smsad);
	if(NULL == sdirearr)
	{
		rets = FALSE;
		goto ERRLabelSDire;			
	}

	idirearr = MMUTranslationIDire(mmu, sdirearr, vaddr, flags, &imsad);
	if(NULL == idirearr)
	{
		rets = FALSE;
		goto ERRLabelIDire;		
	}

	mdirearr = MMUTranslationMDire(mmu, idirearr, vaddr, flags, &mmsad);
	if(NULL == mdirearr)
	{
		rets = FALSE;
		goto ERRLabelMDire;
	}

	rets = MMUTranslationPMSAD(mmu, mdirearr, vaddr, paddr, flags);
	if(TRUE == rets)
	{
		rets = TRUE;
		HalMMUInnerRefresh(mmu);
		goto out;
	}

	rets = FALSE;

	MMUUnTranslationPMSAD(mmu, mdirearr, vaddr);
ERRLabelMDire:
	MMUUnTranslationMDire(mmu, idirearr, mmsad, vaddr);
ERRLabelIDire:
	MMUUnTranslationIDire(mmu, sdirearr, imsad, vaddr);
ERRLabelSDire:
	MMUUnTranslationSDire(mmu, tdirearr, smsad, vaddr);	
OUTLabel:
	HalUnSPinLock(&mmu->Lock);
	return rets;
}

public Bool HalMMUTranslation(MMU* mmu, Addr vaddr, Addr paddr, U64 flags)
{
	if(NULL == mmu)
	{
		return NULL;
	}
	return HalMMUTranslationCore(mmu, vaddr, paddr, flags);
}


private Bool MMUCleanMDireArrAllPMSAD(MMU* mmulocked)
{
	List* pos;
	PMSAD* msad = NULL;
	if(NULL == mmulocked)
	{
		return FALSE;
	}
	ListForEachDeleteOneList(pos, &mmulocked->MDirHead)
	{
		msad = ListEntry(pos, PMSAD, md_list);
		ListDel(&msad->Lists);
		if(HalExPPutKrlOnePMSAD(msad) == FALSE)
		{
			system_error("mmu_clean_mdirearrmsas");
			return FALSE;
		}
		mmulocked->MDirMsaNR--;
	}
	return TRUE;
}

private Bool MMUCleanIDireArrAllPMSAD(MMU* mmulocked)
{
	List* pos;
	PMSAD* msad = NULL;
	if(NULL == mmulocked)
	{
		return FALSE;
	}
	ListForEachDeleteOneList(pos, &mmulocked->IDirHead)
	{
		msad = ListEntry(pos, PMSAD, Lists);
		ListDel(&msad->Lists);
		if(HalExPPutKrlOnePMSAD(msad) == FALSE)
		{
			system_error("mmu_clean_idirearrmsas");
			return FALSE;
		}
		mmulocked->IDirMsaNR--;
	}
	return TRUE;
}

private Bool MMUCleanSDireArrAllPMSAD(MMU* mmulocked)
{
	List* pos;
	PMSAD* msad = NULL;
	if(NULL == mmulocked)
	{
		return FALSE;
	}
	ListForEachDeleteOneList(pos, &mmulocked->SDirHead)
	{
		msad = ListEntry(pos, PMSAD, Lists);
		ListDel(&msad->Lists);
		if(HalExPPutKrlOnePMSAD(msad) == FALSE)
		{
			system_error("MMUCleanSDireArrAllPMSAD");
			return FALSE;
		}
		mmulocked->SDirMsaNR--;
	}
	return TRUE;
}

private Bool MMUCleanTDireArrAllPMSAD(MMU* mmulocked)
{
	List* pos;
	PMSAD* msad = NULL;
	if(NULL == mmulocked)
	{
		return FALSE;
	}
	ListForEachDeleteOneList(pos, &mmulocked->TDirHead)
	{
		msad = ListEntry(pos, PMSAD, Lists);
		ListDel(&msad->Lists);
		if(HalExPPutKrlOnePMSAD(msad) == FALSE)
		{
			system_error("MMUCleanTDireArrAllPMSAD");
			return FALSE;
		}
		mmulocked->TDirMsaNR--;
	}
	return TRUE;
}

public Bool HalMMUClean()
{
	Bool  rets = FALSE;
	Addr pcr3 = NULL, vcr3 = NULL;
	CR3 cr3;
	if(NULL == mmu)
	{
		return FALSE;
	}

	HalSPinlock(&mmu->Lock);

	cr3.Entry = (U64)HalReadCR3();

	pcr3 = (Addr)(cr3.Flags.Plm4a << 12);
	vcr3 = HalPAddrToVAddr(pcr3);

	if(vcr3 == (Addr)(mmu->TDireArrPtr))
	{
		rets = FALSE;
		goto OUTLabel;
	}

	if(MMUCleanMDireArrAllPMSAD(mmu) == FALSE)
	{
		rets = FALSE;
		goto OUTLabel;
	}

	if(MMUCleanIDireArrAllPMSAD(mmu) == FALSE)
	{
		rets = FALSE;
		goto OUTLabel;
	}

	if(MMUCleanSDireArrAllPMSAD(mmu) == FALSE)
	{
		rets = FALSE;
		goto OUTLabel;
	}

	if(MMUCleanTDireArrAllPMSAD(mmu) == FALSE)
	{
		rets = FALSE;
		goto OUTLabel;
	}

	rets = TRUE;

OUTLabel:	
	HalUnSPinLock(&mmu->Lock);
	return rets;	
}

public Bool HalMMUInitExecutorTDireArr(MMU* mmu)
{
	Bool rets = FALSE;
	Addr vcr3 = NULL;
	if(NULL == mmu)
	{
		return FALSE;
	}
	HalSPinLock(&mmu->Lock);

	if(MMUNewTDireArr(mmu) == NULL)
	{
		rets = FALSE;
		goto OUTLable;
	}

	vcr3 = HalPAddrToVAddr(kmachbsp.mb_pml4padr);

	HalMemCopy((void*)vcr3, (void*)mmu->TDireArrPtr, sizeof(TDireArr));
	
	mmu->Cr3.Entry = (U64)HalVAddrToPAddr((Addr)mmu->TDireArrPtr);
	mmu->TDireArrPtr->TDEArr[0].TEntry = 0;
	
	rets = TRUE;

OUTLable:
	HalUnSPinLock(&mmu->Lock);
	return rets;
}

public Bool HalMMUEnable()
{
	//x86_64下没法单纯的开启MMU
	return TRUE;
}

public Bool HalMMUDisable()
{
	//x86_64下没法单纯的关闭MMU
	return FALSE;
}

public Bool HalMMUInit()
{
	return HalMMUEnable();
}