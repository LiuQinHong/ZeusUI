#ifndef __DIRVIEW_H__
#define __DIRVIEW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class DirView;


#include <view/view.h>

#define DIR_VIEW_ICON "/res/view/dirview.png"

class DirView : public View{
public:	
	DirView(std::string iconPathName, ViewInfo& viewInfo);
	virtual int process(struct CookEvent& cookEvent);
private:
	DirView();
};



#endif //__DIRVIEW_H__







