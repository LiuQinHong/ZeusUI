#ifndef __PICVIEW_H__
#define __PICVIEW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class PicView;


#include <view/view.h>

#define PIC_VIEW_ICON "/res/view/pictureview.png"

class PicView : public View{
public:	
	PicView(std::string iconPathName, ViewInfo& viewInfo);
	int showPicInWindow(void);	
	virtual int process(struct CookEvent& cookEvent);
private:
	PicView();
};



#endif //__PICVIEW_H__







