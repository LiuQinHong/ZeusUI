#ifndef __PICPARSER_H__
#define __PICPARSER_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
struct PixelDatas;
class PicParser;

#include <iostream>
#include <string.h>
#include <stdlib.h>

struct PixelDatas {
	int iWidth;   /* 宽度: 一行有多少个象素 */
	int iHeight;  /* 高度: 一列有多少个象素 */
	int iBpp;     /* 一个象素用多少位来表示 */
	int iLineBytes;  /* 一行数据有多少字节 */
	int iTotalBytes; /* 所有字节数 */ 
	char *pcPixelDatas;  /* 象素数据存储的地方 */
};



class PicParser {
public:
	PicParser();
	virtual ~PicParser();

	int openInit(std::string& filePathName);
	void release(void);
	int zoomPic(struct PixelDatas &tZoomPixelDatas);
	int zoomPic(struct PixelDatas& tDestPixelData, struct PixelDatas& tSrcPixelData);
	int getZoomPixelData(std::string& filePathName, PixelDatas& tZoomPixelDatas);

	FILE *getFp(void);
	PixelDatas& getPicPixelDatas(void);

	
	static const char *getFilenameExt(const std::string& fileName);

	/* 需要子类实现的 */
	virtual bool isSupport(void) = 0;
	virtual int getPixelData(int iBpp) = 0;
	virtual int CovertOneLine(int iWidth, int iSrcBpp, int iDstBpp, char *pcSrcDatas, char *pcDstDatas) = 0;

private:
	FILE *mFp;
	PixelDatas mtPicPixelDatas;
	
};


#endif // __PICPARSER_H__

