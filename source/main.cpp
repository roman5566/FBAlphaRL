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

struct PSGLdevice	*device;
struct PSGLcontext	*context;
bool bRun = true;

GLuint			renderWidth = 0, renderHeight = 0;
unsigned int	deviceWidth = 0, deviceHeight = 0;

void c_tex::Render(GLuint x, GLuint y, GLsizei w, GLsizei h)
{
	if(!bTextureOK) return;	

	if(w == 0|| h == 0)
	{
		w = renderWidth;
		h = renderHeight;
	}

	glViewport(x, y, w, h);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, nTexId[0]);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	const GLfloat normals[] = {
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0
	};
	const GLfloat squareVertices[] = {
		-1.0f, -1.0f,
		1.0f, -1.0f,
		-1.0f,  1.0f,
		1.0f,  1.0f,
	};
	const GLfloat texCoords[] = {
		0.0, 1.0,
		1.0, 1.0,
		0.0, 0.0,
		1.0, 0.0
	};

	glVertexPointer(2, GL_FLOAT, 0, squareVertices);
	glNormalPointer(GL_FLOAT, 0, normals);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoords);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); 

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

int c_tex::BindTexture(char* pszPath)
{
	bTextureOK = false;

	glGenTextures(1, &nTexId[0]);

	if(fbaRL->FileExist(pszPath))
	{
		if(!texture_image_load(pszPath, &_texture))
		{
			glDeleteTextures(1, &nTexId[0]);			
			return 0;
		} else {
			bTextureOK = true;

			glEnable(GL_VSYNC_SCE);
			glEnable(GL_TEXTURE_2D);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);

			glBindTexture(GL_TEXTURE_2D, nTexId[0]);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexImage2D(
				GL_TEXTURE_2D, 
				0, 
				GL_ARGB_SCE,
				_texture.width, 
				_texture.height, 
				0,
				GL_BGRA, 
				GL_UNSIGNED_INT_8_8_8_8_REV, 
				_texture.pixels
			);

			glBindTexture(GL_TEXTURE_2D, nTexId[0]);

			SAFE_FREE(_texture.pixels)			
			return 1;
		}
	}
	return 0;
}

c_tex::c_tex(uint32_t _nTexture, char* szPath) 
{
	nTexture = _nTexture;

	pszTexPath = (char*)malloc(2048);
	memset(pszTexPath, 0, 2048);

	strcpy(pszTexPath, szPath);
	
	BindTexture(pszTexPath);
}

c_tex::~c_tex()
{
	if(bTextureOK) 
	{
		glDeleteTextures(1, &nTexId[0]);
		*&nTexId[0] = NULL;
		*&bTextureOK = NULL;
	}
	SAFE_FREE(pszTexPath)
}

void RenderBackground()
{
	if(fbaRL->nSection == SECTION_MAIN) {
		app.textures[TEX_MAIN_MENU]->Render(0,0,0,0);
	}
	if(fbaRL->nSection == SECTION_GAMELIST) 
	{
		app.textures[TEX_GAME_LIST]->Render(0,0,0,0);

		if(!fbaRL->bProcessingGames) 
		{
			// ------------------------------------------------------
			// PROPER ASPECT RATIO CALCULATIONS 

			float w = (float)app.textures[TEX_PREVIEW]->_texture.width * 4.0f;
			float h = (float)app.textures[TEX_PREVIEW]->_texture.height * 4.0f;

			float maxw = (27.604f / 100.0f) * (float)renderWidth;		// 530 @ 1920 x 1080 res
			float maxh = (31.481f / 100.0f) * (float)renderHeight;		// 340 @ 1920 x 1080 res

			if( app.textures[TEX_PREVIEW]->_texture.width > app.textures[TEX_PREVIEW]->_texture.height || 
				app.textures[TEX_PREVIEW]->_texture.width == app.textures[TEX_PREVIEW]->_texture.height )
			{
				maxw = (27.604f / 100.0f) * (float)renderWidth;  // 530 @ 1920 x 1080 res
				maxh = (31.481f / 100.0f) * (float)renderHeight; // 340 @ 1920 x 1080 res
			} else {
				maxw = (31.481f / 100.0f) * (float)renderWidth;  // 340 @ 1920 x 1080 res
				maxh = (45.370f / 100.0f) * (float)renderHeight; // 490 @ 1920 x 1080 res
			}

			// max WIDTH
			if(w > maxw) {
				float nh = maxw * (float)(h / w);
				float nw = maxw;

				// max HEIGHT
				if(nh > maxh) {
					nw = maxh * (float)(nw / nh);
					nh = maxh;
				}

				w = nw;
				h = nh;
			}

			// max HEIGHT
			if(h > maxh) {
				float nw = maxh * (float)(w / h);
				float nh = maxh;

				// max WIDTH
				if(nw > maxw) {
					nh = maxw * (float)(nh / nw);
					nw = maxw;
				}

				w = nw;
				h = nh;
			}
			// ------------------------------------------------------

			// Proper centering of preview
			float xdiff = ((maxw - w) / 2.0f);
			float ydiff = ((maxh - h) / 2.0f);

			if( app.textures[TEX_PREVIEW]->_texture.width > app.textures[TEX_PREVIEW]->_texture.height || 
				app.textures[TEX_PREVIEW]->_texture.width == app.textures[TEX_PREVIEW]->_texture.height )
			{
				float x = renderWidth - (renderWidth / 3) + (GLuint)xdiff;
				float y = renderHeight - (renderHeight / 2) + (GLuint)ydiff;

				app.textures[TEX_PREVIEW]->Render((GLuint)x, (GLuint)y, (GLsizei)w, (GLsizei)h);
			} else {
				// x - 70
				// y - 75
				float xadjust = (3.645833f / 100.0f) * (float)renderWidth;
				float yadjust = (6.944444f / 100.0f) * (float)renderHeight;

				float x = (((float)renderWidth - ((float)renderWidth / 4.0f)) - (xadjust * 2.80f)) + xdiff;
				float y = (((float)renderHeight - ((float)renderHeight / 2.0f)) - yadjust) + ydiff;

				app.textures[TEX_PREVIEW]->Render((GLuint)x, (GLuint)y, (GLsizei)w, (GLsizei)h);
			}
		}
	}
	if(fbaRL->nSection == SECTION_ZIPINFO) {
		app.textures[TEX_ZIP_INFO]->Render(0,0,0,0);
	}
	if(fbaRL->nSection == SECTION_OPTIONS) {
		app.textures[TEX_OPTIONS]->Render(0,0,0,0);
	}
	if(fbaRL->nSection == SECTION_FILEBROWSER) {
		app.textures[TEX_FILEBROWSER]->Render(0,0,0,0);
	}
}

void CapApp::initGraphics(PSGLdevice */*device*/)
{
	textures[TEX_MAIN_MENU]		= new c_tex(nTextures, g_opt_szTextures[TEX_MAIN_MENU]);
	nTextures++;
	textures[TEX_GAME_LIST]		= new c_tex(nTextures, g_opt_szTextures[TEX_GAME_LIST]);
	nTextures++;
	textures[TEX_ZIP_INFO]		= new c_tex(nTextures, g_opt_szTextures[TEX_ZIP_INFO]);
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

	if(bRun && status == CELL_SYSUTIL_REQUEST_EXITGAME) {
		bRun = false;
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

		initGraphics(device);

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

	onShutdown();

	return false;
}

void CapApp::onRender()
{
	if(fbaRL) fbaRL->DlgDisplayFrame();

	// get render target buffer dimensions and set viewport	
	psglGetRenderBufferDimensions(device,&renderWidth,&renderHeight);

	glViewport(0, 0, renderWidth, renderHeight);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderBackground();
	if(fbaRL) fbaRL->DisplayFrame();
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