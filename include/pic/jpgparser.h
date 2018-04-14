#ifndef __JPGPARSER_H__
#define __JPGPARSER_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class JpgParser;

#include <iostream>
#include <pic/picparser.h>
#include <stdio.h>
#include <setjmp.h>
#include <jpeglib.h>
#include <string.h>
#include <stdlib.h>

class JpgParser : public PicParser {
public:
	virtual int CovertOneLine(int iWidth, int iSrcBpp, int iDstBpp, char *pcSrcDatas, char *pcDstDatas);
	virtual bool isSupport(void);
	virtual int getPixelData(int iBpp);
};


#endif // __JPGPARSER_H__


