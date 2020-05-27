// =======================================================================
// UTILITY MODULES

#include "main.h"
#include "misc.h"
#include <sys/process.h>

void c_fbaRL::LaunchFBACore(char* arg1, char* arg2, char* arg3, char* arg4, char* arg5, char* arg6) //CRYSTAL
{
	// TODO: Add error checking for arguments...
	char* argv[14] = { 0 };	//CRYSTAL
	argv[0] = (char*)malloc( strlen(arg1) + 1);
	argv[1] = (char*)malloc( strlen(arg2) + 1);
	/*argv[2] = (char*)malloc( strlen(arg3) + 1);
	argv[3] = (char*)malloc( strlen(arg4) + 1);
	argv[4] = (char*)malloc( strlen(arg5) + 1);
	argv[5] = (char*)malloc( strlen(arg6) + 1); */ //CRYSTAL
    argv[2] = (char *)malloc( 2 + 1);
    argv[3] = (char *)malloc(64);
    argv[4] = (char *)malloc(3);
    argv[5] = (char *)malloc(64);
    argv[6] = (char *)malloc(3);
    argv[7] = (char *)malloc(64);
    argv[8] = (char *)malloc(3);
    argv[9] = (char *)malloc(64);
    argv[10] = (char *)malloc(3);
    strcpy(argv[0], arg1);	// [ROM / ZIP path]			optional, can be dummy / filler argument
	strcpy(argv[1], arg2);	// [FBARL RELOAD.SELF path] MUST be the valid external launcher path
	strcpy(argv[2], "-s");
	strcpy(argv[3], "/dev_hdd0/game/FBNE00123/USRDIR/cores/savefiles");
	strcpy(argv[4], "-c");
	strcpy(argv[5], "/dev_hdd0/game/FBNE00123/USRDIR/retroarch.cfg");
	strcpy(argv[6], "-S");
	strcpy(argv[7], "/dev_hdd0/game/FBNE00123/USRDIR/cores/savestates");
	strcpy(argv[8], "-L");
	strcpy(argv[9], "/dev_hdd0/game/FBNE00123/USRDIR/cores/fb_neo.SELF");
	strcpy(argv[10], "-v");
	argv[11] = NULL;
	argv[12] = NULL;
	argv[13] = NULL;
	/*strcpy(argv[2], arg3);	// [input preset cfg]		optional, can be dummy / filler argument
	strcpy(argv[3], arg4);	// [aspect ratio]			optional, can be dummy / filler argument
	strcpy(argv[4], arg5);	// [alternate menu key combo ] optional, can be dummy / filler argument
	strcpy(argv[5], arg6);	// [UNIBIOS for Neo Geo games ] optional, can be dummy / filler argument  //CRYSTAL
	argv[6] = NULL;*/
//    int kk;
//    for (kk=0; kk< 12; kk++)
//		printf("argv[%d] = %s\n", kk,argv[kk]);
//
//	printf("Print: nSelectedGame: %d - fgames[nSelectedGame]->sysmask: %s - def core: %d\n",nSelectedGame,fgames[nSelectedGame]->sysmask, app.cores[fgames[nSelectedGame]->def_core_id] );
	//printf("Path rom: %s\n",pathUpper);
	if (fgames[nSelectedGame]->def_core_id > 0) {
            char core_path[256];
            sprintf(core_path,"/dev_hdd0/game/FBNE00123/USRDIR/%s%s.SELF",
                    fgames[nSelectedGame]->def_core_id == 3 ? "MAME125/" : "cores/", app.cores[fgames[nSelectedGame]->def_core_id - 1]);
            sysProcessExitSpawn2( core_path, (const char**)argv, NULL, NULL, 0, 1001, SYS_PROCESS_SPAWN_STACK_SIZE_1M );
            return;
	}
	if (strcmp(fgames[nSelectedGame]->sysmask, "MASKMEGADRIVE") == 0){
		char core_path[]	= "/dev_hdd0/game/FBNE00123/USRDIR/cores/genesis_plus_gx.SELF";
//		char core_path[]	= "/dev_hdd0/game/FBNE00123/USRDIR/cores/fb_neo.SELF";
//		argv[11] = (char *)malloc(13);
//        argv[12] = (char *)malloc(3);
//        strcpy(argv[11], "--subsystem");
//        strcpy(argv[12], "md");
		//printf("ARGS: %s %s %s %s %s %s\n", argv[0],argv[1],argv[2],argv[3],argv[4],argv[5]);
		////sys_game_process_exitspawn(snes9x_core_path, (const char**)argv, NULL, NULL, 0, 1000, SYS_PROCESS_PRIMARY_STACK_SIZE_1M);
		sysProcessExitSpawn2( core_path, (const char**)argv, NULL, NULL, 0, 1001, SYS_PROCESS_SPAWN_STACK_SIZE_1M );
		return;

	}
	if (strcmp(fgames[nSelectedGame]->sysmask, "MASKSNES") == 0){
		char snes9x_core_path[]	= "/dev_hdd0/game/FBNE00123/USRDIR/cores/snes9x.SELF";

		//printf("ARGS: %s %s %s %s %s %s\n", argv[0],argv[1],argv[2],argv[3],argv[4],argv[5]);
		////sys_game_process_exitspawn(snes9x_core_path, (const char**)argv, NULL, NULL, 0, 1000, SYS_PROCESS_PRIMARY_STACK_SIZE_1M);
		sysProcessExitSpawn2( snes9x_core_path, (const char**)argv, NULL, NULL, 0, 1001, SYS_PROCESS_SPAWN_STACK_SIZE_1M );
		//return;

	} else
		{
		char *pathUpper = toUpperCase(argv[0], strlen(argv[0])); //Check if is a MAME 0.125   // CRYSTAL START

		if (strstr(pathUpper, "MAME125") != NULL || strcmp(fgames[nSelectedGame]->sysmask, "MASKMIDWAY") == 0){
						free(pathUpper);
						char mame125_core_path[]	= "/dev_hdd0/game/FBNE00123/USRDIR/MAME125/mame125.SELF";

						////sys_game_process_exitspawn(mame125_core_path, (const char**)argv, NULL, NULL, 0, 1000, SYS_PROCESS_PRIMARY_STACK_SIZE_1M);
                        sysProcessExitSpawn2( mame125_core_path, (const char**)argv, NULL, NULL, 0, 1001, SYS_PROCESS_SPAWN_STACK_SIZE_1M );

						/*LaunchFBACore(
									(char*)fgames[nSelectedGame]->path,
									(char*)mame125_core_path,
									(char*)("DUMMY_ARG"),
									(char*)("DUMMY_ARG"),
									(char*)("DUMMY_ARG"),
									(char*)("DUMMY_ARG")
									);*/

		}
		else {
				free(pathUpper);
				char fba_core_path[] = "/dev_hdd0/game/FBNE00123/USRDIR/cores/fb_neo.SELF";
				sysProcessExitSpawn2( fba_core_path, (const char**)argv, NULL, NULL, 0, 1001, SYS_PROCESS_SPAWN_STACK_SIZE_1M );

				////sys_game_process_exitspawn(fba_core_path, (const char**)argv, NULL, NULL, 0, 1000, SYS_PROCESS_PRIMARY_STACK_SIZE_1M);
			}
		}
}







// RetroArch doesn't do this, so let's help it out :)
void c_fbaRL::MakeRetroDirs(char* szMainPath)
{
	if(!dirExist(szMainPath)) return;

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

		if(!dirExist(szDir)) {
			////cellFsMkdir(szDir, CELL_FS_S_IFDIR | 0777);
		}
		SAFE_FREE(szDir)
	}
}

void c_fbaRL::CreateAllInputCFG()
{
	if(dirExist((char*)"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input"))
	{
		char szInputCFG[][2048] = {
			"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input/capcom_misc.cfg",
			"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input/cave.cfg",
			"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input/cps1.cfg",
			"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input/cps2.cfg",
			"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input/cps3.cfg",
			"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input/data_east.cfg",
			"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input/galaxian.cfg",
			"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input/irem.cfg",
			"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input/kaneko.cfg",
			"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input/konami.cfg",
			"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input/midway.cfg",  //CRYSTAL
			"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input/neogeo.cfg",
			"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input/pacman.cfg",
			"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input/pgm.cfg",
			"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input/psikyo.cfg",
			"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input/sega.cfg",
			"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input/seta.cfg",
			"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input/taito.cfg",
			"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input/technos.cfg",
			"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input/toaplan.cfg",
			"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input/misc_pre90.cfg",
			"/dev_hdd0/game/FBNE00123/USRDIR/cores/presets/input/misc_post90.cfg"
		};
		for(uint32_t n = 0; n < sizeof(szInputCFG) / 2048; n++)
		{
			if(!fileExist(szInputCFG[n]))
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
