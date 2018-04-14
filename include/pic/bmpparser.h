#ifndef __BMPPARSER_H__
#define __BMPPARSER_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class BmpParser;

#include <iostream>
#include <pic/picparser.h>

class BmpParser : public PicParser {
public:
	virtual int CovertOneLine(int iWidth, int iSrcBpp, int iDstBpp, char *pudSrcDatas, char *pudDstDatas);
	virtual bool isSupport(void);
	virtual int getPixelData(int iBpp);
};


#endif // __BMPPARSER_H__



