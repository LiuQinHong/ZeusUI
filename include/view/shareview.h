#ifndef __SHAREVIEW_H__
#define __SHAREVIEW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class ShareView;


#include <view/view.h>

#define SHARE_VIEW_ICON "/res/view/shareview.png"

class ShareView : public View{
public:	
	ShareView(std::string iconPathName, ViewInfo& viewInfo);
	virtual int process(struct CookEvent& cookEvent);
private:
	ShareView();
};



#endif //__SHAREVIEW_H__







