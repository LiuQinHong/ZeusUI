#ifndef __OPENCV_H__
#define __OPENCV_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class OpenCV;

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  

#include <iostream>
using namespace cv;

class OpenCV {
public:
	OpenCV();
	OpenCV(const std::string& strFaceCascadeName);
	OpenCV(const std::string& strFaceCascadeName, const std::string& strEyesCascadeName);

	void recognition(Mat& image);
	
	
private:
	std::string mFaceCascadeName;
	std::string mEyesCascadeName;
	CascadeClassifier mFaceCascade;
	CascadeClassifier mEyesCascade;	
	Mat mImage;

};



#endif //__OPENCV_H__







