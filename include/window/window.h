#ifndef __WINDOW_H__
#define __WINDOW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
struct WindowInfo;
class Window;


#include <iostream>
#include <list>
#include <view/view.h>
#include <pic/picparser.h>
#include <input/inputreader.h>
#include <textparser.h>
#include <windowmanager.h>

struct WindowInfo {	
	int iWindowID;
	int iX;
	int iY;
	int iWidth;
	int iHeight;
	unsigned int iLevel;	//表明这个window在桌面上的优先级, Level越大优先级越高,最好由windowmanager分配
	int iCareEventType; //这个window关心的事件类型
	struct PixelDatas tWindowCurPixelDatas; //当前布局了view的window图像
	bool mSelected;		//是否被选中过
	bool mRemovable;	//是否可移动
	bool mPressed;		//是否被按下过
	bool mMoved;		//是否移动	
};


class Window {
public:
	Window();
	Window(const std::string& strTargetFileName);
	virtual ~Window();
	void placeTop(void);
	void setLevel(unsigned int iLevel);
	void setPoint(int iX, int iY, int iWidth, int iHeight);
	void setWindowID(int iWindowID);
	void setCareEvent(int eventType);
	void cancelCareEvent(int eventType);
	bool isCareThisEvent(int eventType);
	struct WindowInfo& getWindowInfo(void);
	int getViewSum(void);
	void addView(View *pView);
	void addFixView(View *pView);
	void showAllFixView(void);
	void printAllView(void);	
	void showAllView(void);
	void delView(View *pView);
	void removeView(View *pView);	
	void delAllView(void);
	void delViewBack(void);
	View *getViewForTargetFileName(std::string& strTargetFileName);
	std::string& getFileName(void);
	std::list<View*>& getViewList(void);
	int windowAndViewMerge(View *pView);
	void showWindow(void);
	void showInfoTitleBar(const std::string& Info, TextParser *textParser);
	void moveWindow(int iX, int iY);
	bool isRemovable(void);
	void setRemovable(bool removable);
	bool isMoved(void);	
	void setMoved(bool Moved);
	bool isSupport(int iX, int iY);
	bool isUpdateStatus(void);
	void setUpdateStatus(bool isUpdate);
	void setFirstEvent(struct CookEvent& cookEvent);
	struct CookEvent& getFirstEvent(void);
	bool isLongPress(struct timeval tTime);
	int dispatchEvent(struct CookEvent& cookEvent);
	virtual void drawWindowPixelData(void) = 0;	
	virtual int onProcess(struct CookEvent& cookEvent) = 0;
	
private:
	std::string strFileName;
	std::list<View*> viewList;
	std::list<View*> fixviewList;
	struct WindowInfo mtWindowInfo;
	View* mCurView;
	bool mIsUpdate;
	struct CookEvent mFirstEvent;	
};



#endif //__WINDOW_H__

