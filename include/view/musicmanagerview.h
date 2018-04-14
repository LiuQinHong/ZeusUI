#ifndef __MUSICMANAGERVIEW_H__
#define __MUSICMANAGERVIEW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class MusicManagerView;


#include <view/view.h>

#define MUSICMANAGER_VIEW_ICON "/res/view/musicmanagerview.png"

class MusicManagerView : public View{
public:	
	MusicManagerView(std::string iconPathName, ViewInfo& viewInfo);
	virtual int process(struct CookEvent& cookEvent);
private:
	MusicManagerView();
};



#endif //__MUSICMANAGERVIEW_H__








