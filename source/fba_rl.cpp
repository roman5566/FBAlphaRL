// ========================================================================================================
// FB Alpha Retro Loader (CaptainCPS-X, 2013)
// ========================================================================================================
/*

Custom RETROARCH changes for FB Alpha RL (v1.XX):

* platform_ps3.c 

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

		> Argument 5 : Use alternate RetroArch menu key combo ("yes" or "no" as char string)

* rmenu.c / rmenu.h

	- Added code to handle Alternate Meny Key combos.

	- Modified / Added menu items for in-game menu and main menu so RetroArch display the
	proper name if launched from FB Alpha RL (Ex. "Return to FB alpha RL").

* Makefile.ps3 
	
	- edited with custom signing using scetool for Salamander / CORE

========================================================================================================*/

//#define FBARL_DEBUG
#include "main.h"

c_fbaRL* fbaRL;

char ipaddress[256] = "255.255.255.255"; //ipv4

c_fbaRL::c_fbaRL() 
{
	CellNetCtlInfo info;
	cellNetCtlGetInfo(CELL_NET_CTL_INFO_IP_ADDRESS, &info);
	strcpy(ipaddress, info.ip_address);

	nFrameStep = 0; // for text shadow

	nBurnSelected = -1;

	bProcessingGames = false;

	MakeRetroDirs((char*)"/dev_hdd0/game/FBAL00123/USRDIR");
	
	if(DirExist((char*)"/dev_hdd0/game/FBAL00123/USRDIR")) 
	{
		MakeRetroDirs((char*)"/dev_hdd0/game/SSNE10000/USRDIR");
	}

	if(g_opt_bAutoInputCfgCreate) {
		CreateAllInputCFG();
	}

	InitMainMenu();
	nSection = SECTION_MAIN;
}
