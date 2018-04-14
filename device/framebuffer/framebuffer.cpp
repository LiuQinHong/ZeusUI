#include <device/framebuffer.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <string.h>
#include <math.h>

FrameBuffer *FrameBuffer::frameBuffer = NULL;
pthread_mutex_t FrameBuffer::tMutex  = PTHREAD_MUTEX_INITIALIZER;


FrameBuffer* FrameBuffer::getFrameBuffer(void)
{
	if (NULL == frameBuffer) {		
		pthread_mutex_lock(&tMutex);
		if (NULL == frameBuffer) {
			frameBuffer = new FrameBuffer();
		}
		pthread_mutex_unlock(&tMutex);
	}

	return frameBuffer;
}


FrameBuffer::FrameBuffer()
{
	FbDeviceInit();
}

FrameBuffer::~FrameBuffer()
{
	if (iFd > 0) {
		close(iFd);
	}

	if (mtFrameBufferPixelData.pcPixelDatas) {
		munmap(mtFrameBufferPixelData.pcPixelDatas, mtFrameBufferPixelData.iTotalBytes);
	}
	
}



int FrameBuffer::FbDeviceInit(void)
{
	int iRet;
	struct fb_var_screeninfo mtFBVar;
	struct fb_fix_screeninfo mtFBFix;
	
	iFd = open(DISPALY_DEVICE, O_RDWR);
	if (iFd < 0) {
		fprintf(stderr, "can't open %s\n", DISPALY_DEVICE);
		return -1;
	}
	
	iRet = ioctl(iFd, FBIOGET_VSCREENINFO, &mtFBVar);
	if (iRet < 0) {
		fprintf(stderr, "can't get var of %s\n", DISPALY_DEVICE);
		return -1;
	}

	iRet = ioctl(iFd, FBIOGET_FSCREENINFO, &mtFBFix);
	if (iRet < 0) {
		fprintf(stderr, "can't get fix of %s\n", DISPALY_DEVICE);
		return -1;
	}

	mFBMemSize = mtFBVar.xres * mtFBVar.yres * mtFBVar.bits_per_pixel / 8;	
	mtFrameBufferPixelData.pcPixelDatas = (char *)mmap(NULL , mFBMemSize, PROT_READ | PROT_WRITE, MAP_SHARED, iFd, 0);
	if (mtFrameBufferPixelData.pcPixelDatas == NULL) {
		fprintf(stderr, "can't mmap\n");
		return -1;
	}

	mScreenInfo.iXres      = mtFBVar.xres;
	mScreenInfo.iYres      = mtFBVar.yres;
	mScreenInfo.iBpp       = mtFBVar.bits_per_pixel;
	mScreenInfo.iLineWidth = mtFBVar.xres * mtFBVar.bits_per_pixel / 8;
	mScreenInfo.iFBMemSize = mFBMemSize;

	mtFrameBufferPixelData.iBpp = mScreenInfo.iBpp;
	mtFrameBufferPixelData.iWidth = mScreenInfo.iXres;
	mtFrameBufferPixelData.iHeight = mScreenInfo.iYres;
	mtFrameBufferPixelData.iLineBytes = mScreenInfo.iLineWidth;
	mtFrameBufferPixelData.iTotalBytes = mFBMemSize;

	return 0;	
}
int FrameBuffer::FbShowPixel(int dwX, int dwY, unsigned int dwColor)
{
	char *pucFbMem_8 = NULL;
	short *pwFbMem_16 = NULL;
	int *pdwFbMem_32 = NULL;
	int iRed;
	int iGreen;
	int iBlue;
	

	
	if ((dwX > mScreenInfo.iXres) || (dwY > mScreenInfo.iYres)) {
		return 0;
	}


	pucFbMem_8	= mtFrameBufferPixelData.pcPixelDatas + (dwX * mScreenInfo.iBpp / 8) + (dwY * mScreenInfo.iLineWidth);
	pwFbMem_16	= (short *)pucFbMem_8;
	pdwFbMem_32 = (int *)pucFbMem_8;



	switch (mScreenInfo.iBpp) {
		case 8:
		{
			*pucFbMem_8 = (unsigned char)dwColor;
			break;
		}

		case 16:
		{
			iRed   = (dwColor >> (16+3)) & 0x1F;
			iGreen = (dwColor >> (8+2 )) & 0x3F;
			iBlue  = (dwColor >> (0+3 )) & 0x1F;

			*pwFbMem_16 = (unsigned short)((iRed << 11) | (iGreen << 5) | iBlue);
			break;
		}

		case 32:
		{
			*pdwFbMem_32 = dwColor;
			break;
		}

		default :
		{
			fprintf(stderr, "can't support %d bpp!\n", mScreenInfo.iBpp);
			return -1;
		}
			
	}
	

	return 0;

}


/* 此处X Y为相对位置 */
int FrameBuffer::FbShowPixel(struct PixelDatas& destPixelDatas, int dwX, int dwY, unsigned int dwColor)
{
	char *pucFbMem_8 = NULL;
	short *pwFbMem_16 = NULL;
	int *pdwFbMem_32 = NULL;
	int iRed;
	int iGreen;
	int iBlue;
	

	
	if ((dwX > destPixelDatas.iWidth) || (dwY > destPixelDatas.iHeight)) {
		return 0;
	}


	pucFbMem_8	= destPixelDatas.pcPixelDatas + (dwX * mScreenInfo.iBpp / 8) + (dwY * destPixelDatas.iLineBytes);
	pwFbMem_16	= (short *)pucFbMem_8;
	pdwFbMem_32 = (int *)pucFbMem_8;



	switch (mScreenInfo.iBpp) {
		case 8:
		{
			*pucFbMem_8 = (unsigned char)dwColor;
			break;
		}

		case 16:
		{
			iRed   = (dwColor >> (16+3)) & 0x1F;
			iGreen = (dwColor >> (8+2 )) & 0x3F;
			iBlue  = (dwColor >> (0+3 )) & 0x1F;

			*pwFbMem_16 = (unsigned short)((iRed << 11) | (iGreen << 5) | iBlue);
			break;
		}

		case 32:
		{
			*pdwFbMem_32 = dwColor;
			break;
		}

		default :
		{
			fprintf(stderr, "can't support %d bpp!\n", mScreenInfo.iBpp);
			return -1;
		}
			
	}
	

	return 0;

}


int FrameBuffer::FbCleanScreen(unsigned int dwColor)
{
	unsigned short *pwFbMem_16 = NULL;
	unsigned int *pdwFbMem_32 = NULL;
	unsigned short wColorTmp;
	int iRed;
	int iGreen;
	int iBlue;
	int iCount;
		
		
	pwFbMem_16	= (unsigned short *)mtFrameBufferPixelData.pcPixelDatas;
	pdwFbMem_32 = (unsigned int *)mtFrameBufferPixelData.pcPixelDatas;


	switch (mScreenInfo.iBpp) {
		case 8:
		{
			memset(mtFrameBufferPixelData.pcPixelDatas, dwColor, mFBMemSize);
			break;
		}

		case 16:
		{
			iRed   = (dwColor >> (16+3)) & 0x1F;
			iGreen = (dwColor >> (8+2 )) & 0x3F;
			iBlue  = (dwColor >> (0+3 )) & 0x1F;
			wColorTmp = (unsigned short)((iRed << 11) | (iGreen << 5) | iBlue);
			
			for (iCount = 0; iCount < mFBMemSize; iCount += 2) {
				
				*pwFbMem_16 = wColorTmp;
				pwFbMem_16++;
			}
			
			break;
		}

		case 32:
		{
			for (iCount = 0; iCount < mFBMemSize; iCount += 4) {
				
				*pdwFbMem_32 = dwColor;
				pdwFbMem_32++;
			}
		}

		default :
		{
			fprintf(stderr, "can't support %d bpp!\n", mScreenInfo.iBpp);
			return -1;
		}
			
	}
	

	return 0;

}

int FrameBuffer::FbSetColour(struct PixelDatas& destPixelDatas, unsigned int dwColor)
{
	unsigned short *pwFbMem_16 = NULL;
	unsigned int *pdwFbMem_32 = NULL;
	unsigned short wColorTmp;
	int iRed;
	int iGreen;
	int iBlue;
	int iCount;
		
		
	pwFbMem_16	= (unsigned short *)destPixelDatas.pcPixelDatas;
	pdwFbMem_32 = (unsigned int *)destPixelDatas.pcPixelDatas;


	switch (mScreenInfo.iBpp) {
		case 8:
		{
			memset(destPixelDatas.pcPixelDatas, dwColor, mFBMemSize);
			break;
		}

		case 16:
		{
			iRed   = (dwColor >> (16+3)) & 0x1F;
			iGreen = (dwColor >> (8+2 )) & 0x3F;
			iBlue  = (dwColor >> (0+3 )) & 0x1F;
			wColorTmp = (unsigned short)((iRed << 11) | (iGreen << 5) | iBlue);
			
			for (iCount = 0; iCount < mFBMemSize; iCount += 2) {
				
				*pwFbMem_16 = wColorTmp;
				pwFbMem_16++;
			}
			
			break;
		}

		case 32:
		{
			for (iCount = 0; iCount < mFBMemSize; iCount += 4) {
				
				*pdwFbMem_32 = dwColor;
				pdwFbMem_32++;
			}
		}

		default :
		{
			fprintf(stderr, "can't support %d bpp!\n", mScreenInfo.iBpp);
			return -1;
		}
			
	}
	

	return 0;

}



/* 两个显存的融合, X Y 是相对坐标 而不是LCD的绝对坐标 */
int FrameBuffer::FbVideoMemFusion(int dwX, int dwY, struct PixelDatas& tDestPixelData, struct PixelDatas& tSrcPixelData)
{
	int iY;
	int iWidth = tSrcPixelData.iWidth;
	int iHeight = tSrcPixelData.iHeight;
	int iLineBytes = 0;
	char *pcSrc = NULL;
	char *pcDest = NULL;

	if ((dwX > tDestPixelData.iWidth) || (dwY > tDestPixelData.iHeight)) {
		/* 完全超出范围 */
		return 0;
	}


	/* 调整坐标 */
	if (dwX < 0) {
		dwX = 0;
	}

	if (dwY < 0) {
		dwY = 0;
	}

	if ((dwX + iWidth) > tDestPixelData.iWidth) {
		iWidth = tDestPixelData.iWidth - dwX;
	}
	
	if ((dwY + iHeight) > tDestPixelData.iHeight) {
		iHeight = tDestPixelData.iHeight - dwY;
	}

	/* 得到真实的拷贝一行的长度 */
	iLineBytes = iWidth * tSrcPixelData.iBpp / 8;

	pcSrc = tSrcPixelData.pcPixelDatas;
	pcDest = tDestPixelData.pcPixelDatas + dwX * tDestPixelData.iBpp / 8 + dwY * tDestPixelData.iLineBytes;

	for (iY = 0; iY < iHeight; iY++) {
		memcpy(pcDest, pcSrc, iLineBytes);
		pcSrc += tSrcPixelData.iLineBytes;
		pcDest += tDestPixelData.iLineBytes;
	}
	
	
	return 0;
}



void FrameBuffer::FbDrawBitMap(FontBitMap& tFontBitMap)
{

	int iX;
	int iY;
	int iBit;
	int i = 0;
	char ucByteBuf = '\0';


	if (tFontBitMap.iBpp == 1) {
		for (iY = tFontBitMap.iYTop; iY < tFontBitMap.iYMax; iY++) {
			i = (iY - tFontBitMap.iYTop) * tFontBitMap.iPitch;
			for (iX = tFontBitMap.iXLeft, iBit = 7; iX < tFontBitMap.iXMax; iX++) {
				if (iBit == 7) {
					ucByteBuf = tFontBitMap.pcBuffer[i++];
				}
				
				if (ucByteBuf & (1<<iBit)) {
					 FbShowPixel(iX, iY, 0xFFFFFFFF);
				}
				else {
					FbShowPixel(iX, iY, 0);
				}
				
				iBit--;
				if (iBit == -1) {
					iBit = 7;
				}
			}
		}
	}
	else if (tFontBitMap.iBpp == 8) {
		for (iY = tFontBitMap.iYTop; iY < tFontBitMap.iYMax; iY++)
			for (iX = tFontBitMap.iXLeft; iX < tFontBitMap.iXMax; iX++) {
				if (tFontBitMap.pcBuffer[i++]) {
					FbShowPixel(iX, iY, 0xFFFFFFFF);
				}
				else {
					FbShowPixel(iX, iY, 0);
				}
				
			}

	}
	else {
		fprintf(stderr, "ShowOneFont error, can't support %d bpp\n", tFontBitMap.iBpp);
	}


}




void FrameBuffer::FbDrawBitMap(struct PixelDatas& tDestPixelData, FontBitMap& tFontBitMap, unsigned int dwColor)
{

	int iX;
	int iY;
	int iBit;
	int i = 0;
	char ucByteBuf = '\0';


	if (tFontBitMap.iBpp == 1) {
		for (iY = tFontBitMap.iYTop; iY < tFontBitMap.iYMax; iY++) {
			i = (iY - tFontBitMap.iYTop) * tFontBitMap.iPitch;
			for (iX = tFontBitMap.iXLeft, iBit = 7; iX < tFontBitMap.iXMax; iX++) {
				if (iBit == 7) {
					ucByteBuf = tFontBitMap.pcBuffer[i++];
				}
				
				if (ucByteBuf & (1<<iBit)) {
					 FbShowPixel(tDestPixelData, iX, iY, dwColor);
				}
				else {
					//FbShowPixel(tDestPixelData, iX, iY, 0);
				}
				
				iBit--;
				if (iBit == -1) {
					iBit = 7;
				}
			}
		}
	}
	else if (tFontBitMap.iBpp == 8) {
		for (iY = tFontBitMap.iYTop; iY < tFontBitMap.iYMax; iY++)
			for (iX = tFontBitMap.iXLeft; iX < tFontBitMap.iXMax; iX++) {
				if (tFontBitMap.pcBuffer[i++]) {
					FbShowPixel(tDestPixelData, iX, iY, 0xFFFFFFFF);
				}
				else {
					FbShowPixel(tDestPixelData, iX, iY, 0);
				}
				
			}

	}
	else {
		fprintf(stderr, "ShowOneFont error, can't support %d bpp\n", tFontBitMap.iBpp);
	}


}


void FrameBuffer::CirclePoint(int iX, int iY, int iXC, int iYC, unsigned int dwColor)
{
	FbShowPixel(iX + iXC, iY + iYC, dwColor);
	FbShowPixel(iY + iXC, iX + iYC, dwColor);
	FbShowPixel(iY + iXC, -iX + iYC, dwColor);
	FbShowPixel( iX + iXC, -iY + iYC, dwColor);
	FbShowPixel(-iX + iXC, -iY + iYC, dwColor);
	FbShowPixel(-iY + iXC,-iX + iYC, dwColor);
	FbShowPixel(-iY + iXC, iX + iYC, dwColor);
	FbShowPixel(-iX + iXC, iY + iYC, dwColor);
}

void FrameBuffer::FbFastCircle(int iXC , int iYC , int iR, unsigned int dwColor)
{
	int iX;
	int iY;
	int iD;

	iX = 0;
	iY = iR;
	iD = -iR / 2;
	CirclePoint(iX, iY, iXC, iYC, dwColor);
	if(iR % 2 == 0) {
		while(iX < iY) {
			iX++;
			if(iD < 0)
				iD += iX;
			else {
				iY--;
				iD += iX - iY;
			}

			CirclePoint(iX, iY, iXC, iYC, dwColor);
		}
	}
	else {
		while(iX < iY) {
			iX++;
			if(iD < 0)
				iD += iX + 1;
			else {
				iY--;
				iD += iX - iY + 1;
			}

			CirclePoint(iX, iY, iXC, iYC, dwColor);
		}
	}
}

/* 在LCD上绘制矩形 */
int FrameBuffer::FbDrawRectangle(int iX, int iY, int iWidth, int iHight, unsigned int iColor)
{
	int i;
	int iLineWidth;
	int iPixelSize = mScreenInfo.iBpp / 8;
	char *pucDest = NULL;

	if (iX < 0)
		iX = 0;
	
	if (iY < 0)
		iY = 0;

	if ((iX + iWidth) > mScreenInfo.iXres)
		iWidth = mScreenInfo.iXres - iX;

	if ((iY + iHight) > mScreenInfo.iYres)
		iHight = mScreenInfo.iYres - iY;

	iLineWidth = iWidth * iPixelSize;
	pucDest = mtFrameBufferPixelData.pcPixelDatas + iX * iPixelSize + iY * mScreenInfo.iLineWidth;

	for (i = 0; i < iHight; i++) {
		memset(pucDest, iColor, iLineWidth);
		pucDest += mScreenInfo.iLineWidth;
	}
	
	return 0;

}

/* 此处X Y 是相对坐标 */
int FrameBuffer::FbDrawRectangle(struct PixelDatas& tDestPixelData, int iX, int iY, int iWidth, int iHight, unsigned int iColor)
{
	int i;
	int iLineWidth;
	int iPixelSize = mScreenInfo.iBpp / 8;
	char *pucDest = NULL;


	if (iX < 0)
		iX = 0;
	
	if (iY < 0)
		iY = 0;

	if ((iX + iWidth) > tDestPixelData.iWidth)
		iWidth = tDestPixelData.iWidth - iX;

	if ((iY + iHight) > tDestPixelData.iHeight)
		iHight = tDestPixelData.iHeight - iY;

	iLineWidth = iWidth * iPixelSize;
	pucDest = tDestPixelData.pcPixelDatas + iX * iPixelSize + iY * tDestPixelData.iLineBytes;

	for (i = 0; i < iHight; i++) {
		memset(pucDest, iColor, iLineWidth);
		pucDest += tDestPixelData.iLineBytes;
	}
	
	return 0;

}

int FrameBuffer::FbDrawLine(int iStartX, int iStartY, int iEndX, int EndY, unsigned int iColor)
{
	int tempX,tempY;
	
	if(iStartX > iEndX) {
		tempX = iStartX;
		iStartX = iEndX;
		iEndX = tempX;
		tempY = iStartY;
		iStartY = EndY;
		EndY = tempY;
	}

	int x,y,dx,dy;
	int p;
	int q;
	int i;
	double e,k;
	dx = iEndX - iStartX;
	dy = EndY - iStartY;
	
	if(dx == 0) {
		if(iStartY > EndY) {
			tempY = iStartY;
			iStartY = EndY;
			EndY = tempY;
		}
		for(p = 0;p < dy;p++) {
			FbShowPixel(iStartX, iStartY, iColor);
			iStartY = iStartY+1;
		}
		return 1;
	}

	if(dy == 0) {
		for(q = 0;q < dx;q++) {
			FbShowPixel(iStartX, iStartY, iColor);
			iStartX = iStartX + 1;
		}
		return 1;
	}

	k = (double)dy/dx;
	e = 0;
	x = iStartX;
	y = iStartY;


	for(i = 0;i < dx;i++) {
		if(k > 0) {
			FbShowPixel(x, y, iColor);
			x++;
			e = e + k;
			
			if(e - (int)e >= 0.5) {
				y = y + (int)e + 1;
				e = e - (int)e - 1;
			}			 
			else {
				y = y + (int)e;
				e = e - (int)e;
			}
		}
		else {
			 FbShowPixel(x, y, iColor);
			x++;
			e = e + k;
			if(e - (int)e <= -0.5) {
				y = y + (int)e - 1;
				e = e - (int)e + 1;
			}
			else {
				y = y + (int)e;
				e = e - (int)e;
			}
			
		}
		
		
	}
	
	return 0;

}

int FrameBuffer::FbDrawLine(struct PixelDatas& tDestPixelData, int iStartX, int iStartY, int iEndX, int EndY, unsigned int iColor)
{
	int tempX,tempY;
	
	if(iStartX > iEndX) {
		tempX = iStartX;
		iStartX = iEndX;
		iEndX = tempX;
		tempY = iStartY;
		iStartY = EndY;
		EndY = tempY;
	}

	int x,y,dx,dy;
	int p;
	int q;
	int i;
	double e,k;
	dx = iEndX - iStartX;
	dy = EndY - iStartY;
	
	if(dx == 0) {
		if(iStartY > EndY) {
			tempY = iStartY;
			iStartY = EndY;
			EndY = tempY;
		}
		for(p = 0;p < dy;p++) {
			FbShowPixel(tDestPixelData, iStartX, iStartY, iColor);
			iStartY = iStartY+1;
		}
		return 1;
	}

	if(dy == 0) {
		for(q = 0;q < dx;q++) {
			FbShowPixel(tDestPixelData, iStartX, iStartY, iColor);
			iStartX = iStartX + 1;
		}
		return 1;
	}

	k = (double)dy/dx;
	e = 0;
	x = iStartX;
	y = iStartY;


	for(i = 0;i < dx;i++) {
		if(k > 0) {
			FbShowPixel(tDestPixelData, x, y, iColor);
			x++;
			e = e + k;
			
			if(e - (int)e >= 0.5) {
				y = y + (int)e + 1;
				e = e - (int)e - 1;
			}			 
			else {
				y = y + (int)e;
				e = e - (int)e;
			}
		}
		else {
			 FbShowPixel(tDestPixelData, x, y, iColor);
			x++;
			e = e + k;
			if(e - (int)e <= -0.5) {
				y = y + (int)e - 1;
				e = e - (int)e + 1;
			}
			else {
				y = y + (int)e;
				e = e - (int)e;
			}
			
		}
		
		
	}
	
	return 0;

}



int FrameBuffer::CovertBGR2RGB(int iSrcWidth, int iSrcHeight, int iSrcBpp, int iDstBpp, char *pcSrcDatas, char *pcDstDatas)
{
	int dwRed;
	int dwGreen;
	int dwBlue;
	int dwColor;

	short *pwDstDatas16bpp = (short *)pcDstDatas;
	int   *pwDstDatas32bpp = (int *)pcDstDatas;

	int i;
	int j;
	unsigned int iSrcPos = 0;
	unsigned int iDstpos = 0;


	if (iSrcBpp == 8) {
		/* 二值图 */
		for (i = 0; i < iSrcHeight; i++)
			for (j = 0; j < iSrcWidth; j++) {
				if (pcSrcDatas[iSrcPos++]) {
					/* 白 */
					pwDstDatas16bpp[iDstpos++] = 0xFFFF;

				}
				else {
					/* 黑 */
					pwDstDatas16bpp[iDstpos++] = 0;
				}
			}

		return 0;
	}

	if (iSrcBpp != 24) {
		printf("bpp is %d : Not currently supported this bpp\n", iSrcBpp);
		return -1;
	}

	switch (iDstBpp) {
		case 16:
		{
			for (i = 0; i < iSrcHeight; i++)
				for (j = 0; j < iSrcWidth; j++) {
					dwBlue  = pcSrcDatas[iSrcPos++];
					dwGreen = pcSrcDatas[iSrcPos++];
					dwRed   = pcSrcDatas[iSrcPos++];
					
					if (iDstBpp == 32) {
						dwColor = (dwRed << 16) | (dwGreen << 8) | dwBlue;
						*pwDstDatas32bpp = dwColor;
						pwDstDatas32bpp++;
					}
					else if (iDstBpp == 16) {
						/* 565 */
						dwRed   = dwRed >> 3;
						dwGreen = dwGreen >> 2;
						dwBlue  = dwBlue >> 3;
						dwColor = (dwRed << 11) | (dwGreen << 5) | (dwBlue);
						*pwDstDatas16bpp = dwColor;
						pwDstDatas16bpp++;
					}
				}
			break;
		}
		case 24:
		{
			for (i = 0; i < iSrcHeight; i++)
				for (j = 0; j < iSrcWidth; j++) {				
					dwBlue 	= pcSrcDatas[iSrcPos++];
					dwGreen = pcSrcDatas[iSrcPos++];
					dwRed	= pcSrcDatas[iSrcPos++];

					pcDstDatas[iDstpos++] = dwRed;
					pcDstDatas[iDstpos++] = dwGreen;
					pcDstDatas[iDstpos++] = dwBlue;				
				}		
			break;
		}

	}

	
	return 0;
}


ScreenInfo FrameBuffer::getScreenInfo(void)
{
	return mScreenInfo;
}

PixelDatas& FrameBuffer::getFrameBufferPixelData(void)
{
	return mtFrameBufferPixelData;
}


