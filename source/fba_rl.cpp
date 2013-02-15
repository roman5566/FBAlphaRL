// ---------------------------------------------------------------------
// FB Alpha Retro Loader (CaptainCPS-X, 2013)
// ---------------------------------------------------------------------
/*

Custom RETROARCH changes for FB Alpha RL (v1.XX):

* platform_ps3.c 

	- Changed custom RetroArch "EMULATOR_CONTENT_DIR" macro to "FBAL00123" (previously set to "SSNE10000"),
	this will allow this custom RetroArch to be independent from an existing RetroArch installation
	on the PS3.

	- Implemented extra agument checking support when launched externally

	Note: Arguments should be passed as follows (argv[])

		> Argument 1 :	This should have a ROM / ZIP valid path in case of loading a game. 
		Or could just be a dummy argument.

		> Argument 2 :	This should have the path of the external Application RELOAD.SELF
		!!! Must be set in this custom RetroArch build !!!

		> Argument 3 :  This should have a valid path of a Input Preset Configuration file.
		(Ex. "/dev_hdd0/.../neogeo.cfg")
		Or could just be a dummy argument.

		> Argument 4 : This should have the Aspect Ratio of the game (Ex. "4:3" as char string)
		Or could be just a dummy argument

		> Argument 5 : Use alternate RetroArch menu key combo ("yes" or "no" as char string)

* rmenu.c / rmenu.h

	- Added code to handle Alternate Meny Key combos.

	- Modified / Added menu items for in-game menu and main menu so RetroArch display the
	proper name if launched from FB Alpha RL (Ex. "Return to FB alpha RL").

* Makefile.ps3 
	
	- edited with custom signing using scetool for Salamander / CORE

*/

// --------------------------------------------------------------------
#define _APP_TITLE "\tFB ALPHA RETRO LOADER v1.01 - by CaptainCPS-X [2013]"
// --------------------------------------------------------------------

//#define FBARL_DEBUG
#include "main.h"
//
#include "miniz.c"
#include "burn_drivers.h"

void _ScanROMs(uint64_t /*arg*/);
void LaunchRetroArch(char* arg1, char* arg2, char* arg3, char* arg4, char* arg5 );

// For creating basic input presets
extern unsigned char example_cfg[20570];

c_fbaRL* fbaRL;

c_fbaRL::c_fbaRL() 
{
	bProcessingGames = false;

	MakeRetroDirs((char*)"/dev_hdd0/game/FBAL00123/USRDIR");
	
	if(DirExist((char*)"/dev_hdd0/game/FBAL00123/USRDIR")) 
	{
		MakeRetroDirs((char*)"/dev_hdd0/game/SSNE10000/USRDIR");
	}

	if(g_opt_bAutoInputCfgCreate) {
		CreateAllInputCFG();
	}

	InitMainMenu();
	nSection = SECTION_MAIN;
}

void c_fbaRL::CreateAllInputCFG()
{
	if(DirExist((char*)"/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input")) 
	{
		char szInputCFG[][2048] = {
			"/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input/capcom_misc.cfg",
			"/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input/cave.cfg",
			"/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input/cps1.cfg",
			"/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input/cps2.cfg",
			"/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input/cps3.cfg",
			"/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input/data_east.cfg",
			"/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input/galaxian.cfg",
			"/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input/irem.cfg",
			"/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input/kaneko.cfg",
			"/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input/konami.cfg",
			"/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input/neogeo.cfg",
			"/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input/pacman.cfg",
			"/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input/pgm.cfg",
			"/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input/psikyo.cfg",
			"/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input/sega.cfg",
			"/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input/seta.cfg",
			"/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input/taito.cfg",
			"/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input/technos.cfg",
			"/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input/toaplan.cfg",
			"/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input/misc_pre90.cfg",
			"/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input/misc_post90.cfg"
		};
		for(uint32_t n = 0; n < sizeof(szInputCFG) / 2048; n++)
		{
			if(!FileExist(szInputCFG[n]))
			{
				FILE* fp = fopen(szInputCFG[n], "w");
				if(fp)
				{
					fwrite(example_cfg, 20570, 1, fp);
					fclose(fp);
					*&fp = NULL;
				}
			}
		}
	}
}

#define MAX_GAMES	30000

// ---------------------------------------------------------------------
//
// TODO: Add this to c_fbaRL / fba_rl.h
//

int nBurnSelected = -1;

char szSysFilter[MASKALL][256] = { 
	"MASKCAPMISC"		,"MASKCAVE"		,"MASKCPS"		,"MASKCPS2",
	"MASKCPS3"			,"MASKDATAEAST"	,"MASKGALAXIAN"	,"MASKIREM",
	"MASKKANEKO"		,"MASKKONAMI"	,"MASKNEOGEO"	,"MASKPACMAN",
	"MASKPGM"			,"MASKPSIKYO"	,"MASKSEGA"		,"MASKSETA",
	"MASKTAITO"			,"MASKTECHNOS"	,"MASKTOAPLAN"	,"MASKMISCPRE90S",
	"MASKMISCPOST90S"	
	//,"MASKMEGADRIVE","MASKPCENGINE"	,"MASKSNES"
};

void c_fbaRL::ResetPreviewImage()
{
	if(app.textures[TEX_PREVIEW]->bTextureOK) 
	{
		delete app.textures[TEX_PREVIEW];
		*&app.textures[TEX_PREVIEW] = NULL;
	}
	// no preview available...
	app.textures[TEX_PREVIEW] = new c_tex(TEX_PREVIEW, g_opt_szTextures[TEX_PREVIEW]);

}

void c_fbaRL::UpdatePreviewImage()
{
	if(nBurnSelected < 0) {
		ResetPreviewImage();
		//nStatus = STATUS_RESETPREVIEW;
		return;
	}

	// clean memory first...
	if(app.textures[TEX_PREVIEW]->bTextureOK) 
	{
		delete app.textures[TEX_PREVIEW];
		*&app.textures[TEX_PREVIEW] = NULL;
	}

	if(nSection == SECTION_GAMELIST)
	{
		char szPreviewPath[2048] = { 0 };
		sprintf(szPreviewPath, "/dev_hdd0/game/FBAL00123/USRDIR/cores/previews/%s.png", fba_drv[nBurnSelected].szName);
		
		// try main romset name...
		app.textures[TEX_PREVIEW] = new c_tex(TEX_PREVIEW, szPreviewPath);
		
		// try parent...
		if(!app.textures[TEX_PREVIEW]->bTextureOK) 
		{
			// clean memory again...
			delete app.textures[TEX_PREVIEW];
			*&app.textures[TEX_PREVIEW] = NULL;
			
			sprintf(szPreviewPath, "/dev_hdd0/game/FBAL00123/USRDIR/cores/previews/%s.png", fba_drv[nBurnSelected].szParent);
			
			app.textures[TEX_PREVIEW] = new c_tex(TEX_PREVIEW, szPreviewPath);
			//app.textures[TEX_PREVIEW]->BindTexture(szPreviewPath);
		}

		// no preview available...
		if(!app.textures[TEX_PREVIEW]->bTextureOK) {
			//nStatus = STATUS_RESETPREVIEW;
			ResetPreviewImage();
		}
	}
}

int GetSystemMaskId(char* szMask)
{
	for(uint32_t n = 0; n < sizeof(szSysFilter) / 256; n++)
	{
		if(strcmp(szMask, szSysFilter[n]) == 0)
		{
			return n;
		}
	}
	return -1;
}

char* GetSystemFilter(int nFilter)
{
	char* pszFilter = NULL;
	pszFilter = (char*)malloc(256);
	memset(pszFilter, 0, 256);

	switch(nFilter)
	{
		case 0:  strcpy(pszFilter, "Capcom Misc"); break;
		case 1:  strcpy(pszFilter, "Cave"); break;
		case 2:  strcpy(pszFilter, "Capcom Play System I (CPS-I)"); break;
		case 3:  strcpy(pszFilter, "Capcom Play System II (CPS-II)"); break;
		case 4:  strcpy(pszFilter, "Capcom Play System III (CPS-III)"); break;
		case 5:  strcpy(pszFilter, "Data East"); break;
		case 6:  strcpy(pszFilter, "Galaxian"); break;
		case 7:  strcpy(pszFilter, "Irem"); break;
		case 8:  strcpy(pszFilter, "Kaneko"); break;
		case 9:  strcpy(pszFilter, "Konami"); break;
		case 10:  strcpy(pszFilter, "Neo Geo"); break;
		case 11:  strcpy(pszFilter, "Pac-Man"); break;
		case 12:  strcpy(pszFilter, "Poly Game Master (PGM)"); break;
		case 13:  strcpy(pszFilter, "Psikyo"); break;
		case 14:  strcpy(pszFilter, "Sega"); break;
		case 15:  strcpy(pszFilter, "Seta"); break;
		case 16:  strcpy(pszFilter, "Taito"); break;
		case 17:  strcpy(pszFilter, "Technos"); break;
		case 18:  strcpy(pszFilter, "Toaplan"); break;
		case 19:  strcpy(pszFilter, "Misc Pre 90\'s"); break;
		case 20:  strcpy(pszFilter, "Misc Post 90\'s"); break;
		//case 21:  strcpy(pszFilter, "Megadrive"); break;
		//case 22:  strcpy(pszFilter, "PC Engine"); break;
		//case 23:  strcpy(pszFilter, "SNES"); break;
		case 21:  strcpy(pszFilter, "All Systems"); break;
		case 22:  strcpy(pszFilter, "Custom System Filter"); break;
	}
	return pszFilter;
}

// --------------------------------------------------------------------

bool bSysFilter[MASKCUSTOM+1] = 
{
	true,true,true,true,
	true,true,true,true,
	true,true,true,true,
	true,true,true,true,
	true,true,true,true,
	true,
	true,true // <--- fillers
};

void c_fbaRL::NextSysFilter()
{
	bProcessingGames = true;

	// custom --> first single filter
	if(g_opt_nActiveSysFilter == MASKCUSTOM)	
	{
		g_opt_nActiveSysFilter = 0;

		for(uint32_t n = 0; n < sizeof(bSysFilter)-2; n++){
			bSysFilter[n] = false;
		}

		bSysFilter[g_opt_nActiveSysFilter] = true;

		// refresh filters
		EndFilterList();
		InitFilterList();
		return;
	}

	// show all --> custom
	if(g_opt_nActiveSysFilter == MASKALL)	
	{
		g_opt_nActiveSysFilter = MASKCUSTOM;

		// use values from FBA_RL.ini
		for(uint32_t n = 0; n < sizeof(bSysFilter)-2; n++){
			bSysFilter[n] = g_opt_bCustomSysFilter[n];
		}

		// refresh filters
		EndFilterList();
		InitFilterList();
		return;
	}

	// next single system filter --> ...
	g_opt_nActiveSysFilter++;

	for(uint32_t n = 0; n < sizeof(bSysFilter)-2; n++){
		bSysFilter[n] = false;
	}

	bSysFilter[g_opt_nActiveSysFilter] = true;

	// refresh filters
	EndFilterList();
	InitFilterList();

	bProcessingGames = false;
}

void c_fbaRL::PrevSysFilter()
{
	bProcessingGames = true;

	// custom --> show all
	if(g_opt_nActiveSysFilter == MASKCUSTOM)	
	{
		g_opt_nActiveSysFilter = MASKALL;

		for(uint32_t n = 0; n < sizeof(bSysFilter)-2; n++){
			bSysFilter[n] = false;
		}

		bSysFilter[g_opt_nActiveSysFilter] = true;

		// refresh filters
		EndFilterList();
		InitFilterList();
		return;
	}

	// first single filter --> custom
	if(g_opt_nActiveSysFilter == 0)	
	{
		g_opt_nActiveSysFilter = MASKCUSTOM;

		// use values from FBA_RL.ini
		for(uint32_t n = 0; n < sizeof(bSysFilter)-2; n++){
			bSysFilter[n] = g_opt_bCustomSysFilter[n];
		}

		// refresh filters
		EndFilterList();
		InitFilterList();
		return;
	}

	// previous single system filter --> ...
	g_opt_nActiveSysFilter--;

	for(uint32_t n = 0; n < sizeof(bSysFilter)-2; n++){
		bSysFilter[n] = false;
	}

	bSysFilter[g_opt_nActiveSysFilter] = true;

	// refresh filters
	EndFilterList();
	InitFilterList();

	bProcessingGames = false;
}

bool c_fbaRL::FilterGame(char* szSystemMask)
{
	// show all?
	if(g_opt_nActiveSysFilter == MASKALL) {
		return true;
	}

	if(g_opt_nActiveSysFilter == MASKCUSTOM) 
	{
		// filter...
		for(uint32_t n=0; n < sizeof(bSysFilter)-2; n++)
		{
			if(strcmp(szSystemMask, szSysFilter[n]) == 0)
			{
				if(g_opt_bCustomSysFilter[n] == true) {
					return true;
				} else {
					return false;
				}
			}
		}

	}

	if(strcmp(szSystemMask, szSysFilter[g_opt_nActiveSysFilter]) == 0)
	{
		return true;
	}

	return false;
}

// RetroArch doesn't do this, so let's help it out :)
void c_fbaRL::MakeRetroDirs(char* szMainPath)
{
	if(!DirExist(szMainPath)) return;

	char szRetroDirs[][256] = 
	{ 
		"previews",
		"presets",
		"presets/input",
		"roms",
		"roms/fba",
		"roms/snes",
		"roms/genesis",
		"roms/nes",
		"roms/neopocket",
		"roms/pce",
		"roms/doom",
		"roms/wonderswan",
		"roms/vba",
		"savestates",
		"sram",
		"system"
	};

	for(uint32_t n = 0; n < (sizeof(szRetroDirs) / 256); n++)
	{
		char* szDir = (char*)malloc(1024);
		sprintf(szDir, "%s/cores/%s", szMainPath, szRetroDirs[n]);

		if(!DirExist(szDir)) {
			cellFsMkdir(szDir, CELL_FS_S_IFDIR | 0777);
		}
		SAFE_FREE(szDir)
	}
}

bool c_fbaRL::DirExist(char* szDir)
{
	struct CellFsStat st;
	if(cellFsStat(szDir,&st) == 0 && ((st.st_mode & S_IFMT) == S_IFDIR)) 
	{
		return true;
	} else {
		return false;
	}
}

bool c_fbaRL::FileExist(char* szFile)
{
	struct CellFsStat st;
	if(cellFsStat(szFile,&st) == 0 && ((st.st_mode & S_IFMT) == S_IFREG)) 
	{
		return true;
	} else {
		return false;
	}
}

void c_fbaRL::InitMainMenu()
{
	main_menu = new c_Menu(7);

	main_menu->AddItem((char*)"GAME LIST");
	main_menu->AddItem((char*)"OPTIONS");
	main_menu->AddItem((char*)"LOAD \"FB Alpha Core\" MOD");
	//main_menu->AddItem((char*)"LOAD \"RetroArch\"");
	main_menu->AddItem((char*)"LOAD \"multiMAN\"");
	main_menu->AddItem((char*)"EXIT");
}

void c_fbaRL::EndMainMenu()
{
	SAFE_DELETE(main_menu)
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

void c_fbaRL::InitFilterList()
{
	bProcessingGames = true;

	nBurnSelected = -1;

	nSelectedGame = 0;
	nGameListTop = 0;
	nFilteredGames = 0;

	fgames = (c_game**)malloc(sizeof(c_game) * MAX_GAMES);
	//memset(fgames, 0, sizeof(c_game) * MAX_GAMES);

	// process...
	
	for(int n=0; n < nTotalGames; n++) 
	{
		if(!g_opt_bDisplayMissingGames && !games[n]->bAvailable)
		{
			continue;
		}

		if(FilterGame(games[n]->sysmask) == false)
		{
			continue;
		}
		fgames[nFilteredGames] = new c_game(nFilteredGames);

		strcpy(fgames[nFilteredGames]->path		, games[n]->path);
		strcpy(fgames[nFilteredGames]->zipname	, games[n]->zipname);
		strcpy(fgames[nFilteredGames]->title	, games[n]->title);
		strcpy(fgames[nFilteredGames]->sysmask	, games[n]->sysmask);
		memcpy(&fgames[nFilteredGames]->nSize	, &games[n]->nSize, sizeof(uint64_t));
		memcpy(&fgames[nFilteredGames]->bAvailable	, &games[n]->bAvailable, sizeof(bool));

		nFilteredGames++;
	}

	if(nFilteredGames < 1) {
		bProcessingGames = false;
		ResetPreviewImage();
		return;
	}

	// Sort Filtered list by Title
	qsort(fgames, nFilteredGames, sizeof(c_game*), _FcCompareStruct);
	bProcessingGames = false;

	nStatus = STATUS_UPDATEPREVIEW;
}

void c_fbaRL::EndFilterList()
{
	bProcessingGames = true;

	for(int n = 0; n < nFilteredGames; n++)	{
		SAFE_DELETE(fgames[n])
	}
	SAFE_FREE(fgames);

	nSelectedGame = 0;
	nGameListTop = 0;
	nFilteredGames = 0;

	bProcessingGames = false;
}

void c_fbaRL::InitOptionsMenu()
{
	options_menu = new c_Menu(7);

	options_menu->AddItem((char*)"AUTO ASPECT RATIO");
	options_menu->AddItem((char*)"AUTO CREATE ALL INPUT .CFG");
	options_menu->AddItem((char*)"ALTERNATE MENU KEY COMBO");
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

		sprintf(pszTmp, "\t[ %s ] [SIZE: %u bytes] [CRC32: %08X] \n", 
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

uint32_t c_fbaRL::GetBurnDriverID(char* szRomzip)
{
	for(uint32_t nDrv = 0; nDrv < sizeof(fba_drv) / sizeof(FBA_DRV); nDrv++)
	{
		char szDrvZip[32] = { 0 };
		sprintf(szDrvZip, "%s.zip", fba_drv[nDrv].szName);

		if(strncmp(szRomzip, szDrvZip, strlen(szDrvZip)) == 0)
		{
			return nDrv;
		}
	}
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

void DlgCallbackFunction(int buttonType, void */*userData*/)
{
	switch( buttonType ) 
	{
	case CELL_MSGDIALOG_BUTTON_YES:
	case CELL_MSGDIALOG_BUTTON_NO:		
	case CELL_MSGDIALOG_BUTTON_ESCAPE:
	case CELL_MSGDIALOG_BUTTON_NONE:
		break;

	default:
		break;
	}
}

void ExitDlgCallbackFunction(int buttonType, void *userData)
{
	int nDialog = (int)userData;

	switch( buttonType ) 
	{
		case CELL_MSGDIALOG_BUTTON_YES:
		{
			if(nDialog == STATUS_ROMSCAN_DLG)
			{
				cellMsgDialogAbort();

				iniWrite();			// save settings

				// delete old cache
				cellFsUnlink("/dev_hdd0/game/FBAL00123/USRDIR/FBA.GAMELIST.CACHE.DAT");
				
				fbaRL->RefreshGameList();	// Refresh gamelist
				break;
			}

			if(nDialog == STATUS_EXIT_MMAN)
			{
				cellMsgDialogAbort();

				iniWrite(); // save settings

				app.onShutdown();

				char path[] = "/dev_hdd0/game/BLES80608/USRDIR/RELOAD.SELF";
				if(fbaRL->FileExist(path)) 
				{ 
					sys_game_process_exitspawn(path, NULL, NULL, NULL, 0, 1000, SYS_PROCESS_PRIMARY_STACK_SIZE_1M);
				}
				fbaRL->EndMainMenu();
				break;
			}

			if(nDialog == STATUS_EXIT_FBARL_RA)
			{
				cellMsgDialogAbort();

				iniWrite(); // save settings

				app.onShutdown();

				char fba_rl_path[]	= "/dev_hdd0/game/FBAL00123/USRDIR/RELOAD.SELF";
				char rom_path[]		= "DUMMY_ARG";
				char preset_cfg[]	= "DUMMY_ARG";
				char aspect_ratio[]	= "DUMMY_ARG";

				if(fbaRL->FileExist(fba_rl_path)) 
				{
					LaunchRetroArch(
						(char*)rom_path, 
						(char*)fba_rl_path, 
						(char*)preset_cfg,
						(char*)aspect_ratio,
						(char*)(g_opt_bUseAltMenuKeyCombo ? "yes" : "no")
					);
				}
				fbaRL->EndMainMenu();
				break;
			}

			if(nDialog == STATUS_EXIT)
			{
				cellMsgDialogAbort();

				iniWrite(); // save settings
				app.onShutdown();
				exit(0);
				fbaRL->EndMainMenu();
				break;
			}
			break;
		}

		case CELL_MSGDIALOG_BUTTON_NO:	
		case CELL_MSGDIALOG_BUTTON_ESCAPE:
		case CELL_MSGDIALOG_BUTTON_NONE:
			break;

		default:
			break;
	}
}

int c_Menu::UpdateTopItem()
{
	if(nSelectedItem > nListMax || nTopItem > 0)
	{
		if(nTopItem < (nSelectedItem - nListMax))
		{
			nTopItem = nSelectedItem - nListMax;
		}

		if(nTopItem > 0) 
		{
			if(nSelectedItem < nTopItem)
			{
				nTopItem = nSelectedItem;
			}
		} else {
			nTopItem = nSelectedItem - nListMax;
		}
	} else {
		nTopItem = nSelectedItem - nListMax;
	}

	if(nTopItem < 0) nTopItem = 0;

	return nTopItem;
}

int c_Filebrowser::UpdateTopItem()
{
	if(nSelectedItem > nListMax || nTopItem > 0)
	{
		if(nTopItem < (nSelectedItem - nListMax))
		{
			nTopItem = nSelectedItem - nListMax;
		}

		if(nTopItem > 0) 
		{
			if(nSelectedItem < nTopItem)
			{
				nTopItem = nSelectedItem;
			}
		} else {
			nTopItem = nSelectedItem - nListMax;
		}
	} else {
		nTopItem = nSelectedItem - nListMax;
	}

	if(nTopItem < 0) nTopItem = 0;

	return nTopItem;
}

void c_fbaRL::InitFileBrowser()
{
	filebrowser = new c_Filebrowser(36);

	if(fbaRL->nFileBrowserType == 1) {
		strcpy(filebrowser->pszCurrentDir, "/dev_hdd0/game/FBAL00123/USRDIR/cores/presets/input/"); // <-- left the last slash to make it start in this directory
	} else {
		strcpy(filebrowser->pszCurrentDir, "/dev_hdd0/game/FBAL00123/USRDIR/cores/roms/");
	}

	filebrowser->NavigateDirUp();
}

void c_fbaRL::EndFileBrowser()
{
	SAFE_DELETE(filebrowser);
}

c_Filebrowser::c_Filebrowser(int nListMaximum)
{
	nSelectedItem	= 0;
	nTopItem		= 0;
	nTotalItem		= 0;
	nListMax		= nListMaximum;

	pszCurrentDir = (char*)malloc(2048);
	memset(pszCurrentDir, 0, 2048);
}

c_Filebrowser::~c_Filebrowser()
{
	for(int n = 0; n < nTotalItem; n++)
	{
		SAFE_DELETE(item[n])
	}
	*&nSelectedItem = NULL;
	*&nTopItem		= NULL;
	*&nTotalItem	= NULL;
	*&nListMax		= NULL;
}

void c_Filebrowser::AddItem(char* szItemLabel, char* szPath, uint8_t _nType) 
{
	item[nTotalItem] =  new c_FilebrowserItem(nTotalItem);

	strcpy(item[nTotalItem]->szMenuLabel, szItemLabel);
	strcpy(item[nTotalItem]->szPath, szPath);
	memcpy(&item[nTotalItem]->nType, &_nType, sizeof(uint8_t)); 

	nTotalItem++;
}
	
void c_Filebrowser::NavigateSelectedDir()
{
	if(nTotalItem < 1) return;
	
	if(item[nSelectedItem]->nType != CELL_FS_TYPE_DIRECTORY) return;

	strcpy(pszCurrentDir, item[nSelectedItem]->szPath);

	CleanUpItems();
	ScanContents();
}

void c_Filebrowser::NavigateDirUp()
{
	if(strlen(pszCurrentDir) == 0)
	{
		if(nTotalItem >= 1) {
			CleanUpItems();
		}

		if(fbaRL->DirExist((char*)"/dev_hdd0")) {
			AddItem((char*)"/dev_hdd0", (char*)"/dev_hdd0", CELL_FS_TYPE_DIRECTORY);
		}

		for(int n = 0; n < 30; n++)
		{		
			char *szUsb = NULL;
			szUsb = (char*)malloc(256);
			memset(szUsb, 0, 256);
			
			sprintf(szUsb, "/dev_usb00%d", n);
			if(fbaRL->DirExist(szUsb)) {
				AddItem(szUsb, szUsb, CELL_FS_TYPE_DIRECTORY);
			}
			SAFE_FREE(szUsb)
		}

		if(fbaRL->DirExist((char*)"/dev_cf")) {
			AddItem((char*)"/dev_cf", (char*)"/dev_cf", CELL_FS_TYPE_DIRECTORY);
		}

		if(fbaRL->DirExist((char*)"/dev_sd")) {
			AddItem((char*)"/dev_sd", (char*)"/dev_sd", CELL_FS_TYPE_DIRECTORY);
		}

		if(fbaRL->DirExist((char*)"/dev_ms")) {
			AddItem((char*)"/dev_ms", (char*)"/dev_ms", CELL_FS_TYPE_DIRECTORY);
		}

	} else {
		
		char* pch = strrchr(pszCurrentDir, '/');
		pszCurrentDir[pch-pszCurrentDir] = 0;

		if(strlen(pszCurrentDir) == 0) {
			return NavigateDirUp();
		}

		CleanUpItems();
		ScanContents();
	}
}

void c_Filebrowser::ScanContents()
{
	int d;		
	*&d = NULL;

	if(cellFsOpendir(pszCurrentDir, &d) == CELL_FS_SUCCEEDED) 
	{
		CellFsDirent* dirEntry = NULL;
		dirEntry = (CellFsDirent*)malloc(sizeof(CellFsDirent));
		memset(dirEntry, 0, sizeof(CellFsDirent));

		uint64_t nread = 0;

		// start scanning for directories...
		uint64_t nFilesProcessed = 0;

		while(cellFsReaddir(d, dirEntry, &nread) == CELL_FS_SUCCEEDED)			
		{
			if(nread == 0) break; 
			if(nFilesProcessed > 1000) break; // We are not trying to do magic here...

			if (fbaRL->nFileBrowserType == 0 && dirEntry->d_type == CELL_FS_TYPE_DIRECTORY)
			{
				if( strcmp(dirEntry->d_name, ".\0") == 0 ||
					strcmp(dirEntry->d_name, "..\0") == 0
				  )
				{
					continue;
				}

				// DIR
				char* pszTmp = NULL;
				pszTmp = (char*)malloc(2048);
				memset(pszTmp, 0, 2048);

				sprintf(pszTmp, "%s/%s", pszCurrentDir, dirEntry->d_name);

				// ------------------------------
				AddItem(dirEntry->d_name, pszTmp, dirEntry->d_type);
				// ------------------------------

				SAFE_FREE(pszTmp)
			} 

			if(fbaRL->nFileBrowserType == 1 && dirEntry->d_type == CELL_FS_TYPE_DIRECTORY) 
			{
				if( strcmp(dirEntry->d_name, ".\0") == 0 ||
					strcmp(dirEntry->d_name, "..\0") == 0
				  )
				{
					continue;
				}

				// DIR
				char* pszTmp = NULL;
				pszTmp = (char*)malloc(2048);
				memset(pszTmp, 0, 2048);

				sprintf(pszTmp, "%s/%s", pszCurrentDir, dirEntry->d_name);

				// ------------------------------
				AddItem(dirEntry->d_name, pszTmp, dirEntry->d_type);
				// ------------------------------

				SAFE_FREE(pszTmp)
			}

			if(fbaRL->nFileBrowserType == 1 && dirEntry->d_type == CELL_FS_TYPE_REGULAR) 
			{
				// FILE

				int nNameLen = strlen(dirEntry->d_name);
				if (nNameLen < 5) continue;

				char* pszFilename = NULL;
										
				pszFilename = (char*)malloc(nNameLen+1);										
				memcpy(pszFilename, dirEntry->d_name, nNameLen);
				pszFilename[nNameLen] = 0;

				// Check file for .CFG
				if(NULL != strstr(toLowerCase(pszFilename, strlen(pszFilename)), ".cfg")) 
				{
					char* pszTmp = NULL;
					pszTmp = (char*)malloc(2048);
					memset(pszTmp, 0, 2048);

					sprintf(pszTmp, "%s/%s", pszCurrentDir, dirEntry->d_name);

					// -------------------------------
					AddItem(dirEntry->d_name, pszTmp, dirEntry->d_type);
					// -------------------------------

					SAFE_FREE(pszTmp)
				}

				if(pszFilename) {
					free(pszFilename);
					pszFilename = NULL;
				}
			}
			nFilesProcessed++;
		}
		SAFE_FREE(dirEntry)

		cellFsClosedir(d);
		*&d = NULL;
	}
}

void c_Filebrowser::CleanUpItems()
{
	for(int n = 0; n < nTotalItem; n++)
	{
		SAFE_DELETE(item[n])
	}
	nSelectedItem	= 0;
	nTopItem		= 0;
	nTotalItem		= 0;
}

void c_fbaRL::DisplayFrame()
{
	if(nSection == SECTION_FILEBROWSER)
	{
		float xPos		= 0.0585f;
		float yPos		= 0.0500f;
		float yPosDiff	= 0.0200f;	
		float nFontSize = 0.6500f;

		yPos += yPosDiff;
		yPos += yPosDiff;
		yPos += yPosDiff;
		yPos += yPosDiff;
		yPos += yPosDiff;


		cellDbgFontPrintf(xPos, 0.8900f, nFontSize, COLOR_WHITE, "CURRENT DIR: %s", filebrowser->pszCurrentDir);
		yPos += yPosDiff;

		if(filebrowser->nTotalItem >= 1)
		{			
			yPos += yPosDiff;

			xPos = 0.0700f;

			int nMenuItem = filebrowser->UpdateTopItem();

			while(nMenuItem <= (filebrowser->nTopItem + filebrowser->nListMax))
			{
				if(nMenuItem == filebrowser->nTotalItem) break;

				uint32_t nColor	= COLOR_WHITE;		// white

				// SELECTED
				if(nMenuItem == filebrowser->nSelectedItem) {
					nColor		= COLOR_YELLOW;		// yellow
				}

				cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, "%s", filebrowser->item[nMenuItem]->szMenuLabel);
				yPos += yPosDiff;

				nMenuItem++;
			}
		}
	}

	if(nSection == SECTION_OPTIONS && !bProcessingGames)
	{
		float xPos		= 0.0600f;
		float yPos		= 0.0485f;
		float yPosDiff	= 0.0400f;	
		float nFontSize = 1.0500f;

		yPos += yPosDiff;	
		yPos += yPosDiff;
		yPos += yPosDiff;
		yPos += yPosDiff;
		
		yPosDiff = 0.0800f;

		int nMenuItem = options_menu->UpdateTopItem();
		
		while(nMenuItem <= (options_menu->nTopItem + options_menu->nListMax))
		{
			if(nMenuItem == options_menu->nTotalItem) break;

			uint32_t nColor	= COLOR_WHITE;		// white

			// SELECTED
			if(nMenuItem == options_menu->nSelectedItem) 
			{
				if(nMenuItem == MENU_OPT_AUTO_AR) {
					cellDbgFontPrintf(0.0585f, 0.8900f, 0.7000f, nColor, "Auto set the proper aspect ratio on game launch.");
				}

				if(nMenuItem == MENU_OPT_AUTO_CFG) {
					cellDbgFontPrintf(0.0585f, 0.8900f, 0.7000f, nColor, "Auto create basic Input Preset CFG files for all systems.");
				}

				if(nMenuItem == MENU_OPT_ALT_MKEY) {
					cellDbgFontPrintf(0.0585f, 0.8900f, 0.7000f, nColor, "Use [SELECT] + [TRIANGLE] and [SELECT] + [SQUARE] to access RetroArch menu.");
				}

				if(nMenuItem == MENU_OPT_DISP_MISS_GMS) {
					cellDbgFontPrintf(0.0585f, 0.8900f, 0.7000f, nColor, "Enable / Disable display of missing games.");
				}

				if(nMenuItem >= MENU_OPT_FILTER_START && nMenuItem <=  MASKMISCPOST90S+MENU_OPT_FILTER_START)
				{
					cellDbgFontPrintf(0.0585f, 0.8900f, 0.7000f, nColor, "Choose the emulated system(s) you want to be displayed / filtered.");
				}

				// Rom Paths (directories)
				if(nMenuItem > MASKMISCPOST90S+MENU_OPT_FILTER_START && nMenuItem <= MASKMISCPOST90S+MENU_OPT_FILTER_START+12)
				{
					int nRomPath = nMenuItem-(MASKMISCPOST90S+MENU_OPT_FILTER_START+1);
					cellDbgFontPrintf(0.0585f, 0.8900f, 0.7000f, nColor, "Current: %s", g_opt_szROMPaths[nRomPath]);
				}

				// Input Preset Paths (CFG)
				if(nMenuItem > MASKMISCPOST90S+MENU_OPT_FILTER_START+12 && nMenuItem <= MASKMISCPOST90S+MENU_OPT_FILTER_START+12+21)
				{
					int nCfgPath = nMenuItem-(MASKMISCPOST90S+MENU_OPT_FILTER_START+12+1);
					cellDbgFontPrintf(0.0585f, 0.8900f, 0.7000f, nColor, "Current: %s", g_opt_szInputCFG[nCfgPath]);
				}

				nColor		= COLOR_YELLOW;		// yellow
			}

			if(nMenuItem == MENU_OPT_AUTO_AR) {
				cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, "%s: [%s]", 
					options_menu->item[nMenuItem]->szMenuLabel,
					g_opt_bAutoAspectRatio ? "ON" : "OFF"
				);
			}

			if(nMenuItem == MENU_OPT_AUTO_CFG) {
				cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, "%s: [%s]", 
					options_menu->item[nMenuItem]->szMenuLabel,
					g_opt_bAutoInputCfgCreate ? "ON" : "OFF"
				);
			}
			
			if(nMenuItem == MENU_OPT_ALT_MKEY) {
				cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, "%s: [%s]", 
					options_menu->item[nMenuItem]->szMenuLabel,
					g_opt_bUseAltMenuKeyCombo ? "ON" : "OFF"
				);
			}

			if(nMenuItem == MENU_OPT_DISP_MISS_GMS) {
				cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, "%s: [%s]", 
					options_menu->item[nMenuItem]->szMenuLabel,
					g_opt_bDisplayMissingGames ? "ON" : "OFF"
				);			
			}

			// Custom System Filters
			if(nMenuItem >= MENU_OPT_FILTER_START && nMenuItem <=  MASKMISCPOST90S+MENU_OPT_FILTER_START)
			{
				int nSysFilter = nMenuItem-MENU_OPT_FILTER_START;

				cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, "%s: [%s]", 
					options_menu->item[nMenuItem]->szMenuLabel,
					g_opt_bCustomSysFilter[nSysFilter] ? "ON" : "OFF"
				);
			}

			// Rom Paths (directories)
			if(nMenuItem > MASKMISCPOST90S+MENU_OPT_FILTER_START && nMenuItem <= MASKMISCPOST90S+MENU_OPT_FILTER_START+12)
			{
				cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, "%s: [...]", options_menu->item[nMenuItem]->szMenuLabel);
			}

			// Input Preset Paths (CFG)
			if(nMenuItem > MASKMISCPOST90S+MENU_OPT_FILTER_START+12 && nMenuItem <= MASKMISCPOST90S+MENU_OPT_FILTER_START+12+21)
			{
				cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, "%s: [...]", options_menu->item[nMenuItem]->szMenuLabel);
			}

			yPos += yPosDiff;

			nMenuItem++;
		}
	}

	if(nSection == SECTION_GAMELIST && !bProcessingGames) 
	{
		float xPos		= 0.0400f;
		float yPos		= 0.0500f;
		float yPosDiff	= 0.0185f;	
		float nFontSize = 0.5500f;		

		xPos = 0.0585f;

		yPos += yPosDiff;
		yPos += yPosDiff;
		yPos += yPosDiff;
		yPos += yPosDiff;

		int nGameListMax = 29;

		if(nFilteredGames >= 1)
		{
			if(nSelectedGame > nGameListMax || nGameListTop > 0)
			{
				if(nGameListTop < (nSelectedGame - nGameListMax))
				{
					nGameListTop = nSelectedGame - nGameListMax;
				}

				if(nGameListTop > 0) 
				{
					if(nSelectedGame < nGameListTop)
					{
						nGameListTop = nSelectedGame;
					}
				} else {
					nGameListTop = nSelectedGame - nGameListMax;
				}
			} else {
				nGameListTop = nSelectedGame - nGameListMax;
			}

			if(nGameListTop < 0) nGameListTop = 0;

			int nGame = nGameListTop;
			
			while(nGame <= (nGameListTop + nGameListMax))
			{
				if(nFilteredGames < 1) break;
				if(nGame == nFilteredGames) break;

				uint32_t nColor	= COLOR_WHITE;		// white
				nFontSize = 0.6000f;

				// GAME MISSING
				if(!fgames[nGame]->bAvailable)
				{
					nColor = COLOR_RED;
				}

				// GAME SELECTED
				if(nGame == nSelectedGame) 
				{
					nColor		= COLOR_YELLOW;		// yellow
					//nFontSize	= 0.6500f;
				}

				// limit chars displayed for now, until i figure something out...
				char* pszFinalText = NULL;
				pszFinalText = (char*)malloc(65);
				memset(pszFinalText, 0, 65);
				memcpy(pszFinalText, fgames[nGame]->title, 64);

				cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, "\t[%d]\t%s",
					nGame+1, 
					pszFinalText
					);
				yPos += yPosDiff;

				SAFE_FREE(pszFinalText)

				nGame++;
			}		

			nFontSize = 0.5500f;
		}

		cellDbgFontPrintf(0.0400f, 0.0440f, nFontSize, COLOR_WHITE, "AVAILABLE: %d MISSING: %d / SYSTEM: %s / FILTERED: %d", 
			nTotalGames-nMissingGames,
			nMissingGames,
			GetSystemFilter(g_opt_nActiveSysFilter),
			nFilteredGames
		);

		yPos = 0.7900f;
		xPos = 0.0550f;
		nFontSize = 0.6000f;

		if(nBurnSelected >= 0) 
		{
			char *pszTitle = NULL;
			pszTitle = (char*)malloc(65);
			memset(pszTitle, 0, 65);
			memcpy(pszTitle, fba_drv[nBurnSelected].szTitle, 64);

			cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, 
				"\tTITLE: %s", pszTitle);
			yPos += yPosDiff;

			SAFE_FREE(pszTitle)

			cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, 
				"\tROMSET: %s", fba_drv[nBurnSelected].szName);
			yPos += yPosDiff;

			cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, 
				"\tPARENT: %s", fba_drv[nBurnSelected].szParent);
			yPos += yPosDiff;

			cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, 
				"\tCOMPANY: %s", fba_drv[nBurnSelected].szCompany);
			yPos += yPosDiff;

			cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, 
				"\tYEAR: %s", fba_drv[nBurnSelected].szYear);
			yPos += yPosDiff;

			cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, 
				"\tSYSTEM: %s", fba_drv[nBurnSelected].szSystem);
			yPos += yPosDiff;

			cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, 
				"\tMAX PLAYERS: %d", fba_drv[nBurnSelected].nMaxPlayers);
			yPos += yPosDiff;

			cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, 
				"\tRESOLUTION: %d x %d (%d:%d)", 
				fba_drv[nBurnSelected].nWidth, fba_drv[nBurnSelected].nHeight,
				fba_drv[nBurnSelected].nAspectX, fba_drv[nBurnSelected].nAspectY
				);
			yPos += yPosDiff;

		}
	}

	if(nSection == SECTION_ZIPINFO)
	{
		float xPos		= 0.0400f;
		float yPos		= 0.0500f;
		float yPosDiff	= 0.0200f;	
		float nFontSize = 0.5500f;

		if(nFilteredGames >= 1)
		{		
			char* pszZipName = NULL;
			pszZipName = (char*)malloc(33);
			memset(pszZipName, 0, 33);
			memcpy(pszZipName, fgames[nSelectedGame]->zipname, 32);

			cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, "ZIP: %s / TOTAL FILES FOUND: %d", pszZipName, zipinfo_menu->nTotalItem);
			yPos += yPosDiff;

			SAFE_FREE(pszZipName)

			xPos = 0.0650f;

			yPos += yPosDiff;
			yPos += yPosDiff;
			yPos += yPosDiff;

			int nMenuItem = zipinfo_menu->UpdateTopItem();

			while(nMenuItem <= (zipinfo_menu->nTopItem + zipinfo_menu->nListMax))
			{
				if(nMenuItem == zipinfo_menu->nTotalItem) break;

				uint32_t nColor	= COLOR_WHITE;		// white

				// SELECTED
				if(nMenuItem == zipinfo_menu->nSelectedItem) {
					nColor		= COLOR_YELLOW;		// yellow
				}

				cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, "[%d] %s", nMenuItem+1, zipinfo_menu->item[nMenuItem]->szMenuLabel);
				yPos += yPosDiff;

				nMenuItem++;
			}
		}
	}

	if(nSection == SECTION_MAIN)
	{
		float xPos		= 0.0600f;
		float yPos		= 0.0485f;
		float yPosDiff	= 0.0400f;	
		float nFontSize = 1.0500f;

		yPos += yPosDiff;	
		yPos += yPosDiff;
		yPos += yPosDiff;
		yPos += yPosDiff;
		
		yPosDiff	= 0.0800f;

		int nMenuItem = main_menu->UpdateTopItem();
		
		while(nMenuItem <= (main_menu->nTopItem + main_menu->nListMax))
		{
			if(nMenuItem == main_menu->nTotalItem) break;

			uint32_t nColor	= COLOR_WHITE;		// white

			// SELECTED
			if(nMenuItem == main_menu->nSelectedItem) {
				nColor		= COLOR_YELLOW;		// yellow
			}

			cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, "%s", main_menu->item[nMenuItem]->szMenuLabel);
			yPos += yPosDiff;

			nMenuItem++;
		}
	}

}

void LaunchRetroArch(char* arg1, char* arg2, char* arg3, char* arg4, char* arg5) 
{
	// TODO: Add error checking for arguments...
	char* argv[6] = { 0 };	
	argv[0] = (char*)malloc( strlen(arg1) + 1); 
	argv[1] = (char*)malloc( strlen(arg2) + 1); 
	argv[2] = (char*)malloc( strlen(arg3) + 1); 
	argv[3] = (char*)malloc( strlen(arg4) + 1);
	argv[4] = (char*)malloc( strlen(arg5) + 1);

	strcpy(argv[0], arg1);	// [ROM / ZIP path]			optional, can be dummy / filler argument
	strcpy(argv[1], arg2);	// [FBARL RELOAD.SELF path] MUST be the valid external launcher path
	strcpy(argv[2], arg3);	// [input preset cfg]		optional, can be dummy / filler argument
	strcpy(argv[3], arg4);	// [aspect ratio]			optional, can be dummy / filler argument
	strcpy(argv[4], arg5);	// [alternate menu key combo ] optional, can be dummy / filler argument
	argv[5] = NULL;

	char fba_core_path[] = "/dev_hdd0/game/FBAL00123/USRDIR/cores/fb_alpha.SELF";
	sys_game_process_exitspawn(fba_core_path, (const char**)argv, NULL, NULL, 0, 1000, SYS_PROCESS_PRIMARY_STACK_SIZE_1M);
}

void c_fbaRL::InputFrame()
{
	if(bProcessingGames) return;

	static int nSelInputFrame = 0;

	// ------------------------------------------------------
	// Navigation UP/DOWN with delay

	if(((app.mFrame + nSelInputFrame) - app.mFrame) == 5)
	{
		// DPAD UP / LEFT ANALOG UP
		if((app.mIsButtPressed[BT_UP] && app.buttPressedNow[BT_UP]) || (app.mValLStickY < 50))
		{
			switch(nSection) 
			{
				case SECTION_OPTIONS:
				{
					if(options_menu->nSelectedItem > 0 && options_menu->nSelectedItem <= options_menu->nTotalItem) 
					{
						options_menu->nSelectedItem--;
					}
					break;
				}

				case SECTION_GAMELIST: 
				{
					if(nSelectedGame > 0 && nSelectedGame <= nFilteredGames) 
					{
						nSelectedGame--;
						if(nFilteredGames >= 1) {
							nStatus = STATUS_UPDATEPREVIEW;
						} else {
							nStatus = STATUS_RESETPREVIEW;
						}
					}
					break;
				}

				case SECTION_FILEBROWSER:
				{
					if(filebrowser->nSelectedItem > 0 && filebrowser->nSelectedItem <= filebrowser->nTotalItem) 
					{
						filebrowser->nSelectedItem--;
					}
					break;
				}

				case SECTION_MAIN:
				{
					if(main_menu->nSelectedItem > 0 && main_menu->nSelectedItem <= main_menu->nTotalItem) 
					{
						main_menu->nSelectedItem--;
					}
					break;
				}

				case SECTION_ZIPINFO:
				{
					if(zipinfo_menu->nSelectedItem > 0 && zipinfo_menu->nSelectedItem <= zipinfo_menu->nTotalItem) 
					{
						zipinfo_menu->nSelectedItem--;
					}
					break;
				}
			}
		}

		// DPAD DOWN / LEFT ANALOG DOWN
		if	( (app.mIsButtPressed[BT_DOWN] && app.buttPressedNow[BT_DOWN]) || (app.mValLStickY > 200) )
		{
			switch(nSection)
			{
				case SECTION_OPTIONS:
				{
					if(options_menu->nSelectedItem >= 0 && options_menu->nSelectedItem < options_menu->nTotalItem-1) 
					{
						options_menu->nSelectedItem++;
					}
					break;
				}

				case SECTION_GAMELIST:
				{
					if(nSelectedGame >= 0 && nSelectedGame < nFilteredGames-1) 
					{
						nSelectedGame++;
						if(nFilteredGames >= 1) {
							nStatus = STATUS_UPDATEPREVIEW;
						} else {
							nStatus = STATUS_RESETPREVIEW;
						}
					}
					break;
				}

				case SECTION_FILEBROWSER:
				{
					if(filebrowser->nSelectedItem >= 0 && filebrowser->nSelectedItem < filebrowser->nTotalItem-1) 
					{
						filebrowser->nSelectedItem++;
					}
					break;
				}

				case SECTION_MAIN:
				{
					if(main_menu->nSelectedItem >= 0 && main_menu->nSelectedItem < main_menu->nTotalItem-1) 
					{
						main_menu->nSelectedItem++;
					}
					break;
				}

				case SECTION_ZIPINFO:
				{
					if(zipinfo_menu->nSelectedItem >= 0 && zipinfo_menu->nSelectedItem < zipinfo_menu->nTotalItem-1) 
					{
						zipinfo_menu->nSelectedItem++;
					}
					break;
				}
			}
		}
		nSelInputFrame = 0;
	}

	nSelInputFrame++;

	// ------------------------------------------------------
	// [ANALOG STICKS]

	// LEFT ANALOG [UP]
	//if(app.mValLStickY < 50){
	//}
	//// LEFT ANALOG [DOWN]
	//if(app.mValLStickY > 200){
	//}
	//// LEFT ANALOG [LEFT]
	//if(app.mValLStickX < 50){
	//}
	//// LEFT ANALOG [RIGHT]
	//if(app.mValLStickX > 200){
	//}
	//// RIGHT ANALOG [UP]
	//if(app.mValRStickY < 50){
	//}
	//// RIGHT ANALOG [DOWN]
	//if(app.mValRStickY > 200){
	//}
	//// RIGHT ANALOG [LEFT]
	//if(app.mValRStickX < 50){
	//}
	//// RIGHT ANALOG [RIGHT]
	//if(app.mValRStickX > 200){
	//}

	// ------------------------------------------------------
	// [ ] - SQUARE

	if(!app.mIsButtPressed[BT_SQUARE] && app.buttPressedNow[BT_SQUARE])
	{
		switch(nSection)
		{
			case SECTION_GAMELIST:
			{
				if(nFilteredGames < 1) break;
				if(!fgames[nSelectedGame]->bAvailable) break;

				InitZipInfoMenu();
				nSection = SECTION_ZIPINFO;
				break;
			}
		}
	}

	// ------------------------------------------------------
	// (X) - CROSS

	if(!app.mIsButtPressed[BT_CROSS] && app.buttPressedNow[BT_CROSS])
	{
		switch(nSection)
		{ 
			case SECTION_FILEBROWSER:
			{
				filebrowser->NavigateSelectedDir();
				break;
			}

			case SECTION_OPTIONS:
			{
				int nMenuItem = options_menu->nSelectedItem;

				// Auto Aspect ratio toggle
				if(nMenuItem == MENU_OPT_AUTO_AR) 
				{
					g_opt_bAutoAspectRatio = g_opt_bAutoAspectRatio ? false : true;
				}

				// Auto Input CFG create toggle
				if(nMenuItem == MENU_OPT_AUTO_CFG) 
				{
					g_opt_bAutoInputCfgCreate = g_opt_bAutoInputCfgCreate ? false : true;
				}

				// Alternate RetroArch Menu Key combo
				if(nMenuItem == MENU_OPT_ALT_MKEY)
				{
					g_opt_bUseAltMenuKeyCombo = g_opt_bUseAltMenuKeyCombo ? false : true;
				}

				if(nMenuItem == MENU_OPT_DISP_MISS_GMS) {
					g_opt_bDisplayMissingGames = g_opt_bDisplayMissingGames ? false : true;
				}

				// Custom System Filters
				if(nMenuItem >= MENU_OPT_FILTER_START && nMenuItem <=  MASKMISCPOST90S+MENU_OPT_FILTER_START)
				{
					int nSysFilter = nMenuItem-MENU_OPT_FILTER_START;
					g_opt_bCustomSysFilter[nSysFilter] = g_opt_bCustomSysFilter[nSysFilter] ? false : true;
				}

				// Rom Paths (directories)
				if(nMenuItem > MASKMISCPOST90S+MENU_OPT_FILTER_START && nMenuItem <= MASKMISCPOST90S+MENU_OPT_FILTER_START+12)
				{
					nFileBrowserType = 0;
					InitFileBrowser();
					nSection = SECTION_FILEBROWSER;
				}

				// Input Preset Paths (CFG)
				if(nMenuItem > MASKMISCPOST90S+MENU_OPT_FILTER_START+12 && nMenuItem <= MASKMISCPOST90S+MENU_OPT_FILTER_START+12+21)
				{
					nFileBrowserType = 1;
					InitFileBrowser();
					nSection = SECTION_FILEBROWSER;
				}
				break;
			}

			case SECTION_GAMELIST: 
			{
				if(nFilteredGames < 1) break;

				if(!fgames[nSelectedGame]->bAvailable) {
					break;
				}

				iniWrite(); // save settings

				app.onShutdown();

				char fba_rl_path[]		= "/dev_hdd0/game/FBAL00123/USRDIR/RELOAD.SELF";

				char aspect_ratio[12]	= { 0 };
				extern int nBurnSelected;
				sprintf(aspect_ratio, "%d:%d", fba_drv[nBurnSelected].nAspectX, fba_drv[nBurnSelected].nAspectY);

				if(FileExist(fba_rl_path) && FileExist(fgames[nSelectedGame]->path)) 
				{
					int nSysMask = GetSystemMaskId(fba_drv[nBurnSelected].szSystemFilter);

					LaunchRetroArch(
						(char*)fgames[nSelectedGame]->path, 
						(char*)fba_rl_path, 
						(char*)(FileExist(g_opt_szInputCFG[nSysMask]) ?  g_opt_szInputCFG[nSysMask] : "DUMMY_ARG"),
						(char*)((g_opt_bAutoAspectRatio) ? aspect_ratio : "DUMMY_ARG"),
						(char*)(g_opt_bUseAltMenuKeyCombo ? "yes" : "no")
					);
				}

				break;
			}

			case SECTION_MAIN:
			{
				int nMenuItem = main_menu->nSelectedItem;

				// Gamelist
				if(nMenuItem == MENU_MAIN_GAMELIST) 
				{					
					InitGameList();
					nSection = SECTION_GAMELIST;
					EndMainMenu();
					break;
				}

				// Options
				if(nMenuItem == MENU_MAIN_OPTIONS) 
				{					
					InitOptionsMenu();
					nSection = SECTION_OPTIONS;
					EndMainMenu();
					break;
				}

				// Load FBA RetroArch Modded Core
				if(nMenuItem == MENU_MAIN_FBARL_RA)
				{
					nStatus = STATUS_EXIT_FBARL_RA;
					break;
				}

				// Load RetroArch
				//if(nMenuItem == MENU_MAIN_RA) 
				//{
				//	iniWrite(); // save settings

				//	app.onShutdown();					

				//	char* argv[1] = { 0 };	
				//	argv[0] = NULL;

				//	char fba_retroarch_path[] = "/dev_hdd0/game/SSNE10000/USRDIR/cores/fb_alpha.SELF";
				//	if(FileExist(fba_retroarch_path))
				//	{
				//		sys_game_process_exitspawn(fba_retroarch_path, (const char**)argv, NULL, NULL, 0, 1000, SYS_PROCESS_PRIMARY_STACK_SIZE_1M);
				//	} else {
				//		strcpy(fba_retroarch_path, "/dev_hdd0/game/SSNE10000/USRDIR/cores/fba.SELF");
				//		if(FileExist(fba_retroarch_path)) {
				//			sys_game_process_exitspawn(fba_retroarch_path, (const char**)argv, NULL, NULL, 0, 1000, SYS_PROCESS_PRIMARY_STACK_SIZE_1M);
				//		} else {
				//			// RetroArch is not installed...
				//		}
				//	}
				//	//EndMainMenu();
				//	break;
				//}

				// Load multiMAN
				if(nMenuItem == MENU_MAIN_MMAN)
				{
					nStatus = STATUS_EXIT_MMAN;
					break;
				}

				// Exit
				if(nMenuItem == MENU_MAIN_EXIT)
				{
					nStatus = STATUS_EXIT;
					break;
				}
				break;
			}
		}

	}	

	// ------------------------------------------------------
	// (O) - CIRCLE

	if (!app.mIsButtPressed[BT_CIRCLE] && app.buttPressedNow[BT_CIRCLE]) 
	{
		switch(nSection) 
		{
			case SECTION_FILEBROWSER:
			{
				filebrowser->NavigateDirUp();
				break;
			}

			case SECTION_ZIPINFO:
			{
				EndZipInfoMenu();
				// no need to init GameList here...
				nSection = SECTION_GAMELIST;
				break;
			}

			case SECTION_OPTIONS:
			{
				iniWrite(); // save settings

				EndOptionsMenu();
				InitMainMenu();
				nSection = SECTION_MAIN;
				break;
			}
		}
	}

	// ------------------------------------------------------
	// (/\) - TRIANGLE

	if(!app.mIsButtPressed[BT_TRIANGLE] && app.buttPressedNow[BT_TRIANGLE])
	{
		switch(nSection) 
		{
			case SECTION_GAMELIST:
			{
				if(nFilteredGames < 1) break;

				if(!fgames[nSelectedGame]->bAvailable) break;
			}
		}
	}

	// ------------------------------------------------------
	// [L1]

	if(!app.mIsButtPressed[BT_L1] && app.buttPressedNow[BT_L1])
	{
		switch(nSection) 
		{
			case SECTION_GAMELIST:
				PrevSysFilter();
				if(nFilteredGames >= 1) {
					nStatus = STATUS_UPDATEPREVIEW;
				} else {
					nStatus = STATUS_RESETPREVIEW;
				}
				break;

			case SECTION_OPTIONS:
				// ...
				break;
		}
	}

	// ------------------------------------------------------
	// [R1]

	if(!app.mIsButtPressed[BT_R1] && app.buttPressedNow[BT_R1])
	{
		switch(nSection) 
		{
			case SECTION_GAMELIST:
				NextSysFilter();
				if(nFilteredGames >= 1) {
					nStatus = STATUS_UPDATEPREVIEW;
				} else {
					nStatus = STATUS_RESETPREVIEW;
				}
				break;

			case SECTION_OPTIONS:
				// ...
				break;
		}
	}

	// ------------------------------------------------------
	// [L2]

	if(((app.mFrame + nSelInputFrame) - app.mFrame) == 5)
	{
		if(app.mIsButtPressed[BT_L2] && app.buttPressedNow[BT_L2])
		{
			switch(nSection)
			{
				case SECTION_GAMELIST:
				{
					nSelectedGame -= 19;

					if(nSelectedGame < 0) {
						nSelectedGame = 0;
					}

					if(nFilteredGames >= 1) {
						nStatus = STATUS_UPDATEPREVIEW;
					} else {
						nStatus = STATUS_RESETPREVIEW;
					}
					break;
				}

				case SECTION_ZIPINFO:
				{
					zipinfo_menu->nSelectedItem -= zipinfo_menu->nListMax;

					if(zipinfo_menu->nSelectedItem < 0) {
						zipinfo_menu->nSelectedItem = 0;
					}
					break;
				}

				case SECTION_FILEBROWSER:
				{
					filebrowser->nSelectedItem -= filebrowser->nListMax;

					if(filebrowser->nSelectedItem < 0) {
						filebrowser->nSelectedItem = 0;
					}
					break;
				}
			}
		}

		// ------------------------------------------------------
		// [R2]

		if(app.mIsButtPressed[BT_R2] && app.buttPressedNow[BT_R2])
		{
			switch(nSection)
			{
				case SECTION_GAMELIST:
				{
					nSelectedGame += 19;
					if(nSelectedGame > nFilteredGames-1) {
						nSelectedGame = nFilteredGames-1;
					}

					if(nFilteredGames >= 1) {
						nStatus = STATUS_UPDATEPREVIEW;
					} else {
						nStatus = STATUS_RESETPREVIEW;
					}

					break;
				}

				case SECTION_FILEBROWSER:
				{
					filebrowser->nSelectedItem += filebrowser->nListMax;

					if(filebrowser->nSelectedItem > filebrowser->nTotalItem-1) {
						filebrowser->nSelectedItem = filebrowser->nTotalItem-1;
					}
					break;
				}

				case SECTION_ZIPINFO:
				{
					zipinfo_menu->nSelectedItem += zipinfo_menu->nListMax;

					if(zipinfo_menu->nSelectedItem > zipinfo_menu->nTotalItem-1) {
						zipinfo_menu->nSelectedItem = zipinfo_menu->nTotalItem-1;
					}
					break;
				}
			}
		}
	}

	// ------------------------------------------------------
	// [SELECT]

	if(!app.mIsButtPressed[BT_SELECT] && app.buttPressedNow[BT_SELECT])
	{
		switch(nSection)
		{
			case SECTION_FILEBROWSER:
			{
				nSection = SECTION_OPTIONS;
				EndFileBrowser();
				break;
			}

			case SECTION_GAMELIST: 
			{
				iniWrite(); // save settings
				
				InitMainMenu();
				nSection = SECTION_MAIN;
				EndGameList();
				break;
			}
		}
	}

	// ------------------------------------------------------
	// [START]
	
	if(!app.mIsButtPressed[BT_START] && app.buttPressedNow[BT_START])
	{
		switch(nSection) 
		{
			case SECTION_FILEBROWSER:
			{
				if(filebrowser->nTotalItem < 1) break;

				int nMenuItem = options_menu->nSelectedItem;

				// Rom Paths (directories)
				if(nFileBrowserType == 0 && filebrowser->item[filebrowser->nSelectedItem]->nType == CELL_FS_TYPE_DIRECTORY)
				{
					if(nMenuItem > MASKMISCPOST90S+MENU_OPT_FILTER_START && nMenuItem <= MASKMISCPOST90S+MENU_OPT_FILTER_START+12)
					{
						int nRomPath = nMenuItem-(MASKMISCPOST90S+MENU_OPT_FILTER_START+1);
						strcpy(g_opt_szROMPaths[nRomPath], filebrowser->item[filebrowser->nSelectedItem]->szPath);
					}
					nStatus = STATUS_ROMPATH_CHANGED;
				}

				// Input Preset Paths (CFG)
				if(nFileBrowserType == 1 && filebrowser->item[filebrowser->nSelectedItem]->nType == CELL_FS_TYPE_REGULAR)
				{
					
					if(nMenuItem > MASKMISCPOST90S+MENU_OPT_FILTER_START+12 && nMenuItem <= MASKMISCPOST90S+MENU_OPT_FILTER_START+12+21)
					{
						int nCfgPath = nMenuItem-(MASKMISCPOST90S+MENU_OPT_FILTER_START+12+1);
						strcpy(g_opt_szInputCFG[nCfgPath], filebrowser->item[filebrowser->nSelectedItem]->szPath);
					}
				}

				nSection = SECTION_OPTIONS;
				EndFileBrowser();

				iniWrite();
				break;
			}

			case SECTION_GAMELIST: 
			{
				nStatus = STATUS_ROMSCAN_DLG;
				break;
			}
		}
	}


	// ------------------------------------------------------
	// [L3]

	if(!app.mIsButtPressed[BT_L3] && app.buttPressedNow[BT_L3])
	{

	}

	// ------------------------------------------------------
	// [R3]
	
	if(!app.mIsButtPressed[BT_R3] && app.buttPressedNow[BT_R3])
	{

	}

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

void c_fbaRL::DlgDisplayFrame()
{
	switch(nStatus)
	{
		case STATUS_ROMPATH_CHANGED:
		{
			::cellMsgDialogOpen2(
				CELL_MSGDIALOG_TYPE_SE_TYPE_NORMAL, 
				"Remeber to rescan ROMs after you finish configuring all paths,\nyou can do so at the Game List section.", 
				DlgCallbackFunction, (void*)STATUS_ROMPATH_CHANGED, NULL
			);
			break;		
		}

		case STATUS_ROMSCAN_DLG:
		{
			::cellMsgDialogOpen2(
				CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO|
				CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_NO, 
				"Do you want to Rescan all configured paths for ROMs ?", 
				ExitDlgCallbackFunction, (void*)STATUS_ROMSCAN_DLG, NULL
			);
			break;
		}

		case STATUS_EXIT:
		{
			::cellMsgDialogOpen2(
				CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO|
				CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_NO, 
				"Do you want to exit FB Alpha RL and return to XMB ?", 
				ExitDlgCallbackFunction, (void*)STATUS_EXIT, NULL
			);
			break;
		}

		case STATUS_EXIT_FBARL_RA:
		{
			::cellMsgDialogOpen2(
				CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO|
				CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_NO, 
				"Do you want to exit and load FB Alpha RetroArch (modified core / fb_alpha.SELF) ?",
				ExitDlgCallbackFunction, (void*)STATUS_EXIT_FBARL_RA, NULL
			);
			break;
		}

		case STATUS_EXIT_MMAN:
		{
			::cellMsgDialogOpen2(				
				CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO|
				CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_NO, 
				"Do you want to exit and load \"multiMAN\" ?",
				ExitDlgCallbackFunction, (void*)STATUS_EXIT_MMAN, NULL
			);
			break;
		}

		case STATUS_UPDATEPREVIEW:
		{
			UpdateBurnSelected(fgames[nSelectedGame]->zipname);
			UpdatePreviewImage();
			break;
		}

		case STATUS_RESETPREVIEW:
		{
			ResetPreviewImage();
			break;
		}

		case STATUS_ROMSCAN_START:
		{
			char szMsg[256] = "";
			sprintf(szMsg, "Scanning ROM(s), please wait...\n\nNotice: If you have MAME complete romset, be patient, the process could take a few minutes.");

			::cellMsgDialogOpen2(
				CELL_MSGDIALOG_TYPE_SE_TYPE_NORMAL
				|CELL_MSGDIALOG_TYPE_BUTTON_TYPE_NONE
				|CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_ON
				|CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_NONE
				|CELL_MSGDIALOG_TYPE_PROGRESSBAR_SINGLE,
				szMsg,
				DlgCallbackFunction, NULL, NULL
			);

			sys_ppu_thread_t thread_id;

			sys_ppu_thread_create(
				&thread_id,
				_ScanROMs,						// callback function
				0x1337,							// arg
				0,								// priority
				0x300000,							// stack size
				SYS_PPU_THREAD_CREATE_JOINABLE, //
				"ROMScan"						// name
			);
			break;
		}

		case STATUS_ROMSCAN_END:
		{
			cellMsgDialogAbort();
			SaveGameListCache();
			fbaRL->InitFilterList();
			break;
		}
	}

	nStatus = STATUS_NORMAL;
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

uint64_t c_fbaRL::GetFileSize(char* szFilePath)
{
	CellFsStat sb;
	memset(&sb, 0, sizeof(CellFsStat));
	cellFsStat(szFilePath, &sb);
	return sb.st_size;
}
