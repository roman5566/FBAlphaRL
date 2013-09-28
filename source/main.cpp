// ---------------------------------------------------------------------
// FB Alpha Retro Loader (CaptainCPS-X, 2013)
// ---------------------------------------------------------------------
#include "main.h"

/*
FUTURE TODO:
- Rominfo
- Rominfo / check for missing / incorrect files
- Configuration backup maker (save in /dev_hdd0/fba_cfg_bkp or /dev_usb00x/...)
*/


void CapApp::initGraphics()
{
	textures[TEX_MAIN_MENU]		= new c_tex(nTextures, g_opt_szTextures[TEX_MAIN_MENU]);
	nTextures++;
	textures[TEX_GAME_LIST]		= new c_tex(nTextures, g_opt_szTextures[TEX_GAME_LIST]);
	nTextures++;
	textures[TEX_ZIP_INFO]		= new c_tex(nTextures, g_opt_szTextures[TEX_ZIP_INFO]);
	nTextures++;
	textures[TEX_ROM_INFO]		= new c_tex(nTextures, g_opt_szTextures[TEX_ROM_INFO]);
	nTextures++;
	textures[TEX_OPTIONS]		= new c_tex(nTextures, g_opt_szTextures[TEX_OPTIONS]);
	nTextures++;
	textures[TEX_FILEBROWSER]	= new c_tex(nTextures, g_opt_szTextures[TEX_FILEBROWSER]);
	nTextures++;

	textures[TEX_PREVIEW]		= new c_tex(TEX_PREVIEW, g_opt_szTextures[TEX_PREVIEW]);

	// clear the screen on startup
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	psglSwap();

}

static int chooseBestResolution(const uint32_t *resolutions, uint32_t numResolutions)
{
	uint32_t bestResolution = 0;

	for (uint32_t i = 0; bestResolution == 0 && i < numResolutions; i++) 
	{
		if(cellVideoOutGetResolutionAvailability(CELL_VIDEO_OUT_PRIMARY, resolutions[i], CELL_VIDEO_OUT_ASPECT_AUTO, 0))
		{
			bestResolution = resolutions[i];
		}
	}
	return bestResolution;
}

static int getResolutionWidthHeight(const uint32_t resolutionId, uint32_t &w, uint32_t &h)
{
	switch(resolutionId)
	{
		case CELL_VIDEO_OUT_RESOLUTION_480: 
			w=720;  h=480;  
			return 1;
		case CELL_VIDEO_OUT_RESOLUTION_576: 
			w=720;  h=576;  
			return 1;
		case CELL_VIDEO_OUT_RESOLUTION_720: 
			w=1280; h=720;  
			return 1;
		case CELL_VIDEO_OUT_RESOLUTION_1080: 
			w=1920; h=1080; 
			return 1;
		case CELL_VIDEO_OUT_RESOLUTION_1600x1080: 
			w=1600; h=1080; 
			return 1;
		case CELL_VIDEO_OUT_RESOLUTION_1440x1080: 
			w=1440; h=1080; 
			return 1;
		case CELL_VIDEO_OUT_RESOLUTION_1280x1080: 
			w=1280; h=1080; 
			return 1;
		case CELL_VIDEO_OUT_RESOLUTION_960x1080: 
			w=960;  h=1080; 
			return 1;
	};

	// error...
	return 0;
}

bool videoOutIsReady()
{
	CellVideoOutState videoState;

	cellVideoOutGetState(CELL_VIDEO_OUT_PRIMARY, 0, &videoState);

	return(videoState.state == CELL_VIDEO_OUT_OUTPUT_STATE_ENABLED);
}

static void callback_sysutil_exit(uint64_t status, uint64_t param, void *userdata)
{
	(void)param;
	(void)userdata;

	if(app.bRun && status == CELL_SYSUTIL_REQUEST_EXITGAME) {
		app.bRun = false;
	}
}

CapApp app;

int main(int argc, char* argv[])
{
	app.onInit(argc, argv);		
	return 0;
}

CapApp::CapApp()
{
	bRun = true;

	ftp_service = 0;

	renderWidth = 0;
	renderHeight = 0;
	deviceWidth = 0;
	deviceHeight = 0;

	normals[0] = 0.0f; normals[1] = 0.0f;	normals[2] = 1.0f;
	normals[3] = 0.0f; normals[4] = 0.0f;	normals[5] = 1.0f;
	normals[6] = 0.0f; normals[7] = 0.0f;	normals[8] = 1.0f;
	normals[9] = 0.0f; normals[10] = 0.0f;	normals[11] = 1.0f;

	squareVertices[0] = -1.0f;	squareVertices[1] = -1.0f;
	squareVertices[2] = 1.0f;	squareVertices[3] = -1.0f;
	squareVertices[4] = -1.0f;	squareVertices[5] = 1.0f;
	squareVertices[6] = 1.0f;	squareVertices[7] = 1.0f;

	texCoords[0] = 0.0f;	texCoords[1] = 1.0f;
	texCoords[2] = 1.0f;	texCoords[3] = 1.0f;
	texCoords[4] = 0.0f;	texCoords[5] = 0.0f;
	texCoords[6] = 1.0f;	texCoords[7] = 0.0f;

	textures = (c_tex**)malloc(sizeof(c_tex) * 10);
	memset(textures, 0, sizeof(c_tex) * 10);

	mFrame = 0;
	for(int n = 0; n < 16; n++)	{
		mIsButtPressed[n] = false;
		buttPressedNow[n] = false;
		nButtons[n] = (1 << n);		// Assign button values in order
	}
	mValRStickX = mValRStickY = 0;
	mValLStickX = mValLStickY = 0;
}

bool CapApp::onInit(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

	// ----------------------------------------------
	// FTP	
	cellSysmoduleLoadModule(CELL_SYSMODULE_NET);
	cellNetCtlInit();
	cellSysmoduleLoadModule(CELL_SYSMODULE_HTTP);
	sys_net_initialize_network();
	ftp_on();

	// Load settings...
	if(!iniRead()) {
		iniWrite(); // create settings file...
	}

	cellSysmoduleLoadModule(CELL_SYSMODULE_FS);

	cellSysmoduleLoadModule(CELL_SYSMODULE_SYSUTIL_SCREENSHOT);
	cellScreenShotEnable();

	InputInit();

	while(!videoOutIsReady())
	{
		// ...
	}


	PSGLinitOptions options = 
	{
		enable:					PSGL_INIT_MAX_SPUS | PSGL_INIT_INITIALIZE_SPUS,
		maxSPUs:				1,
		initializeSPUs:			GL_FALSE,
		persistentMemorySize:	0,
		transientMemorySize:	0,
		errorConsole:			0,
		fifoSize:				0,  
		hostMemorySize:			128* 1024*1024,  // 128 mbs for host memory 
	};

#if CELL_SDK_VERSION < 0x340000
	options.enable |=	PSGL_INIT_HOST_MEMORY_SIZE;
#endif

	// Initialize 6 SPUs but reserve 1 SPU as a raw SPU for PSGL
	sys_spu_initialize(6, 1);
	psglInit(&options);

	const unsigned int resolutions[] = { 
		CELL_VIDEO_OUT_RESOLUTION_1080, 
		CELL_VIDEO_OUT_RESOLUTION_960x1080, 
		CELL_VIDEO_OUT_RESOLUTION_720, 
		CELL_VIDEO_OUT_RESOLUTION_480 
	};

	const int numResolutions = sizeof(resolutions) / sizeof(resolutions[0]);

	int bestResolution = chooseBestResolution(resolutions,numResolutions);

	getResolutionWidthHeight(bestResolution, deviceWidth, deviceHeight);

	if(bestResolution)
	{
		PSGLdeviceParameters params;

		params.enable				= PSGL_DEVICE_PARAMETERS_COLOR_FORMAT |
									  PSGL_DEVICE_PARAMETERS_DEPTH_FORMAT |
									  PSGL_DEVICE_PARAMETERS_MULTISAMPLING_MODE;
		params.colorFormat			= GL_ARGB_SCE;
		params.depthFormat			= GL_NONE;
		params.multisamplingMode	= GL_MULTISAMPLING_NONE_SCE;
		params.enable				|= PSGL_DEVICE_PARAMETERS_WIDTH_HEIGHT;
		params.width				= deviceWidth;
		params.height				= deviceHeight;

		device						= psglCreateDeviceExtended(&params);
		context						= psglCreateContext();

		psglMakeCurrent(context, device);
		psglResetCurrentContext();

		initGraphics();

		if( cellSysutilRegisterCallback( 0, callback_sysutil_exit, NULL ) < 0 ) {
			//...
		}

		dbgFontInit();

		fbaRL = new c_fbaRL();

		while(bRun)
		{
			onRender();
			onUpdate();
			cellSysutilCheckCallback();
		}

	} else {
		// resolution error...
	}

	ftp_off();

	onShutdown();

	return false;
}

void CapApp::onRender()
{
	if(fbaRL) { fbaRL->DlgDisplayFrame(); }

	// get render target buffer dimensions and set viewport	
	psglGetRenderBufferDimensions(device,&app.renderWidth,&app.renderHeight);

	glViewport(0, 0, app.renderWidth, app.renderHeight);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(fbaRL) { fbaRL->RenderBackground(); }
	if(fbaRL) { fbaRL->nFrameStep = 0; fbaRL->DisplayFrame(); }
	if(fbaRL) { fbaRL->nFrameStep = 1; fbaRL->DisplayFrame(); }
	dbgFontDraw();

	psglSwap();
}

bool CapApp::onUpdate()
{
	if(!mFrame) mFrame = 0;
	mFrame++;

	InputFrameStart();
	if(fbaRL) fbaRL->InputFrame();
	InputFrameEnd();	

	return true;
}

void CapApp::onShutdown()
{
	iniWrite(); // save settings

	if(context) psglDestroyContext(context);
	if(device) psglDestroyDevice(device);

	InputExit();
	psglExit();
}

void CapApp::dbgFontInit()
{
	int ret;

	CellDbgFontConfig cfg;
	memset(&cfg, 0, sizeof(CellDbgFontConfig));
	
	cfg.bufSize      = 1024 * 10;
	cfg.screenWidth  = deviceWidth;
	cfg.screenHeight = deviceHeight;
	
	ret = cellDbgFontInit(&cfg);

	if (ret != CELL_OK) 
	{
		// error...
		return;
	}

}

void CapApp::dbgFontDraw()
{
	cellDbgFontDraw();
}

void CapApp::dbgFontExit()
{
	cellDbgFontExit();
}