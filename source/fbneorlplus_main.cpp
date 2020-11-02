// ---------------------------------------------------------------------
// FB Neo Retro Loader Plus (CaptainCPS-X, 2013) - (CrystalCT, 2020)
// ---------------------------------------------------------------------
#include <ppu-lv2.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include "rsxutil.h"
#include <sysutil/video.h>
#include <sysutil/msg.h>
#include <sysutil/sysutil.h>
#include <sys/spu.h>
#include <lv2/spu.h>
#include <rsx/gcm_sys.h>
#include <rsx/rsx.h>
#include <sys/process.h>
#include <sysmodule/sysmodule.h>
#include <pngdec/pngdec.h>
#include <io/pad.h>
#include "main.h"
#include "ftp/ftp.h"
#include "spu_soundlib.h"
#include "audioplayer.h"
#include "hashmap.h"
#include "fnt_print.h"
#include "fnt35.h"
#include <math.h>
#include <sys/thread.h>

#include <ps3sqlite/sqlite3.h>
#include "sqlite.h"

fnt_t fontM, fontL;
static vs32 dialog_action = 0;

//SYS_PROCESS_PARAM(1001, 0x100000);

// SPU
u32 spu = 0;
sysSpuImage spu_image;
#define SPU_SIZE(x) (((x)+127) & ~127)
u32 inited;
#include "spu_soundmodule_bin.h"

extern "C" {

	//gcmContextData *initScreenPrev(void *host_addr, u32 size);
	int SND_Init_cpp(u32 spu);
	void SND_Pause_cpp(int paused);
	void SND_End_cpp();


}

static void dialog_handler(msgButton button,void *usrData) {
	switch(button) {
		case MSG_DIALOG_BTN_OK:
			dialog_action = 1;
			break;
		case MSG_DIALOG_BTN_NO:
		case MSG_DIALOG_BTN_ESCAPE:
			dialog_action = 2;
			break;
		case MSG_DIALOG_BTN_NONE:
			dialog_action = -1;
			break;
		default:
			break;
	}
}






void CapApp::Flip() {
    //waitFlip();
    //flip();
    flip(context, buffers[currentBuffer].id);
    currentBuffer++;
    if (currentBuffer >= MAX_BUFFERS)
				currentBuffer = 0;

}

void CapApp::FlipSimple() {
    //waitFlip();
    flip(context, buffers[currentBuffer].id);
    //flip();
}

void CapApp::initSPUSound() {
    u32 entry = 0;
	u32 segmentcount = 0;
	sysSpuSegment* segments;
	printf("Initializing SPUs... %08x\n", sysSpuInitialize(6, 5));
	printf("Initializing raw SPU... %08x\n", sysSpuRawCreate(&spu, NULL));
	printf("Getting ELF information... %08x\n",
        sysSpuElfGetInformation(spu_soundmodule_bin, &entry, &segmentcount));
	printf("    Entry Point: %08x    Segment Count: %08x\n", entry, segmentcount);
	size_t segmentsize = sizeof(sysSpuSegment) * segmentcount;
	segments = (sysSpuSegment*)memalign(128, SPU_SIZE(segmentsize)); // must be aligned to 128 or it break malloc() allocations
	memset(segments, 0, segmentsize);
	printf("Getting ELF segments... %08x\n",
        sysSpuElfGetSegments(spu_soundmodule_bin, segments, segmentcount));
	printf("Loading ELF image... %08x\n",
        sysSpuImageImport(&spu_image, spu_soundmodule_bin, 0));
	printf("Loading image into SPU... %08x\n",
        sysSpuRawImageLoad(spu, &spu_image));
	SND_Init_cpp(spu);

	SND_Pause_cpp(0);
    std::vector <std::string> tmp;
    tracks.clear();
	readDir("/dev_hdd0/game/FBNE00123/USRDIR/soundtracks", DIR_NO_DOT_AND_DOTDOT | DIR_FILES, tracks, tmp);
	trackID = tracks.begin() + g_opt_nTrackID;
	if (trackID < tracks.end() && g_opt_bMusic) {
        char tpath[256];
        sprintf(tpath, "/dev_hdd0/game/FBNE00123/USRDIR/soundtracks/%s", (*trackID).c_str());
        ftrack = fopen(tpath, "r");
        if (ftrack) {
            //printf("Start track: %s\n", (*trackID).c_str());
            PlayAudiofd(ftrack, 0,  AUDIO_INFINITE_TIME);
        }

    }
}

void CapApp::initGraphics()
{

	//char ResulutonSuffixPNG[11];
	currentBuffer = 0;

	host_addr = memalign (1024*1024, HOST_SIZE);
	//context = initScreenPrev (host_addr, HOST_SIZE);
	context = initScreen(host_addr,HOST_SIZE);

	// Get the current resolution
	getResolution(&width, &height);
	//videoState state;
    int ix=0;
	for (ix = 0; ix < MAX_BUFFERS; ix++)
        makeBuffer( &buffers[ix], width, height, ix);

	//printf("Resolution %dx%d\n", width, height);
	flip(context, MAX_BUFFERS - 1);



    //currentBuffer++;
	//printf("Getresolution: %dx%d - Memory current buffer: %p\n", width, height, (void *)buffers[currentBuffer].ptr);
	assert (videoGetState (0, 0, &state) == 0);   // Get the state of the display
	assert (state.state == 0);    // Make sure display is enabled



    pngData *png = new pngData;
    png->width = 0;
    png->height = 0;
    png->bmp_out = NULL;
    if (state.displayMode.resolution == VIDEO_RESOLUTION_1080) {
            //png->bmp_out = malloc(1920*1080*4);
            pngLoadFromFile("/dev_hdd0/game/FBNE00123/USRDIR/LOADING1.PNG", png);
            //rpng_load_image_argb("/dev_hdd0/game/FBNE00123/USRDIR/LOADING1.PNG", (uint32_t*)png->bmp_out, &png->width, &png->height);
    }
    else if (state.displayMode.resolution == VIDEO_RESOLUTION_720) {
        //png->bmp_out = malloc(1280*720*4);
        pngLoadFromFile("/dev_hdd0/game/FBNE00123/USRDIR/LOADING2.PNG", png);
        //rpng_load_image_argb("/dev_hdd0/game/FBNE00123/USRDIR/LOADING2.PNG", (uint32_t*)png->bmp_out, &png->width, &png->height);
    }
    else {
        dialog_action = 0;
        msgDialogOpen2((msgType)((MSG_DIALOG_NORMAL
                                |MSG_DIALOG_BTN_TYPE_OK
                                |MSG_DIALOG_DISABLE_CANCEL_ON)),
                                "Sorry, only 1080p or 720p video resuliton accepted.",
                                dialog_handler,(void *)&dialog_action, NULL);
        while(dialog_action == 0) {
            waitFlip();
            Flip();
            sysUtilCheckCallback();
        }
        exit(0);
    }

    if (png->bmp_out)
        for (ix= 0; ix <2; ix++) {
            memcpy(buffers[currentBuffer].ptr, (uint32_t *)png->bmp_out, png->width*png->height* sizeof(uint32_t));

            waitFlip();
            Flip();
            sysUtilCheckCallback();
        }

    SAFE_FREE(png->bmp_out);
    SAFE_DELETE(png);


	/*switch (state.displayMode.resolution)
	{
		case VIDEO_RESOLUTION_1080:
			strcat(g_opt_szTextures[TEX_MAIN_MENU],"_1920x1080.PNG");
			strcat(g_opt_szTextures[TEX_GAME_LIST],"_1920x1080.PNG");
			strcat(g_opt_szTextures[TEX_ZIP_INFO],"_1920x1080.PNG");
			strcat(g_opt_szTextures[TEX_ROM_INFO],"_1920x1080.PNG");
			strcat(g_opt_szTextures[TEX_OPTIONS],"_1920x1080.PNG");
			strcat(g_opt_szTextures[TEX_FILEBROWSER],"_1920x1080.PNG");
			//strcat(g_opt_szTextures[TEX_PREVIEW],"_1920x1080.PNG");
		break;
		case VIDEO_RESOLUTION_720:
			strcat(g_opt_szTextures[TEX_MAIN_MENU],"_1280x720.PNG");
			strcat(g_opt_szTextures[TEX_GAME_LIST],"_1280x720.PNG");
			strcat(g_opt_szTextures[TEX_ZIP_INFO],"_1280x720.PNG");
			strcat(g_opt_szTextures[TEX_ROM_INFO],"_1280x720.PNG");
			strcat(g_opt_szTextures[TEX_OPTIONS],"_1280x720.PNG");
			strcat(g_opt_szTextures[TEX_FILEBROWSER],"_1280x720.PNG");
			//strcat(g_opt_szTextures[TEX_PREVIEW],"_1280x720.PNG");
		break;
		case VIDEO_RESOLUTION_576:
			strcat(g_opt_szTextures[TEX_MAIN_MENU],"_720x576.PNG");
			strcat(g_opt_szTextures[TEX_GAME_LIST],"_720x576.PNG");
			strcat(g_opt_szTextures[TEX_ZIP_INFO],"_720x576.PNG");
			strcat(g_opt_szTextures[TEX_ROM_INFO],"_720x576.PNG");
			strcat(g_opt_szTextures[TEX_OPTIONS],"_720x576.PNG");
			strcat(g_opt_szTextures[TEX_FILEBROWSER],"_720x576.PNG");
			//strcat(g_opt_szTextures[TEX_PREVIEW],"_720x576.PNG");
		break;
		case VIDEO_RESOLUTION_480:
			strcat(g_opt_szTextures[TEX_MAIN_MENU],"_720x480.PNG");
			strcat(g_opt_szTextures[TEX_GAME_LIST],"_720x480.PNG");
			strcat(g_opt_szTextures[TEX_ZIP_INFO],"_720x480.PNG");
			strcat(g_opt_szTextures[TEX_ROM_INFO],"_720x480.PNG");
			strcat(g_opt_szTextures[TEX_OPTIONS],"_720x480.PNG");
			strcat(g_opt_szTextures[TEX_FILEBROWSER],"_720x480.PNG");
			//strcat(g_opt_szTextures[TEX_PREVIEW],"_720x480.PNG");
		break;
		default:
		// ERROR
		break;
	}*/





	//printf("Risoluzione è: %d - %s\n", state.displayMode.resolution,g_opt_szTextures[TEX_MAIN_MENU]);

    textures[TEX_MAIN_MENU]		= new c_tex(nTextures, state.displayMode.resolution);
	//textures[TEX_MAIN_MENU]		= new c_tex(nTextures, g_opt_szTextures[TEX_MAIN_MENU]);
	//pngData* png_src;
    //rsxBuffer *buf;
    //rsxBuffer* rsxbuffer;
    //rsxbuffer = &(buffers[currentBuffer]);
    //rsxBuffer *buffer = &buffers[currentBuffer];
	//png_src = app.textures[TEX_MAIN_MENU]->png;
	//memcpy(rsxbuffer->ptr, png_src->bmp_out, png_src->width * png_src->height * sizeof(uint32_t));
	//flip(context, MAX_BUFFERS - 1);

	nTextures++;

	textures[TEX_GAME_LIST]		= new c_tex(nTextures, state.displayMode.resolution);
	nTextures++;

	textures[TEX_ZIP_INFO]		= new c_tex(nTextures, state.displayMode.resolution);
	nTextures++;

	textures[TEX_ROM_INFO]		= new c_tex(nTextures, state.displayMode.resolution);
	nTextures++;

	textures[TEX_OPTIONS]		= new c_tex(nTextures, state.displayMode.resolution);
	nTextures++;

	textures[TEX_FILEBROWSER]	= new c_tex(nTextures, state.displayMode.resolution);
	nTextures++;
    //printf("Fixed texture ok\n");

	textures[TEX_PREVIEW]		= new c_tex(TEX_PREVIEW, g_opt_szTextures[TEX_PREVIEW]);
	//printf("First Preview texture ok\n");
	/*////nTextures++;
	//textures[TEX_GAME_SELECTOR]		= new c_tex(TEX_GAME_SELECTOR,"/dev_hdd0/game/FBNE00123/USRDIR/cores/borders/default/BLANK.PNG");
	textures[TEX_GAME_SELECTOR]		= new c_tex(TEX_GAME_SELECTOR,1141,663);
	*/

}


static void sysutil_exit_callback(uint64_t status, uint64_t param, void *userdata)
{
	if(app.bRun && status == SYSUTIL_EXIT_GAME) {
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
    fba_drv = NULL;
    drvMap = hashmap_new();
    gamesMap = hashmap_new();
	////renderWidth = 0;
	////renderHeight = 0;
	deviceWidth = 0;
	deviceHeight = 0;

	/*////normals[0] = 0.0f; normals[1] = 0.0f;	normals[2] = 1.0f;
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
	*/
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
	tracks.clear();
	ftrack = NULL;
	nTotalBurnDrivers = TOTAL_DRV_GAMES;
}



bool CapApp::onInit(int argc, char* argv[])
{
#ifdef FDEBUG
    FILE* fdebug = NULL;
	fdebug = fopen("/dev_hdd0/game/FBNE00123/USRDIR/fdebug.log", "w");
	if (fdebug == 0){
        printf("Errore nel write\n");
        return 0;
        }
    fprintf(fdebug,"Start FB Neo Rl Plus\n");
    fflush(fdebug);
#endif // FDEBUG
	(void)argc;
	(void)argv;

    if(sysModuleLoad(SYSMODULE_FS) != 0) exit(0);
    if(sysModuleLoad(SYSMODULE_PNGDEC) != 0) exit(0);
    if(sysModuleLoad(SYSMODULE_NETCTL) != 0) exit(0);
#ifdef FDEBUG
    fprintf(fdebug,"SYSMODULE_PNGDEC loaded\n");
    fflush(fdebug);
#endif // FDEBUG

	//atexit(onShutdown);
	//if( cellSysutilRegisterCallback( 0, callback_sysutil_exit, NULL ) < 0 ) {
	if( sysUtilRegisterCallback(0,sysutil_exit_callback,NULL) < 0 ) {
			printf("sysUtilRegisterCallback error.\n");
	}
#ifdef FDEBUG
    fprintf(fdebug,"sysUtilRegisterCallback OK\n");
    fflush(fdebug);
#endif // FDEBUG





	if(!iniRead()) {
#ifdef FDEBUG
    fprintf(fdebug,"iniRead OK\n");
    fflush(fdebug);
#endif
		iniWrite(); // create settings file...
#ifdef FDEBUG
    fprintf(fdebug,"iniWrite OK\n");
    fflush(fdebug);
#endif
	}

    initGraphics();
#ifdef FDEBUG
    fprintf(fdebug,"initGraphics OK\n");
    fflush(fdebug);
#endif
    //printf("End init Graphics\n");

    initSPUSound();
#ifdef FDEBUG
    fprintf(fdebug,"initSPUSound OK\n");
    fflush(fdebug);
#endif
	//CRYSTAL
	//LOAD  FBA Games Array
	if(InitDB())
    {
		printf("Exiting, an error occured while opening DB!\n");
        exit(0);
		return(1);
	}
#ifdef FDEBUG
    fprintf(fdebug,"InitDB OK\n");
    fflush(fdebug);
#endif
    //printf("End init DB\n");
	////
	// ----------------------------------------------
	// FTP

	int r = ftp_init();
#ifdef FDEBUG
    fprintf(fdebug,"ftp_init OK\n");
    fflush(fdebug);
#endif

        if(r == 0)
        {
            ftp_service = 1; //printf("OpenPS3FTP v2.3 by jjolano started.\n");

        } else {
            if(r == -1) printf("Error in netInitialize()\n");
            else if(r == -2) printf("Error in netCtlInit()\n");
            else if(r == -3) printf("Error in netCtlGetInfo()\n");
            else if(r == -4) printf("Net Disconnected or Connection not Established\n");
            else printf("Another FTP service present!\n");
        }


	/*cellSysmoduleLoadModule(CELL_SYSMODULE_NET);
	cellNetCtlInit();
	cellSysmoduleLoadModule(CELL_SYSMODULE_HTTP);
	sys_net_initialize_network();
	ftp_on();*/
    //printf("End ftp init\n");
	// Load settings...



	FontInit();
#ifdef FDEBUG
    fprintf(fdebug,"FontInit OK\n");
    fflush(fdebug);
#endif
	//printf("End init Font\n");
	InputInit();
#ifdef FDEBUG
    fprintf(fdebug,"InputInit OK\n");
    struct smeminfo {
            uint32_t total;
            uint32_t avail;
    } meminfo;
    lv2syscall1(352, (uint64_t) &meminfo);
    fprintf(fdebug,"MEMORY TOTAL: %d - AVAIL: %d\n", meminfo.total / 1024, meminfo.avail / 1024);
    fclose(fdebug);
#endif
    //printf("End init Imput\n");
	//long frame = 0; // To keep track of how many frames we have rendered.



		//fbaRL = new c_fbaRL();  //CRYSTAL
		//printf("onInit argv[0]: %s\n",argv[0]);
		if (argc > 1) {
			//printf("onInit argv[1]: %s\n",argv[1]);
			if (strcmp(argv[1],"gamesList") == 0) {
					fbaRL = new c_fbaRL(true);
					printf("StartwithFileSection = true\n");
			}
			else {
					fbaRL = new c_fbaRL(false);

            }
		}
		else {
					fbaRL = new c_fbaRL(false);
        }
#ifdef FDEBUG
        fdebug = fopen("/dev_hdd0/game/FBNE00123/USRDIR/fdebug.log", "a");
        fprintf(fdebug,"fbaRL class OK\n");
        fflush(fdebug);
#endif
        //printf("fbaRL->InitGameList()\n");
        fbaRL->InitGameList();
#ifdef FDEBUG
        fprintf(fdebug,"InitGameList OK\n");
        fflush(fdebug);
#endif
        //fbaRL->InitGameList();

        //printf("fbaRL->InitFilterList()\n");
        fbaRL->InitFilterList();
#ifdef FDEBUG
        fprintf(fdebug,"InitFilterList OK\n");
        fclose(fdebug);
#endif
        //printf("Start while\n");



		while(bRun)
		{

			onRender();

			onUpdate();
			sysUtilCheckCallback();
			////cellSysutilCheckCallback();
		}

	////} else {
		// resolution error...
	////}

	////ftp_off();


	onShutdown();

	return false;
}

void CapApp::onRender()
{

	//float xPos		= 0.1600f;
	//float yPos		= 0.6985f;

	//Bitmap1.bitmap = (uint32_t *)png->bmp_out;
	//F1.PrintfToBitmap(150,300,&Bitmap1,COLOR_GREEN,30,"FreeType2 with TTF support");
	//memcpy(buffer->ptr, Bitmap1.bitmap, png->width*png->height* sizeof(uint32_t));

	/*fnt_print_vram(&fontM, (u32 *)buffer->ptr, buffer->width, (int)(xPos * buffer->width), (int)(yPos * buffer->height),
						"ABCDEFGHILMNBabmcfdg123456(/&%$£865439ドラゴンズ マジック", 0xffffcc00, 0x00000000, 2, 2);
    fnt_print_vram(&fontS, (u32 *)buffer->ptr, buffer->width, (int)(xPos * buffer->width),
                   (int)((yPos -0.1f)* buffer->height),
						"ABCDEFGHILMNBabmcfdg123456(/&%$£865439", 0xffffcc00, 0x00000000, 2, 2);*/


    waitFlip();
//	currentBuffer++;
//	if (currentBuffer >= MAX_BUFFERS)
//			currentBuffer = 0;

//    if (fbaRL) {
//        fbaRL->DrawIMG(400,500,textures[TEX_PREVIEW]->png);
//
//    }
	if(fbaRL) { fbaRL->DlgDisplayFrame(); }

	// get render target buffer dimensions and set viewport


	/*////if(fbaRL) { fbaRL->RenderBackground(); }
	*/
	if(fbaRL) { fbaRL->nFrameStep = 0; fbaRL->DisplayFrame(); }
	if(fbaRL) { fbaRL->nFrameStep = 1; fbaRL->DisplayFrame(); }
	Flip();
//    waitFlip();
//    flip(context, buffers[currentBuffer].id);
    ////dbgFontDraw();
	//if (fbaRL->nFrameStep == 0)
        // Flip buffer onto screen

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

void CapApp::remakegamesMaps() {
    hashmap_free(gamesMap);
    gamesMap = hashmap_new();
    if (gamesMap == NULL)
        printf("ERRORE - remakegamesMaps: gamesMap = hashmap_new()\n");
}

void CapApp::onShutdown()
{
	printf("Shutdown\n");
	iniWrite(); // save settings



	gcmSetWaitFlip(context);
    hashmap_free(drvMap);
    hashmap_free(gamesMap);
    printf("Pre RSX End\n");
	int i = 0;
	for (i = 0; i < MAX_BUFFERS; i++)
		rsxFree(buffers[i].ptr);

	rsxFinish(context, 1);
	SAFE_FREE(host_addr);
    printf("RSX End.\n");
    deinitSPUSound();


	InputExit();
	//ftp_deinit();
    //printf("FTP stopped\n");
	//exit(0);

}

void CapApp::deinitSPUSound()
{
    StopAudio();
	printf("Audio stopped\n");
	SND_End_cpp();
	printf("Sound end\n");
	if (ftrack != NULL)
            fclose(ftrack);
    sysSpuRawDestroy(spu);
    printf("sysSpuRawDestroy\n");
    sysSpuImageClose(&spu_image);
    printf("sysSpuImageClose\n");
}

void CapApp::FontInit()
{
    //printf("STart font init\n");
    int ret1=0, ret2=0;

	if (state.displayMode.resolution == 2) {
        //printf("Display mode 2\n");
        ret2 = internal_load_font(&fontM, "/dev_hdd0/game/FBNE00123/USRDIR/b14.fnt");
        //printf("Loaded fontM\n");
        ret1 = internal_load_font(&fontL, "/dev_hdd0/game/FBNE00123/USRDIR/b24_b.fnt");
        //printf("Loaded fontL\n");
	}
	else {
        ret2 = internal_load_font(&fontM, "/dev_hdd0/game/FBNE00123/USRDIR/b12.fnt");
        //ret1 = internal_load_font(&fontL, "/dev_hdd0/game/FBNE00123/USRDIR/b24_b.fnt");
        ret1 = 0;
        fnt_load_mem(fnt35, &fontL);
	}
	if(ret1 || ret2) {
        printf("Error load fonts\n");
		msgDialogOpen2((msgType)((MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_BTN_OK)),"Error! Cant load Font file.",dialog_handler,NULL,NULL);
		dialog_action = 0;
		while(dialog_action == 0) {
			Flip();
			sysUtilCheckCallback();
		}

		//printf("Exit while with: %d\n", dialog_action);
		//msgDialogAbort();
		onShutdown();
	}
    //printf("END font init\n");
}



int CapApp::InitDB()
{
    //printf("LOAD ARRAY\n");
    int rc = 1;
    //int row = 0;
    sqlite3 *mdb;
    char buf[512];
    char cache_path[100];
    char *errmsg;
    //char sql[256];

    snprintf(cache_path, sizeof(cache_path),"/dev_hdd0/game/FBNE00123/USRDIR");



    rc = db_init_cpp(cache_path);
    if( rc != SQLITE_OK ) {
            printf("DB Init failed!\n");
            return 1;
    }

    snprintf(buf, sizeof(buf), "/dev_hdd0/game/FBNE00123/USRDIR/retro_local.db");
    if (!fileExist(buf)) {  //CREATE DB LOCAL
            rc = sqlite3_open_v2(buf, &mdb,
			   SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
		       NULL);
            if(mdb == NULL) {
                    printf("Open DB failed!\n");
                    return 1;
            }
            snprintf(buf, sizeof(buf), "CREATE TABLE games_favorite ( ord_game_id INTEGER PRIMARY KEY );");
            rc = sqlite3_exec(mdb, buf, NULL, NULL, &errmsg);
            if(rc) {
                            printf("CREATE TABLE games_favorite failed - %s: failed -- %s\n",
                              buf, errmsg);
                            sqlite3_free(errmsg);
                            sqlite3_close(mdb);
            }
            snprintf(buf, sizeof(buf), "CREATE INDEX ord_game_id_idx ON games_favorite (ord_game_id)");
            rc = sqlite3_exec(mdb, buf, NULL, NULL, &errmsg);
            if(rc) {
                            printf("CREATE INDEX ord_game_id_idx ON games_favorite failed - %s: failed -- %s\n",
                              buf, errmsg);
                            sqlite3_free(errmsg);
                            sqlite3_close(mdb);
            }


            rc = sqlite3_close(mdb);
            if (rc)
                    printf("CLOSE PROBLEM POST CREATE DB retro_local: %d\n",rc);

    }

    sqlite3_stmt *stmt;
    bool fav[app.nTotalBurnDrivers];
    for (unsigned int i=0;i<app.nTotalBurnDrivers;i++)
        fav[i] = false;
    snprintf(buf, sizeof(buf), "/dev_hdd0/game/FBNE00123/USRDIR/retro_local.db");

    //mdb = db_open_cpp(buf);
    rc = sqlite3_open_v2(buf, &mdb,
			   SQLITE_OPEN_READONLY,
		       NULL);
    if(mdb == NULL) {
            printf("Open DB failed!\n");
            return 1;
    }
    snprintf(buf, sizeof(buf), "SELECT ord_game_id COL00 FROM games_favorite");
    rc = sqlite3_prepare_v2(mdb, buf, -1, &stmt, 0);
    if( rc != SQLITE_OK ) {
            printf("Query failed!\n");
            sqlite3_finalize(stmt);
            sqlite3_close(mdb);
            return 1;
    }
    rc = sqlite3_step(stmt);
    int row = 0;
    while (rc == SQLITE_ROW) {
        row = sqlite3_column_int(stmt, COL00);
        fav[row] = true;
        rc = sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
    rc = sqlite3_close(mdb);

    if (rc)
        printf("CLOSE PROBLEM:END ParseGameListCache %d\n",rc);

    snprintf(buf, sizeof(buf), "/dev_hdd0/game/FBNE00123/USRDIR/retro.db");

    rc = sqlite3_open_v2(buf, &mdb,
			   SQLITE_OPEN_READONLY,
		       NULL);
    if(mdb == NULL) {
            printf("Open DB failed!\n");
            return 1;
    }

    snprintf(buf, sizeof(buf), "SELECT corename COL00 FROM cores order by id");
    rc = sqlite3_prepare_v2(mdb, buf, -1, &stmt, COL00);
    if( rc != SQLITE_OK ) {
                    printf("Query failed!\n");
                    return 1;
    }
    rc = sqlite3_step(stmt);
    int i=0;
    while (rc == SQLITE_ROW) {
            snprintf(cores[i],32, "%s", (char *)sqlite3_column_text(stmt, COL00));
            //printf("Core: %s\n", cores[i]);
            i++;
            rc = sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);

    snprintf(buf, sizeof(buf), "select g.id COL00, g.title COL01, g.order_id  COL02, ga.path COL03, "
                "s.sysmask COL04, g.cloneof COL05, g.aspectratio COL06, g.name COL07, "
                "g.publisher COL08, g.year COL09, g.system COL10, g.players COL11, g.resolution COL12, "
                "g.aspectratio COL13, g.def_core_id COL14, ga.coreid COL15, g.subsystem COL16 "
                "from games g inner join systems s on g.subsystem = s.name left join games_available ga on g.id = ga.game_id ");
                //"left join games_favorite fa on g.order_id = fa.ord_game_id order by order_id");
                //"order by title");

	rc = sqlite3_prepare_v2(mdb, buf, -1, &stmt, 0);
    if( rc != SQLITE_OK ) {
            printf("Query failed!\n");
            sqlite3_finalize(stmt);
            sqlite3_close(mdb);
            return 1;
    }
    rc = sqlite3_step(stmt);

	row = 0;
    nMissingGames = 0;

    int error = 0;
    while (rc == SQLITE_ROW) {
                fba_drv = (FBA_DRV*) malloc(sizeof(FBA_DRV));
				if (fba_drv == NULL) {
                    printf("Exit.... memory error malloc(sizeof(FBA_DRV)\n");
                    exit(0);
				}
				snprintf(fba_drv->szTitle, 256, "%s", (char *)sqlite3_column_text(stmt, COL01));
                snprintf(fba_drv->szName, 128, "%s", (char *)sqlite3_column_text(stmt, COL07));		// title
                snprintf(fba_drv->szSystemFilter, 32, "%s", (char *)sqlite3_column_text(stmt, COL04));	// sys filter
                snprintf(fba_drv->szCompany, 96, "%s", (char *)sqlite3_column_text(stmt, COL08) == NULL ? "" : (char *)sqlite3_column_text(stmt, COL08));	//
                snprintf(fba_drv->szYear, 5, "%s", (char *)sqlite3_column_text(stmt, COL09));	//
                snprintf(fba_drv->szSystem, 32, "%s", (char *)sqlite3_column_text(stmt, COL10));	//
                snprintf(fba_drv->szSubSystem, 32, "%s", (char *)sqlite3_column_text(stmt, COL16));	//
                snprintf(fba_drv->szResolution, 10, "%s", (char *)sqlite3_column_text(stmt, COL12));	//
                snprintf(fba_drv->szAspectRatio, 7, "%s", (char *)sqlite3_column_text(stmt, COL13));	//
                fba_drv->nMaxPlayers = sqlite3_column_int(stmt, COL11);
                fba_drv->nDefCoreID = sqlite3_column_int(stmt, COL14);
                fba_drv->nCoreID = sqlite3_column_int(stmt, COL15);
				fba_drv->nGameID = sqlite3_column_int(stmt, COL00);
                fba_drv->nGameOrderID = sqlite3_column_int(stmt, COL02) - 1;
//                if (sqlite3_column_text(stmt, COL16) != NULL)
//                                fba_drv->isFavorite = true;
//                else
                fba_drv->isFavorite = fav[fba_drv->nGameOrderID + 1];

                if (sqlite3_column_text(stmt, COL05) != NULL) { // is a clone?
                        fba_drv->isClone = true;
                        snprintf(fba_drv->szParent, 128, "%s", (char *)sqlite3_column_text(stmt, COL05));

                }
                else
                        fba_drv->isClone = false;

                if (sqlite3_column_text(stmt, COL03) != NULL) { // is available?
                        fba_games = (FBA_GAMES*) malloc(sizeof(FBA_GAMES));
                        if (fba_games == NULL) {
                                    printf("Exit.... memory error malloc(sizeof(FBA_GAMES)\n");
                                    exit(0);
                        }
                        snprintf(fba_games->szPath, 256, "%s", (char *)sqlite3_column_text(stmt, COL03));
                        snprintf(fba_games->key_string, KEY_MAX_LENGTH, "%s%s%d",
                                    (char *)sqlite3_column_text(stmt, COL10), (char *)sqlite3_column_text(stmt, COL07), sqlite3_column_int(stmt, COL15));
                        error = hashmap_put(gamesMap, fba_games->key_string, fba_games);
                        //                        if (error !=MAP_OK) {
//                            printf("Error: %d - %s\n", error, fba_games->szPath);
//                        }
                        fba_drv->isAvailable = true;

                }
                else {
                        fba_drv->isAvailable = false;
                        nMissingGames++;
                }
                snprintf(fba_drv->key_string, KEY_MAX_LENGTH, "%s%s", (char *)sqlite3_column_text(stmt, COL10), (char *)sqlite3_column_text(stmt, COL07));

                error = hashmap_put(drvMap, fba_drv->key_string, fba_drv);
//				if (error !=MAP_OK) {
//                    printf("Error: %d - %s\n", error, fba_drv->szName);
//				}
				row++;

                rc = sqlite3_step(stmt);

    }
	sqlite3_finalize(stmt);

    rc = sqlite3_close(mdb);


    if (rc)
        printf("CLOSE PROBLEM:load cores %d\n",rc);


    //printf("fba_games->size %d\n", ((hashmap_map *)gamesMap)->size);

    //printf("Missing games: %d\n", nMissingGames);
return 0;
}

