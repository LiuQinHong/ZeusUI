#ifndef __FACERECOGNITIONVIEW_H__
#define __FACERECOGNITIONVIEW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class FaceView;

#include <view/view.h>
#define FACE_VIEW_ICON "/res/view/face_recognitionview.png"

class FaceView : public View{
public:	
	FaceView(std::string iconPathName, ViewInfo& viewInfo);
	virtual int process(struct CookEvent& cookEvent);
private:
	FaceView();
};



#endif //__FACERECOGNITIONVIEW_H__






