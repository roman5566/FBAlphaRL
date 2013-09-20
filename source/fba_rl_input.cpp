// =======================================================================
// INPUTS 

#include "main.h"

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

				// Enable / Disable Neo-Geo UNI-BIOS
				if(nMenuItem == MENU_OPT_USE_UNIBIOS) {
					g_opt_bUseUNIBIOS = g_opt_bUseUNIBIOS ? false : true;
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
				if(!fgames[nSelectedGame]->bAvailable) { break;	}
				
				// do not try to load system BIOS...
				if( strcmp(fgames[nSelectedGame]->zipname, "neogeo.zip")==0 ||
					strcmp(fgames[nSelectedGame]->zipname, "pgm.zip")==0 ) { 
						break;
				}

				iniWrite(); // save settings
				
				char fba_rl_path[]		= "/dev_hdd0/game/FBAL00123/USRDIR/RELOAD.SELF";
				char fba_core_path[]	= "/dev_hdd0/game/FBAL00123/USRDIR/cores/fb_alpha.SELF";

				if( FileExist(fba_rl_path) && FileExist(fba_core_path) )
				{
					if(FileExist(fgames[nSelectedGame]->path))
					{
						app.onShutdown();
				
						char aspect_ratio[12]	= { 0 };

						sprintf(aspect_ratio, "%d:%d", fba_drv[nBurnSelected].nAspectX, fba_drv[nBurnSelected].nAspectY);

						int nSysMask = GetSystemMaskId(fba_drv[nBurnSelected].szSystemFilter);

						LaunchFBACore(
							(char*)fgames[nSelectedGame]->path, 
							(char*)fba_rl_path, 
							(char*)(FileExist(g_opt_szInputCFG[nSysMask]) ?  g_opt_szInputCFG[nSysMask] : "DUMMY_ARG"),
							(char*)((g_opt_bAutoAspectRatio) ? aspect_ratio : "DUMMY_ARG"),
							(char*)(g_opt_bUseAltMenuKeyCombo ? "yes" : "no")
						);
					} else {
						// Missing game, probably USB is disconnected or file is no longer present for some reason
						nStatus = STATUS_MISSING_GAME_DLG;
						break;
					}
				} else {
					// Missing important program files...lol
					nStatus = STATUS_CORRUPT_APP_DLG;
					break;
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
			case SECTION_GAMELIST:
			{
				iniWrite(); // save settings
				
				InitMainMenu();
				nSection = SECTION_MAIN;
				EndGameList();
				break;
			}

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
				//iniWrite(); // save settings
				//
				//InitMainMenu();
				//nSection = SECTION_MAIN;
				//EndGameList();
				//break;
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
