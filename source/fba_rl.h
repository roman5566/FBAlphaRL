#ifndef FBA_RL_H
#define FBA_RL_H
// ---------------------------------------------------------------------
// FB Alpha Retro Loader (CaptainCPS-X, 2013)
// ---------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

// --------------------------------------------------------------------
#define _APP_VER "1.03"
#define _APP_TITLE "\tFB ALPHA RETRO LOADER "_APP_VER" - by CaptainCPS-X [2013]"
// --------------------------------------------------------------------

struct FBA_DRV 
{ 
	uint32_t nDrv; 
	char szName[32]; 
	char szParent[32];
	char szBoardROM[32];
	char szTitle[256];
	char szYear[8];
	char szCompany[256];
	char szSystem[256];
	uint32_t nMaxPlayers;
	uint32_t nWidth; 
	uint32_t nHeight;
	uint32_t nAspectX;
	uint32_t nAspectY;
	char szSystemFilter[32];
};

extern FBA_DRV fba_drv[3183];

extern unsigned char	example_cfg[20570];		// For creating basic input presets
void _ScanROMs(uint64_t);

#define STATUS_NORMAL			0
#define STATUS_SHOW_ZIPINFO		1
#define STATUS_UPDATEPREVIEW	2
#define STATUS_RESETPREVIEW		3
#define STATUS_EXIT				4
#define STATUS_EXIT_FBARL_RA	5
#define STATUS_EXIT_RA			6
#define STATUS_EXIT_MMAN		7
#define STATUS_EXIT_IMANAGER	8

#define STATUS_ROMSCAN_DLG		20
#define STATUS_MISSING_GAME_DLG 21
#define STATUS_CORRUPT_APP_DLG	22

#define STATUS_ROMSCAN_START	30
#define STATUS_ROMSCAN_END		31
#define STATUS_ROMPATH_CHANGED	32

#define SECTION_MAIN			0
#define SECTION_GAMELIST		1
#define SECTION_ZIPINFO			2
#define SECTION_HELP			3
#define SECTION_OPTIONS			4
#define SECTION_FILEBROWSER		5

#define MENU_MAIN_GAMELIST		0
#define MENU_MAIN_OPTIONS		1
#define MENU_MAIN_FBARL_RA		2
#define MENU_MAIN_IMANAGER		3
#define MENU_MAIN_MMAN			4
#define MENU_MAIN_EXIT			5

// Filter system
#define MASKCAPMISC		0
#define MASKCAVE		1
#define MASKCPS			2
#define	MASKCPS2		3
#define MASKCPS3		4
#define MASKDATAEAST	5
#define MASKGALAXIAN	6
#define MASKIREM		7
#define MASKKANEKO		8
#define MASKKONAMI		9
#define MASKNEOGEO		10
#define MASKPACMAN		11
#define MASKPGM			12
#define MASKPSIKYO		13
#define MASKSEGA		14
#define MASKSETA		15
#define MASKTAITO		16
#define MASKTECHNOS		17
#define MASKTOAPLAN		18
#define MASKMISCPRE90S	19
#define MASKMISCPOST90S 20

#define MASKALL			21
#define MASKCUSTOM		22

#define MENU_OPT_AUTO_AR		0
#define MENU_OPT_AUTO_CFG		1
#define MENU_OPT_ALT_MKEY		2
#define MENU_OPT_USE_UNIBIOS	3
#define MENU_OPT_DISP_MISS_GMS	4
#define MENU_OPT_FILTER_START	(MENU_OPT_DISP_MISS_GMS+1)

#define SAFE_FREE(x)	if(x) {	free(x); *&x = NULL; }
#define SAFE_DELETE(x)	if(x) { delete x; *&x = NULL; }

#define MAX_GAMES	10000 // <-- should be enough xD

class c_game
{
public:
	
	char*		path;		// ROM Path
	char*		zipname;	// ZIP name
	char*		title;		// Title, if found
	char*		sysmask;	// System Mask / Filter

	uint32_t	nGame;
	uint64_t	nSize;		// size in bytes
	bool		bAvailable; // 

	c_game(uint32_t _nGame)
	{

		path	= (char*)malloc(1024);
		zipname = (char*)malloc(1024);
		title	= (char*)malloc(1024);		
		sysmask = (char*)malloc(1024);
		
		memset(path, 0, 1024);
		memset(zipname, 0, 1024);
		memset(title, 0, 1024);
		memset(sysmask, 0, 1024);

		nGame		= _nGame;
		nSize		= 0;
		bAvailable	= false;
	}

	~c_game()
	{
		SAFE_FREE(path)
		SAFE_FREE(zipname)
		SAFE_FREE(title)
		SAFE_FREE(sysmask)
		*&nGame = NULL;
		*&nSize = NULL;
		*&bAvailable = NULL;
	}
};
class c_MenuItem 
{
public:	
	int		nIndex;
	char*	szMenuLabel;
	
	c_MenuItem(int nId) 
	{
		szMenuLabel = (char*)malloc(1024);
		memset(szMenuLabel, 0, 1024);

		nIndex		= nId;
	}

	~c_MenuItem()
	{
		SAFE_FREE(szMenuLabel)
		*&nIndex = NULL;
	}
};

class c_Menu
{
public:
	int nSelectedItem;
	int nTopItem;
	int nTotalItem;
	int nListMax;
	c_MenuItem* item[2048];

	c_Menu(int nListMaximum)
	{
		nSelectedItem	= 0;
		nTopItem		= 0;
		nTotalItem		= 0;
		nListMax		= nListMaximum;
	}

	~c_Menu()
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

	void AddItem(char* szItemLabel) 
	{
		item[nTotalItem] =  new c_MenuItem(nTotalItem);
		strcpy(item[nTotalItem]->szMenuLabel, szItemLabel);
		nTotalItem++;
	}

	int UpdateTopItem()
	{
		if(nSelectedItem > nListMax || nTopItem > 0)
		{
			if(nTopItem < (nSelectedItem - nListMax)) {
				nTopItem = nSelectedItem - nListMax;
			}
			if(nTopItem > 0 && nSelectedItem < nTopItem) {
				nTopItem = nSelectedItem;
			} else {
				nTopItem = nSelectedItem - nListMax;
			}
		} else {
			nTopItem = nSelectedItem - nListMax;
		}
		if(nTopItem < 0) {
			nTopItem = 0;
		}
		return nTopItem;
	}
};

class c_FilebrowserItem 
{
public:	
	int		nIndex;
	char*	szMenuLabel;
	char*	szPath;
	uint8_t	nType;

	c_FilebrowserItem(int nId) 
	{
		szMenuLabel = (char*)malloc(2048);
		szPath = (char*)malloc(2048);

		memset(szMenuLabel, 0, 2048);		
		memset(szPath, 0, 2048);

		nIndex		= nId;
		nType		= CELL_FS_TYPE_UNKNOWN;
	}

	~c_FilebrowserItem()
	{
		SAFE_FREE(szPath);
		SAFE_FREE(szMenuLabel)
		*&nIndex = NULL;
		*&nType = NULL;
	}
};

class c_Filebrowser
{
public:
	int nSelectedItem;
	int nTopItem;
	int nTotalItem;
	int nListMax;
	c_FilebrowserItem* item[2048];

	char* pszCurrentDir;

	c_Filebrowser(int nListMaximum);

	~c_Filebrowser();

	void AddItem(char* szItemLabel, char* szPath, uint8_t _nType);
	
	void NavigateSelectedDir();

	void NavigateDirUp();

	void ScanContents();

	void CleanUpItems();

	int UpdateTopItem();
};

/*
class c_Files
{
public:
	uint32_t	nFile;

	char*		szName;
	char*		szPath;
	uint64_t	nSize;
	bool		bIsDirectory;

	c_Files(uint32_t _nFile, uint64_t _nSize, char* _szName, char* _szPath, bool _bIsDir)
	{
		szName = (char*)malloc(2048);
		memset(szName, 0, 2048);
		memcpy(szName, _szName, 2047);

		szPath = (char*)malloc(2048);
		memset(szPath, 0, 2048);
		memcpy(szPath, _szPath, 2047);

		nSize = _nSize;
		nFile = _nFile;
		bIsDirectory = _bIsDir;
	}

	~c_Files()
	{
		SAFE_FREE(szName)
		SAFE_FREE(szPath)
		*&nSize = NULL;
		*&nFile = NULL;
		*&bIsDirectory = NULL;
	}
};

class c_Directory
{
public:
	uint32_t	nDirectory;

	uint32_t	nTotalFiles;
	//char*		szName;
	char*		szPath;
	c_Files**	files;

	c_Directory(uint32_t nDir, char* _szPath)
	{
		szPath = (char*)malloc(2048);
		memset(szPath, 0, 2048);
		strcpy(szPath, _szPath);

		//if(szPath[strlen(szPath)-1] == '/')	{
		//	szPath[strlen(szPath)-1] = 0;
		//}

		files = (c_Files**)malloc(sizeof(c_Files) * 30000);
		memset(files, 0, sizeof(c_Files) * 30000);

		nTotalFiles = 0;
		nDirectory = nDir;
	}

	void AddFile(bool bIsDirectory, char* _szFileName, uint64_t _nSize)
	{
		if(bIsDirectory) {
			files[nTotalFiles] = new c_Files(nTotalFiles, 0, _szFileName, szPath, true);
		} else {
			char _szFilePath[2048] = { 0 };
			sprintf(_szFilePath, "%s/%s", szPath, _szFileName);
			files[nTotalFiles] = new c_Files(nTotalFiles, _nSize, _szFileName, _szFilePath, false);
		}
		nTotalFiles++;
	}

	~c_Directory()
	{
		//SAFE_FREE(szName)
		SAFE_FREE(szPath)

		for(uint32_t x=0; x < nTotalFiles; x++) {
			SAFE_DELETE(files[x])
		}

		*&nTotalFiles = NULL;
		*&nDirectory = NULL;
	}
};
*/

//class c_thread
//{
//public:
//	sys_ppu_thread_t thread_id;
//	char* pszThreadName;
//	void (*entry) (__CSTD uint64_t);
//
//	c_thread(void (*_entry) (__CSTD uint64_t), char* szName)
//	{
//		pszThreadName = (char*)malloc(1024);
//		memset(pszThreadName, 0, 1024);
//		strcpy(pszThreadName, szName);
//		entry = _entry;
//	}
//
//	void Start()
//	{
//		sys_ppu_thread_create(
//			&thread_id,
//			entry,							// callback function
//			0x1337,							// arg
//			1500,							// priority
//			0x1000,							// stack size
//			SYS_PPU_THREAD_CREATE_JOINABLE, //
//			pszThreadName					// name
//		);
//	}
//
//	~c_thread()
//	{
//		SAFE_FREE(pszThreadName)
//		*&thread_id = NULL;
//	}
//
//};

class c_fbaRL 
{
public:

	c_fbaRL();

	c_Menu*		main_menu;
	c_Menu*		zipinfo_menu;
	c_Menu*		options_menu;

	c_Filebrowser* filebrowser;
	int			nFileBrowserType;
	void		InitFileBrowser();
	void		EndFileBrowser();

	void		CreateAllInputCFG();

	//c_thread	*thread_ScanRoms;

	bool		bProcessingGames;

	int			nTotalGames;
	int			nMissingGames;
	c_game**	games;

	int			nSelectedGame;
	int			nGameListTop;
	int			nFilteredGames;
	c_game**	fgames;

	int			nStatus;
	int			nSection;

	int			nBurnSelected;

	void		LaunchFBACore(char* arg1, char* arg2, char* arg3, char* arg4, char* arg5 );
	
	int			SaveGameListCache();
	int			ParseGameListCache();

	void		InitFilterList();
	void		EndFilterList();
	void		NextSysFilter();
	void		PrevSysFilter();
	bool		FilterGame(char* szMask);
	
	int			GetSystemMaskId(char* szMask);
	char*		GetSystemFilter(int nFilter);

	void		RenderBackground();
	void		ResetPreviewImage();
	void		UpdatePreviewImage();

	void		MakeRetroDirs(char* szMainPath);

	bool		DirExist(char* szDir);
	bool		FileExist(char* szFile);

	void		InitMainMenu();
	void		EndMainMenu();

	void		InitZipInfoMenu();
	void		EndZipInfoMenu();

	void		InitGameList();
	void		EndGameList();

	void		InitOptionsMenu();
	void		EndOptionsMenu();

	uint32_t	GetBurnDriverID(char* szRomzip);
	void		UpdateBurnSelected(char* szROM);
	bool		IsBurnDuplicate(char* szROM);
	
	void		RefreshGameList();
	uint64_t	GetFileSize(char* szFilePath);
	int			ParseGameList();

	void		ParseDirectory();
	void		FileBrowserFrame();

	//void		Frame();
	int			nFrameStep;
	void		DisplayFrame();

	void		MainMenu_Frame();
	void		GameList_Frame();
	void		Options_Frame();
	void		FileBrowser_Frame();
	void		ZipInfo_Frame();

	void		InputFrame();
	void		DlgDisplayFrame();

private:
	// ...
};

extern c_fbaRL* fbaRL;

#endif