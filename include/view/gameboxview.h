#ifndef __GAMBOXEVIEW_H__
#define __GAMBOXEVIEW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class GameBoxView;


#include <view/view.h>

#define GAMEBOX_VIEW_ICON "/res/view/gameboxview.png"

class GameBoxView : public View{
public:
	GameBoxView(std::string iconPathName, ViewInfo& viewInfo);
	virtual int process(struct CookEvent& cookEvent);
private:
	GameBoxView();
};



#endif //__GAMBOXEVIEW_H__






