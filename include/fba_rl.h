// ---------------------------------------------------------------------
// FB Alpha Retro Loader (CaptainCPS-X, 2013)
// ---------------------------------------------------------------------
struct c_gamelist
{
	char		path[256];		// ROM Path
	char		zipname[256];	// ZIP name
	char		title[256];		// Title, if found
	uint64_t	nSize;			// size in bytes
};

#define STATUS_NORMAL			0
#define STATUS_COPY_START		1
#define STATUS_COPY_OK			2
#define STATUS_COPY_ERROR		10
#define STATUS_RM_QUEUE_START	4
#define STATUS_RM_QUEUE_OK		5

class c_fbaRL 
{
public:

	c_fbaRL();

	int	nSelectedGame;
	int	nGameListTop;
	int	nTotalGames;

	int		nStatus;
	
	c_gamelist	*gamelst;

	uint32_t	GetBurnDriverID(char* szRomzip);
	void		UpdateBurnSelected(char* szROM);
	bool		IsBurnDuplicate(char* szROM);
	
	void		RefreshGameList();
	uint64_t	GetFileSize(char* szFilePath);
	int			ParseGameList();

	void		Frame();
	void		DisplayFrame();
	void		InputFrame();
	void		DlgDisplayFrame();

private:
	// ...
};

extern c_fbaRL* fbaRL;
