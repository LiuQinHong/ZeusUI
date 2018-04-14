#ifndef __PNGPARSER_H__
#define __PNGPARSER_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class PngParser;

#include <iostream>
#include <pic/picparser.h>

class PngParser : public PicParser {
public:
	virtual int CovertOneLine(int iWidth, int iSrcBpp, int iDstBpp, char *pcSrcDatas, char *pcDstDatas);
	virtual bool isSupport(void);
	virtual int getPixelData(int iBpp);
};


#endif // __PNGPARSER_H__



