#ifndef __SWITCHVIEW_H__
#define __SWITCHVIEW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class SwitchView;


#include <view/view.h>

#define SWITCHOPEN_VIEW_ICON  "/res/view/switchopenview.png"
#define SWITCHCLOSE_VIEW_ICON "/res/view/switchcloseview.png"

class SwitchView : public View{
public:	
	SwitchView(std::string iconPathName, ViewInfo& viewInfo);
	virtual int process(struct CookEvent& cookEvent);
private:
	SwitchView();
};



#endif //__SWITCHVIEW_H__






