#include <fonts/freetype.h>

Freetype *Freetype::freetype = NULL;
pthread_mutex_t Freetype::tMutex  = PTHREAD_MUTEX_INITIALIZER;


Freetype* Freetype::getFreetype(void)
{
	if (NULL == freetype) { 	
		pthread_mutex_lock(&tMutex);
		if (NULL == freetype) {
			freetype = new Freetype();
			freetype->fontInit("/etc/MSYH.TTF", 16);
		}
		pthread_mutex_unlock(&tMutex);
	}

	return freetype;
}


void Freetype::fontInit(std::string strFontFile, int dwFontSize)
{
	int iRet;

	iRet = FT_Init_FreeType(&g_tLibrary);
	if (iRet) {
		fprintf(stderr, "FT_Init_FreeType error!\n");
		return ;
	}

	iRet = FT_New_Face(g_tLibrary, strFontFile.c_str(), 0, &g_tFace);
	if (iRet) {
		fprintf(stderr, "FT_New_Face error!\n");
		return ;
	}

	
	g_tSlot = g_tFace->glyph;
	mFontSize = dwFontSize;

	iRet = FT_Set_Pixel_Sizes(g_tFace, dwFontSize, 0);
	if (iRet) {
		fprintf(stderr, "FT_Set_Pixel_Sizes failed : %d\n", dwFontSize);
		return ;
	}

}


int Freetype::getFontBitmap(unsigned int dwCode, FontBitMap& tFontBitMap)
{
	int iRet;
	int iPenX = tFontBitMap.iCurOriginX;
	int iPenY = tFontBitMap.iCurOriginY;


	iRet = FT_Load_Char(g_tFace, dwCode, FT_LOAD_RENDER | FT_LOAD_MONOCHROME);
	if (iRet) {
		fprintf(stderr, "FT_Load_Char error for code : 0x%x\n", dwCode);
		return -1;
	}

	tFontBitMap.iXLeft	= iPenX + g_tSlot->bitmap_left;
	tFontBitMap.iYTop 	= iPenY - g_tSlot->bitmap_top;
	tFontBitMap.iXMax 	= tFontBitMap.iXLeft + g_tSlot->bitmap.width;
	tFontBitMap.iYMax 	= tFontBitMap.iYTop  + g_tSlot->bitmap.rows;
	tFontBitMap.iBpp	= 1;
	tFontBitMap.iPitch	= g_tSlot->bitmap.pitch;
	tFontBitMap.pcBuffer  = (char *)g_tSlot->bitmap.buffer;
	tFontBitMap.iWidth	= g_tSlot->bitmap.width;
	tFontBitMap.iHeight = g_tSlot->bitmap.rows;
		
	tFontBitMap.iNextOriginX = iPenX + g_tSlot->advance.x / 64;
	tFontBitMap.iNextOriginY = iPenY;


	return 0;
}


int Freetype::getFontSize(void)
{
	return mFontSize;
}

int Freetype::setFontSize(int dwFontSize)
{
	FT_Set_Pixel_Sizes(g_tFace, dwFontSize, 0);
	mFontSize = dwFontSize;
	return 0;
}

