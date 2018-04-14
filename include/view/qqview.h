#ifndef __QQVIEW_H__
#define __QQVIEW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class QQView;


#include <view/view.h>

#define QQ_VIEW_ICON "/res/view/qqview.png"

class QQView : public View{
public:	
	QQView(std::string iconPathName, ViewInfo& viewInfo);
	virtual int process(struct CookEvent& cookEvent);
private:
	QQView();
};



#endif //__QQVIEW_H__






