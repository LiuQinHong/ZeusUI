#ifndef __MUSICVIEW_H__
#define __MUSICVIEW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class MusicView;


#include <view/view.h>

#define MUSIC_VIEW_ICON "/res/view/musicview.png"

class MusicView : public View{
public:	
	MusicView(std::string iconPathName, ViewInfo& viewInfo);
	virtual int process(struct CookEvent& cookEvent);
private:
	MusicView();
};



#endif //__MUSICVIEW_H__







