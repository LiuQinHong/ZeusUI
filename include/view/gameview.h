#ifndef __GAMEVIEW_H__
#define __GAMEVIEW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class GameView;


#include <view/view.h>

#define GAME_VIEW_ICON "/res/view/gameview.png"

class GameView : public View{
public:	
	GameView(std::string iconPathName, ViewInfo& viewInfo);
	virtual int process(struct CookEvent& cookEvent);
private:
	GameView();
};



#endif //__GAMEVIEW_H__






