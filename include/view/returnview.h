#ifndef __RETURNVIEW_H__
#define __RETURNVIEW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class ReturnView;


#include <view/view.h>

#define RETURN_VIEW_ICON "/res/view/returnview.png"

class ReturnView : public View{
public:	
	ReturnView(std::string iconPathName, ViewInfo& viewInfo);
	virtual int process(struct CookEvent& cookEvent);
private:
	ReturnView();
};



#endif //__RETURNVIEW_H__






