#include <iostream>
#include <pic/picparser.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>


PicParser::PicParser()
{
	mtPicPixelDatas.pcPixelDatas = NULL;
}
PicParser::~PicParser()
{
	if (mtPicPixelDatas.pcPixelDatas) {
		delete mtPicPixelDatas.pcPixelDatas;
	}
}

int PicParser::openInit(std::string& filePathName)
{
	mFp = fopen(filePathName.c_str(), "r");
	if (mFp == NULL) {
		fprintf(stderr, "can't open %s file!\n", filePathName.c_str());
		return -1;
	}
	
	return 0;	
}


void PicParser::release(void)
{
	if (mtPicPixelDatas.pcPixelDatas) {
		delete mtPicPixelDatas.pcPixelDatas;
		mtPicPixelDatas.pcPixelDatas = NULL;
	}
	
	fclose(mFp);
}


int PicParser::zoomPic(struct PixelDatas& tZoomPixelDatas)
{
	PixelDatas& PicPixelDatas = getPicPixelDatas();
	int iY;
	int iX;
	float iRatioX;
	float iRatioY;
	int iPixelSize = PicPixelDatas.iBpp / 8;
	int iLineBytes;
	unsigned long adwZoomXTable[2048];
	unsigned long dwZoomY;

	int iDestWidth  = tZoomPixelDatas.iWidth;
	int iDestHeight = tZoomPixelDatas.iHeight;
	int iSrcWidth  = PicPixelDatas.iWidth;
	int iSrcHeight = PicPixelDatas.iHeight;
	char *pcDestbufTmp;
	char *pcSrcbuf;


	if (iDestHeight > iSrcHeight) {
		iDestHeight = iSrcHeight;
	}

	iRatioY = (float)iDestHeight / iSrcHeight;

	if (iDestWidth == 0) {
		iDestWidth = iRatioY * iSrcWidth;
		tZoomPixelDatas.iWidth = iDestWidth;
	}
	iRatioX = (float)iDestWidth / iSrcWidth;
	iLineBytes = iDestWidth * iPixelSize;




	if (!tZoomPixelDatas.pcPixelDatas) {
		tZoomPixelDatas.pcPixelDatas = new char[iDestWidth * iDestHeight * iPixelSize];
		if (tZoomPixelDatas.pcPixelDatas == NULL) {
			fclose(mFp);
			delete mtPicPixelDatas.pcPixelDatas;
			fprintf(stderr, "malloc tZoomPixelDatas.pcPixelDatas error!\n");
			return -1;
		}		
	}



	for (iX = 0; iX < iDestWidth; iX++) {
		adwZoomXTable[iX] = (unsigned long)(iX / iRatioX);
	}
	
	for (iY = 0; iY < iDestHeight; iY++) {
		dwZoomY = (unsigned long)(iY / iRatioY);
		pcDestbufTmp = tZoomPixelDatas.pcPixelDatas + iY * iLineBytes;
		pcSrcbuf  = PicPixelDatas.pcPixelDatas + dwZoomY * PicPixelDatas.iLineBytes;
		for (iX = 0; iX < iDestWidth; iX++) {
			memcpy(pcDestbufTmp + iX*iPixelSize, pcSrcbuf + adwZoomXTable[iX]*iPixelSize, iPixelSize);
		}
	}

	tZoomPixelDatas.iBpp          = PicPixelDatas.iBpp;
	tZoomPixelDatas.iLineBytes    = iLineBytes;
	tZoomPixelDatas.iTotalBytes   = iLineBytes * iDestHeight;
	tZoomPixelDatas.iHeight       = iDestHeight;

	return 0;
}




int PicParser::zoomPic(struct PixelDatas& tDestPixelData, struct PixelDatas& tSrcPixelData)
{
	int iY;
	int iX;
	float iRatioX;
	float iRatioY;
	int iPixelSize = tSrcPixelData.iBpp / 8;
	int iLineBytes;
	unsigned long adwZoomXTable[2048];
	unsigned long dwZoomY;

	int iDestWidth	= tDestPixelData.iWidth;
	int iDestHeight = tDestPixelData.iHeight;
	int iSrcWidth  = tSrcPixelData.iWidth;
	int iSrcHeight = tSrcPixelData.iHeight;
	char *pcDestbufTmp;
	char *pcSrcbuf;


	if (iDestHeight > iSrcHeight) {
		iDestHeight = iSrcHeight;
	}

	iRatioY = (float)iDestHeight / iSrcHeight;

	if (iDestWidth == 0) {
		iDestWidth = iRatioY * iSrcWidth;
		tDestPixelData.iWidth = iDestWidth;
	}
	iRatioX = (float)iDestWidth / iSrcWidth;
	iLineBytes = iDestWidth * iPixelSize;




	if (!tDestPixelData.pcPixelDatas) {
		tDestPixelData.pcPixelDatas = new char[iDestWidth * iDestHeight * iPixelSize];
		if (tDestPixelData.pcPixelDatas == NULL) {
			fclose(mFp);
			delete mtPicPixelDatas.pcPixelDatas;
			fprintf(stderr, "malloc tDestPixelData.pcPixelDatas error!\n");
			return -1;
		}		
	}



	for (iX = 0; iX < iDestWidth; iX++) {
		adwZoomXTable[iX] = (unsigned long)(iX / iRatioX);
	}
	
	for (iY = 0; iY < iDestHeight; iY++) {
		dwZoomY = (unsigned long)(iY / iRatioY);
		pcDestbufTmp = tDestPixelData.pcPixelDatas + iY * iLineBytes;
		pcSrcbuf  = tSrcPixelData.pcPixelDatas + dwZoomY * tSrcPixelData.iLineBytes;
		for (iX = 0; iX < iDestWidth; iX++) {
			memcpy(pcDestbufTmp + iX*iPixelSize, pcSrcbuf + adwZoomXTable[iX]*iPixelSize, iPixelSize);
		}
	}

	tDestPixelData.iBpp		  = tSrcPixelData.iBpp;
	tDestPixelData.iLineBytes	  = iLineBytes;
	tDestPixelData.iTotalBytes   = iLineBytes * iDestHeight;
	tDestPixelData.iHeight 	  = iDestHeight;

	return 0;
}



int PicParser::getZoomPixelData(std::string& filePathName, PixelDatas& tZoomPixelDatas)
{
	int iRet;

	iRet = openInit(filePathName);
	if (iRet < 0) {
		fprintf(stderr, "openInit error : file : %s ret = %d!\n", filePathName.c_str(), iRet);
		return iRet;
	}	


	if (!isSupport()) {
		fclose(getFp());
		fprintf(stderr, "no Support this pic file\n");	
		return -1;
	}
	
	
	iRet = getPixelData(tZoomPixelDatas.iBpp);
	if (iRet) {
		fprintf(stderr, "jpg parser getPixelData error\n");	
		return -1;
		
	}	

	iRet = zoomPic(tZoomPixelDatas);
	if (iRet) {
		fclose(getFp());
		fprintf(stderr, "jpg parser getPixelData error\n");	
		return -1;	
	}
	/* 处理完就关闭文件 */
	release();
	return 0;	
}



FILE *PicParser::getFp(void)
{
	return mFp;
}

PixelDatas& PicParser::getPicPixelDatas(void)
{
	return mtPicPixelDatas;
}


const char *PicParser::getFilenameExt(const std::string& fileName)
{
	
	const char *dot = (const char *)strrchr(fileName.c_str(), '.');
	if(!dot || dot == fileName.c_str()) 
		return " ";

	return dot;
}


