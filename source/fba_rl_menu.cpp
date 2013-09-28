// =======================================================================
// UI / MENUS

#include "main.h"

void c_fbaRL::InitMainMenu()
{
	main_menu = new c_Menu(7);

	main_menu->AddItem((char*)"GAME LIST");
	main_menu->AddItem((char*)"OPTIONS");
	main_menu->AddItem((char*)"LOAD \"FB Alpha Core\" MOD");
	main_menu->AddItem((char*)"LOAD \"Iris Manager\"");
	main_menu->AddItem((char*)"LOAD \"multiMAN\"");
	main_menu->AddItem((char*)"EXIT");
}

void c_fbaRL::EndMainMenu()
{
	SAFE_DELETE(main_menu)
}

void c_fbaRL::InitOptionsMenu()
{
	options_menu = new c_Menu(7);

	options_menu->AddItem((char*)"AUTO ASPECT RATIO");
	options_menu->AddItem((char*)"AUTO CREATE ALL INPUT .CFG");
	options_menu->AddItem((char*)"ALTERNATE MENU KEY COMBO");
	options_menu->AddItem((char*)"USE NEO-GEO UNI-BIOS");
	options_menu->AddItem((char*)"DISPLAY MISSING GAMES");
	
	options_menu->AddItem((char*)"FILTER [CAPCOM MISC]");
	options_menu->AddItem((char*)"FILTER [CAVE]");
	options_menu->AddItem((char*)"FILTER [CPS1]");
	options_menu->AddItem((char*)"FILTER [CPS2]");
	options_menu->AddItem((char*)"FILTER [CPS3]");
	options_menu->AddItem((char*)"FILTER [DATA EAST]");
	options_menu->AddItem((char*)"FILTER [GALAXIAN]");
	options_menu->AddItem((char*)"FILTER [IREM]");
	options_menu->AddItem((char*)"FILTER [KANEKO]");
	options_menu->AddItem((char*)"FILTER [KONAMI]");
	options_menu->AddItem((char*)"FILTER [NEO GEO]");
	options_menu->AddItem((char*)"FILTER [PAC-MAN]");
	options_menu->AddItem((char*)"FILTER [PGM]");
	options_menu->AddItem((char*)"FILTER [PSIKYO]");
	options_menu->AddItem((char*)"FILTER [SEGA]");
	options_menu->AddItem((char*)"FILTER [SETA]");
	options_menu->AddItem((char*)"FILTER [TAITO]");
	options_menu->AddItem((char*)"FILTER [TECHNOS]");
	options_menu->AddItem((char*)"FILTER [TOAPLAN]");
	options_menu->AddItem((char*)"FILTER [MISC PRE 90\'s]");
	options_menu->AddItem((char*)"FILTER [MISC POST 90\'s]");

	options_menu->AddItem((char*)"ROMs PATH #1");
	options_menu->AddItem((char*)"ROMs PATH #2");
	options_menu->AddItem((char*)"ROMs PATH #3");
	options_menu->AddItem((char*)"ROMs PATH #4");
	options_menu->AddItem((char*)"ROMs PATH #5");
	options_menu->AddItem((char*)"ROMs PATH #6");
	options_menu->AddItem((char*)"ROMs PATH #7");
	options_menu->AddItem((char*)"ROMs PATH #8");
	options_menu->AddItem((char*)"ROMs PATH #9");
	options_menu->AddItem((char*)"ROMs PATH #10");
	options_menu->AddItem((char*)"ROMs PATH #11");
	options_menu->AddItem((char*)"ROMs PATH #12");
	
	options_menu->AddItem((char*)"[CAPCOM MISC] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[CAVE] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[CPS1] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[CPS2] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[CPS3] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[DATA EAST] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[GALAXIAN] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[IREM] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[KANEKO] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[KONAMI] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[NEO GEO] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[PAC-MAN] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[PGM] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[PSIKYO] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[SEGA] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[SETA] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[TAITO] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[TECHNOS] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[TOAPLAN] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[MISC PRE 90\'s] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[MISC POST 90's] INPUT .CFG PATH");
}

void c_fbaRL::EndOptionsMenu()
{
	SAFE_DELETE(options_menu)
}

void c_fbaRL::InitZipInfoMenu()
{
	if(nFilteredGames < 1) return;

	zipinfo_menu = new c_Menu(34);

	uint32_t		i;
	mz_bool			bZipStatus;
	//size_t			uncomp_size;
	mz_zip_archive	zip_archive;

	// Now try to open the archive.
	memset(&zip_archive, 0, sizeof(zip_archive));
	bZipStatus = mz_zip_reader_init_file(&zip_archive, fgames[nSelectedGame]->path, 0);
	if (!bZipStatus)
	{
		//FILE* fp = fopen("/dev_hdd0/game/FBAL00123/USRDIR/zip_error_log.txt", "w");
		//if(fp) {
		//	fprintf(fp, "mz_zip_reader_init_file() failed!\n");
		//	fclose(fp);
		//	fp = NULL;
		//}

		SAFE_DELETE(zipinfo_menu)

		return;
	}

	// Get and print information about each file in the archive.
	for (i = 0; i < mz_zip_reader_get_num_files(&zip_archive); i++)
	{
		mz_zip_archive_file_stat file_stat;
		if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat))
		{
			//FILE* fp = fopen("/dev_hdd0/game/FBAL00123/USRDIR/zip_error_log.txt", "w");
			//if(fp) {
			//	fprintf(fp, "mz_zip_reader_file_stat() failed!\n");
			//	fclose(fp);
			//	fp = NULL;
			//}
			mz_zip_reader_end(&zip_archive);

			SAFE_DELETE(zipinfo_menu)

			return;
		}

		char* pszTmp = NULL;
		pszTmp = (char*)malloc(1024);
		memset(pszTmp, 0, 1024);

		sprintf(pszTmp, " [ %s ] [SIZE: %u bytes] [CRC32: %08X] \n", 
			file_stat.m_filename,
			(uint32_t)file_stat.m_uncomp_size, 
			//(uint32_t)file_stat.m_comp_size,
			(uint32_t)file_stat.m_crc32
		);

		zipinfo_menu->AddItem((char*)pszTmp);

		SAFE_FREE(pszTmp)
	}
}

void c_fbaRL::EndZipInfoMenu()
{
	SAFE_DELETE(zipinfo_menu)
}

// TEMP
#define ROMINFO_DB	"/dev_hdd0/game/FBAL00123/USRDIR/fbarl-rominfo.txt"

#define _getBasicInfo(out)				\
	pch = strchr(pszLine, ';');			\
	if(pch) {							\
		int sep	= (int)(pch-pszLine);	\
		memset(&out, 0, sizeof(out));	\
		strncpy(out, pszLine, sep);		\
	}

#define _getline()						\
	memset(pszLine, 0, 2048+1);			\
	fgets(pszLine, 2048, fp);			

struct BurnRomInfo {
	char szName[64];
	uint32_t nLen;
	uint32_t nCrc;
	char szType[128];
};

struct BurnSampleInfo {
	char szName[64];
	uint32_t nFlags;
};

struct FBA_ROMINFO {
	unsigned int nDrv;
	char szName[32];
	char szBoardrom[32];
	
	BurnRomInfo roms[100];
	unsigned int nRoms;
	
	BurnRomInfo board_roms[100];
	unsigned int nBoardRoms;
	
	BurnSampleInfo samples_info[100];
	unsigned int nSamples;
};

int getRominfo(FILE* fp, char* szRomset, FBA_ROMINFO *rinfo)
{
	if(!fp) return NULL;

	rewind(fp);

	char* pszLine = NULL;
	pszLine = (char*)malloc(2048+1);
	memset(pszLine, 0, 2048+1);
	
	FBA_ROMINFO rominfo;
	memset(&rominfo, 0, sizeof(FBA_ROMINFO));

	while (!feof(fp)) 
	{
		_getline();

		// end of database
		if(strncmp("<//EOF>", pszLine, 7) == 0) { break; }

		// ignore comments
		if(strncmp("//", pszLine, 2) == 0) { continue; }
		
		// rominfo segment end
		if(strncmp("<//>", pszLine, 4) == 0) { rominfo.nDrv++; continue; }

		// rominfo segment start
		if(strncmp("<--->", pszLine, strlen("<--->")) == 0) 
		{
			char* pch = NULL;

			_getline();
			char szTmp[16] = { 0 };
			_getBasicInfo(szTmp);
			sscanf(szTmp, "%d", &rominfo.nDrv);

			_getline();
			_getBasicInfo(rominfo.szName);

			if(strcmp(rominfo.szName, szRomset) == 0) 
			{
				_getline();
				_getBasicInfo(rominfo.szBoardrom);

				_getline(); // <--- this line is a comment
				_getline();

				if(pszLine[0] == '{')
				{
					rominfo.nRoms = 0;

					while (pszLine[0] != '}')
					{
						_getline();
						if(pszLine[0] == '}') break;

						int nField = 0;

						pch = strtok(pszLine, "/");
						while (pch != NULL) 
						{
							if(pch[0] == '\n') break;

							switch(nField)
							{
								// rom name
								case 0:
								{
									strcpy(rominfo.roms[rominfo.nRoms].szName, pch);
									break;
								}
								// rom size (bytes)
								case 1:
								{
									char szTmp[32] = { 0 };
									strcpy(szTmp, pch);
									sscanf(szTmp, "%d", &rominfo.roms[rominfo.nRoms].nLen);
									break;
								}
								// crc32 (hex)
								case 2:
								{
									char szTmp[32] = { 0 };
									strcpy(szTmp, pch);
									sscanf(szTmp, "%X", &rominfo.roms[rominfo.nRoms].nCrc);
									break;
								}
								// ROM Type
								case 3:
								{
									strcpy(rominfo.roms[rominfo.nRoms].szType, pch);
									break;
								}
							} // switch

							pch = strtok(NULL, "/");
							nField++;
						} // while
						rominfo.nRoms++;
					} // while
				} // if

				_getline(); // <--- this line is a comment
				_getline();

				if(pszLine[0] == '{')
				{

					rominfo.nBoardRoms = 0;
					while (pszLine[0] != '}')
					{
						_getline();
						if(pszLine[0] == '}') break;
						if(strncmp(pszLine, "NULL/", strlen("NULL/"))==0) break;

						int nField = 0;

						pch = strtok(pszLine, "/");
						while (pch != NULL) 
						{
							if(pch[0] == '\n') break;

							switch(nField)
							{
								// rom name
								case 0:
								{
									strcpy(rominfo.board_roms[rominfo.nBoardRoms].szName, pch);
									break;
								}
								// rom size (bytes)
								case 1:
								{
									char szTmp[32] = { 0 };
									strcpy(szTmp, pch);
									sscanf(szTmp, "%d", &rominfo.board_roms[rominfo.nBoardRoms].nLen);
									break;
								}
								// crc32 (hex)
								case 2:
								{
									char szTmp[32] = { 0 };
									strcpy(szTmp, pch);
									sscanf(szTmp, "%X", &rominfo.board_roms[rominfo.nBoardRoms].nCrc);
									break;
								}
								// ROM Type
								case 3:
								{
									strcpy(rominfo.board_roms[rominfo.nBoardRoms].szType, pch);
									break;
								}
							} // switch
							pch = strtok(NULL, "/");
							nField++;
						} // while
						rominfo.nBoardRoms++;
					} // while
				} // if

				_getline(); // <--- this line is a comment
				_getline();

				if(pszLine[0] == '{')
				{
					rominfo.nSamples = 0;
					while (pszLine[0] != '}')
					{
						_getline();
						if(pszLine[0] == '}') break;
						if(strncmp(pszLine, "NULL/", strlen("NULL/"))==0) break;

						int nField = 0;

						pch = strtok(pszLine, "/");
						while (pch != NULL) 
						{
							if(pch[0] == '\n') break;

							switch(nField)
							{
								// sample name
								case 0:
								{
									strcpy(rominfo.samples_info[rominfo.nSamples].szName, pch);
									break;
								}
								// sample flags
								case 1:
								{
									char szTmp[32] = { 0 };
									strcpy(szTmp, pch);
									sscanf(szTmp, "%d", &rominfo.samples_info[rominfo.nSamples].nFlags);
									break;
								}
							} // switch
							pch = strtok(NULL, "/");
							nField++;
						} // while
						rominfo.nSamples++;
					} // while
				} // if
				
				memcpy(rinfo, &rominfo, sizeof(FBA_ROMINFO));
				return 1;				
			} // if	
		} // if
	} // while
	return 0;
}

void c_fbaRL::InitRomInfoMenu()
{
	if(nFilteredGames < 1) return;

	rominfo_menu = new c_Menu(34);

// ---

	FILE* fp = NULL;
	fp = fopen(ROMINFO_DB, "r");

	if(fp) 
	{
		FBA_ROMINFO *rominfo = NULL;
		rominfo = (FBA_ROMINFO*) malloc(sizeof(FBA_ROMINFO));
		memset(rominfo, 0, sizeof(FBA_ROMINFO)); 

		int nLen = strlen(fgames[nSelectedGame]->zipname);
		char szRomset[32] = {0};
		strncpy(szRomset, fgames[nSelectedGame]->zipname, nLen-4); // without ".zip"
		if(!getRominfo(fp, szRomset, rominfo)) {
			//printf("ROM info not found on database...\n"); 
		} else {
			// ---------------------------
			// DEBUG OUTPUT
			// ---------------------------

			//printf("-- driver basic info --\n");

			/*printf(
				"nDrv: %d\n"
				"Name: %s\n"
				"Boardrom: %s\n", 
				rominfo->nDrv, 
				rominfo->szName, 
				rominfo->szBoardrom
				);
				*/
			//printf("-- roms --\n");

			for(unsigned int n = 0; n < rominfo->nRoms; n++)
			{
				char* pszTmp = NULL;
				pszTmp = (char*)malloc(1024);
				memset(pszTmp, 0, 1024);
		
				sprintf(pszTmp,
					"[ %s ][SIZE: %d bytes][CRC32: %X]", 
					rominfo->roms[n].szName, 
					rominfo->roms[n].nLen, 
					rominfo->roms[n].nCrc
				);

				rominfo_menu->AddItem((char*)pszTmp);
				SAFE_FREE(pszTmp)
			}

			//printf("-- board roms --\n");

			for(unsigned int n = 0; n < rominfo->nBoardRoms; n++)
			{
				char* pszTmp = NULL;
				pszTmp = (char*)malloc(1024);
				memset(pszTmp, 0, 1024);

				sprintf(pszTmp,
					"[ %s ][SIZE: %d bytes][CRC32: %X]",
					rominfo->board_roms[n].szName, 
					rominfo->board_roms[n].nLen, 
					rominfo->board_roms[n].nCrc
				);

				rominfo_menu->AddItem((char*)pszTmp);
				SAFE_FREE(pszTmp)
			}

			//printf("-- samples --\n");
			/*
			for(int n = 0; n < rominfo->nSamples; n++){
				printf(
					"[ %s ][ %d ]", 
					rominfo->samples_info[n].szName, 
					rominfo->samples_info[n].nFlags
				);
			}
			*/
		}

		fclose(fp);
		fp = NULL;
	}
}

void c_fbaRL::EndRomInfoMenu()
{
	SAFE_DELETE(rominfo_menu)
}
