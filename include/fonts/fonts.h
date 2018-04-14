#ifndef __FONTS_H__
#define __FONTS_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
struct FontBitMap;
struct FontInfo;
class Fonts;

#include <iostream>

struct FontBitMap {
	int iWidth;
	int iHeight;
	int iXLeft;  /* 位图左上角X座标 */
	int iYTop;   /* 位图左上角Y座标 */
	int iXMax;   /* 位图的最大X座标值 */
	int iYMax;   /* 位图的最大Y座标值 */
	int iBpp;    /* 位图中一个象素用多少位来表示 */
	int iPitch;  /* 对于单色位图, 两行象素之间的跨度, 即第N行、第N+1行象素数据之间的偏移值 */
	int iCurOriginX;  /* 位图的原点X座标(一般是左下角), 使用原点来确定相邻字符的位置 */
	int iCurOriginY;  /* 位图的原点Y座标 */
	int iNextOriginX; /* 下一个字符即右边字符的原点X座标 */
	int iNextOriginY; /* 下一个字符即右边字符的原点Y座标 */
	char *pcBuffer;  /* 存有字符的位图数据 */
};


struct FontInfo {
	int iFontSize;
	int iFontColor;		//字体颜色
	int iBackcolor;		//背景颜色
};

class Fonts {
public:
	
	std::string& getFontTypeName(void);
	void setFontTypeName(std::string fontTypeName);

	virtual void fontInit(std::string strFontFile, int dwFontSize) = 0;
	virtual int getFontBitmap(unsigned int dwCode, FontBitMap& tFontBitMap) = 0;
	virtual int setFontSize(int dwFontSize) = 0;
	virtual int getFontSize(void) = 0;

private:
	std::string mFontTypeName;
};


#endif // __FONTS_H__




