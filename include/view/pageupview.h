#ifndef __PAGEUPVIEW_H__
#define __PAGEUPVIEW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class PageUpView;


#include <view/view.h>

#define PAGEUP_VIEW_ICON "/res/view/pageupview.png"

class PageUpView : public View{
public:	
	PageUpView(std::string iconPathName, ViewInfo& viewInfo);
	virtual int process(struct CookEvent& cookEvent);
private:
	PageUpView();
};



#endif //__PAGEUPVIEW_H__






