#ifndef __TEXTVIEW_H__
#define __TEXTVIEW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class TextView;


#include <view/view.h>

#define TEXT_VIEW_ICON "/res/view/txtview.png"

class TextView : public View{
public:	
	TextView(std::string iconPathName, ViewInfo& viewInfo);	
	virtual int process(struct CookEvent& cookEvent);
private:
	TextView();
};



#endif //__TEXTVIEW_H__







