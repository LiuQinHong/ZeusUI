#ifndef __PICWINDOW_H__
#define __PICWINDOW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class QQWindow;

#include <window/window.h>
#include <textparser.h>


class QQWindow : public Window{
public:	
	QQWindow();
	virtual void drawWindowPixelData(void);
	virtual int onProcess(struct CookEvent& cookEvent);
	
private:

};



#endif //__PICWINDOW_H__



