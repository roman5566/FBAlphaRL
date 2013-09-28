// =======================================================================
// DISPLAY SECTIONS (FRAME)

#include "main.h"

static float nShadowXYpos		= 0.00250f;
static float nShadowXYpos2		= 0.00200f;
static uint32_t nShadowColor	= COLOR_BLACK;
static uint32_t nTextColor		= COLOR_WHITE;
static uint32_t nSelectColor	= COLOR_YELLOW;
static uint32_t nMissingColor	= COLOR_RED;
static float nBigSize		= 1.0500f;
static float nSmallSize		= 0.5500f;
static float nSmallSize2	= 0.6500f;
static float nSmallSizeSel	= 0.6000f;

void c_fbaRL::DisplayFrame()
{
	if(nSection == SECTION_FILEBROWSER)	{
		FileBrowser_Frame();
	}
	if(nSection == SECTION_OPTIONS && !bProcessingGames) {
		Options_Frame();
	}
	if(nSection == SECTION_GAMELIST && !bProcessingGames) {
		GameList_Frame();
	}
	if(nSection == SECTION_ZIPINFO)	{
		ZipInfo_Frame();
	}
	if(nSection == SECTION_ROMINFO)	{
		RomInfo_Frame();
	}
	if(nSection == SECTION_MAIN) {
		MainMenu_Frame();
	}
}

void c_fbaRL::MainMenu_Frame()
{
	float xPos		= 0.0600f;
	float yPos		= 0.0485f;
	float yPosDiff	= 0.0400f;	
	float nFontSize = 1.0500f; // big text
	uint32_t nColor = nTextColor;

	if(nFrameStep == 0) { yPos += nShadowXYpos2; xPos += nShadowXYpos2; }
	if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color

	extern char ipaddress[256];
	cellDbgFontPrintf(xPos + 0.0300f + (0.0600f * 2), yPos + 0.0100f + (yPosDiff * 2), nSmallSize, nColor, "ver: "_APP_VER" (ip: %s)", ipaddress);	

	if(nFrameStep == 0) { yPos -= nShadowXYpos2; xPos -= nShadowXYpos2; } // reset shadow xy pos
	if(nFrameStep == 0) { yPos += nShadowXYpos; xPos += nShadowXYpos; }

	yPos += (yPosDiff * 4);		
	yPosDiff = 0.0800f;

	int nMenuItem = main_menu->UpdateTopItem();	

	while(nMenuItem <= (main_menu->nTopItem + main_menu->nListMax))
	{
		if(nMenuItem == main_menu->nTotalItem) break;

		// normal
		nColor = nTextColor;

		// selected
		if(nMenuItem == main_menu->nSelectedItem) {
			nColor = nSelectColor;
		}
		
		if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color

		cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, "%s", main_menu->item[nMenuItem]->szMenuLabel);
		yPos += yPosDiff;

		nMenuItem++;
	}
}

void c_fbaRL::GameList_Frame()
{
	float xPos		= 0.0585f;
	float yPos		= 0.0450f;
	float yPosDiff	= 0.0200f;
	float nFontSize = nSmallSize; // small text

	if(nFrameStep == 0) { yPos += nShadowXYpos2; xPos += nShadowXYpos2; }

	yPos += (yPosDiff * 4);

	uint32_t nColor = nTextColor;

	int nGameListMax = 29;

	if(nFilteredGames >= 1)
	{
		if(nSelectedGame > nGameListMax || nGameListTop > 0)
		{
			if(nGameListTop < (nSelectedGame - nGameListMax)){
				nGameListTop = nSelectedGame - nGameListMax;
			}
			if((nGameListTop > 0) && (nSelectedGame < nGameListTop)) {
				nGameListTop = nSelectedGame;
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

			nColor = nTextColor;
			nFontSize = nSmallSizeSel; // small text (selected)

			// missing
			if(!fgames[nGame]->bAvailable) {
				nColor = nMissingColor;
			}

			// selected
			if(nGame == nSelectedGame) 
			{
				nColor = nSelectColor;
				nFontSize = nSmallSize2;
			}

			if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color

			// limit chars displayed for now, until i figure something out...
			char* pszFinalText = NULL;
			pszFinalText = (char*)malloc(65);
			memset(pszFinalText, 0, 65);
			memcpy(pszFinalText, fgames[nGame]->title, 64);

			cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, " [%d] %s",
				nGame+1, 
				pszFinalText
			);
			yPos += yPosDiff;

			SAFE_FREE(pszFinalText)

			nGame++;
		}		

		nFontSize = nSmallSize; // small text
	}

	nColor = nTextColor;	
	if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color

	xPos = 0.0400f;
	yPos = 0.0440f;
	if(nFrameStep == 0) { yPos += nShadowXYpos2; xPos +=nShadowXYpos2; }

	cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, "AVAILABLE: %d MISSING: %d / SYSTEM: %s / FILTERED: %d", 
		nTotalGames-nMissingGames,
		nMissingGames,
		GetSystemFilter(g_opt_nActiveSysFilter),
		nFilteredGames
	);

	xPos = 0.0585f;
	yPos = 0.7900f;
	
	if(nFrameStep == 0) { yPos += nShadowXYpos2; xPos += nShadowXYpos2; }
	
	nFontSize = nSmallSize; // small text

	if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color

	if(nBurnSelected >= 0) 
	{
		char *pszTitle = NULL;
		pszTitle = (char*)malloc(65);
		memset(pszTitle, 0, 65);
		memcpy(pszTitle, fba_drv[nBurnSelected].szTitle, 64);

		cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, " TITLE:       %s", pszTitle);
		yPos += yPosDiff;

		SAFE_FREE(pszTitle)

		cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, " ROMSET:      %s", fba_drv[nBurnSelected].szName);
		yPos += yPosDiff;

		cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, " PARENT:      %s", fba_drv[nBurnSelected].szParent);
		yPos += yPosDiff;

		cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, " COMPANY:     %s", fba_drv[nBurnSelected].szCompany);
		yPos += yPosDiff;

		cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, " YEAR:        %s", fba_drv[nBurnSelected].szYear);
		yPos += yPosDiff;

		cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, " SYSTEM:      %s", fba_drv[nBurnSelected].szSystem);
		yPos += yPosDiff;

		cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, " MAX PLAYERS: %d", fba_drv[nBurnSelected].nMaxPlayers);
		yPos += yPosDiff;

		cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, " RESOLUTION:  %d x %d (%d:%d)", 
			fba_drv[nBurnSelected].nWidth, 
			fba_drv[nBurnSelected].nHeight, 
			fba_drv[nBurnSelected].nAspectX, 
			fba_drv[nBurnSelected].nAspectY
		);
		yPos += yPosDiff;
	}
}

void c_fbaRL::Options_Frame()
{
	float xPos		= 0.0600f;
	float yPos		= 0.0485f;
	float yPosDiff	= 0.0400f;	
	float nFontSize = nBigSize; // big text

	if(nFrameStep == 0) { yPos += nShadowXYpos; xPos +=nShadowXYpos; }

	yPos += (yPosDiff * 4);		
	yPosDiff = 0.0800f;

	int nMenuItem = options_menu->UpdateTopItem();

	while(nMenuItem <= (options_menu->nTopItem + options_menu->nListMax))
	{
		if(nMenuItem == options_menu->nTotalItem) break;

		// normal
		uint32_t nColor	= nTextColor;

		if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color

		// selected
		if(nMenuItem == options_menu->nSelectedItem) 
		{
			float x = 0.0585f;
			float y = 0.8900f;
			float size = nSmallSize2; // small text 2

			if(nFrameStep == 0) { y += nShadowXYpos2; x +=nShadowXYpos2; }

			if(nMenuItem == MENU_OPT_AUTO_AR) {
				cellDbgFontPrintf(x, y, size, nColor, "Auto set the proper aspect ratio on game launch.");
			}

			if(nMenuItem == MENU_OPT_AUTO_CFG) {
				cellDbgFontPrintf(x, y, size, nColor, "Auto create basic Input Preset CFG files for all systems.");
			}

			if(nMenuItem == MENU_OPT_ALT_MKEY) {
				cellDbgFontPrintf(x, y, size, nColor, "Use [SELECT] + [TRIANGLE] and [SELECT] + [SQUARE] to access RetroArch menu.");
			}

			if(nMenuItem == MENU_OPT_USE_UNIBIOS) {
				cellDbgFontPrintf(x, y, size, nColor, "Use UNI-BIOS when playing Neo-Geo games.");
			}

			if(nMenuItem == MENU_OPT_DISP_MISS_GMS) {
				cellDbgFontPrintf(x, y, size, nColor, "Enable / Disable display of missing games.");
			}

			if(nMenuItem >= MENU_OPT_FILTER_START && nMenuItem <=  MASKMISCPOST90S+MENU_OPT_FILTER_START) {
				cellDbgFontPrintf(x, y, size, nColor, "Choose the emulated system(s) you want to be displayed / filtered.");
			}

			// Rom Paths (directories)
			if(nMenuItem > MASKMISCPOST90S+MENU_OPT_FILTER_START && nMenuItem <= MASKMISCPOST90S+MENU_OPT_FILTER_START+12) {
				int nRomPath = nMenuItem-(MASKMISCPOST90S+MENU_OPT_FILTER_START+1);
				cellDbgFontPrintf(x, y, size, nColor, "Current: %s", g_opt_szROMPaths[nRomPath]);
			}

			// Input Preset Paths (CFG)
			if(nMenuItem > MASKMISCPOST90S+MENU_OPT_FILTER_START+12 && nMenuItem <= MASKMISCPOST90S+MENU_OPT_FILTER_START+12+21) {
				int nCfgPath = nMenuItem-(MASKMISCPOST90S+MENU_OPT_FILTER_START+12+1);
				cellDbgFontPrintf(x, y, size, nColor, "Current: %s", g_opt_szInputCFG[nCfgPath]);
			}

			nColor = nSelectColor;

			if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color
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

		if(nMenuItem == MENU_OPT_USE_UNIBIOS) {
			cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, "%s: [%s]", 
				options_menu->item[nMenuItem]->szMenuLabel,
				g_opt_bUseUNIBIOS ? "ON" : "OFF"
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

void c_fbaRL::FileBrowser_Frame()
{
	float xPos		= 0.0750f;
	float yPos		= 0.0500f;
	float yPosDiff	= 0.0200f;	
	float nFontSize = nSmallSize2; // small text

	if(nFrameStep == 0) { yPos += nShadowXYpos2; xPos +=nShadowXYpos2; }

	yPos += (yPosDiff * 5);

	float yHeadPos = 0.8900f;
	if(nFrameStep == 0) { yHeadPos += nShadowXYpos2; }

	uint32_t nColor	= nTextColor;
	if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color

	cellDbgFontPrintf(xPos, yHeadPos, nFontSize, nColor, "CURRENT DIR: %s", filebrowser->pszCurrentDir);
	yPos += yPosDiff;

	if(filebrowser->nTotalItem >= 1)
	{			
		yPos += yPosDiff;

		int nMenuItem = filebrowser->UpdateTopItem();

		while(nMenuItem <= (filebrowser->nTopItem + filebrowser->nListMax))
		{
			if(nMenuItem == filebrowser->nTotalItem) break;

			// normal
			nColor = nTextColor;

			// selected
			if(nMenuItem == filebrowser->nSelectedItem) {
				nColor = nSelectColor;
			}

			if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color

			cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, "%s", filebrowser->item[nMenuItem]->szMenuLabel);
			yPos += yPosDiff;

			nMenuItem++;
		}
	}
}

void c_fbaRL::ZipInfo_Frame()
{
	float xPos		= 0.0400f;
	float yPos		= 0.0440f;
	float yPosDiff	= 0.0200f;	
	float nFontSize = nSmallSize; // small text

	if(nFrameStep == 0) { yPos += nShadowXYpos2; xPos +=nShadowXYpos2; }

	// normal
	uint32_t nColor	= nTextColor;

	if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color

	if(nFilteredGames >= 1)
	{		
		char* pszZipName = NULL;
		pszZipName = (char*)malloc(33);
		memset(pszZipName, 0, 33);
		memcpy(pszZipName, fgames[nSelectedGame]->zipname, 32);

		cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, "ZIP: %s / TOTAL FILES FOUND: %d", pszZipName, zipinfo_menu->nTotalItem);
		yPos += yPosDiff;

		SAFE_FREE(pszZipName)

		xPos = 0.0650f;
		if(nFrameStep == 0) { xPos +=nShadowXYpos2; }

		yPos += (yPosDiff * 3);

		int nMenuItem = zipinfo_menu->UpdateTopItem();

		while(nMenuItem <= (zipinfo_menu->nTopItem + zipinfo_menu->nListMax))
		{
			if(nMenuItem == zipinfo_menu->nTotalItem) break;

			// normal
			nColor = nTextColor;

			// selected
			if(nMenuItem == zipinfo_menu->nSelectedItem) {
				nColor = nSelectColor;
			}

			if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color

			cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, "[%d] %s", nMenuItem+1, zipinfo_menu->item[nMenuItem]->szMenuLabel);
			yPos += yPosDiff;

			nMenuItem++;
		}
	}
}

void c_fbaRL::RomInfo_Frame()
{
	float xPos		= 0.0400f;
	float yPos		= 0.0440f;
	float yPosDiff	= 0.0200f;	
	float nFontSize = nSmallSize; // small text

	if(nFrameStep == 0) { yPos += nShadowXYpos2; xPos +=nShadowXYpos2; }

	// normal
	uint32_t nColor	= nTextColor;

	if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color

	if(nFilteredGames >= 1)
	{		
		char* pszZipName = NULL;
		pszZipName = (char*)malloc(33);
		memset(pszZipName, 0, 33);
		memcpy(pszZipName, fgames[nSelectedGame]->zipname, 32);

		cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, "ROMSET: %s / TOTAL ROMS: %d", pszZipName, rominfo_menu->nTotalItem);
		yPos += yPosDiff;

		SAFE_FREE(pszZipName)

		xPos = 0.0650f;
		if(nFrameStep == 0) { xPos +=nShadowXYpos2; }

		yPos += (yPosDiff * 3);

		int nMenuItem = rominfo_menu->UpdateTopItem();

		while(nMenuItem <= (rominfo_menu->nTopItem + rominfo_menu->nListMax))
		{
			if(nMenuItem == rominfo_menu->nTotalItem) break;

			// normal
			nColor = nTextColor;

			// selected
			if(nMenuItem == rominfo_menu->nSelectedItem) {
				nColor = nSelectColor;
			}

			if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color

			cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, "[%d] %s", nMenuItem+1, rominfo_menu->item[nMenuItem]->szMenuLabel);
			yPos += yPosDiff;

			nMenuItem++;
		}
	}
}
