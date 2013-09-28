#include "main.h"

// =======================================================================
// TEXTURE / GRAPHICS  / RENDERING 

void c_tex::Render(GLuint x, GLuint y, GLsizei w, GLsizei h)
{
	if(!bTextureOK) return;	

	if(w == 0 || h == 0)	{
		w = app.renderWidth;
		h = app.renderHeight;
	}

	glViewport(x, y, w, h);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, nTexId[0]);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2, GL_FLOAT, 0, app.squareVertices);
	glNormalPointer(GL_FLOAT, 0, app.normals);
	glTexCoordPointer(2, GL_FLOAT, 0, app.texCoords);

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

void c_fbaRL::RenderBackground()
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

			float maxw = (27.604f / 100.0f) * (float)app.renderWidth;		// 530 @ 1920 x 1080 res
			float maxh = (31.481f / 100.0f) * (float)app.renderHeight;		// 340 @ 1920 x 1080 res

			if( app.textures[TEX_PREVIEW]->_texture.width > app.textures[TEX_PREVIEW]->_texture.height || 
				app.textures[TEX_PREVIEW]->_texture.width == app.textures[TEX_PREVIEW]->_texture.height )
			{
				maxw = (27.604f / 100.0f) * (float)app.renderWidth;  // 530 @ 1920 x 1080 res
				maxh = (31.481f / 100.0f) * (float)app.renderHeight; // 340 @ 1920 x 1080 res
			} else {
				maxw = (31.481f / 100.0f) * (float)app.renderWidth;  // 340 @ 1920 x 1080 res
				maxh = (45.370f / 100.0f) * (float)app.renderHeight; // 490 @ 1920 x 1080 res
			}

			float ax = (float)fba_drv[nBurnSelected].nAspectX; 
			float ay = (float)fba_drv[nBurnSelected].nAspectY;

			bool bNoPreview = false;
			if(strcmp(app.textures[TEX_PREVIEW]->pszTexPath, g_opt_szTextures[TEX_PREVIEW]) == 0) {
				bNoPreview = true;
			}

			// max WIDTH
			if(w > maxw) {
				float nh = maxw * (float)(bNoPreview ? (h / w) : (ay / ax));
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
				float nw = maxh * (float)(bNoPreview ? (w / h) : (ax / ay));
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
				float x = app.renderWidth - (app.renderWidth / 3) + (GLuint)xdiff;
				float y = app.renderHeight - (app.renderHeight / 2) + (GLuint)ydiff;

				app.textures[TEX_PREVIEW]->Render((GLuint)x, (GLuint)y, (GLsizei)w, (GLsizei)h);
			} else {
				// x - 70
				// y - 75
				float xadjust = (3.645833f / 100.0f) * (float)app.renderWidth;
				float yadjust = (6.944444f / 100.0f) * (float)app.renderHeight;

				float x = (((float)app.renderWidth - ((float)app.renderWidth / 4.0f)) - (xadjust * 2.80f)) + xdiff;
				float y = (((float)app.renderHeight - ((float)app.renderHeight / 2.0f)) - yadjust) + ydiff;

				app.textures[TEX_PREVIEW]->Render((GLuint)x, (GLuint)y, (GLsizei)w, (GLsizei)h);
			}
		}
	}
	if(fbaRL->nSection == SECTION_ZIPINFO) {
		app.textures[TEX_ZIP_INFO]->Render(0,0,0,0);
	}
	if(fbaRL->nSection == SECTION_ROMINFO) {
		app.textures[TEX_ROM_INFO]->Render(0,0,0,0);
	}
	if(fbaRL->nSection == SECTION_OPTIONS) {
		app.textures[TEX_OPTIONS]->Render(0,0,0,0);
	}
	if(fbaRL->nSection == SECTION_FILEBROWSER) {
		app.textures[TEX_FILEBROWSER]->Render(0,0,0,0);
	}
}

void c_fbaRL::ResetPreviewImage()
{
	if(app.textures[TEX_PREVIEW]->bTextureOK) 
	{
		delete app.textures[TEX_PREVIEW];
		*&app.textures[TEX_PREVIEW] = NULL;
	}
	// no preview available...
	app.textures[TEX_PREVIEW] = new c_tex(TEX_PREVIEW, g_opt_szTextures[TEX_PREVIEW]);

}

void c_fbaRL::UpdatePreviewImage()
{
	if(nBurnSelected < 0) {
		ResetPreviewImage();
		//nStatus = STATUS_RESETPREVIEW;
		return;
	}

	// clean memory first...
	if(app.textures[TEX_PREVIEW]->bTextureOK) 
	{
		delete app.textures[TEX_PREVIEW];
		*&app.textures[TEX_PREVIEW] = NULL;
	}

	if(nSection == SECTION_GAMELIST)
	{
		char szPreviewPath[2048] = { 0 };
		sprintf(szPreviewPath, "/dev_hdd0/game/FBAL00123/USRDIR/cores/previews/%s.png", fba_drv[nBurnSelected].szName);
		
		// try main romset name...
		app.textures[TEX_PREVIEW] = new c_tex(TEX_PREVIEW, szPreviewPath);
		
		// try parent...
		if(!app.textures[TEX_PREVIEW]->bTextureOK) 
		{
			// clean memory again...
			delete app.textures[TEX_PREVIEW];
			*&app.textures[TEX_PREVIEW] = NULL;
			
			sprintf(szPreviewPath, "/dev_hdd0/game/FBAL00123/USRDIR/cores/previews/%s.png", fba_drv[nBurnSelected].szParent);
			
			app.textures[TEX_PREVIEW] = new c_tex(TEX_PREVIEW, szPreviewPath);
			//app.textures[TEX_PREVIEW]->BindTexture(szPreviewPath);
		}

		// no preview available...
		if(!app.textures[TEX_PREVIEW]->bTextureOK) {
			//nStatus = STATUS_RESETPREVIEW;
			ResetPreviewImage();
		}
	}
}
