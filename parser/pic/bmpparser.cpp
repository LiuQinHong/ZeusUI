#include <pic/bmpparser.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


typedef struct tagBITMAPFILEHEADER { /* bmfh */
	unsigned short bfType; 
	unsigned long  bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned long  bfOffBits;
}  __attribute__ ((packed)) BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER { /* bmih */
	unsigned long  biSize;
	unsigned long  biWidth;
	unsigned long  biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned long  biCompression;
	unsigned long  biSizeImage;
	unsigned long  biXPelsPerMeter;
	unsigned long  biYPelsPerMeter;
	unsigned long  biClrUsed;
	unsigned long  biClrImportant;
}  __attribute__ ((packed)) BITMAPINFOHEADER;


bool BmpParser::isSupport(void)
{
	char acFileHead[16];

	fseek(getFp(), 0, SEEK_SET);
	fread(acFileHead, sizeof(char), 16, getFp());   
	fseek(getFp(), 0, SEEK_SET);

	/* BMP 文件开头"B M" 两个字符分别对应 0x42 0x4d */
	if (acFileHead[0] != 0x42 || acFileHead[1] != 0x4d)
		return false;
	else
		return true;	
}
int BmpParser::getPixelData(int iBpp)
{
	BITMAPFILEHEADER *ptBITMAPFILEHEADER;
	BITMAPINFOHEADER *ptBITMAPINFOHEADER;

    char *pcFileData;
	PixelDatas& PicPixelDatas = getPicPixelDatas();
	
	int iWidth;
	int iHeight;
	int iBMPBpp;
	int iY;
    int iFileSize;

	char *pcSrc;
	char *pcDest;
	int iLineWidthAlign;
	int iLineWidthReal;

    fseek(getFp(), 0, SEEK_END);  
	iFileSize = ftell(getFp());
	fseek(getFp(), 0, SEEK_SET);
	
    pcFileData = new char[iFileSize];
	if (pcFileData == NULL) {
		fclose(getFp());		
		fprintf(stderr, "malloc for pcFileData error!\n");
		return -1;		
	}
	fread(pcFileData, 1, iFileSize, getFp());

	ptBITMAPFILEHEADER = (BITMAPFILEHEADER *)pcFileData;
	ptBITMAPINFOHEADER = (BITMAPINFOHEADER *)(pcFileData + sizeof(BITMAPFILEHEADER));

	iWidth = ptBITMAPINFOHEADER->biWidth;
	iHeight = ptBITMAPINFOHEADER->biHeight;
	iBMPBpp = ptBITMAPINFOHEADER->biBitCount;

	if (iBMPBpp != 24) {
		delete pcFileData;
		fclose(getFp());		
		fprintf(stderr, "iBMPBpp = %d\n", iBMPBpp);
		fprintf(stderr, "sizeof(BITMAPFILEHEADER) = %d\n", sizeof(BITMAPFILEHEADER));
		return -1;
	}

	PicPixelDatas.iWidth  = iWidth;
	PicPixelDatas.iHeight = iHeight;
	PicPixelDatas.iBpp    = iBpp;
	PicPixelDatas.iLineBytes    = iWidth * PicPixelDatas.iBpp / 8;
    PicPixelDatas.iTotalBytes   = PicPixelDatas.iHeight * PicPixelDatas.iLineBytes;

	PicPixelDatas.pcPixelDatas = new char[PicPixelDatas.iTotalBytes];
	if (NULL == PicPixelDatas.pcPixelDatas) {
		fprintf(stderr, "malloc for PicPixelDatas.iTotalBytes error!\n");
		delete pcFileData;
		fclose(getFp());
		return -1;
	}

	iLineWidthReal = iWidth * iBMPBpp / 8;
	iLineWidthAlign = (iLineWidthReal + 3) & ~0x3;   /* 向4取整 */
		
	pcSrc = pcFileData + ptBITMAPFILEHEADER->bfOffBits;
	pcSrc = pcSrc + (iHeight - 1) * iLineWidthAlign;

	pcDest = PicPixelDatas.pcPixelDatas;
	for (iY = 0; iY < iHeight; iY++) {		
		CovertOneLine(iWidth, iBMPBpp, PicPixelDatas.iBpp, pcSrc, pcDest);
		pcSrc  -= iLineWidthAlign;
		pcDest += PicPixelDatas.iLineBytes;
	}

	delete pcFileData;
	
	return 0;	

}

int BmpParser::CovertOneLine(int iWidth, int iSrcBpp, int iDstBpp, char *pcSrcDatas, char *pcDstDatas)
{
	int dwRed;
	int dwGreen;
	int dwBlue;
	int dwColor;

	short *pwDstDatas16bpp = (short *)pcDstDatas;
	int   *pwDstDatas32bpp = (int *)pcDstDatas;

	int i;
	int pos = 0;

	if (iSrcBpp != 24) {
		return -1;
	}

	if (iDstBpp == 24) {
		memcpy(pcDstDatas, pcSrcDatas, iWidth*3);
	}
	else {
		for (i = 0; i < iWidth; i++) {
			dwBlue  = pcSrcDatas[pos++];
			dwGreen = pcSrcDatas[pos++];
			dwRed   = pcSrcDatas[pos++];
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
	}
	return 0;	
}

