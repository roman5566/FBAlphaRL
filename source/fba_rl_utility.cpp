// =======================================================================
// UTILITY MODULES

#include "main.h"

void c_fbaRL::LaunchFBACore(char* arg1, char* arg2, char* arg3, char* arg4, char* arg5) 
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

uint64_t c_fbaRL::GetFileSize(char* szFilePath)
{
	CellFsStat sb;
	memset(&sb, 0, sizeof(CellFsStat));
	cellFsStat(szFilePath, &sb);
	return sb.st_size;
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