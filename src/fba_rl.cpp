// ---------------------------------------------------------------------
// FB Alpha Retro Loader (CaptainCPS-X, 2013)
// ---------------------------------------------------------------------
/*

Custom RETROARCH Recent Changes:

- Implemented a static variable "g_bExtLaunchMM" to serve as "MODE_EXTLAUNCH_MULTIMAN" since
it is being cleared at some unknown point creating problems when launching
from external applications to load a game on the fly.

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

- Modified / Added menu items for in-game menu and main menu so RetroArch display the
proper name if launched from FB Alpha RL. 

- Source Files Affected:

frontend/frontend_console.c
frontend/menu/rmenu.c
frontend/menu/rmenu.h
frontend/platform/platform_ps3.c

Makefile.ps3 (was edited with custom signing using scetool for Salamander / CORE)

TODO: 

*- Generate proper shaders and files to be packaged alongside custom RetroArch

- Implement filtering by System type
- Implement Configuration file module for FB Alpha RL

WIP:

- Configure default on load input (CFG) preset.

*/

// --------------------------------------------------------------------
#define _APP_TITLE "\tFB ALPHA RETRO LOADER v1.01 - by CaptainCPS-X [2013]"
// --------------------------------------------------------------------

#define FBARL_DEBUG

#include "main.h"
#include "misc.h"
#include "fba_rl.h"
#include "burn_drivers.h" // Driverlist Generated via custom FB Alpha build [ v0.2.97.28 ][ Windows ]
//#include "burn_rominfo.h"

#include <sys/stat.h>
#include "miniz.c"

c_fbaRL* fbaRL;

int nBurnSelected = -1;
// -------------------------------------------------------

void c_fbaRL::InitMainMenu()
{
	main_menu = new c_Menu(9);

	main_menu->AddItem((char*)"-[         GAME LIST          ]-");
	main_menu->AddItem((char*)"-[          OPTIONS           ]-");
	main_menu->AddItem((char*)"-[  LOAD \"FB Alpha Core\" MOD  ]-");
	main_menu->AddItem((char*)"-[      LOAD \"RetroArch\"      ]-");
	main_menu->AddItem((char*)"-[       LOAD \"multiMAN\"      ]-");
	main_menu->AddItem((char*)"-[            EXIT            ]-");
}

void c_fbaRL::EndMainMenu()
{
	SAFE_DELETE(main_menu);
}

void c_fbaRL::InitGameList()
{
	nSelectedGame = 0;
	nGameListTop = 0;
	nTotalGames = 0;

	ParseGameList();

	// Sort by Title
	//qsort(games, nTotalGames, sizeof(class c_game), _FcCompareStruct);

	if(games[0]) UpdateBurnSelected(games[nSelectedGame]->zipname);
}

void c_fbaRL::EndGameList()
{
	for(int n = 0; n < nTotalGames; n++)
	{
		SAFE_DELETE(games[n]);
	}
	nSelectedGame = 0;
	nGameListTop = 0;
	nTotalGames = 0;
}

void c_fbaRL::InitOptionsMenu()
{
	options_menu = new c_Menu(9);
}

void c_fbaRL::EndOptionsMenu()
{
	SAFE_DELETE(options_menu);
}

void c_fbaRL::InitZipInfoMenu()
{
	zipinfo_menu = new c_Menu(34);

	uint32_t		i;
	mz_bool			bZipStatus;
	//size_t			uncomp_size;
	mz_zip_archive	zip_archive;

	// Now try to open the archive.
	memset(&zip_archive, 0, sizeof(zip_archive));
	bZipStatus = mz_zip_reader_init_file(&zip_archive, games[nSelectedGame]->path, 0);
	if (!bZipStatus)
	{
		FILE* fp = fopen("/dev_hdd0/game/FBAL00123/USRDIR/zip_error_log.txt", "w");
		if(fp) {
			fprintf(fp, "mz_zip_reader_init_file() failed!\n");
			fclose(fp);
			fp = NULL;
		}

		SAFE_DELETE(zipinfo_menu);

		return;
	}

	// Get and print information about each file in the archive.
	for (i = 0; i < mz_zip_reader_get_num_files(&zip_archive); i++)
	{
		mz_zip_archive_file_stat file_stat;
		if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat))
		{
			FILE* fp = fopen("/dev_hdd0/game/FBAL00123/USRDIR/zip_error_log.txt", "w");
			if(fp) {
				fprintf(fp, "mz_zip_reader_file_stat() failed!\n");
				fclose(fp);
				fp = NULL;
			}
			mz_zip_reader_end(&zip_archive);

			SAFE_DELETE(zipinfo_menu);

			return;
		}

		char szTmp[1024] = { 0 };
		sprintf(szTmp, "\t[ %s ] [SIZE: %u bytes] [CRC32: %08X] \n", 
			file_stat.m_filename,
			(uint32_t)file_stat.m_uncomp_size, 
			//(uint32_t)file_stat.m_comp_size,
			(uint32_t)file_stat.m_crc32
		);

		zipinfo_menu->AddItem((char*)szTmp);
	}
}

void c_fbaRL::EndZipInfoMenu()
{
	SAFE_DELETE(zipinfo_menu);
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

void c_fbaRL::Frame()
{
	DisplayFrame();
	InputFrame();
	DlgDisplayFrame();
}

#define COLOR_ORANGE	0xff1780f8
#define COLOR_GREEN		0xff00ff00
#define COLOR_YELLOW	0xff00ccff
#define COLOR_RED		0xff0000ff
#define COLOR_WHITE		0xffffffff

//char* szZipInfoMsg[2048] = { 0 }; // TODO: put this properly in the class

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

void c_fbaRL::DisplayFrame()
{
	if(nSection == SECTION_GAMELIST) 
	{
		float xPos		= 0.0400f;
		float yPos		= 0.0500f;
		float yPosDiff	= 0.0200f;	
		float nFontSize = 0.5500f;

		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_ORANGE, "+--------------------------------------------------------------------------------------------+" );
		yPos += yPosDiff;	
		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, _APP_TITLE);
		yPos += yPosDiff;
		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_ORANGE, "+--------------------------------------------------------------------------------------------+" );
		yPos += yPosDiff;

		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, "\tPRESS -(X)- TO LOAD GAME");
		yPos += yPosDiff;
		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, "\tPRESS -[ ]- TO VIEW ZIP INFO");
		yPos += yPosDiff;
		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, "\tPRESS -(/\\)- TO VIEW GAME INFO");
		yPos += yPosDiff;
		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, "\tPRESS -[L2 / R2]- FOR QUICK LIST NAVIGATION");
		yPos += yPosDiff;
		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, "\tPRESS -[SELECT]- TO RETURN TO MAIN MENU");
		yPos += yPosDiff;

		//cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, "\tDEBUG-> LSX: %d LSY: %d RSX: %d RSY: %d ", app.mValLStickX, app.mValLStickY, app.mValRStickX, app.mValRStickY);
		//yPos += yPosDiff;

		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_ORANGE, "+--------------------------------------------------------------------------------------------+" );
		yPos += yPosDiff;
		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, "\tTOTAL GAMES FOUND: %d", nTotalGames);
		yPos += yPosDiff;
		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_ORANGE, "+--------------------------------------------------------------------------------------------+" );
		yPos += yPosDiff;

		int nGameListMax = 19;

		if(games[0])
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
				if(nGame == nTotalGames) break;

				uint32_t nColor	= COLOR_WHITE;		// white
				nFontSize = 0.5500f;

				// GAME SELECTED
				if(nGame == nSelectedGame) 
				{
					nColor		= COLOR_YELLOW;		// yellow
					nFontSize	= 0.6500f;
				}

				cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, "\t[%d]\t%s", 
					nGame+1, 
					games[nGame]->title
					);
				yPos += yPosDiff;

				nGame++;
			}
		
			nFontSize = 0.5500f;
		}
		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_ORANGE, "+--------------------------------------------------------------------------------------------+" );
		yPos += yPosDiff;

		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, "\tGAME INFORMATION:" );
		yPos += yPosDiff;

		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_ORANGE, "+--------------------------------------------------------------------------------------------+" );
		yPos += yPosDiff;

		if(nBurnSelected >= 0) {

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

		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_ORANGE, "+--------------------------------------------------------------------------------------------+" );
		yPos += yPosDiff;
	}

	if(nSection == SECTION_ZIPINFO)
	{
		float xPos		= 0.0400f;
		float yPos		= 0.0500f;
		float yPosDiff	= 0.0200f;	
		float nFontSize = 0.5500f;

		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_ORANGE, "+--------------------------------------------------------------------------------------------+" );
		yPos += yPosDiff;	
		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE,  "\tFB ALPHA RETRO LOADER \\ ZIP INFORMATION \\ %s", games[nSelectedGame]->zipname);
		yPos += yPosDiff;
		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_ORANGE, "+--------------------------------------------------------------------------------------------+" );
		yPos += yPosDiff;
		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE,  "\tPRESS -[L2 / R2]- FOR QUICK LIST NAVIGATION");
		yPos += yPosDiff;
		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE,  "\tPRESS -(O)- TO RETURN TO GAME LIST");
		yPos += yPosDiff;
		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_ORANGE, "+--------------------------------------------------------------------------------------------+" );
		yPos += yPosDiff;
		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, "\tTOTAL FILES FOUND: %d", zipinfo_menu->nTotalItem);
		yPos += yPosDiff;
		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_ORANGE, "+--------------------------------------------------------------------------------------------+" );
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

		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_ORANGE, "+--------------------------------------------------------------------------------------------+" );
		yPos += yPosDiff;
	}

	if(nSection == SECTION_MAIN)
	{
		float xPos		= 0.0400f;
		float yPos		= 0.0500f;
		float yPosDiff	= 0.0400f;	
		float nFontSize = 1.0500f;

		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_ORANGE, "+--------------------------------------------------------------------+" );
		yPos += yPosDiff;	
		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE,  "                        FB ALPHA RETRO LOADER                        " );
		yPos += yPosDiff;
		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE,  "                                v1.01                                " );
		yPos += yPosDiff;
		cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_ORANGE, "+--------------------------------------------------------------------+" );
		yPos += yPosDiff;

		yPos += yPosDiff;

		int nMenuItem = main_menu->UpdateTopItem();
		
		while(nMenuItem <= (main_menu->nTopItem + main_menu->nListMax))
		{
			if(nMenuItem == main_menu->nTotalItem) break;

			uint32_t nColor	= COLOR_WHITE;		// white

			// SELECTED
			if(nMenuItem == main_menu->nSelectedItem) {
				nColor		= COLOR_YELLOW;		// yellow
			}

			// This tricky code will center text properly...
			int nLen			= strlen(main_menu->item[nMenuItem]->szMenuLabel);
			int nMaxLineChars	= 69;
			int nSpaces			= (nMaxLineChars - nLen) / 2;

			char szFinalString[256] = "";

			for(int n = 0; n < nSpaces; n++)
			{
				strcat(szFinalString, " ");
			}
			strcat(szFinalString, main_menu->item[nMenuItem]->szMenuLabel);

			// Display centered text :)...
			cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, "%s", szFinalString);
			yPos += yPosDiff;

			nMenuItem++;
		}
	}

}

void LaunchRetroArch(char* arg1, char* arg2, char* arg3, char* arg4) 
{
	// TODO: Add error checking for arguments...

	char* argv[5] = { 0 };	
	argv[0] = (char*)malloc( strlen(arg1) + 1); 
	argv[1] = (char*)malloc( strlen(arg2) + 1); 
	argv[2] = (char*)malloc( strlen(arg3) + 1); 
	argv[3] = (char*)malloc( strlen(arg4) + 1);
	

	strcpy(argv[0], arg1);	// [ROM / ZIP path]			optional, can be dummy / filler argument
	strcpy(argv[1], arg2);	// [FBARL RELOAD.SELF path] MUST be the valid external launcher path
	strcpy(argv[2], arg3);	// [input preset cfg]		optional, can be dummy / filler argument
	strcpy(argv[3], arg4);	// [aspect ratio]			optional, can be dummy / filler argument
	argv[4] = NULL;

	char fba_core_path[] = "/dev_hdd0/game/FBAL00123/USRDIR/cores/fb_alpha.SELF";
	sys_game_process_exitspawn(fba_core_path, (const char**)argv, NULL, NULL, 0, 1000, SYS_PROCESS_PRIMARY_STACK_SIZE_1M);
}

void c_fbaRL::InputFrame()
{
	static int nSelInputFrame = 0;

	// ------------------------------------------------------
	// Navigation UP/DOWN with delay

	if(((app.mFrame + nSelInputFrame) - app.mFrame) == 5)
	{
		// DPAD UP / LEFT ANALOG UP
		if((app.mIsUpPressed && app.upPressedNow) || (app.mValLStickY < 50))
		{
			switch(nSection) 
			{
				case SECTION_GAMELIST: 
				{
					if(nSelectedGame > 0 && nSelectedGame <= nTotalGames) 
					{
						nSelectedGame--;
						if(games[0]) UpdateBurnSelected(games[nSelectedGame]->zipname);
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
		if	( (app.mIsDownPressed && app.downPressedNow) ||	(app.mValLStickY > 200) )
		{
			switch(nSection)
			{
				case SECTION_GAMELIST:
				{
					if(nSelectedGame >= 0 && nSelectedGame < nTotalGames-1) 
					{
						nSelectedGame++;
						if(games[0]) UpdateBurnSelected(games[nSelectedGame]->zipname);
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
	if(app.mValLStickY < 50){
	}
	// LEFT ANALOG [DOWN]
	if(app.mValLStickY > 200){
	}
	// LEFT ANALOG [LEFT]
	if(app.mValLStickX < 50){
	}
	// LEFT ANALOG [RIGHT]
	if(app.mValLStickX > 200){
	}
	// RIGHT ANALOG [UP]
	if(app.mValRStickY < 50){
	}
	// RIGHT ANALOG [DOWN]
	if(app.mValRStickY > 200){
	}
	// RIGHT ANALOG [LEFT]
	if(app.mValRStickX < 50){
	}
	// RIGHT ANALOG [RIGHT]
	if(app.mValRStickX > 200){
	}

	// ------------------------------------------------------
	// [ ] - SQUARE

	if(!app.mIsSquarePressed && app.squarePressedNow)
	{
		switch(nSection)
		{
			case SECTION_GAMELIST:
			{
				InitZipInfoMenu();
				nSection = SECTION_ZIPINFO;
				break;
			}
		}
	}

	// ------------------------------------------------------
	// (X) - CROSS

	if ( !app.mIsCrossPressed && app.crossPressedNow ) 
	{
		switch(nSection)
		{ 
			case SECTION_GAMELIST: 
			{
				app.onShutdown();

				char fba_rl_path[]		= "/dev_hdd0/game/FBAL00123/USRDIR/RELOAD.SELF";
				char preset_cfg[]		= "DUMMY_ARG"; // todo: implement module to set this
				char aspect_ratio[12]	= { 0 };
				extern int nBurnSelected;
				sprintf(aspect_ratio, "%d:%d", fba_drv[nBurnSelected].nAspectX, fba_drv[nBurnSelected].nAspectY);

				LaunchRetroArch(
					(char*)games[nSelectedGame]->path, 
					(char*)fba_rl_path, 
					(char*)preset_cfg,
					(char*)aspect_ratio
				);

				break;
			}

			case SECTION_MAIN:
			{
				int nMenuItem = main_menu->nSelectedItem;

				// Gamelist
				if(nMenuItem == 0)
				{
					InitGameList();
					nSection = SECTION_GAMELIST;
					EndMainMenu();
					break;
				}

				// Options
				if(nMenuItem == 1)
				{
					//nSection = SECTION_OPTIONS;
					EndMainMenu();
					break;
				}

				// Load RetroArch
				if(nMenuItem == 2)
				{
					app.onShutdown();

					char fba_rl_path[]	= "/dev_hdd0/game/FBAL00123/USRDIR/RELOAD.SELF";
					char rom_path[]		= "DUMMY_ARG";
					char preset_cfg[]	= "DUMMY_ARG";
					char aspect_ratio[]	= "DUMMY_ARG";

					LaunchRetroArch(
						(char*)rom_path, 
						(char*)fba_rl_path, 
						(char*)preset_cfg,
						(char*)aspect_ratio
					);

					EndMainMenu();
					break;
				}

				// Load RetroArch
				if(nMenuItem == 3)
				{
					app.onShutdown();

					char fba_retroarch_path[]	= "/dev_hdd0/game/SSNE10000/USRDIR/cores/fb_alpha.SELF";
					sys_game_process_exitspawn(fba_retroarch_path, NULL, NULL, NULL, 0, 1000, SYS_PROCESS_PRIMARY_STACK_SIZE_1M);

					EndMainMenu();
					break;
				}

				// Load multiMAN
				if(nMenuItem == 4)
				{
					app.onShutdown();
					char path[] = "/dev_hdd0/game/BLES80608/USRDIR/RELOAD.SELF";
					sys_game_process_exitspawn(path, NULL, NULL, NULL, 0, 1000, SYS_PROCESS_PRIMARY_STACK_SIZE_1M);

					EndMainMenu();
					break;
				}

				// Exit
				if(nMenuItem == 5)
				{
					app.onShutdown();
					exit(0);

					EndMainMenu();
					break;
				}
				break;
			}
		}

	}	

	// ------------------------------------------------------
	// (O) - CIRCLE

	if (!app.mIsCirclePressed && app.circlePressedNow) 
	{
		switch(nSection) 
		{
			case SECTION_ZIPINFO:
			{
				EndZipInfoMenu();
				// no need to init GameList here...
				nSection = SECTION_GAMELIST;
				break;
			}
		}
	}

	// ------------------------------------------------------
	// (/\) - TRIANGLE

	if( !app.mIsTrianglePressed && app.trianglePressedNow)
	{
		// ...
	}

	// ------------------------------------------------------
	// [L1]

	if( !app.mIsL1Pressed && app.l1PressedNow)
	{
		// ...
	}

	// ------------------------------------------------------
	// [R1]

	if( !app.mIsR1Pressed && app.r1PressedNow)
	{
		switch(nSection) 
		{
			case SECTION_GAMELIST:
				// ...
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
		if(app.mIsL2Pressed && app.l2PressedNow)
		{
			switch(nSection)
			{
				case SECTION_GAMELIST:
				{
					nSelectedGame -= 19;

					if(nSelectedGame < 0) {
						nSelectedGame = 0;
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
			}
		}

		// ------------------------------------------------------
		// [R2]

		if(app.mIsR2Pressed && app.r2PressedNow)
		{
			switch(nSection)
			{
				case SECTION_GAMELIST:
				{
					nSelectedGame += 19;
					if(nSelectedGame > nTotalGames-1) {
						nSelectedGame = nTotalGames-1;
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

	if(!app.mIsSelectPressed && app.selectPressedNow)
	{
		switch(nSection)
		{
			case SECTION_GAMELIST: 
			{
				EndGameList();
				InitMainMenu();
				nSection = SECTION_MAIN;
				break;
			}
		}
	}

	// ------------------------------------------------------
	// [START]


	if(!app.mIsStartPressed && app.startPressedNow)
	{
		switch(nSection) 
		{
			case SECTION_GAMELIST: 
			{
				// Refresh gamelist
				RefreshGameList();
			}
		}
	}


	// ------------------------------------------------------
	// [L3]

	if( !app.mIsL3Pressed && app.l3PressedNow)
	{

	}

	// ------------------------------------------------------
	// [R3]
	
	if( !app.mIsR3Pressed && app.r3PressedNow)
	{

	}

}

// Todo: Remove this...
void c_fbaRL::DlgDisplayFrame()
{
	// ...
	nStatus = STATUS_NORMAL;
}

int c_fbaRL::ParseGameList()
{
	// ROM(s) location(s)
	char szDir[][256] = {
		"ROMS"	, "roms", "Roms",
		"FBA"	, "fba"	, "Fba"	,
		"USRDIR"
	};

	// Where to look for directories
	char szDevice[][256] = 
	{
		"/dev_hdd0",
		"/dev_hdd0/roms",
		"/dev_hdd0/Roms",
		"/dev_hdd0/ROMS",

		"/dev_hdd0/game/FBAL00123",
		"/dev_hdd0/game/FBAL00123/USRDIR",

		"/dev_hdd0/game/ROMS00000",
		"/dev_hdd0/game/ROMS00000/USRDIR",

		"/dev_hdd0/game/SSNE10000/USRDIR",
		"/dev_hdd0/game/SSNE10000/USRDIR/cores",

		"/dev_usb000", "/dev_usb001", "/dev_usb002", "/dev_usb003",		
		"/dev_usb000/roms", "/dev_usb001/roms", "/dev_usb002/roms", "/dev_usb003/roms",
		"/dev_usb000/Roms", "/dev_usb001/Roms", "/dev_usb002/Roms", "/dev_usb003/Roms",
		"/dev_usb000/ROMS", "/dev_usb001/ROMS", "/dev_usb002/ROMS", "/dev_usb003/ROMS",

		"/dev_cf",
		"/dev_cf/roms",
		"/dev_cf/Roms",
		"/dev_cf/ROMS",

		"/dev_sd",
		"/dev_sd/roms",
		"/dev_sd/Roms",
		"/dev_sd/ROMS",

		"/dev_ms",
		"/dev_ms/roms",
		"/dev_ms/Roms",
		"/dev_ms/ROMS"
	};

	// Scan each device for directories...
	for(unsigned int nDev = 0; nDev < (sizeof(szDevice) / 256); nDev++)
	{
		int d = 0;

		// device is mounted
		//DIR* d = opendir(szDevice[nDev]);
		//if(d != NULL)
		if(cellFsOpendir(szDevice[nDev], &d) == CELL_FS_SUCCEEDED) 
		{
			CellFsDirent* dirEntry = (CellFsDirent*)malloc(sizeof(CellFsDirent));
			memset(dirEntry, 0, sizeof(CellFsDirent));
			//dirent* dirEntry = NULL;
			//memset(dirEntry, 0, sizeof(dirent));

			uint64_t nread = 0;

			// start scanning for directories...
			//while((dirEntry = readdir(d)) != NULL) 
			while(cellFsReaddir(d, dirEntry, &nread) == CELL_FS_SUCCEEDED)			 
			{
				if(nread == 0) break;

				// found a directory...
				if (dirEntry->d_type == DT_DIR)
				{					
					// check if its one of the directories we are looking for...					
					for(unsigned int nDir = 0; nDir < (sizeof(szDir) / 256); nDir++) 
					{
						// if we got a winner, then look for ZIP files inside the directory...
						if(strcmp(dirEntry->d_name, szDir[nDir]) == 0)
						{
							// ---
							char szDirectory[256] = { 0 };
							sprintf(szDirectory, "%s/%s", szDevice[nDev], szDir[nDir]);
							
							//DIR* d2 = NULL;
							//d2 = opendir(szDirectory);
							//if(d2 != NULL)

							int d2 = 0;
							if(cellFsOpendir(szDirectory, &d) == CELL_FS_SUCCEEDED)							
							{
								CellFsDirent* dirEntry2 = (CellFsDirent*)malloc(sizeof(CellFsDirent));
								memset(dirEntry2, 0, sizeof(CellFsDirent));

								//dirent* dirEntry2 = NULL;
								//memset(dirEntry2, 0, sizeof(dirent));

								uint64_t nread2 = 0;
								
								//while((dirEntry2 = readdir(d2)) != NULL)  
								while(cellFsReaddir(d, dirEntry2, &nread2) == CELL_FS_SUCCEEDED)								
								{
									if (dirEntry2->d_type == DT_DIR)
									{
										// DIRECTORY
										// we're not looking for directories here...
									} else {

										if(dirEntry2->d_type != DT_REG) break;

										// FILE

										int nNameLen = strlen(dirEntry2->d_name);
										if (nNameLen < 5) continue;

										char* pszFilename = NULL;

										pszFilename = (char*)malloc(nNameLen+1);										
										memcpy(pszFilename, dirEntry2->d_name, nNameLen);
										pszFilename[nNameLen] = 0;

										// Check file extension...
										if(NULL != strstr(toLowerCase(pszFilename, strlen(pszFilename)), ".zip")) 
										{											
											UpdateBurnSelected(toLowerCase(pszFilename, strlen(pszFilename)));											
											
											// Discard listing of BIOS ROM(s)
											if( strncmp(toLowerCase(pszFilename, strlen(pszFilename)), "neogeo"	, strlen("neogeo")) == 0 ||
												strncmp(toLowerCase(pszFilename, strlen(pszFilename)), "pgm"	, strlen("pgm"))	== 0 ||
												strncmp(toLowerCase(pszFilename, strlen(pszFilename)), "neocdz"	, strlen("neocdz")) == 0   )
											{
												continue;
											}

											// Validate FBA ROM (no CRC32 checking for now, just ZIP name)
											if(nBurnSelected >= 0) 
											{
												if((games[0]) && IsBurnDuplicate(toLowerCase(pszFilename, strlen(pszFilename))))
												{
													continue; // keep going we already have this game added...
												} else {

													// check if zip has all files 
													games[nTotalGames] = new c_game;
													sprintf(games[nTotalGames]->title, "%s", fba_drv[nBurnSelected].szTitle);
												}
											} else {
												// not a FBA ROM, continue...
												continue;
											}

											// Path
											if(szDirectory[strlen(szDirectory)-1] == '/')
											{
												szDirectory[strlen(szDirectory)-1] = 0;
											}

											sprintf(games[nTotalGames]->path, "%s/%s", szDirectory, dirEntry2->d_name);				

											// zip title
											sprintf(games[nTotalGames]->zipname, "%s", dirEntry2->d_name);

											// Size in bytes
											games[nTotalGames]->nSize = GetFileSize(games[nTotalGames]->path);

											nTotalGames++;
										}

										SAFE_FREE(pszFilename);
									}
								}
								cellFsClosedir(d2);
								//closedir(d2);
								//d2 = NULL;
							}
							// ---
						}
					}

				} else {
					// FILE
				}
			}
			cellFsClosedir(d);
			d = NULL;
			//closedir(d);
			//d = NULL;
		}
	}
	return 1;
}

void c_fbaRL::RefreshGameList()
{
	EndGameList();
	InitGameList();
	// Update info module current driver selection
	UpdateBurnSelected(games[nSelectedGame]->zipname);
}


uint64_t c_fbaRL::GetFileSize(char* szFilePath)
{
	CellFsStat sb;
	memset(&sb, 0, sizeof(CellFsStat));

	cellFsStat(szFilePath, &sb);

	return sb.st_size;
}
