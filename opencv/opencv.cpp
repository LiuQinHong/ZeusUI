#include <opencv.h>

RNG rng(12345);


OpenCV::OpenCV()
{

	mFaceCascadeName = "/OpenCV/haarcascades/haarcascade_frontalface_alt.xml";
	
	//-- 1. 加载级联分类器文件
	if(!mFaceCascade.load(mFaceCascadeName))
		printf("%s (!)Error loading\n", mFaceCascadeName.c_str()); 

}


OpenCV::OpenCV(const std::string& strFaceCascadeName)
{
	mFaceCascadeName = strFaceCascadeName;
	
	//-- 1. 加载级联分类器文件
	if(!mFaceCascade.load(mFaceCascadeName))
		printf("%s (!)Error loading\n", mFaceCascadeName.c_str()); 

}

OpenCV::OpenCV(const std::string& strFaceCascadeName, const std::string& strEyesCascadeName)
{
	mFaceCascadeName = strFaceCascadeName;
	mEyesCascadeName = strEyesCascadeName;

	//-- 1. 加载级联分类器文件
	if(!mFaceCascade.load(mFaceCascadeName))
		printf("%s (!)Error loading\n", mFaceCascadeName.c_str()); 

	if(!mEyesCascade.load(mEyesCascadeName))
		printf("%s (!)Error loading\n", mEyesCascadeName.c_str());	
}


void OpenCV::recognition(Mat& image)
{
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor(image, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- 多尺寸检测人脸
	mFaceCascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30));

	for(unsigned int i = 0; i < faces.size(); i++ ) {
		Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
		ellipse( image, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );

		if (!mEyesCascadeName.empty()) {
			Mat faceROI = frame_gray(faces[i]);
			std::vector<Rect> eyes;

			//-- 在每张人脸上检测双眼
			mEyesCascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );

			for(unsigned int j = 0; j < eyes.size(); j++ ) {
				Point center( faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5 );
				int radius = cvRound( (eyes[j].width + eyes[i].height)*0.25 );
				circle( image, center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
			}			
		}

	}
	
}



