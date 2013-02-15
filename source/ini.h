#ifndef INI_H
#define INI_H
// ---------------------------------------------------------------------
// FB Alpha Retro Loader (CaptainCPS-X, 2013)
// ---------------------------------------------------------------------
#include "fba_rl.h"

#define FBARL_INI_PATH "/dev_hdd0/game/FBAL00123/USRDIR/FBA_RL.ini"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

extern char g_opt_szROMPaths[12][2048];
extern char g_opt_szInputCFG[][2048];
extern char g_opt_szTextures[][2048];
extern bool g_opt_bAutoAspectRatio;
extern bool g_opt_bAutoInputCfgCreate;
extern bool g_opt_bCustomSysFilter[MASKCUSTOM+1];
extern bool g_opt_bUseAltMenuKeyCombo;
extern bool g_opt_bDisplayMissingGames;
extern unsigned int g_opt_nActiveSysFilter;

int iniWrite();
int iniRead();
char* getStrOption(FILE* fp, char* option);
int getBoolOption(FILE* fp, char* option, bool *bOption);
int getIntOption(FILE* fp, char* option, unsigned int *nOption);

#endif
