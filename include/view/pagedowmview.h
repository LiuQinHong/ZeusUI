#ifndef __PAGEDOWMVIEW_H__
#define __PAGEDOWMVIEW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class PageDowmView;


#include <view/view.h>

#define PAGEDOWM_VIEW_ICON "/res/view/pagedowmview.png"

class PageDowmView : public View{
public:	
	PageDowmView(std::string iconPathName, ViewInfo& viewInfo);	
	virtual int process(struct CookEvent& cookEvent);
private:
	PageDowmView();
};



#endif //__PAGEDOWMVIEW_H__






