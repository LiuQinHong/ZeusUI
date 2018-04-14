#include <convert/convert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h> 
#include <setjmp.h>
#include "color.h"
#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"
#include <jpeglib.h>


void Convert::setStrName(const std::string& strName)
{
	mStrName = strName;
}

std::string& Convert::getStrName(void)
{
	return mStrName;
}



typedef struct MyErrorMgr {
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
}T_MyErrorMgr, *PT_MyErrorMgr;
extern void jpeg_mem_src_tj(j_decompress_ptr, unsigned char *, unsigned long);



int Mjpeg2rgb::isSupport(int iPixelFormatIn, int iPixelFormatOut)
{	
	if (!((iPixelFormatIn == V4L2_PIX_FMT_JPEG) || (iPixelFormatIn == V4L2_PIX_FMT_MJPEG)))
		return 0;

	if (iPixelFormatOut == V4L2_PIX_FMT_RGB565)
		return 1;
	
	if (iPixelFormatOut == V4L2_PIX_FMT_RGB32)
		return 1;

	return 0;
}

static void MyErrorExit(j_common_ptr ptCInfo)
{
    static char errStr[JMSG_LENGTH_MAX];
    
	PT_MyErrorMgr ptMyErr = (PT_MyErrorMgr)ptCInfo->err;

    /* Create the message */
    (*ptCInfo->err->format_message) (ptCInfo, errStr);

	longjmp(ptMyErr->setjmp_buffer, 1);
}





static int CovertOneLine(int iWidth, int iSrcBpp, int iDstBpp, char *pcSrcDatas, char *pcDstDatas)
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
		memcpy(pcDstDatas, pcSrcDatas, iWidth * 3);
	}
	else {
		for (i = 0; i < iWidth; i++) {
			dwRed   = pcSrcDatas[pos++];
			dwGreen = pcSrcDatas[pos++];
			dwBlue  = pcSrcDatas[pos++];
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


int Mjpeg2rgb::convert(struct VideoBuf& tVideoBufIn, struct VideoBuf& tVideoBufOut)
{
	struct jpeg_decompress_struct tDInfo;
	//struct jpeg_error_mgr tJErr;
	int iRet;
	int iRowStride;
	char *pcLineBuffer = NULL;
	char *pcDest;
	T_MyErrorMgr tJerr;
	struct PixelDatas *ptPixelDatas = &tVideoBufOut.tPixelDatas;

	// 分配和初始化一个decompression结构体
	//tDInfo.err = jpeg_std_error(&tJErr);

	tDInfo.err				 = jpeg_std_error(&tJerr.pub);
	tJerr.pub.error_exit	 = MyErrorExit;

	if(setjmp(tJerr.setjmp_buffer)) {
		/* 如果程序能运行到这里, 表示JPEG解码出错 */
		jpeg_destroy_decompress(&tDInfo);
		if (pcLineBuffer) {
			delete pcLineBuffer;
			pcLineBuffer = NULL;
		}
		if (ptPixelDatas->pcPixelDatas) {
			delete ptPixelDatas->pcPixelDatas;
			ptPixelDatas->pcPixelDatas = NULL;
		}
		return -1;
	}

	jpeg_create_decompress(&tDInfo);

	// 用jpeg_read_header获得jpg信息
	//jpeg_stdio_src(&tDInfo, ptFileMap->tFp);
	/* 把数据设为内存中的数据 */
	jpeg_mem_src_tj (&tDInfo, (unsigned char *)tVideoBufIn.tPixelDatas.pcPixelDatas, tVideoBufIn.tPixelDatas.iTotalBytes);
	

	iRet = jpeg_read_header(&tDInfo, TRUE);

	// 设置解压参数,比如放大、缩小
	tDInfo.scale_num = tDInfo.scale_denom = 1;
	
	// 启动解压：jpeg_start_decompress 
	jpeg_start_decompress(&tDInfo);
	
	// 一行的数据长度
	iRowStride = tDInfo.output_width * tDInfo.output_components;
	pcLineBuffer = new char[iRowStride];

	if (NULL == pcLineBuffer) {
		return -1;
	}

	ptPixelDatas->iWidth  = tDInfo.output_width;
	ptPixelDatas->iHeight = tDInfo.output_height;
	//ptPixelDatas->iBpp	= iBpp;
	ptPixelDatas->iLineBytes	= ptPixelDatas->iWidth * ptPixelDatas->iBpp / 8;
	ptPixelDatas->iTotalBytes	= ptPixelDatas->iHeight * ptPixelDatas->iLineBytes;
	if (NULL == ptPixelDatas->pcPixelDatas) {
		ptPixelDatas->pcPixelDatas = new char[ptPixelDatas->iTotalBytes];
	}

	pcDest = ptPixelDatas->pcPixelDatas;

	// 循环调用jpeg_read_scanlines来一行一行地获得解压的数据
	while (tDInfo.output_scanline < tDInfo.output_height) {
		/* 得到一行数据,里面的颜色格式为0xRR, 0xGG, 0xBB */
		(void) jpeg_read_scanlines(&tDInfo, (JSAMPLE**)&pcLineBuffer, 1);

		// 转到ptPixelDatas去
		CovertOneLine(ptPixelDatas->iWidth, 24, ptPixelDatas->iBpp, pcLineBuffer, pcDest);
		pcDest += ptPixelDatas->iLineBytes;
	}
	
	delete pcLineBuffer;
	pcLineBuffer = NULL;
	jpeg_finish_decompress(&tDInfo);
	jpeg_destroy_decompress(&tDInfo);

	return 0;
}


int Mjpeg2rgb::convertExit(struct VideoBuf& tVideoBufOut)
{
	if (tVideoBufOut.tPixelDatas.pcPixelDatas) {
		delete tVideoBufOut.tPixelDatas.pcPixelDatas;
		tVideoBufOut.tPixelDatas.pcPixelDatas = NULL;
	}
	return 0;
}





int Rgb2rgb::isSupport(int iPixelFormatIn, int iPixelFormatOut)
{
	if (iPixelFormatIn != V4L2_PIX_FMT_RGB565)
		return 0;

	if ((iPixelFormatOut != V4L2_PIX_FMT_RGB565) && (iPixelFormatOut != V4L2_PIX_FMT_RGB32))
		return 0;

	return 1;
}


int Rgb2rgb::convert(struct VideoBuf& tVideoBufIn, struct VideoBuf& tVideoBufOut)
{	
	struct PixelDatas *ptPixelDatasIn  = &tVideoBufIn.tPixelDatas;
	struct PixelDatas *ptPixelDatasOut = &tVideoBufOut.tPixelDatas;

	int x, y;
	int r, g, b;
	int color;
	unsigned short *pwSrc = (unsigned short *)ptPixelDatasIn->pcPixelDatas;
	unsigned int *pdwDest;

	if (tVideoBufIn.iPixelFormat != V4L2_PIX_FMT_RGB565) {
		return -1;
	}

	if (tVideoBufOut.iPixelFormat == V4L2_PIX_FMT_RGB565) {
		ptPixelDatasOut->iWidth  = ptPixelDatasIn->iWidth;
		ptPixelDatasOut->iHeight = ptPixelDatasIn->iHeight;
		ptPixelDatasOut->iBpp	 = 16;
		ptPixelDatasOut->iLineBytes  = ptPixelDatasOut->iWidth * ptPixelDatasOut->iBpp / 8;
		ptPixelDatasOut->iTotalBytes = ptPixelDatasOut->iLineBytes * ptPixelDatasOut->iHeight;
		if (!ptPixelDatasOut->pcPixelDatas) {
			ptPixelDatasOut->pcPixelDatas = new char[ptPixelDatasOut->iTotalBytes];
		}
		
		memcpy(ptPixelDatasOut->pcPixelDatas, ptPixelDatasIn->pcPixelDatas, ptPixelDatasOut->iTotalBytes);
		return 0;
	}
	else if (tVideoBufOut.iPixelFormat == V4L2_PIX_FMT_RGB32) {
		ptPixelDatasOut->iWidth  = ptPixelDatasIn->iWidth;
		ptPixelDatasOut->iHeight = ptPixelDatasIn->iHeight;
		ptPixelDatasOut->iBpp	 = 32;
		ptPixelDatasOut->iLineBytes  = ptPixelDatasOut->iWidth * ptPixelDatasOut->iBpp / 8;
		ptPixelDatasOut->iTotalBytes = ptPixelDatasOut->iLineBytes * ptPixelDatasOut->iHeight;
		if (!ptPixelDatasOut->pcPixelDatas) {
			ptPixelDatasOut->pcPixelDatas = new char[ptPixelDatasOut->iTotalBytes];
		}

		pdwDest = (unsigned int *)ptPixelDatasOut->pcPixelDatas;
		
		for (y = 0; y < ptPixelDatasOut->iHeight; y++) {
			for (x = 0; x < ptPixelDatasOut->iWidth; x++) {
				color = *pwSrc++;

				r = color >> 11;
				g = (color >> 5) & (0x3f);
				b = color & 0x1f;

				color = ((r << 3) << 16) | ((g << 2) << 8) | (b << 3);

				*pdwDest = color;
				pdwDest++;
			}
		}
		return 0;
	}

	return -1;
}


int Rgb2rgb::convertExit(struct VideoBuf& tVideoBufOut)
{
	if (tVideoBufOut.tPixelDatas.pcPixelDatas) {
		delete tVideoBufOut.tPixelDatas.pcPixelDatas;
		tVideoBufOut.tPixelDatas.pcPixelDatas = NULL;
	}
	return 0;
}



Yuv2rgb::Yuv2rgb()
{
	initLut();
}

Yuv2rgb::~Yuv2rgb()
{
	freeLut();
}

int Yuv2rgb::isSupport(int iPixelFormatIn, int iPixelFormatOut)
{
	if (iPixelFormatIn != V4L2_PIX_FMT_YUYV)
		return 0;
	if ((iPixelFormatOut != V4L2_PIX_FMT_RGB565) && (iPixelFormatOut != V4L2_PIX_FMT_RGB32))
	{
		return 0;
	}
	return 1;
}





/* translate YUV422Packed to rgb24 */

static unsigned int
Pyuv422torgb565(unsigned char * input_ptr, unsigned char * output_ptr, unsigned int image_width, unsigned int image_height)
{
	unsigned int i, size;
	unsigned char Y, Y1, U, V;
	unsigned char *buff = input_ptr;
	unsigned char *output_pt = output_ptr;

    unsigned int r, g, b;
    unsigned int color;
    
	size = image_width * image_height /2;
	for (i = size; i > 0; i--) {
		/* bgr instead rgb ?? */
		Y = buff[0] ;
		U = buff[1] ;
		Y1 = buff[2];
		V = buff[3];
		buff += 4;
		r = R_FROMYV(Y,V);
		g = G_FROMYUV(Y,U,V); //b
		b = B_FROMYU(Y,U); //v

        /* 把r,g,b三色构造为rgb565的16位值 */
        r = r >> 3;
        g = g >> 2;
        b = b >> 3;
        color = (r << 11) | (g << 5) | b;
        *output_pt++ = color & 0xff;
        *output_pt++ = (color >> 8) & 0xff;
			
		r = R_FROMYV(Y1,V);
		g = G_FROMYUV(Y1,U,V); //b
		b = B_FROMYU(Y1,U); //v
		
        /* 把r,g,b三色构造为rgb565的16位值 */
        r = r >> 3;
        g = g >> 2;
        b = b >> 3;
        color = (r << 11) | (g << 5) | b;
        *output_pt++ = color & 0xff;
        *output_pt++ = (color >> 8) & 0xff;
	}
	
	return 0;
} 



/* translate YUV422Packed to rgb24 */

static unsigned int
Pyuv422torgb32(unsigned char * input_ptr, unsigned char * output_ptr, unsigned int image_width, unsigned int image_height)
{
	unsigned int i, size;
	unsigned char Y, Y1, U, V;
	unsigned char *buff = input_ptr;
	unsigned int *output_pt = (unsigned int *)output_ptr;

    unsigned int r, g, b;
    unsigned int color;

	size = image_width * image_height /2;
	for (i = size; i > 0; i--) {
		/* bgr instead rgb ?? */
		Y = buff[0] ;
		U = buff[1] ;
		Y1 = buff[2];
		V = buff[3];
		buff += 4;

        r = R_FROMYV(Y,V);
		g = G_FROMYUV(Y,U,V); //b
		b = B_FROMYU(Y,U); //v
		/* rgb888 */
		color = (r << 16) | (g << 8) | b;
        *output_pt++ = color;
			
		r = R_FROMYV(Y1,V);
		g = G_FROMYUV(Y1,U,V); //b
		b = B_FROMYU(Y1,U); //v
		color = (r << 16) | (g << 8) | b;
        *output_pt++ = color;
	}
	
	return 0;
} 


int Yuv2rgb::convert(struct VideoBuf& tVideoBufIn, struct VideoBuf& tVideoBufOut)
{
	struct PixelDatas *ptPixelDatasIn  = &tVideoBufIn.tPixelDatas;
	struct PixelDatas *ptPixelDatasOut = &tVideoBufOut.tPixelDatas;

	ptPixelDatasOut->iWidth  = ptPixelDatasIn->iWidth;
	ptPixelDatasOut->iHeight = ptPixelDatasIn->iHeight;
	
	if (tVideoBufOut.iPixelFormat == V4L2_PIX_FMT_RGB565) {
		ptPixelDatasOut->iBpp = 16;
		ptPixelDatasOut->iLineBytes  = ptPixelDatasOut->iWidth * ptPixelDatasOut->iBpp / 8;
		ptPixelDatasOut->iTotalBytes = ptPixelDatasOut->iLineBytes * ptPixelDatasOut->iHeight;

		if (!ptPixelDatasOut->pcPixelDatas) {
			ptPixelDatasOut->pcPixelDatas = new char[ptPixelDatasOut->iTotalBytes];
		}
		
		Pyuv422torgb565((unsigned char *)ptPixelDatasIn->pcPixelDatas, (unsigned char *)ptPixelDatasOut->pcPixelDatas, ptPixelDatasOut->iWidth, ptPixelDatasOut->iHeight);
		return 0;
	}
	else if (tVideoBufOut.iPixelFormat == V4L2_PIX_FMT_RGB32) {
		ptPixelDatasOut->iBpp = 32;
		ptPixelDatasOut->iLineBytes  = ptPixelDatasOut->iWidth * ptPixelDatasOut->iBpp / 8;
		ptPixelDatasOut->iTotalBytes = ptPixelDatasOut->iLineBytes * ptPixelDatasOut->iHeight;

		if (!ptPixelDatasOut->pcPixelDatas) {
			ptPixelDatasOut->pcPixelDatas = new char[ptPixelDatasOut->iTotalBytes];
		}
		
		Pyuv422torgb32((unsigned char *)ptPixelDatasIn->pcPixelDatas, (unsigned char *)ptPixelDatasOut->pcPixelDatas, ptPixelDatasOut->iWidth, ptPixelDatasOut->iHeight);
		return 0;
	}
	
	return -1;
}


int Yuv2rgb::convertExit(struct VideoBuf& tVideoBufOut)
{
	if (tVideoBufOut.tPixelDatas.pcPixelDatas) {
		delete tVideoBufOut.tPixelDatas.pcPixelDatas;
		tVideoBufOut.tPixelDatas.pcPixelDatas = NULL;
	}
	return 0;
}



