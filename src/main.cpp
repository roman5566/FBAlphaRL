// ----------------------------------------------------------------------------
// FB Alpha Retro Loader (CaptainCPS-X, 2013)
// ----------------------------------------------------------------------------
#include "main.h"
#include "misc.h"
#include "fba_rl.h"

GLuint menu_texture_id[1];
static struct texture_image menu_texture;

CapApp app;

void DlgCallbackFunction(int buttonType, void */*userData*/);

#define PADBIND(_in1, _in2)	\
	_in1 = pPad->bindFilter();	\
	_in1->setChannel(_in2);	

#define RARCH_GL_INTERNAL_FORMAT32 GL_ARGB_SCE
#define RARCH_GL_INTERNAL_FORMAT16 GL_RGB5
#define RARCH_GL_TEXTURE_TYPE32 GL_BGRA
#define RARCH_GL_TEXTURE_TYPE16 GL_BGRA
#define RARCH_GL_FORMAT32 GL_UNSIGNED_INT_8_8_8_8_REV
#define RARCH_GL_FORMAT16 GL_RGB5

bool CapApp::onInit(int argc, char **argv) 
{
    // always call the superclass's method
    ::FWGLApplication::onInit(argc, argv);

	cellSysmoduleLoadModule(CELL_SYSMODULE_SYSUTIL_SCREENSHOT);
	cellScreenShotEnable();

	dbgFontInit();

	::FWInputDevice *pPad = FWInput::getDevice(FWInput::DeviceType_Pad, 0);

	if(pPad != NULL)
	{
		PADBIND(mpSquare	, FWInput::Channel_Button_Square);
		PADBIND(mpCross		, FWInput::Channel_Button_Cross);
		PADBIND(mpCircle	, FWInput::Channel_Button_Circle);
		PADBIND(mpTriangle	, FWInput::Channel_Button_Triangle);
		PADBIND(mpSelect	, FWInput::Channel_Button_Select);
		PADBIND(mpStart		, FWInput::Channel_Button_Start);
		PADBIND(mpUp		, FWInput::Channel_Button_Up);
		PADBIND(mpDown		, FWInput::Channel_Button_Down);
		PADBIND(mpLeft		, FWInput::Channel_Button_Left);
		PADBIND(mpRight		, FWInput::Channel_Button_Right);
		PADBIND(mpR1		, FWInput::Channel_Button_R1);
		PADBIND(mpR2		, FWInput::Channel_Button_R2);
		PADBIND(mpR3		, FWInput::Channel_Button_R3);
		PADBIND(mpL1		, FWInput::Channel_Button_L1);
		PADBIND(mpL2		, FWInput::Channel_Button_L2);
		PADBIND(mpL3		, FWInput::Channel_Button_L3);

		PADBIND(mpLStickX		, FWInput::Channel_XAxis_0);
		PADBIND(mpLStickY		, FWInput::Channel_YAxis_0);
		PADBIND(mpRStickX		, FWInput::Channel_XAxis_1);
		PADBIND(mpRStickY		, FWInput::Channel_YAxis_1);
	}
    
	glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_SRC_COLOR);

	glGenTextures(1, &menu_texture_id[0]);

	if (!texture_image_load("/dev_hdd0/game/FBAL00123/PIC1.PNG", &menu_texture))
	{
		// error
		return false;
	}

   glBindTexture(GL_TEXTURE_2D, menu_texture_id[0]);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   glTexImage2D(GL_TEXTURE_2D, 0, RARCH_GL_INTERNAL_FORMAT32,
         menu_texture.width, menu_texture.height, 0,
         RARCH_GL_TEXTURE_TYPE32, RARCH_GL_FORMAT32, menu_texture.pixels);

   glBindTexture(GL_TEXTURE_2D, menu_texture_id[0]);

   free(menu_texture.pixels);

	fbaRL = new c_fbaRL();

    return true;
}

void drawView()
{
    static GLfloat rot = 0.0;
    
    
    glColor4f(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    static const GLfloat vertices[] = {
        -1.0,  1.0, -0.0,
         1.0,  1.0, -0.0,
        -1.0, -1.0, -0.0,
         1.0, -1.0, -0.0
    };
    static const GLfloat normals[] = {
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0
    };

   static const GLfloat squareVertices[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f,  1.0f,
        1.0f,  1.0f,
    };

    static const GLfloat textureVertices[] = {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f,  1.0f,
        0.0f,  0.0f,
    };

    static const GLfloat texCoords[] = {
        0.0, 1.0,
        1.0, 1.0,
        0.0, 0.0,
        1.0, 0.0
    };
    
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -3.0);
    glRotatef(rot, 1.0, 1.0, 1.0);
    
    glBindTexture(GL_TEXTURE_2D, menu_texture_id[0]);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glNormalPointer(GL_FLOAT, 0, normals);
    glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    
    //static NSTimeInterval lastDrawTime;
    //if (lastDrawTime)
    //{
    //    NSTimeInterval timeSinceLastDraw = [NSDate timeIntervalSinceReferenceDate] - lastDrawTime;
    //    rot+=  60 * timeSinceLastDraw;                
    //}
    //lastDrawTime = [NSDate timeIntervalSinceReferenceDate];
}

CapApp::CapApp()
{
	mFrame = 0;

	mIsCirclePressed = mIsCrossPressed = mIsSquarePressed = mIsTrianglePressed = false;
	mIsSelectPressed = mIsStartPressed = false;
	mIsUpPressed = mIsDownPressed = mIsLeftPressed = mIsRightPressed = false;
	mIsL1Pressed = mIsL2Pressed = mIsL3Pressed = false;
	mIsR1Pressed = mIsR2Pressed = mIsR3Pressed = false;

	squarePressedNow = crossPressedNow = circlePressedNow = trianglePressedNow = false;	
	selectPressedNow = startPressedNow = false;
	upPressedNow = downPressedNow = leftPressedNow = rightPressedNow = false;
	l1PressedNow = l2PressedNow = l3PressedNow = false;
	r1PressedNow = r2PressedNow = r3PressedNow = false;


    // set up mStartupInfo if necessary
}

void CapApp::dbgFontInit()
{
	// initialize debug font library
	int ret;

	CellDbgFontConfig cfg;
	memset(&cfg, 0, sizeof(CellDbgFontConfig));
	
	cfg.bufSize      = 1024 * 10;
	cfg.screenWidth  = mDispInfo.mWidth;
	cfg.screenHeight = mDispInfo.mHeight;
	
	ret = cellDbgFontInit(&cfg);

	if (ret != CELL_OK) 
	{
		cellMsgDialogOpen2(CELL_MSGDIALOG_DIALOG_TYPE_NORMAL, "cellDbgFontInit() failed", DlgCallbackFunction, NULL, NULL);
		return;
	}

	// Debug font consoles - Unused at this moment...
	
	/*
	CellDbgFontConsoleConfig ccfg[2];
	memset(ccfg, 0, sizeof(CellDbgFontConsoleConfig) * 2);

	ccfg[0].posLeft     = 0.1f;
	ccfg[0].posTop      = 0.8f;
	ccfg[0].cnsWidth    = 32;
	ccfg[0].cnsHeight   = 4;
	ccfg[0].scale       = 1.0f;

	// ABGR -> orange
	ccfg[0].color       = 0xff0080ff;

	mDbgFontID[0] = cellDbgFontConsoleOpen(&ccfg[0]);

	if (mDbgFontID[0] < 0) 
	{
		cellMsgDialogOpen2(CELL_MSGDIALOG_DIALOG_TYPE_NORMAL, "cellDbgFontConsoleOpen() failed", DlgCallbackFunction, NULL, NULL);
		return;
	}

	ccfg[1].posLeft     = 0.25f;
	ccfg[1].posTop      = 0.2f;
	ccfg[1].cnsWidth    = 256;
	ccfg[1].cnsHeight   = 64;
	ccfg[1].scale       = 0.5f;

	// ABGR -> pale blue
	ccfg[1].color       = 0xffff8080;

	mDbgFontID[0] = cellDbgFontConsoleOpen(&ccfg[1]);

	if (mDbgFontID[1] < 0) 
	{
		cellMsgDialogOpen2(CELL_MSGDIALOG_DIALOG_TYPE_NORMAL, "cellDbgFontConsoleOpen() failed", DlgCallbackFunction, NULL, NULL);
		return;
	}
	*/
}

void CapApp::dbgFontDraw()
{
	::cellDbgFontDraw();
}

void CapApp::dbgFontExit()
{
	::cellDbgFontExit();
}

void CapApp::InputFrameStart() 
{
	squarePressedNow	= mpSquare->getBoolValue();
	crossPressedNow		= mpCross->getBoolValue();
	circlePressedNow	= mpCircle->getBoolValue();
	trianglePressedNow	= mpTriangle->getBoolValue();
	selectPressedNow	= mpSelect->getBoolValue();
	startPressedNow		= mpStart->getBoolValue();
	upPressedNow		= mpUp->getBoolValue();
	downPressedNow		= mpDown->getBoolValue();
	leftPressedNow		= mpLeft->getBoolValue();
	rightPressedNow		= mpRight->getBoolValue();
	r1PressedNow		= mpR1->getBoolValue();
	r2PressedNow		= mpR2->getBoolValue();
	r3PressedNow		= mpR3->getBoolValue();
	l1PressedNow		= mpL1->getBoolValue();
	l2PressedNow		= mpL2->getBoolValue();
	l3PressedNow		= mpL3->getBoolValue();
	mValRStickX			= mpRStickX->getRawValue();
	mValRStickY			= mpRStickY->getRawValue();
	mValLStickX			= mpLStickX->getRawValue();
	mValLStickY			= mpLStickY->getRawValue();
}

void CapApp::InputFrameEnd()
{
	mIsSquarePressed	= squarePressedNow;
	mIsCrossPressed		= crossPressedNow;
	mIsCirclePressed	= circlePressedNow;
	mIsTrianglePressed	= trianglePressedNow;
	mIsSelectPressed	= selectPressedNow;
	mIsStartPressed		= startPressedNow;
	mIsUpPressed		= upPressedNow;
	mIsDownPressed		= downPressedNow;
	mIsLeftPressed		= leftPressedNow;
	mIsRightPressed		= rightPressedNow;
	mIsR1Pressed		= r1PressedNow;
	mIsR2Pressed		= r2PressedNow;
	mIsR3Pressed		= r3PressedNow;
	mIsL1Pressed		= l1PressedNow;
	mIsL2Pressed		= l2PressedNow;
	mIsL3Pressed		= l3PressedNow;
}

bool CapApp::onUpdate()
{
	mFrame++;

	InputFrameStart();

	fbaRL->Frame();

	InputFrameEnd();

	//return FWGLApplication::onUpdate();
	return true;
}

void CapApp::onRender() 
{
    // again, call the superclass method
   // FWGLApplication::onRender();

	//glClearColor(mClearRed, mClearGreen, mClearBlue, mClearAlpha);
	
	glClearColor(0, 0, 0, 0);
	glClearDepthf(mClearDepth);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
//	glMultMatrixf((float *)&mViewMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();	

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	drawView();

	dbgFontDraw();

	psglSwap();

	
}

//void CapApp::onSize(const FWDisplayInfo &dispInfo)
//{
//	// copy disp info class
//	mDispInfo.mWidth = dispInfo.mWidth;
//	mDispInfo.mHeight = dispInfo.mHeight;
//	mDispInfo.mWideScreen = dispInfo.mWideScreen;
//
//	// set new viewport size and scissor bounds
//	glViewport(0, 0, mDispInfo.mWidth, mDispInfo.mHeight);
//	glScissor(0, 0, mDispInfo.mWidth, mDispInfo.mHeight);
//
//	// set projection matrix
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//
//	switch(mProjMode)
//	{
//	case ProjectionMode_Perspective:
//		gluPerspectivef(mFOV, mDispInfo.mWideScreen ? 16.f/9.f : 4.f/3.f, mNear, mFar);
//		break;
//	case ProjectionMode_Orthographic:
//		gluOrtho2Df(0, (float)mDispInfo.mWidth, 0, (float)mDispInfo.mHeight);
//		break;
//	}
//
//	// notify the debug font component
//	FWDebugFont::setScreenRes(mDispInfo.mWidth, mDispInfo.mHeight);
//}

void CapApp::onShutdown() 
{
	FWInputDevice *pPad = FWInput::getDevice(FWInput::DeviceType_Pad, 0);

	if(pPad != NULL)
	{
		pPad->unbindFilter(mpSquare);
		pPad->unbindFilter(mpCross);
		pPad->unbindFilter(mpCircle);
		pPad->unbindFilter(mpTriangle);
		pPad->unbindFilter(mpSelect);
		pPad->unbindFilter(mpStart);
		pPad->unbindFilter(mpUp);
		pPad->unbindFilter(mpDown);
		pPad->unbindFilter(mpLeft);
		pPad->unbindFilter(mpRight);
		pPad->unbindFilter(mpL1);
		pPad->unbindFilter(mpL2);
		pPad->unbindFilter(mpL3);
		pPad->unbindFilter(mpR1);
		pPad->unbindFilter(mpR2);
		pPad->unbindFilter(mpR3);
		pPad->unbindFilter(mpLStickX);
		pPad->unbindFilter(mpLStickY);
		pPad->unbindFilter(mpRStickX);
		pPad->unbindFilter(mpRStickY);
	}

    FWGLApplication::onShutdown();
}
