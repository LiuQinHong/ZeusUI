#ifndef __TEXTPARSER_H__
#define __TEXTPARSER_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
struct TextFileInfo;
class TextParser;

#include <iostream>
#include <encode/encodeparser.h>
#include <fonts/fonts.h>
#include <device/framebuffer.h>




struct TextFileInfo {
	int iFileSize;
	char *pcFileDataStart;
	char *pcFileDataEnd;
};

class TextParser {
public:
	TextParser();
	virtual ~TextParser();
	int openTextInit(const std::string& fileName);
	void releaseText(void);
	void showText(void);
	int showText(struct PixelDatas& tPixelDatas, int iX, int iY, unsigned int dwColor, bool isDowm);	
	int showText(struct PixelDatas& tPixelDatas, char *pcBufStart, char *pcBufEnd, int iX, int iY, unsigned int dwColor, bool isDowm);	
	bool isFirstPage(void);
	bool isTheLastPage(void);
	void setFont(FontInfo& mtFont);
	FontInfo& getFont(void);
	void putStrToLCD(int iX, int iY, const char *pcStrBufStart, int iLen);
	void putStr(struct PixelDatas& tDestPixelData, int iX, int iY, const char *pcStrBufStart, int iLen, unsigned int dwColor);
	void putStr(struct PixelDatas& tDestPixelData, int iX, int iY, const char *pcStrBufStart, int iLen, unsigned int dwColor, const std::string& strEncodeName);	
	EncodeParser *getEncodeParser(void);
	Fonts *getFonts(void);

private:
	FontInfo mtFontInfo;
	std::string mFileName;
	TextFileInfo mtTextFileInfo;
	char *mPosStart;
	std::list<char *> mPoslist;	//每一页的首地址链表
	EncodeParser *mpEncodeParser;
	Fonts *mpFonts;
	FrameBuffer *mpFrameBuffer;
	
};


#endif // __TEXTPARSER_H__


