#ifndef MAIN_H
#define MAIN_H
// ---------------------------------------------------------------------
// FB Alpha Retro Loader (CaptainCPS-X, 2013)
// ---------------------------------------------------------------------
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <dirent.h>
#include <inttypes.h>
//
#include <PSGL/psgl.h>
#include <PSGL/psglu.h>
//
#include <sysutil/sysutil_common.h>
#include <sysutil/sysutil_msgdialog.h>
#include <sysutil/sysutil_sysparam.h>  // used for cellVideoOutGetResolutionAvailability() and videoOutIsReady()
#include <sysutil/sysutil_screenshot.h>
//
#include <sys/spu_initialize.h>
#include <sys/memory.h>
#include <sys/paths.h>
#include <sys/process.h>
#include <sys/stat.h>
#include <sys/ppu_thread.h>
//
#include <cell/cell_fs.h>
//#include <cell/fios/fios_scheduler.h>
#include <cell/sysmodule.h>
#include <cell/pad.h>
#include <cell/dbgfont.h>
//
#include "FWInput.h"
#include "FWDebugFont.h"

//
#define COLOR_ORANGE	0xff1780f8
#define COLOR_GREEN		0xff00ff00
#define COLOR_YELLOW	0xff00ccff
#define COLOR_RED		0xff0000ff
#define COLOR_WHITE		0xffffffff

//
#define TEX_MAIN_MENU	0
#define TEX_GAME_LIST	1
#define TEX_ZIP_INFO	2
#define TEX_OPTIONS		3
#define TEX_FILEBROWSER	4
#define TEX_PREVIEW		5

#include "image.h"
#include "misc.h"
#include "ini.h"
#include "fba_rl.h"

// Button organized by values ( ex. (1<<0), (1<<1), etc... )
#define BT_SELECT	0 
#define BT_L3		1	
#define BT_R3		2
#define BT_START	3
#define BT_UP		4
#define BT_RIGHT	5
#define BT_DOWN		6
#define BT_LEFT		7
#define BT_L2		8
#define BT_R2		9
#define BT_L1		10
#define BT_R1		11
#define BT_TRIANGLE 12
#define BT_CIRCLE	13
#define BT_CROSS	14
#define BT_SQUARE	15

class c_tex
{
public:
	uint32_t				nTexture;
	char*					pszTexPath;
	GLuint					nTexId[1];
	struct texture_image	_texture;
	bool bTextureOK;

	c_tex(uint32_t _nTexture, char* szPath);
	void Render(GLuint x, GLuint y, GLsizei w, GLsizei h);
	int BindTexture(char* pszPath); 
	~c_tex();

};

class CapApp
{
public:	
	CapApp();
	~CapApp() {
	}

	int nTextures;
	c_tex** textures;
	void initGraphics(PSGLdevice *device);

	bool onInit(int argc, char **argv);
	void onRender();
	void onShutdown();
	bool onUpdate();
	
	void dbgFontInit();
	void dbgFontDraw();
	void dbgFontExit();

	uint64_t	mFrame;

	// input
	bool		mIsButtPressed[16];
	bool		buttPressedNow[16];
	
	uint32_t	mValRStickX, mValRStickY;
	uint32_t	mValLStickX, mValLStickY;

	uint32_t	nButtons[16];

	int			InputInit();
	int			npad_read();
	void		InputFrameStart();
	void		InputFrame();
	void		InputFrameEnd();
	void		InputExit();

protected:
private:
};

extern CapApp app;

#endif
