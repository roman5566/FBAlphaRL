// =======================================================================
// GAMELIST (PARSING)

#include "main.h"

#define _GETTAG_STR_VALUE(_opentag, _closetag, _outstr)	\
	fgets(pszLine, 2048, fp);							\
	pch = strstr(pszLine, _opentag);					\
	if(pch)	{											\
		char* pszOption =  NULL;						\
		pszOption = (char*)malloc(2048+1);				\
		memset(pszOption, 0, 2048+1);					\
		strncpy(pszOption, pch+(strlen(_opentag)), 2048);\
		pch = strstr(pszOption, _closetag);				\
		pszOption[(int)(pch-pszOption)] = 0;			\
		strcpy(_outstr, pszOption);						\
		SAFE_FREE(pszOption)							\
	}

#define _GETTAG_INT_VALUE(_opentag, _closetag, _outint)	\
	fgets(pszLine, 2048, fp);							\
	pch = strstr(pszLine, _opentag);					\
	if(pch)	{											\
		char* pszOption =  NULL;						\
		pszOption = (char*)malloc(2048+1);				\
		memset(pszOption, 0, 2048+1);					\
		strncpy(pszOption, pch+(strlen(_opentag)), 2048);\
		pch = strstr(pszOption, _closetag);				\
		pszOption[(int)(pch-pszOption)] = 0;			\
		sscanf(pszOption, "%"PRIu64, _outint);			\
		SAFE_FREE(pszOption)							\
	}

#define _GETTAG_BOOL_VALUE(_opentag, _closetag, _outbool)\
	fgets(pszLine, 2048, fp);							\
	pch = strstr(pszLine, _opentag);					\
	if(pch)	{											\
		char* pszOption =  NULL;						\
		pszOption = (char*)malloc(2048+1);				\
		memset(pszOption, 0, 2048+1);					\
		strncpy(pszOption, pch+(strlen(_opentag)), 2048);\
		pch = strstr(pszOption, _closetag);				\
		pszOption[(int)(pch-pszOption)] = 0;			\
		if(strcmp(pszOption, "yes")==0) {_outbool = true;}	\
		if(strcmp(pszOption, "no")==0)	{_outbool = false;}	\
		SAFE_FREE(pszOption)							\
	}


int c_fbaRL::ParseGameListCache()
{
	bProcessingGames = true;

	FILE* fp = NULL;
	fp = fopen("/dev_hdd0/game/FBAL00123/USRDIR/FBA.GAMELIST.CACHE.DAT", "r");
	
	if(fp)
	{
		char* pszLine = NULL;
		pszLine = (char*)malloc(2048+1);
		memset(pszLine, 0, 2048+1);		

		while (!feof(fp)) 
		{
			fgets(pszLine, 2048, fp);

			char* pch = NULL;
			pch = strstr(pszLine, "<totalgames>");

			if(pch) {
				char* pszOption =  NULL;
				pszOption = (char*)malloc(2048+1);
				memset(pszOption, 0, 2048+1);				
				strncpy(pszOption, pch+(strlen("<totalgames>")), 2048);
				pch = strstr(pszOption, "</totalgames>");
				pszOption[(int)(pch-pszOption)] = 0;
				sscanf(pszOption, "%d", &nTotalGames);
				SAFE_FREE(pszOption)
				break;
			}
		}
		
		SAFE_FREE(pszLine)
		
		rewind(fp);

		for(int n = 0; n < nTotalGames; n++)
		{
			char* pszLine = NULL;
			pszLine = (char*)malloc(2048+1);
			memset(pszLine, 0, 2048+1);

			while (!feof(fp)) 
			{
				fgets(pszLine, 2048, fp);

				char* pch = NULL;

				char szTag[64] = { 0 };
				sprintf(szTag, "<game id=%d>", n);
				pch = strstr(pszLine, szTag);
				
				if(pch) 
				{
					games[n] = new c_game(n);

					_GETTAG_STR_VALUE("<title>", "</title>"			, games[n]->title)
					
					_GETTAG_STR_VALUE("<zipname>", "</zipname>"		, games[n]->zipname)
					
					_GETTAG_STR_VALUE("<sysmask>", "</sysmask>"		, games[n]->sysmask)
					
					_GETTAG_BOOL_VALUE("<available>", "</available>", games[n]->bAvailable)
					
					if(games[n]->bAvailable) 
					{
						_GETTAG_STR_VALUE("<path>", "</path>"			, games[n]->path)					
					
						_GETTAG_INT_VALUE("<size>", "</size>"			, &games[n]->nSize)
					} else {
						fgets(pszLine, 2048, fp);
						fgets(pszLine, 2048, fp);

						nMissingGames++;
					}

					fgets(pszLine, 2048, fp);
					pch = strstr(pszLine, "</game>");
					if(pch)	break;
				}
			}
			SAFE_FREE(pszLine)
		}
		fclose(fp);
		fp = NULL;

		InitFilterList();
		
		return 1;
	}

	InitFilterList();

	bProcessingGames = false;
	return 0;
}

int c_fbaRL::SaveGameListCache()
{
	FILE* fp = NULL;
	fp = fopen("/dev_hdd0/game/FBAL00123/USRDIR/FBA.GAMELIST.CACHE.DAT", "w");
	
	if(fp) 
	{
		fprintf(fp, "<totalgames>%d</totalgames>\n", nTotalGames);

		for(int n=0; n < nTotalGames; n++) 
		{
			fprintf(fp, "<game id=%d>\n", n);
			fprintf(fp, "<title>%s</title>\n"			, games[n]->title);
			fprintf(fp, "<zipname>%s</zipname>\n"		, games[n]->zipname);
			fprintf(fp, "<sysmask>%s</sysmask>\n"		, games[n]->sysmask);
			fprintf(fp, "<available>%s</available>\n"	, games[n]->bAvailable ? "yes" : "no");
			fprintf(fp, "<path>%s</path>\n"				, games[n]->bAvailable ? games[n]->path : "NULL");			
			fprintf(fp, "<size>%" PRIu64 "</size>\n"	, games[n]->bAvailable ? games[n]->nSize : 0);			
			fprintf(fp, "</game>\n");
		}

		fclose(fp);
		fp = NULL;
		return 1;
	}
	return 0;
}

int c_fbaRL::ParseGameList()
{
	bProcessingGames = true;	
	fbaRL->nStatus = STATUS_ROMSCAN_START;
	return 1;
}

void c_fbaRL::RefreshGameList()
{
	EndGameList();
	InitGameList();
}

void c_fbaRL::InitGameList()
{
	nTotalGames = 0;
	nMissingGames = 0;

	bProcessingGames = true;

	games = (c_game**)malloc(sizeof(c_game) * MAX_GAMES);
	//memset(games, 0, sizeof(c_game) * MAX_GAMES);

	// Check for FBA.GAMELIST.CACHE.DAT...
	if(!ParseGameListCache())
	{
		// Not found, rescan ROMs
		ParseGameList();
	}
}

void c_fbaRL::EndGameList()
{
	bProcessingGames = true;

	for(int n = 0; n < nTotalGames; n++)
	{
		SAFE_DELETE(games[n])
	}
	SAFE_FREE(games);
	nTotalGames = 0;

	bProcessingGames = false;

	EndFilterList();
}

// (re-wrote) this function will just look for specific ROMs on specific locations,
// that means it will NOT process unnecesary files, ex. there could be 40,000 ZIP
// files, but I'm just checking 5,000, not comparing each of the 40,000.
void _ScanROMs(uint64_t /*arg*/)
{
	uint32_t nTotalBurnDrivers = sizeof(fba_drv) / sizeof(FBA_DRV);
	uint32_t nPaths = sizeof(g_opt_szROMPaths) / 2048;
	
	char* pszFilePath = NULL;
	pszFilePath = (char*)malloc(2048);
	memset(pszFilePath, 0, 2048);

	double fDelta = 0.0f;
	double fDeltaCnt = 0.0f;

	// go through all drivers...
	for(uint32_t nDrv = 0; nDrv < nTotalBurnDrivers; nDrv++)
	{
		fbaRL->games[fbaRL->nTotalGames] = new c_game(fbaRL->nTotalGames);	

		// try every path possible...
		for(uint32_t x = 0; x < nPaths; x++)
		{
			if(fbaRL->DirExist(g_opt_szROMPaths[x]))
			{							
				sprintf(pszFilePath, "%s/%s.zip", g_opt_szROMPaths[x], fba_drv[nDrv].szName);
				
				// Available...
				if(fbaRL->FileExist(pszFilePath)) 
				{	
					// TODO: check if zip has all files			
					// Add found game info...									
					sprintf(fbaRL->games[fbaRL->nTotalGames]->title, "%s", fba_drv[nDrv].szTitle);			// title					
					sprintf(fbaRL->games[fbaRL->nTotalGames]->sysmask, "%s", fba_drv[nDrv].szSystemFilter);	// sys filter					
					strcpy(fbaRL->games[fbaRL->nTotalGames]->path, pszFilePath);							// path					
					sprintf(fbaRL->games[fbaRL->nTotalGames]->zipname, "%s.zip", fba_drv[nDrv].szName);		// zip title					
					fbaRL->games[fbaRL->nTotalGames]->nSize = 0/*fbaRL->GetFileSize(pszFilePath)*/;				// Size in bytes					
					fbaRL->games[fbaRL->nTotalGames]->bAvailable = true;
					break;
				}
			}
		}

		// Missing...
		if(!fbaRL->games[fbaRL->nTotalGames]->bAvailable) 
		{
			sprintf(fbaRL->games[fbaRL->nTotalGames]->title		, "%s"		, fba_drv[nDrv].szTitle);			// title
			sprintf(fbaRL->games[fbaRL->nTotalGames]->sysmask	, "%s"		, fba_drv[nDrv].szSystemFilter);	// sys filter
			sprintf(fbaRL->games[fbaRL->nTotalGames]->zipname	, "%s.zip"	, fba_drv[nDrv].szName);		// zip title				
			fbaRL->games[fbaRL->nTotalGames]->nSize = 0;				// Size in bytes
			fbaRL->games[fbaRL->nTotalGames]->bAvailable = false;

			fbaRL->nMissingGames++;			
		}

		fbaRL->nTotalGames++;

		// Progress bar...
		double fTotalPct = ((double)nDrv / (double)nTotalBurnDrivers) * 100.0f;

		fDeltaCnt += (fTotalPct - fDelta);

		char msg[256] = "";
		sprintf(msg,
			"%d / %d (%.1f %%)",
			nDrv, 
			nTotalBurnDrivers, 
			fTotalPct
		);
		cellMsgDialogProgressBarSetMsg(CELL_MSGDIALOG_PROGRESSBAR_INDEX_SINGLE, msg);
					
		if(fDeltaCnt >= 1.0f)
		{
			fDeltaCnt -= 1.0f;
			cellMsgDialogProgressBarInc(CELL_MSGDIALOG_PROGRESSBAR_INDEX_SINGLE, 1);
		}					
					
		fDelta = fTotalPct;
	}
	fbaRL->bProcessingGames = false;
	fbaRL->nStatus = STATUS_ROMSCAN_END;
	sys_ppu_thread_exit(0);
}
