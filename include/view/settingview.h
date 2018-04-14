#ifndef __SETTINGVIEW_H__
#define __SETTINGVIEW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class SettingView;


#include <view/view.h>

#define SETTING_VIEW_ICON "/res/view/settingview.png"

class SettingView : public View{
public:	
	SettingView(std::string iconPathName, ViewInfo& viewInfo);
	virtual int process(struct CookEvent& cookEvent);
private:
	SettingView();
};



#endif //__SETTINGVIEW_H__


