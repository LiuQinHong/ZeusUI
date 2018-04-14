#ifndef __UNKNOWNVIEW_H__
#define __UNKNOWNVIEW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class UnknownView;


#include <view/view.h>

#define UNKNOWN_VIEW_ICON "/res/view/unknownview.png"

class UnknownView : public View{
public:	
	UnknownView(std::string iconPathName, ViewInfo& viewInfo);
	virtual int process(struct CookEvent& cookEvent);
private:
	UnknownView();
};



#endif //__UNKNOWNVIEW_H__






