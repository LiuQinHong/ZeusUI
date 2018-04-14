#ifndef __CAMERAVIEW_H__
#define __CAMERAVIEW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class CameraView;


#include <view/view.h>

#define CAMERAVIEW_VIEW_ICON "/res/view/cameraview.png"

class CameraView : public View{
public:	
	CameraView(std::string iconPathName, ViewInfo& viewInfo);
	virtual int process(struct CookEvent& cookEvent);
private:
	pthread_t mTreadID;
	bool isCreate;
	CameraView();
};



#endif //__CAMERAVIEW_H__




