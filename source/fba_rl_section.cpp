// =======================================================================
// DISPLAY SECTIONS (FRAME)

#include "main.h"
#include "fnt_print.h"
#include <rsx/rsx.h>
#include <ppu-lv2.h>
#include <lv2/memory.h>

#define COLOR_WHITE		0xFFFFFF
#define COLOR_BLACK		0x000000
#define COLOR_BLUE		0x0000FF
#define COLOR_GREEN		0x00FF00
#define COLOR_RED		0xFF0000
#define COLOR_YELLOW	0xFFFF00
#define COLOR_CYAN		0X00FFFF
#define COLOR_GREY		0x808080
#define COLOR_ORANGE	0xFF4500
#define NSHADOWCOLOR	0xFF050505
#define NSHADOWXYPOS2   0.00150f

#define NGAMELISTMAX    33
#define DRVMAP          ((hashmap_map *) app.drvMap)
#define GAMESMAP        ((hashmap_map *) app.gamesMap)
#define PNG_SRC         (app.textures[TEX_GAME_LIST]->png)
#define FBADRV          ((FBA_DRV *)DRVMAP->data[hashmap_position].data)
#define FBAGAMES        ((FBA_GAMES *)GAMESMAP->data[hashmap_position].data)


extern fnt_t fontM,fontL;

//static float nShadowXYpos		= 0.00250f;
//static float nShadowXYpos2		= 0.00200f;
//static uint32_t nShadowColor	= COLOR_BLACK;
static uint32_t nTextColor		= COLOR_WHITE;
static uint32_t nSelectColor	= COLOR_YELLOW;
//static uint32_t nMissingColor	= COLOR_RED;
//static float nBigSize		= 1.0500f;
//static float nSmallSize		= 0.5500f;
//static float nSmallSize2	= 0.6500f;
//static float nSmallSizeSel	= 0.6000f;
static uint32_t nColor;
static uint32_t game_ord_id;
static float xPos;
static float yPos;
static float yPosDiff;
static uint32_t fontSize;
static int error_x, error_y;
static u16 X,Y;

struct smeminfo {
            uint32_t total;
            uint32_t avail;
} meminfo;
static char txt[256];
extern char ipaddress[32];

void c_fbaRL::DisplayFrame()
{
	if(nSection == SECTION_FILEBROWSER)	{
		FileBrowser_Frame();
	}
	if(nSection == SECTION_OPTIONS && !bProcessingGames) {
		Options_Frame();
	}
	if(nSection == SECTION_GAMELIST && !bProcessingGames) {
		GameList_Frame();
	}
	if(nSection == SECTION_ZIPINFO)	{
		ZipInfo_Frame();
	}
	if(nSection == SECTION_ROMINFO)	{
		RomInfo_Frame();
	}
	if(nSection == SECTION_MAIN) {
		MainMenu_Frame();
	}
}

void c_fbaRL::MainMenu_Frame()
{
	xPos		= 0.0600f;
	yPos		= 0.0485f;
	yPosDiff	= 0.0400f;
	//float nfontLize = 1.0500f; // big text
	nColor = nTextColor;
	pngData* png_src;

    if (app.state.displayMode.resolution == 1)
        fontSize = 2;
    else
        fontSize = 1;
    //rsxBuffer *buf;
    rsxbuffer = &(app.buffers[app.currentBuffer]);
    //rsxBuffer *buffer = &buffers[currentBuffer];
	png_src = app.textures[TEX_MAIN_MENU]->png;
	memcpy(rsxbuffer->ptr, png_src->bmp_out, png_src->width * png_src->height * sizeof(uint32_t));
	////if(nFrameStep == 0) { yPos += nShadowXYpos2; xPos += nShadowXYpos2; }
	////if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color



	snprintf(txt,sizeof(txt),"ver: %s (ip: %s)", _APP_VER, ipaddress);
	////cellDbgFontPrintf(xPos + 0.0300f + (0.0600f * 2), yPos + 0.0100f + (yPosDiff * 2), nSmallSize, nColor, "ver: "_APP_VER" (ip: %s)", ipaddress);
    fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)((xPos + 0.0200f + (0.0600f * 2)) * rsxbuffer->width),
                   (int)((yPos + 0.0100f + (yPosDiff * 2)) * rsxbuffer->height),
                    txt, nColor, 0x00000000, fontSize, fontSize);
	//if(nFrameStep == 0) { yPos -= nShadowXYpos2; xPos -= nShadowXYpos2; } // reset shadow xy pos
	//if(nFrameStep == 0) { yPos += nShadowXYpos; xPos += nShadowXYpos; }

	yPos += (yPosDiff * 4) - 0.005f;
	yPosDiff = 0.0800f;

	////int nMenuItem = main_menu->UpdateTopItem();
	int nMenuItem = 0;

	////while(nMenuItem <= (main_menu->nTopItem + main_menu->nListMax))
	while(nMenuItem < main_menu->nTotalItem)
	{
		////if(nMenuItem == main_menu->nTotalItem) break;

		// normal
		nColor = nTextColor;

		// selected
		if(nMenuItem == main_menu->nSelectedItem) {
			nColor = 0xff00ccff;
		}

		////if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color

		////cellDbgFontPrintf(xPos, yPos, nfontLize, nColor, "%s", main_menu->item[nMenuItem]->szMenuLabel);

		snprintf(txt,sizeof(txt),"%s", ipaddress);
		fnt_print_vram(&fontL, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(xPos * rsxbuffer->width),
                   (int)(yPos * rsxbuffer->height),
                    main_menu->item[nMenuItem]->szMenuLabel, nColor, 0x00000000, 1, 1);
		/*fnt_print_vram(&font, (u32 *)app.textures[TEX_MAIN_MENU]->_texture.pixels,
					app.textures[TEX_MAIN_MENU]->_texture.width,
					(int)(xPos * app.textures[TEX_MAIN_MENU]->_texture.width),
					(int)(yPos * app.textures[TEX_MAIN_MENU]->_texture.height),
					main_menu->item[nMenuItem]->szMenuLabel, nColor, 0x00000000, 1, 1);		*/
		yPos += yPosDiff;

		nMenuItem++;
	}
}

void c_fbaRL::GameList_Frame()
{


	xPos		= 0.3273f;
	yPos		= 0.2042f;
	yPosDiff	= 0.0200f;

    if (app.state.displayMode.resolution == 1)
        fontSize = 2;
    else
        fontSize = 1;

	//float nfontLize = nSmallSize; // small text

	if(nFrameStep == 0) { yPos += NSHADOWXYPOS2; xPos += NSHADOWXYPOS2; }

	//yPos += (yPosDiff * 4);
	if (nFrameStep == 0) {
        rsxbuffer = &(app.buffers[app.currentBuffer]);
        //rsxBuffer *buffer = &buffers[currentBuffer];
        //png_src = app.textures[TEX_GAME_LIST]->png;
        memcpy(rsxbuffer->ptr, PNG_SRC->bmp_out, PNG_SRC->width * PNG_SRC->height * sizeof(uint32_t));
	}
    game_ord_id = 0;

	nColor = nTextColor;


    if(nFilteredGames >= 1)
	{
		game_ord_id = fgames[nSelectedGame]->GameID;
		if(nSelectedGame > NGAMELISTMAX || nGameListTop > 0)
		{
			if(nGameListTop < (nSelectedGame - NGAMELISTMAX)){
				nGameListTop = nSelectedGame - NGAMELISTMAX;
			}
			if((nGameListTop > 0) && (nSelectedGame < nGameListTop)) {
				nGameListTop = nSelectedGame;
			} else {
				nGameListTop = nSelectedGame - NGAMELISTMAX;
			}
		} else {
			nGameListTop = nSelectedGame - NGAMELISTMAX;
		}
		if(nGameListTop < 0) nGameListTop = 0;

		int nGame = nGameListTop;

		while(nGame <= (nGameListTop + NGAMELISTMAX))
		{
			if(nFilteredGames < 1) break;
			if(nGame == nFilteredGames) break;

			nColor = nTextColor;
			//nfontLize = nSmallSizeSel; // small text (selected)

			// missing
			if(!fgames[nGame]->bAvailable) {
				nColor = 0xffff0000;
			}

			if(fgames[nGame]->isFavorite) {
				nColor = 0xff00ccff;
			}

			// selected
			if(nGame == nSelectedGame)
			{
				nColor = 0xffffcc00;
				//nfontLize = nSmallSize2;
			}

			if(nFrameStep == 0) { nColor = NSHADOWCOLOR; } // Shadow color

            hashmap_position = games[fgames[nGame]->GameID]->nSize;
			//fba_drv = (FBA_DRV *)DRVMAP->data[hashmap_position].data;
            //memcpy(pszFinalText, games[fgames[nGame]->GameID]->title, 104);
            snprintf(txt, 128, "%s", FBADRV->szTitle );
            //memcpy(pszFinalText, fba_drv->szTitle, 104);
			fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr,
					rsxbuffer->width,
					(int)(xPos * rsxbuffer->width),
					(int)(yPos * rsxbuffer->height),
						txt, nColor, 0x00000000, fontSize, fontSize);

			yPos += yPosDiff;

			nGame++;
		}


	}

	X = round(19.0f * app.width /1920.0f + (551.0f * app.width /1920.0f - app.textures[TEX_PREVIEW]->png->width)/2.0f);
	Y = round(404.0f * app.height/1080.0f + (460.0f * app.height/1080.0f - app.textures[TEX_PREVIEW]->png->height)/2.0f);
	//printf("Draw to: %d:%d\n",X,Y);
	fbaRL->DrawIMG(X,Y, app.textures[TEX_PREVIEW]->png);

	X = round(19.0f * app.width /1920.0f + (551.0f * app.width /1920.0f - app.textures[TEX_PREVIEW]->pngSec->width)/2.0f);
	Y = round(19.0f * app.height/1080.0f + (360.0f * app.height/1080.0f - app.textures[TEX_PREVIEW]->pngSec->height)/2.0f);
    //printf("Draw to: %d:%d\n",X,Y);
	fbaRL->DrawIMG(X,Y, app.textures[TEX_PREVIEW]->pngSec);


	nColor = nTextColor;
	if(nFrameStep == 0) { nColor = NSHADOWCOLOR; } // Shadow color

	xPos = 0.3050f;
	yPos = 0.9190f;
	//if(nFrameStep == 0) { yPos += nShadowXYpos2; xPos +=nShadowXYpos2; }

	snprintf(txt,sizeof(txt),"GAMES AVAILABLE: %d - MISSING: %d",
                nTotalGames,
                app.nMissingGames);
    fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr,
					rsxbuffer->width,
					(int)(xPos * rsxbuffer->width),
					(int)(yPos * rsxbuffer->height),
						txt, nColor, 0x00000000, fontSize, fontSize);

    yPos += yPosDiff;
 	snprintf(txt,sizeof(txt),"FILTER: %s - FILTERED: %d",
                GetSystemFilter(g_opt_nActiveSysFilter),
                nFilteredGames);
    fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr,
					rsxbuffer->width,
					(int)(xPos * rsxbuffer->width),
					(int)(yPos * rsxbuffer->height),
						txt, nColor, 0x00000000, fontSize, fontSize);

    yPos += yPosDiff;
    if (app.ftrack != NULL) {
        snprintf(txt,sizeof(txt),"TRACK: %s", (*app.trackID).c_str());
        fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr,
					rsxbuffer->width,
					(int)(xPos * rsxbuffer->width),
					(int)(yPos * rsxbuffer->height),
						txt, nColor, 0x00000000, fontSize, fontSize);
    }


	xPos = 0.5422f;
	yPos = 0.0361f;
	yPosDiff	= 0.0200f;



	if(nFilteredGames >= 1) //if(nBurnSelected >= 0)
	{

		hashmap_position = games[game_ord_id]->nSize;

		snprintf(txt,sizeof(txt)," TITLE:       %s", FBADRV->szTitle);

		fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr,
					rsxbuffer->width,
					(int)(xPos * rsxbuffer->width),
					(int)(yPos * rsxbuffer->height),
						txt, nColor, 0x00000000, fontSize, fontSize);
		yPos += yPosDiff;

		snprintf(txt,sizeof(txt)," ROMSET:      %s  -  PARENT:      %s",
                    games[game_ord_id]->name,games[nBurnSelected]->parent_name);
		fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr,
					rsxbuffer->width,
					(int)(xPos * rsxbuffer->width),
					(int)(yPos * rsxbuffer->height),
						txt, nColor, 0x00000000, fontSize, fontSize);

		yPos += yPosDiff;


		snprintf(txt,sizeof(txt)," COMPANY:     %s", games[game_ord_id]->company);
		fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr,
					rsxbuffer->width,
					(int)(xPos * rsxbuffer->width),
					(int)(yPos * rsxbuffer->height),
						txt, nColor, 0x00000000, fontSize, fontSize);
		yPos += yPosDiff;

		snprintf(txt,sizeof(txt)," YEAR:        %s  -  SYSTEM:      %s",
           games[game_ord_id]->year, games[game_ord_id]->subsystem);
		fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr,
					rsxbuffer->width,
					(int)(xPos * rsxbuffer->width),
					(int)(yPos * rsxbuffer->height),
						txt, nColor, 0x00000000, fontSize, fontSize);
		yPos += yPosDiff;

		snprintf(txt,sizeof(txt)," MAX PLAYERS: %d  -  RESOLUTION:  %s (%s)",
                    games[game_ord_id]->players,games[nBurnSelected]->resolution,games[game_ord_id]->aspectratio);
		fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr,
					rsxbuffer->width,
					(int)(xPos * rsxbuffer->width),
					(int)(yPos * rsxbuffer->height),
						txt, nColor, 0x00000000, fontSize, fontSize);
		yPos += yPosDiff;


		if (games[game_ord_id]->bAvailable) {
                hashmap_position = fgames[nSelectedGame]->nSize;


                snprintf(txt,sizeof(txt)," PATH: %s",FBAGAMES->szPath);
                fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr,
                            rsxbuffer->width,
                            (int)(xPos * rsxbuffer->width),
                            (int)(yPos * rsxbuffer->height),
                            txt, nColor, 0x00000000, fontSize, fontSize);

        }
        yPos += yPosDiff;

        if (app.state.displayMode.resolution == 1)
            xPos = xPos + 0.22;
        else
            xPos = xPos + 0.25;
        lv2syscall1(352, (uint64_t) &meminfo);
        snprintf(txt,sizeof(txt),"MEMORY TOTAL: %d - AVAIL: %d", meminfo.total / 1024, meminfo.avail / 1024);
        fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr,
					rsxbuffer->width,
					(int)(xPos  * rsxbuffer->width),
					(int)((yPos + 0.012) * rsxbuffer->height),
                    txt, nColor, 0x00000000, fontSize, fontSize);
		yPos += yPosDiff;
	}


}



void c_fbaRL::DrawIMG(int x, int y, pngData *png1){
    //rsxBuffer *buf;

	//buf = &(app.buffers[app.currentBuffer]);
	if(png1->bmp_out){
		u32 *scr = (u32 *)rsxbuffer->ptr;
		u32 *png = (u32 *)png1->bmp_out;


		if(x<0){
			png = png-x;
			error_x=-x;
			x=0;
		}else
			error_x=0;

		if(y<0){
			error_y=-y;
			png = png + y*png1->width;
			y=0;
		}else
			error_y=0;

		scr += y*rsxbuffer->width+x;
		int height = png1->height;
		if((y+height) >= (u32)rsxbuffer->height)
			height = rsxbuffer->height-1-y;

		int width = png1->width;
		if((x+width) >= (u32)rsxbuffer->width)
			width = rsxbuffer->width-x-1;

		width-=error_x;
		height-=error_y;

		if(width>0)
        while(height>0){
			memcpy(scr,png,width*sizeof(u32));
			png+=png1->pitch>>2;
			scr+=rsxbuffer->width;
			height--;
		}
	}
}

void c_fbaRL::Options_Frame()
{
	xPos		= 0.0600f;
	yPos		= 0.0460f;
	yPosDiff	= 0.0410f;
	//float nfontLize = nBigSize; // big text
	//uint32_t nColor = nTextColor;
    pngData*    png_src;
    rsxbuffer = &(app.buffers[app.currentBuffer]);

    if (app.state.displayMode.resolution == 1)
        fontSize = 2;
    else
        fontSize = 1;
    //rsxBuffer *buffer = &buffers[currentBuffer];
	png_src = app.textures[TEX_OPTIONS]->png;
	memcpy(rsxbuffer->ptr, png_src->bmp_out, png_src->width * png_src->height * sizeof(uint32_t));


	//snprintf(txt,sizeof(txt),"ver: %s (ip: %s)", _APP_VER, ipaddress);

	////if(nFrameStep == 0) { yPos += nShadowXYpos; xPos +=nShadowXYpos; }

	yPos += (yPosDiff * 4);
	yPosDiff = 0.0800f;

	int nMenuItem = options_menu->UpdateTopItem();

	while(nMenuItem <= (options_menu->nTopItem + options_menu->nListMax))
	{
		if(nMenuItem == options_menu->nTotalItem) break;

		// normal
		nColor	= nTextColor;

		////if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color

		// selected
		if(nMenuItem == options_menu->nSelectedItem)
		{
			float x = 0.1085f;
			float y = 0.8900f;
			//float size = nSmallSize2; // small text 2

			if(nMenuItem == MENU_OPT_AUTO_CFG) {
				snprintf(txt,sizeof(txt),"Auto create basic Input Preset CFG files for all systems.");
                fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(x * rsxbuffer->width),
                           (int)(y * rsxbuffer->height),
                            txt, nColor, 0x00000000, fontSize, fontSize);
			}

			if(nMenuItem == MENU_OPT_MUSIC) {
				snprintf(txt,sizeof(txt),"Enable / Disable background MP3 music.");
                fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(x * rsxbuffer->width),
                           (int)(y * rsxbuffer->height),
                            txt, nColor, 0x00000000, fontSize, fontSize);
			}

			if(nMenuItem == MENU_OPT_RETROARCH_MENU) {
				snprintf(txt,sizeof(txt),"Change Retro Arch menu driver.");
                fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(x * rsxbuffer->width),
                           (int)(y * rsxbuffer->height),
                            txt, nColor, 0x00000000, fontSize, fontSize);
			}

			if(nMenuItem == MENU_OPT_DISP_CLONES) {
				snprintf(txt,sizeof(txt),"Enable / Disable display of clone games.");
                fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(x * rsxbuffer->width),
                           (int)(y * rsxbuffer->height),
                            txt, nColor, 0x00000000, fontSize, fontSize);
			}

			if(nMenuItem == MENU_OPT_USE_UNIBIOS) {
				snprintf(txt,sizeof(txt),"Use UNI-BIOS when playing Neo-Geo games.");
                fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(x * rsxbuffer->width),
                           (int)(y * rsxbuffer->height),
                            txt, nColor, 0x00000000, fontSize, fontSize);
			}

			if(nMenuItem == MENU_OPT_DISP_MISS_GMS) {
				snprintf(txt,sizeof(txt),"Enable / Disable display of missing games.");
                fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(x * rsxbuffer->width),
                           (int)(y * rsxbuffer->height),
                            txt, nColor, 0x00000000, fontSize, fontSize);
			}

			if (nMenuItem == MENU_OPT_MD_DEF_CORE) {
				snprintf(txt, sizeof(txt), "Select default core for MegaDrive games.");
				fnt_print_vram(&fontM, (u32*)rsxbuffer->ptr, rsxbuffer->width, (int)(x * rsxbuffer->width),
					(int)(y * rsxbuffer->height),
					txt, nColor, 0x00000000, fontSize, fontSize);
			}

			if(nMenuItem >= MENU_OPT_FILTER_START && nMenuItem <=  MASKFAVORITE+MENU_OPT_FILTER_START) {
				snprintf(txt,sizeof(txt),"Choose the emulated system(s) you want to be displayed / filtered.");
                fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(x * rsxbuffer->width),
                           (int)(y * rsxbuffer->height),
                            txt, nColor, 0x00000000, fontSize, fontSize);
			}

			// Rom Paths (directories)
			if(nMenuItem > MASKFAVORITE+MENU_OPT_FILTER_START && nMenuItem <= MASKFAVORITE+MENU_OPT_FILTER_START+NDIRPATH) {
				int nRomPath = nMenuItem-(MASKFAVORITE+MENU_OPT_FILTER_START+1);
				snprintf(txt,sizeof(txt),"Current: %s", g_opt_szROMPaths[nRomPath]);
                fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(x * rsxbuffer->width),
                           (int)(y * rsxbuffer->height),
                            txt, nColor, 0x00000000, fontSize, fontSize);
			}

			// Input Preset Paths (CFG)
			if(nMenuItem > MASKFAVORITE+MENU_OPT_FILTER_START+12 && nMenuItem <= MASKFAVORITE+MENU_OPT_FILTER_START+NDIRPATH+MASKFAVORITE) {
				int nCfgPath = nMenuItem-(MASKFAVORITE+MENU_OPT_FILTER_START+12+1);
				snprintf(txt,sizeof(txt),"Current: %s", g_opt_szInputCFG[nCfgPath]);
                fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(x * rsxbuffer->width),
                           (int)(y * rsxbuffer->height),
                            txt, nColor, 0x00000000, fontSize, fontSize);
			}

			nColor = nSelectColor;

			////if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color
		}

		if(nMenuItem == MENU_OPT_AUTO_CFG) {
                snprintf(txt,sizeof(txt),"%s: [%s]", options_menu->item[nMenuItem]->szMenuLabel,
                    g_opt_bAutoInputCfgCreate ? "ON" : "OFF");
                fnt_print_vram(&fontL, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(xPos * rsxbuffer->width),
                           (int)(yPos * rsxbuffer->height),
                            txt, nColor, 0x00000000, 1, 1);
		}

		if(nMenuItem == MENU_OPT_MUSIC) {
                snprintf(txt,sizeof(txt),"%s: [%s]", options_menu->item[nMenuItem]->szMenuLabel,
                    //g_opt_bUseAltMenuKeyCombo ? "ON" : "OFF");
                    g_opt_bMusic ? "ON" : "OFF");
                fnt_print_vram(&fontL, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(xPos * rsxbuffer->width),
                           (int)(yPos * rsxbuffer->height),
                            txt, nColor, 0x00000000, 1, 1);
		}

		if(nMenuItem == MENU_OPT_RETROARCH_MENU) {
                snprintf(txt,sizeof(txt),"%s: [%s]", options_menu->item[nMenuItem]->szMenuLabel,
                    g_opt_sRetroArchMenu[g_opt_nRetroArchMenu]);
                fnt_print_vram(&fontL, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(xPos * rsxbuffer->width),
                           (int)(yPos * rsxbuffer->height),
                            txt, nColor, 0x00000000, 1, 1);
		}

		if(nMenuItem == MENU_OPT_DISP_CLONES) { //CRYSTAL
                snprintf(txt,sizeof(txt),"%s: [%s]", options_menu->item[nMenuItem]->szMenuLabel,
                    g_opt_bDisplayCloneGames ? "ON" : "OFF");
                fnt_print_vram(&fontL, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(xPos * rsxbuffer->width),
                           (int)(yPos * rsxbuffer->height),
                            txt, nColor, 0x00000000, 1, 1);
		}

		if(nMenuItem == MENU_OPT_USE_UNIBIOS) {
                snprintf(txt,sizeof(txt),"%s: [%s]", options_menu->item[nMenuItem]->szMenuLabel,
                    g_opt_bUseUNIBIOS ? "ON" : "OFF");
                fnt_print_vram(&fontL, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(xPos * rsxbuffer->width),
                           (int)(yPos * rsxbuffer->height),
                            txt, nColor, 0x00000000, 1, 1);
		}

		if(nMenuItem == MENU_OPT_DISP_MISS_GMS) {
                snprintf(txt,sizeof(txt),"%s: [%s]", options_menu->item[nMenuItem]->szMenuLabel,
                    g_opt_bDisplayMissingGames ? "ON" : "OFF");
                fnt_print_vram(&fontL, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(xPos * rsxbuffer->width),
                           (int)(yPos * rsxbuffer->height),
                            txt, nColor, 0x00000000, 1, 1);
		}

		if (nMenuItem == MENU_OPT_MD_DEF_CORE) {
			snprintf(txt, sizeof(txt), "%s: [%s]", options_menu->item[nMenuItem]->szMenuLabel,
				g_opt_sMegaDriveCores[g_opt_nMegaDriveDefaultCore]);
			fnt_print_vram(&fontL, (u32*)rsxbuffer->ptr, rsxbuffer->width, (int)(xPos * rsxbuffer->width),
				(int)(yPos * rsxbuffer->height),
				txt, nColor, 0x00000000, 1, 1);
		}

		// Custom System Filters
		if(nMenuItem >= MENU_OPT_FILTER_START && nMenuItem <=  MASKFAVORITE+MENU_OPT_FILTER_START)
		{
			int nSysFilter = nMenuItem-MENU_OPT_FILTER_START;

                snprintf(txt,sizeof(txt),"%s: [%s]", options_menu->item[nMenuItem]->szMenuLabel,
                    g_opt_bCustomSysFilter[nSysFilter] ? "ON" : "OFF");
                fnt_print_vram(&fontL, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(xPos * rsxbuffer->width),
                           (int)(yPos * rsxbuffer->height),
                            txt, nColor, 0x00000000, 1, 1);
//			cellDbgFontPrintf(xPos, yPos, nfontLize, nColor, "%s: [%s]",
//				options_menu->item[nMenuItem]->szMenuLabel,
//				g_opt_bCustomSysFilter[nSysFilter] ? "ON" : "OFF"
//			);
		}

		// Rom Paths (directories)
		if(nMenuItem > MASKFAVORITE+MENU_OPT_FILTER_START && nMenuItem <= MASKFAVORITE+MENU_OPT_FILTER_START+NDIRPATH)
		{
                snprintf(txt,sizeof(txt),"%s: [...]", options_menu->item[nMenuItem]->szMenuLabel);
                fnt_print_vram(&fontL, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(xPos * rsxbuffer->width),
                           (int)(yPos * rsxbuffer->height),
                            txt, nColor, 0x00000000, 1, 1);
//			cellDbgFontPrintf(xPos, yPos, nfontLize, nColor, "%s: [...]", options_menu->item[nMenuItem]->szMenuLabel);
		}

		// Input Preset Paths (CFG)
		if(nMenuItem > MASKFAVORITE+MENU_OPT_FILTER_START+12 && nMenuItem <= MASKFAVORITE+MENU_OPT_FILTER_START+NDIRPATH+MASKFAVORITE)
		{
                snprintf(txt,sizeof(txt),"%s: [...]", options_menu->item[nMenuItem]->szMenuLabel);
                fnt_print_vram(&fontL, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(xPos * rsxbuffer->width),
                           (int)(yPos * rsxbuffer->height),
                            txt, nColor, 0x00000000, 1, 1);
//			cellDbgFontPrintf(xPos, yPos, nfontLize, nColor, "%s: [...]", options_menu->item[nMenuItem]->szMenuLabel);
		}

		yPos += yPosDiff;

		nMenuItem++;
	}
}


void c_fbaRL::FileBrowser_Frame()
{
	xPos		= 0.0750f;
	yPos		= 0.0500f;
	yPosDiff	= 0.0265f;
	//float nfontLize = nSmallSize2; // small text
    rsxbuffer = &(app.buffers[app.currentBuffer]);
    pngData*    png_src;

    if (app.state.displayMode.resolution == 1)
        fontSize = 2;
    else
        fontSize = 1;
    //rsxBuffer *buffer = &buffers[currentBuffer];
	png_src = app.textures[TEX_FILEBROWSER]->png;
	memcpy(rsxbuffer->ptr, png_src->bmp_out, png_src->width * png_src->height * sizeof(uint32_t));


	//if(nFrameStep == 0) { yPos += nShadowXYpos2; xPos +=nShadowXYpos2; }

	yPos = 0.1250L;

	float yHeadPos = 0.8900f;
	////if(nFrameStep == 0) { yHeadPos += nShadowXYpos2; }

	nColor	= nTextColor;
	////if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color

	////cellDbgFontPrintf(xPos, yHeadPos, nfontLize, nColor, "CURRENT DIR: %s", filebrowser->pszCurrentDir);
	snprintf(txt,sizeof(txt),"CURRENT DIR: %s", filebrowser->pszCurrentDir);
    fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(xPos * rsxbuffer->width),
                           (int)(yHeadPos * rsxbuffer->height),
                            txt, nColor, 0x00000000, fontSize, fontSize);
	yPos += yPosDiff;

	if(filebrowser->nTotalItem >= 1)
	{
		yPos += yPosDiff;

		int nMenuItem = filebrowser->UpdateTopItem();

		while(nMenuItem <= (filebrowser->nTopItem + filebrowser->nListMax))
		{
			if(nMenuItem == filebrowser->nTotalItem) break;

			// normal
			nColor = nTextColor;

			// selected
			if(nMenuItem == filebrowser->nSelectedItem) {
				nColor = nSelectColor;
			}

			//if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color

			////cellDbgFontPrintf(xPos, yPos, nfontLize, nColor, "%s", filebrowser->item[nMenuItem]->szMenuLabel);
            snprintf(txt,sizeof(txt),"%s", filebrowser->item[nMenuItem]->szMenuLabel);
            fnt_print_vram(&fontL, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(xPos * rsxbuffer->width),
                           (int)(yPos * rsxbuffer->height),
                            txt, nColor, 0x00000000, 1, 1);
			yPos += yPosDiff;

			nMenuItem++;
		}
	}
}


void c_fbaRL::ZipInfo_Frame()
{
	xPos		= 0.0400f;
	yPos		= 0.0440f;
	yPosDiff	= 0.0200f;
	//float nfontLize = nSmallSize; // small text
    rsxbuffer = &(app.buffers[app.currentBuffer]);
    pngData*    png_src;

    if (app.state.displayMode.resolution == 1)
        fontSize = 2;
    else
        fontSize = 1;
    //rsxBuffer *buffer = &buffers[currentBuffer];
	png_src = app.textures[TEX_ZIP_INFO]->png;
	memcpy(rsxbuffer->ptr, png_src->bmp_out, png_src->width * png_src->height * sizeof(uint32_t));


	//if(nFrameStep == 0) { yPos += nShadowXYpos2; xPos +=nShadowXYpos2; }

	// normal
	nColor	= nTextColor;

	//if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color

	if(nFilteredGames >= 1)
	{
		char* pszZipName = NULL;
		pszZipName = (char*)malloc(33);
		memset(pszZipName, 0, 33);
		memcpy(pszZipName, fgames[nSelectedGame]->zipname, 32);

		snprintf(txt,sizeof(txt),"ZIP: %s / TOTAL FILES FOUND: %d", pszZipName, zipinfo_menu->nTotalItem);
        fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(xPos * rsxbuffer->width),
                           (int)(yPos * rsxbuffer->height),
                            txt, nColor, 0x00000000, fontSize, fontSize);
		//cellDbgFontPrintf(xPos, yPos, nfontLize, nColor, "ZIP: %s / TOTAL FILES FOUND: %d", pszZipName, zipinfo_menu->nTotalItem);
		yPos += yPosDiff;

		SAFE_FREE(pszZipName)

		xPos = 0.0650f;
		//if(nFrameStep == 0) { xPos +=nShadowXYpos2; }

		yPos += (yPosDiff * 3);

		int nMenuItem = zipinfo_menu->UpdateTopItem();

		while(nMenuItem <= (zipinfo_menu->nTopItem + zipinfo_menu->nListMax))
		{
			if(nMenuItem == zipinfo_menu->nTotalItem) break;

			// normal
			nColor = nTextColor;

			// selected
			if(nMenuItem == zipinfo_menu->nSelectedItem) {
				nColor = nSelectColor;
			}

			//if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color

            snprintf(txt,sizeof(txt),"%s", zipinfo_menu->item[nMenuItem]->szMenuLabel);
            fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(xPos * rsxbuffer->width),
                           (int)(yPos * rsxbuffer->height),
                            txt, nColor, 0x00000000, fontSize, fontSize);
			//cellDbgFontPrintf(xPos, yPos, nfontLize, nColor, "[%d] %s", nMenuItem+1, zipinfo_menu->item[nMenuItem]->szMenuLabel);
			yPos += yPosDiff;

			nMenuItem++;
		}
	}
}

void c_fbaRL::RomInfo_Frame()
{
	xPos		= 0.0400f;
	yPos		= 0.0440f;
	yPosDiff	= 0.0200f;
	//float nfontLize = nSmallSize; // small text
    rsxbuffer = &(app.buffers[app.currentBuffer]);
    pngData*    png_src;

    if (app.state.displayMode.resolution == 1)
        fontSize = 2;
    else
        fontSize = 1;
    //rsxBuffer *buffer = &buffers[currentBuffer];
	png_src = app.textures[TEX_ROM_INFO]->png;
	memcpy(rsxbuffer->ptr, png_src->bmp_out, png_src->width * png_src->height * sizeof(uint32_t));
    u16 X,Y;
    X = round(1269.0f * app.width /1920.0f + (551.0f * app.width /1920.0f - app.textures[TEX_PREVIEW]->pngSec->width)/2.0f);
	Y = round(532.0f * app.height/1080.0f + (360.0f * app.height/1080.0f - app.textures[TEX_PREVIEW]->pngSec->height)/2.0f);
//	printf("Draw to: %d:%d\n",X,Y);
	fbaRL->DrawIMG(X,Y, app.textures[TEX_PREVIEW]->pngSec);

	//if(nFrameStep == 0) { yPos += nShadowXYpos2; xPos +=nShadowXYpos2; }

	// normal
	nColor	= nTextColor;

	//if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color

	if(nFilteredGames >= 1)
	{
		char* pszZipName = NULL;
		pszZipName = (char*)malloc(33);
		memset(pszZipName, 0, 33);
		memcpy(pszZipName, fgames[nSelectedGame]->zipname, 32);

		snprintf(txt,sizeof(txt),"ROMSET: %s ", pszZipName);
        fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(xPos * rsxbuffer->width),
                           (int)(yPos * rsxbuffer->height),
                            txt, nColor, 0x00000000, fontSize, fontSize);
		//cellDbgFontPrintf(xPos, yPos, nfontLize, nColor, "ROMSET: %s / TOTAL ROMS: %d", pszZipName, rominfo_menu->nTotalItem);
		yPos += yPosDiff;

		SAFE_FREE(pszZipName)

		xPos = 0.0650f;
		//if(nFrameStep == 0) { xPos +=nShadowXYpos2; }

		yPos += (yPosDiff * 3);

		int nMenuItem = rominfo_menu->UpdateTopItem();

		while(nMenuItem <= (rominfo_menu->nTopItem + rominfo_menu->nListMax))
		{
			if(nMenuItem == rominfo_menu->nTotalItem) break;

			// normal
			nColor = nTextColor;

			// selected
			if(nMenuItem == rominfo_menu->nSelectedItem) {
				nColor = nSelectColor;
			}

			//if(nFrameStep == 0) { nColor = nShadowColor; } // Shadow color

			snprintf(txt,sizeof(txt),"%s", rominfo_menu->item[nMenuItem]->szMenuLabel);
            fnt_print_vram(&fontM, (u32 *)rsxbuffer->ptr, rsxbuffer->width, (int)(xPos * rsxbuffer->width),
                           (int)(yPos * rsxbuffer->height),
                            txt, nColor, 0x00000000, fontSize, fontSize);

			//cellDbgFontPrintf(xPos, yPos, nfontLize, nColor, "[%d] %s", nMenuItem+1, rominfo_menu->item[nMenuItem]->szMenuLabel);
			yPos += yPosDiff;

			nMenuItem++;
		}
	}
}
