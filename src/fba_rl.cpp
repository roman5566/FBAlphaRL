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
#define _APP_TITLE "FB Alpha Retro Loader v1.00 - by CaptainCPS-X [2013]"
// --------------------------------------------------------------------

//#define FBARL_DEBUG

#include "main.h"
#include "misc.h"
#include "fba_rl.h"
#include "burn_drivers.h" // Driverlist Generated via custom FB Alpha build [ v0.2.97.28 ][ Windows ]

int nBurnSelected = -1;

c_fbaRL* fbaRL;

// -------------------------------------------------------

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
		if(strcmp(gamelst[nGame].zipname, szROM) == 0)
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

// Initialize fbaRL module
c_fbaRL::c_fbaRL()
{
	nSelectedGame	= 0;
	nGameListTop	= 0;
	nTotalGames		= 0;
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

void c_fbaRL::DisplayFrame()
{
	float xPos		= 0.04f;
	float yPos		= 0.05f;
	float yPosDiff	= 0.03f;	
	float nFontSize = 0.55f;

	::cellDbgFontPuts(xPos, yPos, nFontSize, COLOR_ORANGE, "+--------------------------------------------------------------------------------------------+" );
	yPos += yPosDiff;	
	::cellDbgFontPuts(xPos, yPos, nFontSize, COLOR_WHITE, _APP_TITLE);
	yPos += yPosDiff;
	::cellDbgFontPuts(xPos, yPos, nFontSize, COLOR_ORANGE, "+--------------------------------------------------------------------------------------------+" );
	yPos += yPosDiff;
	::cellDbgFontPuts(xPos, yPos, nFontSize, COLOR_WHITE, "PRESS -(O)- TO EXIT AND RETURN TO XMB");
	yPos += yPosDiff;
	::cellDbgFontPuts(xPos, yPos, nFontSize, COLOR_WHITE, "PRESS -(X)- TO LOAD GAME");
	yPos += yPosDiff;
	//::cellDbgFontPuts(xPos, yPos, nFontSize, COLOR_WHITE, "PRESS -(/\\)- TO VIEW GAME INFO");
	//yPos += yPosDiff;
	::cellDbgFontPuts(xPos, yPos, nFontSize, COLOR_WHITE, "PRESS -[SELECT]- TO LOAD \"RetroArch (FB Alpha Core)\"");
	yPos += yPosDiff;

#ifdef FBARL_DEBUG
	::cellDbgFontPuts(xPos, yPos, nFontSize, COLOR_WHITE, "PRESS -[START]- TO LOAD \"multiMAN\"");
	yPos += yPosDiff;
#endif

	::cellDbgFontPuts(xPos, yPos, nFontSize, COLOR_ORANGE, "+--------------------------------------------------------------------------------------------+" );
	yPos += yPosDiff;

	::cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, 
		"Total Games found on all scanned directories: %d", nTotalGames);

	yPos += yPosDiff;

	::cellDbgFontPuts(xPos, yPos, nFontSize, COLOR_ORANGE, "+--------------------------------------------------------------------------------------------+" );
	yPos += yPosDiff;

	int nGameListMax = 9;

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
		nFontSize		= 0.6f;

		// GAME SELECTED
		if(nGame == nSelectedGame) 
		{
			nColor		= COLOR_YELLOW;		// yellow
			nFontSize	= 0.6f;
		}

		::cellDbgFontPrintf(xPos, yPos, nFontSize, nColor, "[%d] %s", 
			nGame+1, 
			gamelst[nGame].title
			);
		yPos += yPosDiff;

		nGame++;
	}

	nFontSize = 0.55f;

	::cellDbgFontPuts(xPos, yPos, nFontSize, COLOR_ORANGE, "+--------------------------------------------------------------------------------------------+" );
	yPos += yPosDiff;

	::cellDbgFontPuts(xPos, yPos, nFontSize, COLOR_WHITE, "Game Info:" );
	yPos += yPosDiff;

	::cellDbgFontPuts(xPos, yPos, nFontSize, COLOR_ORANGE, "+--------------------------------------------------------------------------------------------+" );
	yPos += yPosDiff;

	if(nBurnSelected >= 0) {

		::cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, 
			"Romset: %s", fba_drv[nBurnSelected].szName);
		yPos += yPosDiff;

		::cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, 
			"Parent: %s", fba_drv[nBurnSelected].szParent);
		yPos += yPosDiff;

		::cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, 
			"Company: %s", fba_drv[nBurnSelected].szCompany);
		yPos += yPosDiff;

		::cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, 
			"Year: %s", fba_drv[nBurnSelected].szYear);
		yPos += yPosDiff;

		::cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, 
			"System: %s", fba_drv[nBurnSelected].szSystem);
		yPos += yPosDiff;

		::cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, 
			"Max Players: %d", fba_drv[nBurnSelected].nMaxPlayers);
		yPos += yPosDiff;

		::cellDbgFontPrintf(xPos, yPos, nFontSize, COLOR_WHITE, 
			"Game Resolution: %d x %d (%d:%d)", 
			fba_drv[nBurnSelected].nWidth, fba_drv[nBurnSelected].nHeight,
			fba_drv[nBurnSelected].nAspectX, fba_drv[nBurnSelected].nAspectY
			);
		yPos += yPosDiff;

	}

	::cellDbgFontPuts(xPos, yPos, nFontSize, COLOR_ORANGE, "+--------------------------------------------------------------------------------------------+" );
	yPos += yPosDiff;

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
	// Navigation UP/DOWN with no delay

	if( !app.mIsUpPressed && app.upPressedNow)
	{
		if(nSelectedGame > 0 && nSelectedGame <= nTotalGames) 
		{
			nSelectedGame--;
			UpdateBurnSelected(gamelst[nSelectedGame].zipname);
		}
		nSelInputFrame = 0;
	} 

	if( !app.mIsDownPressed && app.downPressedNow)
	{
		if(nSelectedGame >= 0 && nSelectedGame < nTotalGames-1)
		{
			nSelectedGame++;
			UpdateBurnSelected(gamelst[nSelectedGame].zipname);
		}
		nSelInputFrame = 0;
	}

	// ------------------------------------------------------
	// Navigation UP/DOWN with delay

	if(((app.mFrame + nSelInputFrame) - app.mFrame) == 5)
	{
		if( app.mIsUpPressed && app.upPressedNow)
		{
			if(nSelectedGame > 0 && nSelectedGame <= nTotalGames) 
			{
				nSelectedGame--;
				UpdateBurnSelected(gamelst[nSelectedGame].zipname);
			}
		}
		if( app.mIsDownPressed && app.downPressedNow)
		{		
			if(nSelectedGame >= 0 && nSelectedGame < nTotalGames-1) 
			{
				nSelectedGame++;
				UpdateBurnSelected(gamelst[nSelectedGame].zipname);
			}
		}
		nSelInputFrame = 0;
	}

	nSelInputFrame++;

	// ------------------------------------------------------
	// [ ] - SQUARE

	if(!app.mIsSquarePressed && app.squarePressedNow)
	{
		// ...
	}

	// ------------------------------------------------------
	// (X) - CROSS

	if ( !app.mIsCrossPressed && app.crossPressedNow ) 
	{
		app.onShutdown();

		char fba_rl_path[]		= "/dev_hdd0/game/FBAL00123/USRDIR/RELOAD.SELF";
		char preset_cfg[]		= "DUMMY_ARG"; // todo: implement module to set this
		char aspect_ratio[12]	= { 0 };
		sprintf(aspect_ratio, "%d:%d", fba_drv[nBurnSelected].nAspectX, fba_drv[nBurnSelected].nAspectY);

		LaunchRetroArch(
			(char*)gamelst[nSelectedGame].path, 
			(char*)fba_rl_path, 
			(char*)preset_cfg,
			(char*)aspect_ratio
		);

	}	

	// ------------------------------------------------------
	// (O) - CIRCLE

	if (!app.mIsCirclePressed && app.circlePressedNow) 
	{
		app.onShutdown();
		exit(0);
	}

	// ------------------------------------------------------
	// [SELECT]

	if(!app.mIsSelectPressed && app.selectPressedNow)
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
	}

	// ------------------------------------------------------
	// [START]

#ifdef FBARL_DEBUG
	if(!app.mIsStartPressed && app.startPressedNow)
	{
		app.onShutdown();
		char path[] = "/dev_hdd0/game/BLES80608/USRDIR/RELOAD.SELF";
		sys_game_process_exitspawn(path, NULL, NULL, NULL, 0, 1000, SYS_PROCESS_PRIMARY_STACK_SIZE_1M);
	}
#endif

}

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
		//int device;

		// device is mounted
		//if(cellFsOpendir(szDevice[nDev], &device) == CELL_FS_SUCCEEDED) 
		DIR* d = NULL;

		d = opendir(szDevice[nDev]);
		if(d != NULL)
		{
			//CellFsDirent dirEntry;
			dirent* dirEntry = NULL;
			//memset(dirEntry, 0, sizeof(dirent));

			//uint64_t nread = 0;

			// start scanning for directories...
			//while(cellFsReaddir(device, &dirEntry, &nread) == CELL_FS_SUCCEEDED)	
			while((dirEntry = readdir(d)) != NULL)  
			{
				//if(nread == 0) break;

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
							//int d;

							DIR* d2 = NULL;

							char szDirectory[256] = { 0 };
							sprintf(szDirectory, "%s/%s", szDevice[nDev], szDir[nDir]);

							d2 = opendir(szDirectory);

							//if(cellFsOpendir(szDirectory, &d) == CELL_FS_SUCCEEDED)
							if(d2 != NULL)
							{
								//CellFsDirent dirEntry2;
								//memset(&dirEntry2, 0, sizeof(CellFsDirent));

								dirent* dirEntry2 = NULL;
								//memset(dirEntry2, 0, sizeof(dirent));


								//uint64_t nread2 = 0;

								//while(cellFsReaddir(d, &dirEntry2, &nread2) == CELL_FS_SUCCEEDED)	
								while((dirEntry2 = readdir(d2)) != NULL)  
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

											// Validate FBA ROM (no CRC32 checking for now, just ZIP name)
											if(nBurnSelected >= 0) 
											{
												if(IsBurnDuplicate(toLowerCase(pszFilename, strlen(pszFilename))))
												{
													continue; // keep going we already have this game added...
												} else {
													sprintf(gamelst[nTotalGames].title, "%s", fba_drv[nBurnSelected].szTitle);
												}
											} else {
												// not a FBA ROM, continue...
												continue;
											}

											// Discard listing of BIOS ROM(s)
											if( strncmp(toLowerCase(pszFilename, strlen(pszFilename)), "neogeo"	, strlen("neogeo")) == 0 ||
												strncmp(toLowerCase(pszFilename, strlen(pszFilename)), "pgm"	, strlen("pgm"))	== 0 ||
												strncmp(toLowerCase(pszFilename, strlen(pszFilename)), "neocdz"	, strlen("neocdz")) == 0   )
											{
												continue;
											}

											// Path
											if(szDirectory[strlen(szDirectory)-1] == '/')
											{
												szDirectory[strlen(szDirectory)-1] = 0;
											}

											sprintf(gamelst[nTotalGames].path, "%s/%s", szDirectory, dirEntry2->d_name);				

											// zip title
											sprintf(gamelst[nTotalGames].zipname, "%s", dirEntry2->d_name);

											// Size in bytes
											gamelst[nTotalGames].nSize = GetFileSize(gamelst[nTotalGames].path);

											nTotalGames++;
										}

										if(pszFilename) {
											free(pszFilename);
											pszFilename = NULL;
										}
									}
								}
								//cellFsClosedir(d);
								closedir(d2);
								d2 = NULL;
							}
							// ---
						}
					}

				} else {
					// FILE
				}
			}
			//cellFsClosedir(device);
			//device = NULL;
			closedir(d);
			d = NULL;
		}
	}
	return 1;
}

void c_fbaRL::RefreshGameList()
{
	// TODO: Debug this loop (should edit frame modules to avoid conflict)
	if(nTotalGames > 0) 
	{
		if(gamelst) {
			free(gamelst);
			gamelst = NULL;
		}
		nTotalGames = 0;
	}

	// Up to 15,000 listed games (LMAO! xD)
	gamelst = (struct c_gamelist*)malloc(sizeof(struct c_gamelist) * 15000);

	ParseGameList();

	// Sort by Title
	qsort(gamelst, nTotalGames, sizeof(struct c_gamelist), _FcCompareStruct);

	// Update info module current driver selection
	UpdateBurnSelected(gamelst[nSelectedGame].zipname);
}


uint64_t c_fbaRL::GetFileSize(char* szFilePath)
{
	CellFsStat sb;
	memset(&sb, 0, sizeof(CellFsStat));

	cellFsStat(szFilePath, &sb);

	return sb.st_size;
}
