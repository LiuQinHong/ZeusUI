#ifndef __BINARYZATIONVIEW_H__
#define __BINARYZATIONVIEW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class BinaryzationView;


#include <view/view.h>

#define BINARYZATION_VIEW_ICON "/res/view/binaryzationview.png"

class BinaryzationView : public View{
public:	
	BinaryzationView(std::string iconPathName, ViewInfo& viewInfo);
	virtual int process(struct CookEvent& cookEvent);
private:
	BinaryzationView();
};



#endif //__BINARYZATIONVIEW_H__






