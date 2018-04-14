#ifndef __ASCII_FONTS_H__
#define __ASCII_FONTS_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class AsciiFont;

#include <iostream>
#include <fonts/fonts.h>


class AsciiFont : public Fonts{
public:
	static AsciiFont* getAsciiFont(void);	
	virtual int getFontBitmap(unsigned int dwCode, FontBitMap& tFontBitMap);
	virtual int setFontSize(int dwFontSize);
	virtual int getFontSize(void);	
private:
	int g_iFdHZK;
	char *g_pcHZKMem;
	char *g_pcHZKMemEnd;

	static AsciiFont *asciiFont;
	static pthread_mutex_t tMutex;
	AsciiFont() {}	 //把构造函数私有化，避免使用构造函数构造另一个实例化对象
	virtual void fontInit(std::string strFontFile, int dwFontSize);
	int getAsciiFontBitmap(unsigned int dwCode, FontBitMap& tFontBitMap);
	int getHZKFontBitmap(unsigned int dwCode, FontBitMap& tFontBitMap);
	

};


#endif // __ASCII_FONTS_H__






