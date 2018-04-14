#ifndef __STARTWINDOW_H__
#define __STARTWINDOW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class StartWindow;

#include <window/window.h>


class StartWindow : public Window{
public:
	StartWindow();
	void recalculateViewLocation(void);	
	virtual void drawWindowPixelData(void);		
	virtual int onProcess(struct CookEvent& cookEvent);
};



#endif //__STARTWINDOW_H__


