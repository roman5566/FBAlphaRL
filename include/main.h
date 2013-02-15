// ---------------------------------------------------------------------
// FB Alpha Retro Loader (CaptainCPS-X, 2013)
// ---------------------------------------------------------------------
//#ifndef MAIN_H
//#define MAIN_H

#include "psgl/FWGLApplication.h"
#include "psgl/FWGLGrid.h"
#include "FWInput.h"
#include "FWDebugFont.h"

#include <cell/dbgfont.h>
#include <cell/cell_fs.h>
#include <sys/paths.h>
#include <sys/process.h>
#include <sys/ppu_thread.h>
#include <sysutil/sysutil_msgdialog.h>
#include <sysutil/sysutil_screenshot.h>
#include <cell/sysmodule.h>
#include <sys/memory.h>
#include <sys/types.h>

//using namespace std;

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
//#include <cstring>
#include <dirent.h>
#include "image.h"

class CapApp : public FWGLApplication
{
public:
	
	CapApp();

	virtual bool onInit(int argc, char **argv);
	virtual void onRender();
	//virtual void onSize(const FWDisplayInfo &dispInfo) = 0;
	virtual void onShutdown();
	virtual bool onUpdate();

	virtual void InputFrameStart();
	virtual void InputFrameEnd();

	virtual void dbgFontInit();
	virtual void dbgFontDraw();
	virtual void dbgFontExit();

	unsigned int mFrame;

	bool	mIsCirclePressed, mIsCrossPressed, mIsSquarePressed, mIsTrianglePressed;
	bool	mIsSelectPressed, mIsStartPressed;
	bool	mIsUpPressed, mIsDownPressed, mIsLeftPressed, mIsRightPressed;

	bool	mIsR1Pressed, mIsL1Pressed;
	bool	mIsR2Pressed, mIsL2Pressed;
	bool	mIsR3Pressed, mIsL3Pressed;

	int		mValRStickX, mValRStickY;
	int		mValLStickX, mValLStickY;

	bool	squarePressedNow, crossPressedNow, circlePressedNow, trianglePressedNow;	
	bool	selectPressedNow, startPressedNow;
	bool	upPressedNow, downPressedNow, leftPressedNow, rightPressedNow;
	
	bool	r1PressedNow, l1PressedNow;
	bool	r2PressedNow, l2PressedNow;
	bool	r3PressedNow, l3PressedNow;
	bool	rstickPressedNow, lstickPressedNow; 
	
	FWInputFilter   *mpCircle, *mpCross, *mpSquare, *mpTriangle;		
	FWInputFilter   *mpSelect, *mpStart;
	FWInputFilter   *mpUp, *mpDown, *mpLeft, *mpRight;

	FWInputFilter	*mpR1, *mpL1;
	FWInputFilter	*mpR2, *mpL2;
	FWInputFilter	*mpR3, *mpL3;
	FWInputFilter	*mpLStickX, *mpLStickY, *mpRStickX, *mpRStickY;

	//CellDbgFontConsoleId mDbgFontID[2];

protected:
private:

};

extern CapApp app;

//#endif // MAIN_H
