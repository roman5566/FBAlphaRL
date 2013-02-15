//#ifndef FBA_RL_H
//#define FBA_RL_H
// ---------------------------------------------------------------------
// FB Alpha Retro Loader (CaptainCPS-X, 2013)
// ---------------------------------------------------------------------

#define SAFE_FREE(x)	\
	if(x) {	free(x); *&x = NULL; }

#define SAFE_DELETE(x)	\
	if(x) { delete x; *&x = NULL; }

class c_game
{
public:

	char*		path;		// ROM Path
	char*		zipname;	// ZIP name
	char*		title;		// Title, if found
	uint64_t	nSize;		// size in bytes

	c_game()
	{

		path	= (char*)malloc(1024);
		zipname = (char*)malloc(1024);
		title	= (char*)malloc(1024);
		nSize	= 0;
	}

	~c_game()
	{
		SAFE_FREE(path);
		SAFE_FREE(zipname);
		SAFE_FREE(title);
		*&nSize = NULL;
	}
};

#define STATUS_NORMAL			0
#define STATUS_SHOW_ZIPINFO		1

#define SECTION_MAIN			0
#define SECTION_GAMELIST		1
#define SECTION_ZIPINFO			2
#define SECTION_HELP			3
#define SECTION_OPTIONS			4

class c_MenuItem 
{
public:	
	int		nIndex;
	char*	szMenuLabel;
	
	c_MenuItem(int nId) 
	{
		szMenuLabel = (char*)malloc(1024);
		nIndex		= nId;
	}

	~c_MenuItem()
	{
		SAFE_FREE(szMenuLabel);
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
			SAFE_DELETE(item[n]);
		}
		*&nSelectedItem = NULL;
		*&nTopItem		= NULL;
		*&nTotalItem	= NULL;
		*&nListMax		= NULL;
	}

	void AddItem(char* szItemLabel) 
	{
		item[nTotalItem] =  new c_MenuItem(nTotalItem);
		sprintf(item[nTotalItem]->szMenuLabel, "%s", szItemLabel);
		nTotalItem++;
	}

	int UpdateTopItem();
};

class c_Files
{
public:
	uint32_t	nFile;

	char*		szName;
	char*		szPath;
	uint64_t	nSize;

	c_Files()
	{
		szName = (char*)malloc(1024);
		szPath = (char*)malloc(1024);
		nSize = 0;
		nFile = 0;
	}

	~c_Files()
	{
		SAFE_FREE(szName);
		SAFE_FREE(szPath);
		*&nSize = NULL;
		*&nFile = NULL;
	}
};

class c_Directory
{
public:
	uint32_t	nDirectory;

	uint32_t	nTotalFiles;
	char*		szName;
	char*		szPath;
	c_Files*	files;

	c_Directory()
	{
		szName = (char*)malloc(1024);
		szPath = (char*)malloc(1024);
		nTotalFiles = 0;
		nDirectory = 0;
	}

	~c_Directory()
	{
		SAFE_FREE(szName);
		SAFE_FREE(szPath);
		*&nTotalFiles = NULL;
		*&nDirectory = NULL;
	}
};

class c_fbaRL 
{
public:
	int	nSelectedGame;
	int	nGameListTop;
	int	nTotalGames;

	c_fbaRL() {
		InitMainMenu();
		nSection = SECTION_MAIN;
	}

	c_Menu		*main_menu;
	c_Menu		*zipinfo_menu;
	c_Menu		*options_menu;
	c_game		*games[20000];

	//

	int	nStatus;
	int	nSection;
	
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

	// ...
	void		ParseDirectory();
	void		FileBrowserFrame();
	// ...

	void		Frame();
	void		DisplayFrame();
	void		InputFrame();
	void		DlgDisplayFrame();

private:
	// ...
};

extern c_fbaRL* fbaRL;

//#endif