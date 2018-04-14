#ifndef __TIMEVIEW_H__
#define __TIMEVIEW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class TimeView;


#include <view/view.h>

#define TIME_VIEW_ICON "/res/view/timeview.png"

class TimeView : public View{
public:	
	TimeView(std::string iconPathName, ViewInfo& viewInfo);
	virtual int process(struct CookEvent& cookEvent);
private:
	TimeView();
};



#endif //__TIMEVIEW_H__





