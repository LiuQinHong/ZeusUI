#ifndef __CLOSEVIEW_H__
#define __CLOSEVIEW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class CloseView;


#include <view/view.h>

#define CLOSE_VIEW_ICON "/res/view/closeview.png"

class CloseView : public View{
public:	
	CloseView(std::string iconPathName, ViewInfo& viewInfo);
	virtual int process(struct CookEvent& cookEvent);
private:
	CloseView();
};



#endif //__CLOSEVIEW_H__






