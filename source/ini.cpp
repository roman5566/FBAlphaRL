// ---------------------------------------------------------------------
// FB Alpha Retro Loader (CaptainCPS-X, 2013)
// ---------------------------------------------------------------------
#include "main.h"

char g_opt_szROMPaths[12][2048] = {
	"/dev_hdd0/ROMS/fba",
	"/dev_hdd0/ROMS/",
	"/dev_hdd0/game/ROMS00000/USRDIR",
	"/dev_hdd0/game/ROMS00000/USRDIR/roms",
	"/dev_hdd0/game/ROMS00000/USRDIR/roms/fba",
	"/dev_hdd0/game/ROMS00000/USRDIR/fba",
	"/dev_hdd0/game/FBAL00123/USRDIR/cores/roms",
	"/dev_hdd0/game/FBAL00123/USRDIR/cores/roms/fba",
	"/dev_hdd0/game/SSNE10000/USRDIR/cores/roms",
	"/dev_hdd0/game/SSNE10000/USRDIR/cores/roms/fba",
	"/dev_hdd0/game/FBAL00123/USRDIR/cores/roms",
	"/dev_hdd0/game/FBAL00123/USRDIR/cores/roms"
};

char g_opt_szInputCFG[][2048] = {
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

char g_opt_szTextures[][2048] = {
	"/dev_hdd0/game/FBAL00123/USRDIR/cores/borders/default/MAIN_MENU.PNG",
	"/dev_hdd0/game/FBAL00123/USRDIR/cores/borders/default/GAME_LIST.PNG",
	"/dev_hdd0/game/FBAL00123/USRDIR/cores/borders/default/ZIP_INFO.PNG",
	"/dev_hdd0/game/FBAL00123/USRDIR/cores/borders/default/OPTIONS_MENU.PNG",
	"/dev_hdd0/game/FBAL00123/USRDIR/cores/borders/default/FILE_BROWSER.PNG",
	"/dev_hdd0/game/FBAL00123/USRDIR/cores/borders/default/PREVIEW.PNG"
};

bool g_opt_bAutoAspectRatio		= true;
bool g_opt_bAutoInputCfgCreate	= true;
bool g_opt_bUseAltMenuKeyCombo	= false;
bool g_opt_bDisplayMissingGames = false;

unsigned int g_opt_nActiveSysFilter = MASKCUSTOM;

bool g_opt_bCustomSysFilter[MASKCUSTOM+1] = 
{
	true,true,true,true,
	true,true,true,true,
	true,true,true,true,
	true,true,true,true,
	true,true,true,true,
	true,
	true,true // <--- fillers
};

int iniWrite()
{
	FILE* fp = NULL;
	fp = fopen(FBARL_INI_PATH, "w");
	if(fp) 
	{
		fprintf(fp, "// --------------------------------------------------------------------------\n");
		fprintf(fp, "// FB Alpha RL Options \n");
		fprintf(fp, "// --------------------------------------------------------------------------\n");
		fprintf(fp, "// If you edit this manually, make sure you do so properly, any malformation \n");
		fprintf(fp, "// of this configuration will force the application to generate a new one, resulting \n");
		fprintf(fp, "// in the loss of any previous configuration you made. \n");
		fprintf(fp, "\n");

		fprintf(fp, "// --------------------------------------------------------------------------\n");
		fprintf(fp, "// Custom ROM Paths [up to 12 alternate paths]\n");
		fprintf(fp, "// --------------------------------------------------------------------------\n");
		fprintf(fp, "rompath_1:\"%s\"\n"			, g_opt_szROMPaths[0]);
		fprintf(fp, "rompath_2:\"%s\"\n"			, g_opt_szROMPaths[1]);
		fprintf(fp, "rompath_3:\"%s\"\n"			, g_opt_szROMPaths[2]);
		fprintf(fp, "rompath_4:\"%s\"\n"			, g_opt_szROMPaths[3]);
		fprintf(fp, "rompath_5:\"%s\"\n"			, g_opt_szROMPaths[4]);
		fprintf(fp, "rompath_6:\"%s\"\n"			, g_opt_szROMPaths[5]);
		fprintf(fp, "rompath_7:\"%s\"\n"			, g_opt_szROMPaths[6]);
		fprintf(fp, "rompath_8:\"%s\"\n"			, g_opt_szROMPaths[7]);
		fprintf(fp, "rompath_9:\"%s\"\n"			, g_opt_szROMPaths[8]);
		fprintf(fp, "rompath_10:\"%s\"\n"			, g_opt_szROMPaths[9]);
		fprintf(fp, "rompath_11:\"%s\"\n"			, g_opt_szROMPaths[10]);
		fprintf(fp, "rompath_12:\"%s\"\n"			, g_opt_szROMPaths[11]);
		fprintf(fp, "\n");

		fprintf(fp, "// --------------------------------------------------------------------------\n");
		fprintf(fp, "// Custom System Filters. Choose specific systems to show on this category.\n");
		fprintf(fp, "// --------------------------------------------------------------------------\n");
		fprintf(fp, "filter_capcom_misc:\"%s\"\n"	, g_opt_bCustomSysFilter[0] ? "yes" : "no");
		fprintf(fp, "filter_cave:\"%s\"\n"			, g_opt_bCustomSysFilter[1] ? "yes" : "no");
		fprintf(fp, "filter_cps1:\"%s\"\n"			, g_opt_bCustomSysFilter[2] ? "yes" : "no");
		fprintf(fp, "filter_cps2:\"%s\"\n"			, g_opt_bCustomSysFilter[3] ? "yes" : "no");
		fprintf(fp, "filter_cps3:\"%s\"\n"			, g_opt_bCustomSysFilter[4] ? "yes" : "no");
		fprintf(fp, "filter_dataeast:\"%s\"\n"		, g_opt_bCustomSysFilter[5] ? "yes" : "no");
		fprintf(fp, "filter_galaxian:\"%s\"\n"		, g_opt_bCustomSysFilter[6] ? "yes" : "no");
		fprintf(fp, "filter_irem:\"%s\"\n"			, g_opt_bCustomSysFilter[7] ? "yes" : "no");
		fprintf(fp, "filter_kaneko:\"%s\"\n"		, g_opt_bCustomSysFilter[8] ? "yes" : "no");
		fprintf(fp, "filter_konami:\"%s\"\n"		, g_opt_bCustomSysFilter[9] ? "yes" : "no");
		fprintf(fp, "filter_neogeo:\"%s\"\n"		, g_opt_bCustomSysFilter[10] ? "yes" : "no");
		fprintf(fp, "filter_pacman:\"%s\"\n"		, g_opt_bCustomSysFilter[11] ? "yes" : "no");
		fprintf(fp, "filter_pgm:\"%s\"\n"			, g_opt_bCustomSysFilter[12] ? "yes" : "no");
		fprintf(fp, "filter_psikyo:\"%s\"\n"		, g_opt_bCustomSysFilter[13] ? "yes" : "no");
		fprintf(fp, "filter_sega:\"%s\"\n"			, g_opt_bCustomSysFilter[14] ? "yes" : "no");
		fprintf(fp, "filter_seta:\"%s\"\n"			, g_opt_bCustomSysFilter[15] ? "yes" : "no");
		fprintf(fp, "filter_taito:\"%s\"\n"			, g_opt_bCustomSysFilter[16] ? "yes" : "no");
		fprintf(fp, "filter_technos:\"%s\"\n"		, g_opt_bCustomSysFilter[17] ? "yes" : "no");
		fprintf(fp, "filter_toaplan:\"%s\"\n"		, g_opt_bCustomSysFilter[18] ? "yes" : "no");
		fprintf(fp, "filter_misc_pre90:\"%s\"\n"	, g_opt_bCustomSysFilter[19] ? "yes" : "no");
		fprintf(fp, "filter_misc_post90:\"%s\"\n"	, g_opt_bCustomSysFilter[20] ? "yes" : "no");
		fprintf(fp, "\n");
		
		fprintf(fp, "// --------------------------------------------------------------------------\n");
		fprintf(fp, "// Active System Filter \n");
		fprintf(fp, "// --------------------------------------------------------------------------\n");
		fprintf(fp, "filter_active:\"%d\"\n"		, g_opt_nActiveSysFilter);
		fprintf(fp, "\n");

		fprintf(fp, "// --------------------------------------------------------------------------\n");
		fprintf(fp, "// Enable / Disable the display of missing games \n");
		fprintf(fp, "// --------------------------------------------------------------------------\n");
		fprintf(fp, "display_missing:\"%s\"\n"		, g_opt_bDisplayMissingGames ? "yes" : "no");
		fprintf(fp, "\n");
		
		fprintf(fp, "// --------------------------------------------------------------------------\n");
		fprintf(fp, "// Automatically set proper Screen Aspect Ratio on game launch \n");
		fprintf(fp, "// --------------------------------------------------------------------------\n");
		fprintf(fp, "auto_ar:\"%s\"\n"				, g_opt_bAutoAspectRatio ? "yes" : "no");
		fprintf(fp, "\n");

		fprintf(fp, "// --------------------------------------------------------------------------\n");
		fprintf(fp, "// Enable Alternate RetroArch menu key combo \n");
		fprintf(fp, "// --------------------------------------------------------------------------\n");
		fprintf(fp, "alt_menu_key_combo:\"%s\"\n"	, g_opt_bUseAltMenuKeyCombo ? "yes" : "no");
		fprintf(fp, "\n");		

		fprintf(fp, "// --------------------------------------------------------------------------\n");
		fprintf(fp, "// Input preset configuration file paths for specific systems.\n");
		fprintf(fp, "// When configured, they will be auto-loaded on game launch, \n");
		fprintf(fp, "// unless you specify an individual game configuration.\n");
		fprintf(fp, "// --------------------------------------------------------------------------\n");
		fprintf(fp, "input_capcom_misc:\"%s\"\n"	, g_opt_szInputCFG[0]);
		fprintf(fp, "input_cave:\"%s\"\n"			, g_opt_szInputCFG[1]);
		fprintf(fp, "input_cps1:\"%s\"\n"			, g_opt_szInputCFG[2]);
		fprintf(fp, "input_cps2:\"%s\"\n"			, g_opt_szInputCFG[3]);
		fprintf(fp, "input_cps3:\"%s\"\n"			, g_opt_szInputCFG[4]);
		fprintf(fp, "input_dataeast:\"%s\"\n"		, g_opt_szInputCFG[5]);
		fprintf(fp, "input_galaxian:\"%s\"\n"		, g_opt_szInputCFG[6]);
		fprintf(fp, "input_irem:\"%s\"\n"			, g_opt_szInputCFG[7]);
		fprintf(fp, "input_kaneko:\"%s\"\n"			, g_opt_szInputCFG[8]);
		fprintf(fp, "input_konami:\"%s\"\n"			, g_opt_szInputCFG[9]);
		fprintf(fp, "input_neogeo:\"%s\"\n"			, g_opt_szInputCFG[10]);
		fprintf(fp, "input_pacman:\"%s\"\n"			, g_opt_szInputCFG[11]);
		fprintf(fp, "input_pgm:\"%s\"\n"			, g_opt_szInputCFG[12]);
		fprintf(fp, "input_psikyo:\"%s\"\n"			, g_opt_szInputCFG[13]);
		fprintf(fp, "input_sega:\"%s\"\n"			, g_opt_szInputCFG[14]);
		fprintf(fp, "input_seta:\"%s\"\n"			, g_opt_szInputCFG[15]);
		fprintf(fp, "input_taito:\"%s\"\n"			, g_opt_szInputCFG[16]);
		fprintf(fp, "input_technos:\"%s\"\n"		, g_opt_szInputCFG[17]);
		fprintf(fp, "input_toaplan:\"%s\"\n"		, g_opt_szInputCFG[18]);
		fprintf(fp, "input_misc_pre90:\"%s\"\n"		, g_opt_szInputCFG[19]);
		fprintf(fp, "input_misc_post90:\"%s\"\n"	, g_opt_szInputCFG[20]);
		fprintf(fp, "\n");

		fprintf(fp, "// --------------------------------------------------------------------------\n");
		fprintf(fp, "// When missing, automatically create all basic Input Configuration files \n");
		fprintf(fp, "// --------------------------------------------------------------------------\n");
		fprintf(fp, "input_auto_create:\"%s\"\n"	, g_opt_bAutoInputCfgCreate ? "yes" : "no");
		fprintf(fp, "\n");

		fprintf(fp, "// --------------------------------------------------------------------------\n");
		fprintf(fp, "// Custom GUI Textures (PNG) paths \n");
		fprintf(fp, "// --------------------------------------------------------------------------\n");
		fprintf(fp, "texture_main_menu:\"%s\"\n"	, g_opt_szTextures[0]);
		fprintf(fp, "texture_gamelist:\"%s\"\n"		, g_opt_szTextures[1]);
		fprintf(fp, "texture_zipinfo:\"%s\"\n"		, g_opt_szTextures[2]);
		fprintf(fp, "texture_options:\"%s\"\n"		, g_opt_szTextures[3]);
		fprintf(fp, "texture_filebrowser:\"%s\"\n"	, g_opt_szTextures[4]);
		fprintf(fp, "texture_preview:\"%s\"\n"		, g_opt_szTextures[5]);
		fprintf(fp, "\n");

		fclose(fp);
		*&fp = NULL;

		return 1;
	} else {
		// error...
		return 0;
	}

	return 0;
}

char* getStrOption(FILE* fp, char* option)
{
	if(!fp) return NULL;

	rewind(fp);

	char* pszLine = NULL;
	pszLine = (char*)malloc(2048+1);
	memset(pszLine, 0, 2048+1);
	
	while (!feof(fp)) 
	{
		fgets(pszLine, 2048, fp);

		char* pch = NULL;
		pch = strstr(pszLine, option);

		if(pch) {
			char* pszOption =  NULL;
			pszOption = (char*)malloc(2048+1);
			memset(pszOption, 0, 2048+1);
				
			strncpy(pszOption, pch+(strlen(option)), 2048);

			pch = strrchr(pszOption, '"');
			pszOption[(int)(pch-pszOption)] = 0;

			if(pszLine) { free(pszLine); *&pszLine = NULL; }

			if(!pszOption){
				// debug...
				FILE* fpd = fopen("/dev_hdd0/game/FBAL00123/USRDIR/inidebug.txt", "w");
				if(fpd) 
				{
					fprintf(fpd, "error: getStrOption() option--> %s \n", option);
					fclose(fpd);
					fpd = NULL;
				}
			}

			return pszOption;
		}
	}
	return NULL;
}

int getBoolOption(FILE* fp, char* option, bool *bOption)
{
	if(!fp) return 0;

	rewind(fp);

	char* pszLine = NULL;
	pszLine = (char*)malloc(2048+1);
	memset(pszLine, 0, 2048+1);
	
	while (!feof(fp)) 
	{
		fgets(pszLine, 2048, fp);

		char* pch = NULL;
		pch = strstr(pszLine, option);

		if(pch) {
			char* pszOption =  NULL;
			pszOption = (char*)malloc(2048+1);
			memset(pszOption, 0, 2048+1);
				
			strncpy(pszOption, pch+(strlen(option)), 2048);

			pch = strrchr(pszOption, '"');
			pszOption[(int)(pch-pszOption)] = 0;

			if(pszLine) { free(pszLine); *&pszLine = NULL; }

			if(!pszOption){
				// debug...
				FILE* fpd = fopen("/dev_hdd0/game/FBAL00123/USRDIR/inidebug.txt", "w");
				if(fpd) 
				{
					fprintf(fpd, "error: getBoolOption() option--> %s \n", option);
					fclose(fpd);
					fpd = NULL;
				}
				return 0;
			}

			if( strcmp(pszOption, "yes")==0 || 
				strcmp(pszOption, "YES")==0 ||
				strcmp(pszOption, "Yes")==0 ||
				strcmp(pszOption, "true")==0 ||
				strcmp(pszOption, "True")==0 ||
				strcmp(pszOption, "TRUE")==0 ||
				strcmp(pszOption, "1")==0 )
			{
				if(pszOption) { free(pszOption); *&pszOption = NULL; }
				*bOption = true;
				return 1;
			}
			if( strcmp(pszOption, "no")==0 || 
				strcmp(pszOption, "NO")==0 ||
				strcmp(pszOption, "No")==0 ||
				strcmp(pszOption, "false")==0 ||
				strcmp(pszOption, "False")==0 ||
				strcmp(pszOption, "FALSE")==0 ||
				strcmp(pszOption, "0")==0 )
			{
				if(pszOption) { free(pszOption); *&pszOption = NULL; }
				*bOption = false;
				return 1;
			}


		}
	}
	return 0;
}

int getIntOption(FILE* fp, char* option, unsigned int *nOption)
{
	if(!fp) return 0;

	rewind(fp);

	char* pszLine = NULL;
	pszLine = (char*)malloc(2048+1);
	memset(pszLine, 0, 2048+1);
	
	while (!feof(fp)) 
	{
		fgets(pszLine, 2048, fp);

		char* pch = NULL;
		pch = strstr(pszLine, option);

		if(pch) {
			char* pszOption =  NULL;
			pszOption = (char*)malloc(2048+1);
			memset(pszOption, 0, 2048+1);
				
			strncpy(pszOption, pch+(strlen(option)), 2048);

			pch = strrchr(pszOption, '"');
			pszOption[(int)(pch-pszOption)] = 0;

			if(pszLine) { free(pszLine); *&pszLine = NULL; }

			if(!pszOption){
				// debug...
				FILE* fpd = fopen("/dev_hdd0/game/FBAL00123/USRDIR/inidebug.txt", "w");
				if(fpd) 
				{
					fprintf(fpd, "error: getIntOption() option--> %s \n", option);
					fclose(fpd);
					fpd = NULL;
				}
				return 0;
			}

			sscanf(pszOption, "%d", nOption);

			return 1;
		}
	}
	return 0;
}

int iniRead()
{
	FILE* fp = NULL;
	fp = fopen(FBARL_INI_PATH, "r");
	if(fp) 
	{
		char* pszStrOption = NULL;

		// -----------------------------------------------------------------------------------------
		// Rom path options
		// -----------------------------------------------------------------------------------------
		pszStrOption = getStrOption(fp, "rompath_1:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szROMPaths[0], pszStrOption);

		pszStrOption = getStrOption(fp, "rompath_2:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szROMPaths[1], pszStrOption);

		pszStrOption = getStrOption(fp, "rompath_3:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szROMPaths[2], pszStrOption);

		pszStrOption = getStrOption(fp, "rompath_4:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szROMPaths[3], pszStrOption);

		pszStrOption = getStrOption(fp, "rompath_5:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szROMPaths[4], pszStrOption);

		pszStrOption = getStrOption(fp, "rompath_6:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szROMPaths[5], pszStrOption);

		pszStrOption = getStrOption(fp, "rompath_7:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szROMPaths[6], pszStrOption);

		pszStrOption = getStrOption(fp, "rompath_8:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szROMPaths[7], pszStrOption);

		pszStrOption = getStrOption(fp, "rompath_9:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szROMPaths[8], pszStrOption);

		pszStrOption = getStrOption(fp, "rompath_10:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szROMPaths[9], pszStrOption);

		pszStrOption = getStrOption(fp, "rompath_11:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szROMPaths[10], pszStrOption);

		pszStrOption = getStrOption(fp, "rompath_12:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szROMPaths[11], pszStrOption);			

		// -----------------------------------------------------------------------------------------
		// Custom System Filters options
		// -----------------------------------------------------------------------------------------
		if(!getBoolOption(fp, "filter_capcom_misc:\""	, &g_opt_bCustomSysFilter[0])) {return 0;}
		if(!getBoolOption(fp, "filter_cave:\""			, &g_opt_bCustomSysFilter[1])) {return 0;}
		if(!getBoolOption(fp, "filter_cps1:\""			, &g_opt_bCustomSysFilter[2])) {return 0;}
		if(!getBoolOption(fp, "filter_cps2:\""			, &g_opt_bCustomSysFilter[3])) {return 0;}
		if(!getBoolOption(fp, "filter_cps3:\""			, &g_opt_bCustomSysFilter[4])) {return 0;}
		if(!getBoolOption(fp, "filter_dataeast:\""		, &g_opt_bCustomSysFilter[5])) {return 0;}
		if(!getBoolOption(fp, "filter_galaxian:\""		, &g_opt_bCustomSysFilter[6])) {return 0;}
		if(!getBoolOption(fp, "filter_irem:\""			, &g_opt_bCustomSysFilter[7])) {return 0;}
		if(!getBoolOption(fp, "filter_kaneko:\""		, &g_opt_bCustomSysFilter[8])) {return 0;}
		if(!getBoolOption(fp, "filter_konami:\""		, &g_opt_bCustomSysFilter[9])) {return 0;}
		if(!getBoolOption(fp, "filter_neogeo:\""		, &g_opt_bCustomSysFilter[10])) {return 0;}
		if(!getBoolOption(fp, "filter_pacman:\""		, &g_opt_bCustomSysFilter[11])) {return 0;}
		if(!getBoolOption(fp, "filter_pgm:\""			, &g_opt_bCustomSysFilter[12])) {return 0;}
		if(!getBoolOption(fp, "filter_psikyo:\""		, &g_opt_bCustomSysFilter[13])) {return 0;}
		if(!getBoolOption(fp, "filter_sega:\""			, &g_opt_bCustomSysFilter[14])) {return 0;}
		if(!getBoolOption(fp, "filter_seta:\""			, &g_opt_bCustomSysFilter[15])) {return 0;}
		if(!getBoolOption(fp, "filter_taito:\""			, &g_opt_bCustomSysFilter[16])) {return 0;}
		if(!getBoolOption(fp, "filter_technos:\""		, &g_opt_bCustomSysFilter[17])) {return 0;}
		if(!getBoolOption(fp, "filter_toaplan:\""		, &g_opt_bCustomSysFilter[18])) {return 0;}
		if(!getBoolOption(fp, "filter_misc_pre90:\""	, &g_opt_bCustomSysFilter[19])) {return 0;}
		if(!getBoolOption(fp, "filter_misc_post90:\""	, &g_opt_bCustomSysFilter[20])) {return 0;}

		// -----------------------------------------------------------------------------------------
		// Active System Filter
		// -----------------------------------------------------------------------------------------
		if(!getIntOption(fp, "filter_active:\"", &g_opt_nActiveSysFilter)) {return 0;}

		// -----------------------------------------------------------------------------------------
		// Enable / Disable display of missing games
		// -----------------------------------------------------------------------------------------
		if(!getBoolOption(fp, "display_missing:\"", &g_opt_bDisplayMissingGames)) {return 0;}

		// -----------------------------------------------------------------------------------------
		// Auto aspect ratio option
		// -----------------------------------------------------------------------------------------
		if(!getBoolOption(fp, "auto_ar:\"", &g_opt_bAutoAspectRatio)) {return 0;}

		// -----------------------------------------------------------------------------------------
		// Alternate RetroArch menu key combo option
		// -----------------------------------------------------------------------------------------
		if(!getBoolOption(fp, "alt_menu_key_combo:\"", &g_opt_bUseAltMenuKeyCombo)) {return 0;}

		// -----------------------------------------------------------------------------------------
		// Game System Input preset options
		// -----------------------------------------------------------------------------------------
		pszStrOption = getStrOption(fp, "input_capcom_misc:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szInputCFG[0], pszStrOption);

		pszStrOption = getStrOption(fp, "input_cave:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szInputCFG[1], pszStrOption);

		pszStrOption = getStrOption(fp, "input_cps1:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szInputCFG[2], pszStrOption);

		pszStrOption = getStrOption(fp, "input_cps2:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szInputCFG[3], pszStrOption);

		pszStrOption = getStrOption(fp, "input_cps3:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szInputCFG[4], pszStrOption);

		pszStrOption = getStrOption(fp, "input_dataeast:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szInputCFG[5], pszStrOption);

		pszStrOption = getStrOption(fp, "input_galaxian:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szInputCFG[6], pszStrOption);

		pszStrOption = getStrOption(fp, "input_irem:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szInputCFG[7], pszStrOption);

		pszStrOption = getStrOption(fp, "input_kaneko:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szInputCFG[8], pszStrOption);

		pszStrOption = getStrOption(fp, "input_konami:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szInputCFG[9], pszStrOption);

		pszStrOption = getStrOption(fp, "input_neogeo:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szInputCFG[10], pszStrOption);

		pszStrOption = getStrOption(fp, "input_pacman:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szInputCFG[11], pszStrOption);

		pszStrOption = getStrOption(fp, "input_pgm:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szInputCFG[12], pszStrOption);

		pszStrOption = getStrOption(fp, "input_psikyo:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szInputCFG[13], pszStrOption);

		pszStrOption = getStrOption(fp, "input_sega:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szInputCFG[14], pszStrOption);

		pszStrOption = getStrOption(fp, "input_seta:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szInputCFG[15], pszStrOption);

		pszStrOption = getStrOption(fp, "input_taito:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szInputCFG[16], pszStrOption);

		pszStrOption = getStrOption(fp, "input_technos:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szInputCFG[17], pszStrOption);

		pszStrOption = getStrOption(fp, "input_toaplan:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szInputCFG[18], pszStrOption);

		pszStrOption = getStrOption(fp, "input_misc_pre90:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szInputCFG[19], pszStrOption);

		pszStrOption = getStrOption(fp, "input_misc_post90:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szInputCFG[20], pszStrOption);

		// -----------------------------------------------------------------------------------------
		// Auto Input Configuration creation option
		// -----------------------------------------------------------------------------------------
		if(!getBoolOption(fp, "input_auto_create:\"", &g_opt_bAutoInputCfgCreate)) {return 0;}	

		// -----------------------------------------------------------------------------------------
		// Texture options
		// -----------------------------------------------------------------------------------------
		pszStrOption = getStrOption(fp, "texture_main_menu:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szTextures[0], pszStrOption);

		pszStrOption = getStrOption(fp, "texture_gamelist:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szTextures[1], pszStrOption);

		pszStrOption = getStrOption(fp, "texture_zipinfo:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szTextures[2], pszStrOption);

		pszStrOption = getStrOption(fp, "texture_options:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szTextures[3], pszStrOption);

		pszStrOption = getStrOption(fp, "texture_filebrowser:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szTextures[4], pszStrOption);

		pszStrOption = getStrOption(fp, "texture_preview:\"");
		if(pszStrOption == NULL) {return 0;}
		strcpy(g_opt_szTextures[5], pszStrOption);

		// -----------------------------------------------------------------------------------------

		if(pszStrOption) { free(pszStrOption); *&pszStrOption = NULL; }
		
		fclose(fp);
		return 1;
	} else {
		return 0;
	}
	return 0;
}
