#include <pic/pngparser.h>
#include <png.h>



int PngParser::CovertOneLine(int iWidth, int iSrcBpp, int iDstBpp, char *pcSrcDatas, char *pcDstDatas)
{
	int dwRed;
	int dwGreen;
	int dwBlue;
	int dwColor;

	short *pwDstDatas16bpp = (short *)pcDstDatas;
	int   *pwDstDatas32bpp = (int *)pcDstDatas;

	int i;
	int pos = 0;

	if (iSrcBpp == 32) {
		if (iDstBpp == 32) {
			memcpy(pcDstDatas, pcSrcDatas, iWidth * 4);
		}
		else {
			for (i = 0; i < iWidth; i++) {
				dwRed	= pcSrcDatas[pos++];
				dwGreen = pcSrcDatas[pos++];
				dwBlue	= pcSrcDatas[pos++];
				pos++; //滑过ALPHA通道
				if (iDstBpp == 24) {
					dwColor = (dwRed << 16) | (dwGreen << 8) | dwBlue;
					*pwDstDatas32bpp = dwColor;
					pwDstDatas32bpp++;
				}
				else if (iDstBpp == 16) {
					/* 565 */
					dwRed	= dwRed >> 3;
					dwGreen = dwGreen >> 2;
					dwBlue	= dwBlue >> 3;
					dwColor = (dwRed << 11) | (dwGreen << 5) | (dwBlue);
					*pwDstDatas16bpp = dwColor;
					pwDstDatas16bpp++;
				}
			}
		}		
	}
	else if (iSrcBpp == 24) {
		if (iDstBpp == 24) {
			memcpy(pcDstDatas, pcSrcDatas, iWidth * 3);
		}
		else {
			for (i = 0; i < iWidth; i++) {
				dwRed	= pcSrcDatas[pos++];
				dwGreen = pcSrcDatas[pos++];
				dwBlue	= pcSrcDatas[pos++];
				if (iDstBpp == 32) {
					dwColor = (dwRed << 16) | (dwGreen << 8) | dwBlue;
					*pwDstDatas32bpp = dwColor;
					pwDstDatas32bpp++;
				}
				else if (iDstBpp == 16) {
					/* 565 */
					dwRed	= dwRed >> 3;
					dwGreen = dwGreen >> 2;
					dwBlue	= dwBlue >> 3;
					dwColor = (dwRed << 11) | (dwGreen << 5) | (dwBlue);
					*pwDstDatas16bpp = dwColor;
					pwDstDatas16bpp++;
				}
			}
		}

	}



	return 0;
}


bool PngParser::isSupport(void)
{

	char acFileHead[8];

	fseek(getFp(), 0, SEEK_SET);
	fread(acFileHead, sizeof(char), 8, getFp());   
	fseek(getFp(), 0, SEEK_SET);

	/* 传入的字节数越多越准确，返回0表示是PNG文件 */
    if (png_sig_cmp((const png_byte*)acFileHead, 0, 8)) {
       return false;
    }

	return true;
}

int PngParser::getPixelData(int iBpp)
{
	int iY;
	png_infop info_ptr;			//图片信息的结构体
	png_structp png_ptr;		//初始化结构体，初始生成，调用api时注意传入
	PixelDatas& PicPixelDatas = getPicPixelDatas();
	char *pucDest;
	int iSize;
	
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);	//创建初始化libpng库结构体
	info_ptr = png_create_info_struct(png_ptr); 	//创建图片信息结构体

	//设置错误的返回点
    if (setjmp(png_jmpbuf(png_ptr))) { 
		//png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		/* 没有创建 end_info     */
		fclose(getFp());		
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		return -1; 
    } 	

	png_init_io(png_ptr, getFp());	//把文件信息加载到libpng库结构体中
	/* 读文件内容到info_ptr中 */
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);


	/* 保存一行所需的字节数 */
	PicPixelDatas.iLineBytes = png_get_rowbytes(png_ptr, info_ptr); 
	PicPixelDatas.iWidth = png_get_image_width(png_ptr, info_ptr);
	PicPixelDatas.iHeight = png_get_image_height(png_ptr, info_ptr);
	PicPixelDatas.iBpp = iBpp;
	PicPixelDatas.iTotalBytes = PicPixelDatas.iLineBytes * PicPixelDatas.iHeight;

	iSize = PicPixelDatas.iHeight * PicPixelDatas.iLineBytes;
	PicPixelDatas.pcPixelDatas = new char[iSize];
	if (NULL == PicPixelDatas.pcPixelDatas) {
		fclose(getFp());
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		printf("%s, %d, PicPixelDatas.pcPixelDatas == NULL\n", __FUNCTION__, __LINE__);
		return -1;
	}

	/* row_pointers是指向每行像素数据的指针数组 	 */
	png_bytep *row_pointers = png_get_rows(png_ptr, info_ptr);
	pucDest = PicPixelDatas.pcPixelDatas;
	for (iY = 0; iY < PicPixelDatas.iHeight; iY++) {
		CovertOneLine(PicPixelDatas.iWidth, PicPixelDatas.iLineBytes / PicPixelDatas.iWidth * 8, PicPixelDatas.iBpp, (char *)row_pointers[iY], pucDest);
		pucDest += PicPixelDatas.iLineBytes;
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	return 0;
}

