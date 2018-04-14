#include <pic/jpgparser.h>

typedef struct MyErrorMgr
{
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
}T_MyErrorMgr, *PT_MyErrorMgr;


static void MyErrorExit(j_common_ptr ptCInfo)
{
    static char errStr[JMSG_LENGTH_MAX];
    
	PT_MyErrorMgr ptMyErr = (PT_MyErrorMgr)ptCInfo->err;

    /* Create the message */
    (*ptCInfo->err->format_message) (ptCInfo, errStr);

	longjmp(ptMyErr->setjmp_buffer, 1);
}


bool JpgParser::isSupport(void)
{
	struct jpeg_decompress_struct tDInfo;

	T_MyErrorMgr tJerr;
	int iRet;
	
	fseek(getFp(), 0, SEEK_SET);

	tDInfo.err = jpeg_std_error(&tJerr.pub);
	tJerr.pub.error_exit = MyErrorExit;

	if(setjmp(tJerr.setjmp_buffer))
	{
		jpeg_destroy_decompress(&tDInfo);
		return false;
	}
	
	jpeg_create_decompress(&tDInfo);

	jpeg_stdio_src(&tDInfo, getFp());

	iRet = jpeg_read_header(&tDInfo, TRUE);
	jpeg_abort_decompress(&tDInfo);

	return (iRet == JPEG_HEADER_OK);
}


int JpgParser::getPixelData(int iBpp)
{
	struct jpeg_decompress_struct tDInfo;
	T_MyErrorMgr tJerr;
	PixelDatas& PicPixelDatas = getPicPixelDatas();

	int iRet;
	int iRowStride;
	char *pcLineBuffer = NULL;
	char *pucDest = NULL;


	fseek(getFp(), 0, SEEK_SET);

	tDInfo.err				 = jpeg_std_error(&tJerr.pub);
	tJerr.pub.error_exit	 = MyErrorExit;

	if(setjmp(tJerr.setjmp_buffer)) {
		fclose(getFp());
		jpeg_destroy_decompress(&tDInfo);
		if (pcLineBuffer) {
			delete pcLineBuffer;
		}
		if (PicPixelDatas.pcPixelDatas) {
			delete PicPixelDatas.pcPixelDatas;
		}
		fprintf(stderr, "mjpg error!\n");
		return -1;
	}

	jpeg_create_decompress(&tDInfo);

	jpeg_stdio_src(&tDInfo, getFp());

	iRet = jpeg_read_header(&tDInfo, TRUE);

	tDInfo.scale_num = tDInfo.scale_denom = 1;
	
	jpeg_start_decompress(&tDInfo);

	
	iRowStride = tDInfo.output_width * tDInfo.output_components;
	pcLineBuffer = new char[iRowStride];
	if (NULL == pcLineBuffer) {
		fclose(getFp());
		jpeg_destroy_decompress(&tDInfo);
		fprintf(stderr, "malloc for pcLineBuffer error!\n");
		return -1;
	}

	PicPixelDatas.iWidth  = tDInfo.output_width;
	PicPixelDatas.iHeight = tDInfo.output_height;
	PicPixelDatas.iBpp	= iBpp;
	PicPixelDatas.iLineBytes	= PicPixelDatas.iWidth * PicPixelDatas.iBpp / 8;
	PicPixelDatas.iTotalBytes	= PicPixelDatas.iHeight * PicPixelDatas.iLineBytes;
	PicPixelDatas.pcPixelDatas = new char[PicPixelDatas.iTotalBytes];
	if (NULL == PicPixelDatas.pcPixelDatas) {
		delete pcLineBuffer;
		fclose(getFp());
		jpeg_finish_decompress(&tDInfo);	
		jpeg_destroy_decompress(&tDInfo);		
		fprintf(stderr, "malloc for PicPixelDatas.pcPixelDatas error!\n");
		return -1;
	}

	pucDest = PicPixelDatas.pcPixelDatas;

	while (tDInfo.output_scanline < tDInfo.output_height) {
		(void) jpeg_read_scanlines(&tDInfo, (JSAMPLE**)&pcLineBuffer, 1);

		CovertOneLine(PicPixelDatas.iWidth, 24, PicPixelDatas.iBpp, pcLineBuffer, pucDest);
		pucDest += PicPixelDatas.iLineBytes;
	}
	
	delete pcLineBuffer;

	jpeg_finish_decompress(&tDInfo);
	jpeg_destroy_decompress(&tDInfo);

	return 0;	
}



int JpgParser::CovertOneLine(int iWidth, int iSrcBpp, int iDstBpp, char *pcSrcDatas, char *pcDstDatas)
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


