#include "main.h"
#include "MAIN_MENU_1920x1080_PNG_bin.h"
#include "OPTIONS_MENU_1920x1080_PNG_bin.h"
#include "GAME_LIST_1920x1080_PNG_bin.h"
#include "ROM_INFO_1920x1080_PNG_bin.h"
#include "ZIP_INFO_1920x1080_PNG_bin.h"
#include "FILE_BROWSER_1920x1080_PNG_bin.h"
#include "MAIN_MENU_1280x720_PNG_bin.h"
#include "OPTIONS_MENU_1280x720_PNG_bin.h"
#include "GAME_LIST_1280x720_PNG_bin.h"
#include "ROM_INFO_1280x720_PNG_bin.h"
#include "ZIP_INFO_1280x720_PNG_bin.h"
#include "FILE_BROWSER_1280x720_PNG_bin.h"


// =======================================================================
// TEXTURE / GRAPHICS  / RENDERING

void c_fbaRL::ScaleLine(u32 *Target, u32 *Source, u32 SrcWidth, u32 TgtWidth) {
 //Thanks to: http://www.compuphase.com/graphic/scale.htm
	int NumPixels1 = 0;
	u32 IntPart1 = 0;
	u32 FractPart1 = 0;
	NumPixels1 = TgtWidth;
	IntPart1 = SrcWidth / TgtWidth;
	FractPart1 = SrcWidth % TgtWidth;
	int E1 = 0;

	while (NumPixels1-- > 0) {
		*Target++ = *Source;
		Source += IntPart1;
		E1 += FractPart1;
		if ((u32)E1 >= TgtWidth) {
			E1 -= TgtWidth;
			Source++;
		} /* if */
	} /* while */
	return;
}





int c_fbaRL::ResizeImage(pngData *png_in, void *TgtTexture, u32 TgtWidth, u32 TgtHeight) {
 //Thanks to: http://www.compuphase.com/graphic/scale.htm
    if (TgtTexture == NULL)
        return 1;

    int NumPixels0 = 0;
    u32 IntPart0 = 0;
    u32 FractPart0 = 0;
    int E0 = 0;
	if(png_in->bmp_out){
		//pngData *png_out = new pngData;
		//png_out->bmp_out = (u32 *) malloc(TgtHeight*TgtWidth*sizeof(u32));
		u32 *Source = (u32 *)(void *)png_in->bmp_out;
		u32 *Target = (u32 *)TgtTexture;

//		png_out->height = TgtHeight;
//		png_out->width  = TgtWidth;
//		png_out->pitch  = TgtWidth*4;

		NumPixels0 = TgtHeight;
		IntPart0 = (png_in->height / TgtHeight) * png_in->width;
		FractPart0 = png_in->height % TgtHeight;
		E0 = 0;
		u32 *PrevSource = NULL;

		while (NumPixels0-- > 0) {
			if (Source == PrevSource) {
				memcpy(Target, Target-TgtWidth, TgtWidth*sizeof(*Target));
			} else {
				ScaleLine(Target, Source, png_in->width, TgtWidth);
				PrevSource = Source;
			}
			Target += TgtWidth;
			Source += IntPart0;
			E0 += FractPart0;
			if ((u32)E0 >= TgtHeight) {
				E0 -= TgtHeight;
				Source += png_in->width;
			}

		}
		//memset(TgtTexture, 33, TgtWidth * TgtHeight * sizeof(u32));
        return 0;
	}
	return 1;
}

void c_tex::Render(u16 x, u16 y, u16 w, u16 h)
{
	if(!bTextureOK) return;

	if(w == 0 || h == 0)	{
		w = app.width;
		h = app.height;
	}

}

int c_tex::BindTexture(char* pszPath)
{
	//printf("Bind texture: %d - %s\n", nTexture, pszPath);
	bTextureOK = false;
    pngData *pngTmp = new pngData;

	if(fileExist(pszPath))
	{
		if (png != NULL) {
            if(png->bmp_out != NULL) {
                //printf("png pieno\n");
                SAFE_FREE(png->bmp_out);
                }
        SAFE_DELETE(png)
        }
        png = new pngData;
		//printf("Load png.... ");
		pngLoadFromFile(pszPath, pngTmp);

		u32 out_w, out_h;
            if (pngTmp->width < pngTmp->height) {
                out_h = round( 460.0f * ((float)app.height / 1080.0f));
                out_w = round((float)out_h * ((float)pngTmp->width / (float)pngTmp->height));

            }
            else {
                out_w = round( 551.0f * ((float)app.width / 1920.0f));
                out_h = round((float)out_w * ((float)pngTmp->height / (float)pngTmp->width));
                if (out_h > round( 460.0f * ((float)app.height / 1080.0f))) {
                    out_h = round( 460.0f * ((float)app.height / 1080.0f));
                    out_w = round((float)out_h * ((float)pngTmp->width / (float)pngTmp->height));
                }
            }


            png->bmp_out = malloc(out_w * out_h * sizeof(u32));

            if (png->bmp_out == NULL) {
                printf("Memory error png temp\n");
                exit(0);
            }

            int i = 0;

            i =  fbaRL->ResizeImage(pngTmp, png->bmp_out, out_w, out_h);

            SAFE_FREE(pngTmp->bmp_out);
            SAFE_DELETE(pngTmp);

            if (i == 0) {

                   png->width = out_w;
                    png->pitch = out_w * sizeof(uint32_t);
                    png->height = out_h;
                    bTextureOK = true;
                    //printf("Prim resize: %dx%d\n", png->width, png->height);
                    return 1;

            }
            else {
                printf("Memory error png resize\n");
                return 0;

            }

	}
	return 0;
}

int c_tex::BindSecTexture(char* pszPath)
{
	//printf("Bind Sec texture: %d - %s\n", nTexture, pszPath);
	pngData *pngTmp = new pngData;
	bTextureOK = false;
	//pngSec = new pngData;

        if (pngSec != NULL) {
            if(pngSec->bmp_out != NULL) {
                //printf("pngSec pieno\n");
                SAFE_FREE(pngSec->bmp_out);
                }

        SAFE_DELETE(pngSec)
        }
        pngSec = new pngData;

		pngLoadFromFile(pszPath, pngTmp);
		if(pngTmp->width <= 0)
		{
			return 0;
		} else {
			u32 out_w, out_h;
			if (pngTmp->width < pngTmp->height) {
                out_h = round( 360.0f * ((float)app.height / 1080.0f));
                out_w = round((float)out_h * ((float)pngTmp->width / (float)pngTmp->height));

            }
            else {
                out_w = round( 551.0f * ((float)app.width / 1920.0f));
                out_h = round((float)out_w * ((float)pngTmp->height / (float)pngTmp->width));
                if (out_h > round( 360.0f * ((float)app.height / 1080.0f))) {
                    out_h = round( 360.0f * ((float)app.height / 1080.0f));
                    out_w = round((float)out_h * ((float)pngTmp->width / (float)pngTmp->height));
                }
            }


            pngSec->bmp_out = malloc(out_w * out_h * sizeof(u32));
            if (pngSec->bmp_out == NULL) {
                printf("Memory error pngSec temp\n");
                exit(0);
            }

            int i = 0;

            i =  fbaRL->ResizeImage(pngTmp, pngSec->bmp_out, out_w, out_h);

            SAFE_FREE(pngTmp->bmp_out);
            SAFE_DELETE(pngTmp);

            if (i == 0) {

                    pngSec->width = out_w;
                    pngSec->pitch = out_w * sizeof(uint32_t);
                    pngSec->height = out_h;
                    bTextureOK = true;
                    return 1;

            }
            else {
                printf("Memory error png resize\n");
                return 0;

            }


		}
		return 0;
}



c_tex::c_tex(uint32_t _nTexture, uint32_t display)
{
	png = NULL;
	pngSec = NULL;
	//Min = new Minimum;
	//IMG = new Image(Min);
	png = new pngData;

    //char fileTexture[256];
	nTexture = _nTexture;
    uint32_t i = display * 7 + nTexture;
    //printf("Bind texture: %d\n", nTexture);
	bTextureOK = false;

	switch (i) {
        case 7:
            pngLoadFromBuffer(MAIN_MENU_1920x1080_PNG_bin, MAIN_MENU_1920x1080_PNG_bin_size, png);
            break;
        case 8:
            pngLoadFromBuffer(GAME_LIST_1920x1080_PNG_bin, GAME_LIST_1920x1080_PNG_bin_size, png);
            break;
        case 9:
           pngLoadFromBuffer(ZIP_INFO_1920x1080_PNG_bin, ZIP_INFO_1920x1080_PNG_bin_size, png);
            break;
        case 10:
            pngLoadFromBuffer(ROM_INFO_1920x1080_PNG_bin, ROM_INFO_1920x1080_PNG_bin_size, png);
            break;
        case 11:
            pngLoadFromBuffer(OPTIONS_MENU_1920x1080_PNG_bin, OPTIONS_MENU_1920x1080_PNG_bin_size, png);
            break;
        case 12:
            pngLoadFromBuffer(FILE_BROWSER_1920x1080_PNG_bin, FILE_BROWSER_1920x1080_PNG_bin_size, png);
            break;
        case 14:
            pngLoadFromBuffer(MAIN_MENU_1280x720_PNG_bin, MAIN_MENU_1280x720_PNG_bin_size, png);
            break;
        case 15:
            pngLoadFromBuffer(GAME_LIST_1280x720_PNG_bin, GAME_LIST_1280x720_PNG_bin_size, png);
            break;
        case 16:
           pngLoadFromBuffer(ZIP_INFO_1280x720_PNG_bin, ZIP_INFO_1280x720_PNG_bin_size, png);
            break;
        case 17:
            pngLoadFromBuffer(ROM_INFO_1280x720_PNG_bin, ROM_INFO_1280x720_PNG_bin_size, png);
            break;
        case 18:
            pngLoadFromBuffer(OPTIONS_MENU_1280x720_PNG_bin, OPTIONS_MENU_1280x720_PNG_bin_size, png);
            break;
        case 19:
            pngLoadFromBuffer(FILE_BROWSER_1280x720_PNG_bin, FILE_BROWSER_1280x720_PNG_bin_size, png);
            break;
//        case 20:
//            pngLoadFromBuffer(PREVIEW_1280x720_PNG_bin, PREVIEW_1280x720_PNG_bin_size, png);
//            break;
    }

    bTextureOK = true;



}

c_tex::c_tex(uint32_t _nTexture, char* szPath)
{
	//printf("Texture: %s\n", szPath);
	png = NULL;
	pngSec = NULL;

	//Min = new Minimum;
	//IMG = new Image(Min);
	png = new pngData;
	png->bmp_out = NULL;
	pngSec = new pngData;
	pngSec->bmp_out = NULL;

	nTexture = _nTexture;



//	pszTexPath = (char*)malloc(2048);
//	memset(pszTexPath, 0, 2048);

//	strcpy(pszTexPath, szPath);

	if(!BindTexture(szPath))
		printf("Error: can't BinTexture: %s\n", szPath);


	if (_nTexture == TEX_PREVIEW) { // preview has to be resized
            //pngData *png_out = NULL;
//            printf("(Screen (%dx%d) - pre resize %d - %d - %p\n", app.width, app.height, png->width, png->height, png->bmp_out);


            if (!BindSecTexture((char *)"/dev_hdd0/game/FBNE00123/USRDIR/cores/borders/default/TITLES.PNG"))
                                    printf("Error: can't BinSecTexture: %s\n", szPath);
    }
    //printf("End texture %s\n", szPath);
}


c_tex::~c_tex()
{
	printf("Destroy c_tex %d\n", nTexture);
	if(bTextureOK)
	{
		////glDeleteTextures(1, &nTexId[0]);
		//*&nTexId[0] = NULL;
		//*&bTextureOK = NULL;
	}
	////DA rivedere!!!!
//	printf("pszTexPath c_tex\n");
//	if (pszTexPath != NULL)
//        SAFE_FREE(pszTexPath);
//	printf("IMG c_tex\n");
//	if (IMG != NULL)
//        delete (IMG);
//	printf("Min c_tex\n");
//    if (Min != NULL)
//        delete(Min);

	if (png->bmp_out != NULL) {
//        printf("%d - %d - %p\n", png->width, png->height, png->bmp_out);
        SAFE_FREE(png->bmp_out);
	}


    //printf("png c_tex\n");
	SAFE_DELETE (png);
	if (pngSec != NULL) {
        if (pngSec->bmp_out != NULL) {
//        printf("%d - %d - %p\n", png->width, png->height, png->bmp_out);
        SAFE_FREE(pngSec->bmp_out);
        }

	}



    SAFE_DELETE(pngSec);
//	printf("Exit\n");
}


void c_fbaRL::ResetPreviewImage()
{
    app.textures[TEX_PREVIEW]->BindTexture(g_opt_szTextures[TEX_PREVIEW]);
	app.textures[TEX_PREVIEW]->BindSecTexture(g_opt_szTextures[TEX_PREVIEW_TITLES]);
    //printf("Exit ResetPreviewImage\n");
}

void c_fbaRL::UpdatePreviewImage()
{


	if(nSection == SECTION_GAMELIST || nSection == SECTION_MAIN)
	{
		//printf("UpdatePreviewImage enter SECTION_GAMELIST %s\n", g_opt_szTextures[TEX_PREVIEW]);
		char szPreviewPath[512] = { 0 };

		if (strcmp(games[nBurnSelected]->sysmask, "MASKSNES") == 0) {
                sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/previews/SNES/%s.png",games[nBurnSelected]->name);
		}
		else
                if (strcmp(games[nBurnSelected]->sysmask, "MASKMEGADRIVE") == 0) {
                        sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/previews/MEGADRIVE/%s.png",games[nBurnSelected]->name);
                }
                else
                    if (strcmp(games[nBurnSelected]->sysmask, "MASKAMIGA") == 0) {
                        sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/previews/AMIGA/%s.png",games[nBurnSelected]->name);
                    }
                    else
                        if (strcmp(games[nBurnSelected]->sysmask, "MASKCOLECO") == 0) {
                            sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/previews/COLECO/%s.png", games[nBurnSelected]->name);
                        }
                        else
                            if (strcmp(games[nBurnSelected]->sysmask, "MASKTG16") == 0) {
                                sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/previews/TG16/%s.png", games[nBurnSelected]->name);
                            }
                            else
                                if (strcmp(games[nBurnSelected]->sysmask, "MASKPCE") == 0) {
                                    sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/previews/PCE/%s.png", games[nBurnSelected]->name);
                                }
                                else
                                    if (strcmp(games[nBurnSelected]->sysmask, "MASKSGX") == 0) {
                                        sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/previews/SGX/%s.png", games[nBurnSelected]->name);
                                    }
                                    else
                                        sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/previews/%s.png", games[nBurnSelected]->name);
        //printf("szPreviewPath: %s\n", szPreviewPath);
		// try main romset name...
		if(fileExist(szPreviewPath)) {
            app.textures[TEX_PREVIEW]->BindTexture(szPreviewPath);
		}
        else
		// try parent...
		{
			// clean memory again...
			////delete app.textures[TEX_PREVIEW];
			////*&app.textures[TEX_PREVIEW] = NULL;

			if (games[nBurnSelected]->isClone == false)
                    //app.textures[TEX_PREVIEW] = new c_tex(TEX_PREVIEW, g_opt_szTextures[TEX_PREVIEW]);
                    app.textures[TEX_PREVIEW]->BindTexture(g_opt_szTextures[TEX_PREVIEW]);
            else {
//                uint32_t parent = games[fgames[nSelectedGame]->GameID]->parent_id;
                if (strcmp(games[nBurnSelected]->sysmask, "MASKSNES") == 0) {
                sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/previews/SNES/%s.png",games[nBurnSelected]->parent_name);
                }
                else
                        if (strcmp(games[nBurnSelected]->sysmask, "MASKMEGADRIVE") == 0) {
                                sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/previews/MEGADRIVE/%s.png",games[nBurnSelected]->parent_name);
                        }
                        else
                            if (strcmp(games[nBurnSelected]->sysmask, "MASKAMIGA") == 0) {
                                sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/previews/AMIGA/%s.png",games[nBurnSelected]->parent_name);
                            }
                            else
                                if (strcmp(games[nBurnSelected]->sysmask, "MASKCOLECO") == 0) {
                                    sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/previews/COLECO/%s.png", games[nBurnSelected]->parent_name);
                                }
                                else
                                    if (strcmp(games[nBurnSelected]->sysmask, "MASKTG16") == 0) {
                                        sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/previews/TG16/%s.png", games[nBurnSelected]->parent_name);
                                    }
                                    else
                                        if (strcmp(games[nBurnSelected]->sysmask, "MASKPCE") == 0) {
                                            sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/previews/PCE/%s.png", games[nBurnSelected]->parent_name);
                                        }
                                        else
                                            if (strcmp(games[nBurnSelected]->sysmask, "MASKSGX") == 0) {
                                                sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/previews/SGX/%s.png", games[nBurnSelected]->parent_name);
                                            }
                                            else
                                                sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/previews/%s.png", games[nBurnSelected]->parent_name);



                if(fileExist(szPreviewPath))
                        //app.textures[TEX_PREVIEW] = new c_tex(TEX_PREVIEW, szPreviewPath);
                        app.textures[TEX_PREVIEW]->BindTexture(szPreviewPath);
                //app.textures[TEX_PREVIEW]->BindTexture(szPreviewPath);
                else
                    // no preview available...
                    //ResetPreviewImage();
                    //app.textures[TEX_PREVIEW] = new c_tex(TEX_PREVIEW, g_opt_szTextures[TEX_PREVIEW]);
                    app.textures[TEX_PREVIEW]->BindTexture(g_opt_szTextures[TEX_PREVIEW]);
            }
		}

		//Boxart
		//printf("Update title\n");
		if (strcmp(games[nBurnSelected]->sysmask, "MASKSNES") == 0) {
                sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/titles/SNES/%s.png",games[nBurnSelected]->name);
		}
		else
                if (strcmp(games[nBurnSelected]->sysmask, "MASKMEGADRIVE") == 0) {
                        sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/titles/MEGADRIVE/%s.png",games[nBurnSelected]->name);
                }
                else
                    if (strcmp(games[nBurnSelected]->sysmask, "MASKAMIGA") == 0) {
                        sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/titles/AMIGA/%s.png",games[nBurnSelected]->name);
                    }
                    else
                        if (strcmp(games[nBurnSelected]->sysmask, "MASKCOLECO") == 0) {
                            sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/titles/COLECO/%s.png", games[nBurnSelected]->name);
                        }
                        else
                            if (strcmp(games[nBurnSelected]->sysmask, "MASKTG16") == 0) {
                                sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/titles/TG16/%s.png", games[nBurnSelected]->name);
                            }
                            else
                                if (strcmp(games[nBurnSelected]->sysmask, "MASKPCE") == 0) {
                                    sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/titles/PCE/%s.png", games[nBurnSelected]->name);
                                }
                                else
                                    if (strcmp(games[nBurnSelected]->sysmask, "MASKSGX") == 0) {
                                        sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/titles/SGX/%s.png", games[nBurnSelected]->name);
                                    }
                                    else
                                        sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/titles/%s.png", games[nBurnSelected]->name);



		// try main romset name...
		if(fileExist(szPreviewPath)) {
          //printf("Pre title %s\n", szPreviewPath);
            app.textures[TEX_PREVIEW]->BindSecTexture(szPreviewPath);
            //printf("Post title\n");
		}
        else {

			if (games[nBurnSelected]->isClone == false)
                    app.textures[TEX_PREVIEW]->BindSecTexture(g_opt_szTextures[TEX_PREVIEW_TITLES]);
            else {
//                uint32_t parent = games[fgames[nSelectedGame]->GameID]->parent_id;

                if (strcmp(games[nBurnSelected]->sysmask, "MASKSNES") == 0) {
                sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/titles/SNES/%s.png",games[nBurnSelected]->parent_name);
                }
                else
                        if (strcmp(games[nBurnSelected]->sysmask, "MASKMEGADRIVE") == 0) {
                                sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/titles/MEGADRIVE/%s.png",games[nBurnSelected]->parent_name);
                        }
                        else
                            if (strcmp(games[nBurnSelected]->sysmask, "MASKAMIGA") == 0) {
                                sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/titles/AMIGA/%s.png",games[nBurnSelected]->parent_name);
                            }
                            else
                                if (strcmp(games[nBurnSelected]->sysmask, "MASKCOLECO") == 0) {
                                    sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/titles/COLECO/%s.png", games[nBurnSelected]->parent_name);
                                }
                                else
                                    if (strcmp(games[nBurnSelected]->sysmask, "MASKTG16") == 0) {
                                        sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/titles/TG16/%s.png", games[nBurnSelected]->parent_name);
                                    }
                                    else
                                        if (strcmp(games[nBurnSelected]->sysmask, "MASKPCE") == 0) {
                                            sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/titles/PCE/%s.png", games[nBurnSelected]->parent_name);
                                        }
                                        else
                                            if (strcmp(games[nBurnSelected]->sysmask, "MASKSGX") == 0) {
                                                sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/titles/SGX/%s.png", games[nBurnSelected]->parent_name);
                                            }
                                            else
                                                sprintf(szPreviewPath, "/dev_hdd0/game/FBNE00123/USRDIR/cores/titles/%s.png", games[nBurnSelected]->parent_name);

                if(fileExist(szPreviewPath))
                        app.textures[TEX_PREVIEW]->BindSecTexture(szPreviewPath);
                //app.textures[TEX_PREVIEW]->BindTexture(szPreviewPath);
                else
                    // no preview available...
                    //ResetPreviewImage();
                    app.textures[TEX_PREVIEW]->BindSecTexture(g_opt_szTextures[TEX_PREVIEW_TITLES]);
            }
		}
		//End Boxart

	}
}
