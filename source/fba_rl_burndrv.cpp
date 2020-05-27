// =======================================================================
// BURN DRIVER INFO MODULES

#include "main.h"

uint32_t c_fbaRL::GetBurnDriverID(char* szRomzip)
{
//	for(uint32_t nDrv = 0; nDrv < sizeof(fba_drv) / sizeof(FBA_DRV); nDrv++)
//	{
//		char szDrvZip[128] = { 0 };
//		sprintf(szDrvZip, "%s.zip", fba_drv[nDrv].szName);
//
//		if(strncmp(szRomzip, szDrvZip, strlen(szDrvZip)) == 0)
//		{
//			return nDrv;
//		}
//	}
	return -1;
}

void c_fbaRL::UpdateBurnSelected(char* szROM)
{
	nBurnSelected = GetBurnDriverID(szROM);
}

bool c_fbaRL::IsBurnDuplicate(char* szROM)
{
	if(nTotalGames < 1) return false;

	for(int nGame = 0; nGame < nTotalGames; nGame++)
	{
		if(strcmp(games[nGame]->zipname, szROM) == 0)
		{
			return true; // busted! duplicate! xD
		}
	}
	return false; // you're good to go :)
}
