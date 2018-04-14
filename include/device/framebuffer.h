#ifndef __FRAME_BUFFER_H__
#define __FRAME_BUFFER_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
struct ScreenInfo;
class FrameBuffer;


#include <view/view.h>
#include <pic/picparser.h>
#include <fonts/fonts.h>


#define DISPALY_DEVICE "/dev/fb0"

struct ScreenInfo {
	int iXres;
	int iYres;
	int iBpp;
	int iLineWidth;
	int iFBMemSize;
};


class FrameBuffer {
public:
	~FrameBuffer();
	int FbDeviceInit(void);

	int FbShowPixel(int dwX,     int dwY, unsigned int dwColor);
	int FbShowPixel(struct PixelDatas& destPixelDatas, int dwX,     int dwY, unsigned int dwColor);

	int FbCleanScreen(unsigned int dwColor);

	int FbSetColour(struct PixelDatas& destPixelDatas, unsigned int dwColor);	

	int FbVideoMemFusion(int dwX,       int dwY, struct PixelDatas& tDestPixelData, struct PixelDatas& tSrcPixelData);

	void FbDrawBitMap(FontBitMap& tFontBitMap);
	void FbDrawBitMap(struct PixelDatas& tDestPixelData, FontBitMap& tFontBitMap, unsigned int dwColor);

	void CirclePoint(int iX, int iY, int iXC, int iYC, unsigned int dwColor);	
	void FbFastCircle(int iXC , int iYC , int iR, unsigned int dwColor);

	/* 绘制矩形 */
	int FbDrawRectangle(int iX, int iY, int iWidth, int iHight, unsigned int iColor);
	int FbDrawRectangle(struct PixelDatas& tDestPixelData, int iX, int iY, int iWidth, int iHight, unsigned int iColor);

	int FbDrawLine(int iStartX, int iStartY, int iEndX, int EndY, unsigned int iColor);
	int FbDrawLine(struct PixelDatas& tDestPixelData, int iStartX, int iStartY, int iEndX, int EndY, unsigned int iColor);	

	int CovertBGR2RGB(int iSrcWidth, int iSrcHeight, int iSrcBpp, int iDstBpp, char *pcSrcDatas, char *pcDstDatas);
	
	ScreenInfo getScreenInfo(void);
	PixelDatas& getFrameBufferPixelData(void);
	static FrameBuffer* getFrameBuffer(void);
	
private:	
	int iFd;
	int mFBMemSize;
	PixelDatas mtFrameBufferPixelData;
	ScreenInfo mScreenInfo;
	static FrameBuffer *frameBuffer;
	static pthread_mutex_t tMutex;
	/* 把构造函数私有化，避免使用构造函数构造另一个实例化对象 */
	FrameBuffer();
	

};


#endif // __FRAME_BUFFER_H__

