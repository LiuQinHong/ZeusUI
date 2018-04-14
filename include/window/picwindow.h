#ifndef __PICWINDOW_H__
#define __PICWINDOW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class PicWindow;

#include <window/window.h>
#include <textparser.h>
#include <opencv.h>


class PicWindow : public Window{
public:	
	PicWindow(const std::string& strTargetFileName);
	PicWindow(Mat& image);
	int showPic(void);	
	int showPic(Mat& image);	
	std::string& getPicFileName(void);
	virtual void drawWindowPixelData(void);
	virtual int onProcess(struct CookEvent& cookEvent);
private:
	std::string strPicFileName;
	Mat mImage;
	struct PixelDatas tPixelDatas; //显示图片的数据
	PicWindow();
};



#endif //__PICWINDOW_H__



