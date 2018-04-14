#ifndef __CAMERAWINDOW_H__
#define __CAMERAWINDOW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class CameraWindow;
struct VideoDevice;
struct VideoBuf;


#include <window/window.h>
#include <linux/videodev2.h>
#include <convert/convert.h>
#include <textparser.h>
#define NB_BUFFER 4


struct VideoDevice {
    int iFd;
    int iPixelFormat;
    int iWidth;
    int iHeight;

    int iVideoBufCnt;
    int iVideoBufMaxLen;
    int iVideoBufCurIndex;
    char *pucVideBuf[NB_BUFFER];

};

struct VideoBuf {
	struct PixelDatas tPixelDatas;
    int iPixelFormat;
};

struct CameraThread {
	pthread_t tTreadID;
	int iPixelFormatOfDisp;
	int iPixelFormatOfVideo;
	struct VideoDevice *ptVideoDevice;
	Window *window;
	Convert *convert;	
};

class CameraWindow : public Window{
public:	
	CameraWindow(const std::string& strDeviceName);
	~CameraWindow();
    int initCameraDevice(void);
    int exitCameraDevice(void);
    int getFrame(struct VideoBuf& tVideoBuf);
    int getFormat(void);
    int putFrame(struct VideoBuf& tVideoBuf);
    int startCameraDevice(void);
    int stopCameraDevice(void);
	Convert* selectVideoConvertForFormats(int iPixelFormatIn, int iPixelFormatOut);
	struct VideoBuf& getConvertBuf(void);
	virtual void drawWindowPixelData(void);
	virtual int onProcess(struct CookEvent& cookEvent);
private:
	CameraWindow();
	struct VideoDevice mtVideoDevice;
	struct VideoBuf mtConvertBuf;
	std::string mStrDeviceName;
	struct CameraThread mCameraThread;
	Convert *mConvert;
};



#endif //__CAMERAWINDOW_H__




