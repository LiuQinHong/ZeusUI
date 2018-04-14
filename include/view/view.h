#ifndef __VIEW_H__
#define __VIEW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
struct ViewInfo;
class View;


#include <iostream>
#include <device/framebuffer.h>
#include <pic/picparser.h>
#include <window/window.h>
#include <input/inputreader.h>
#include <windowmanager.h>


struct ViewInfo {
	int iViewID;
	int iX;	//相对窗口的坐标
	int iY;	//相对窗口的坐标
	int iWidth;
	int iHeight;
	int iCareEventType; //这个window关心的事件类型
	struct PixelDatas tZoomPixelDatas;
	struct PixelDatas tOrgPixelDatas;	//保存了显示view之前的那块区域的图像数据
	bool mSelected;		//是否被选中过
	bool mRemovable;	//是否可移动
	bool mPressed;		//是否被按下过
	bool mRemoved;		//是否移动
};


class View {
public:
	View(const std::string& iconPathName, ViewInfo& viewInfo);
	virtual ~View();
	ViewInfo& getViewInfoRef(void);
	std::string& getIconPathName(void);
	void showViewToWindow(void);
	void setCareEvent(int eventType);
	void cancelCareEvent(int eventType);
	bool isCareThisEvent(int eventType);
	void setParentWindow(Window *mParentWindow);
	Window *getParentWindow(void);
	void changeIconImage(const std::string& iconPathName);
	int reversalPixelData(void);
	bool isPressed(void);	
	void setPressed(bool pressed);
	bool isSupport(int iX, int iY);
	void setTargetFileName(const std::string& strTargetFileName);
	std::string& getTargetFileName(void);
	int onProcess(struct CookEvent& cookEvent);
	virtual int process(struct CookEvent& cookEvent) = 0;

private:
	std::string mIconPathName;
	std::string mTargetFileName;
	PicParser *mPicParser;
	Window *mParentWindow;
	struct ViewInfo mViewInfo;
	View();	
};



#endif //__VIEW_H__

