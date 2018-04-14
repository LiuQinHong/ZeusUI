#ifndef __FREETYPE_FONTS_H__
#define __FREETYPE_FONTS_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class Freetype;

#include <iostream>
#include <fonts/fonts.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

class Freetype : public Fonts{
public:
	virtual int getFontBitmap(unsigned int dwCode, FontBitMap& tFontBitMap);
	virtual int setFontSize(int dwFontSize);
	virtual int getFontSize(void);	
	static Freetype* getFreetype(void);
private:
	FT_Library g_tLibrary;
	FT_Face g_tFace;
	FT_GlyphSlot g_tSlot;
	int mFontSize;	
	static Freetype *freetype;
	static pthread_mutex_t tMutex;
	virtual void fontInit(std::string strFontFile, int dwFontSize);	
	Freetype() {}

};

#endif // __FREETYPE_FONTS_H__





