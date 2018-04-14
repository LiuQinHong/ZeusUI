#include <view/facerecognitionview.h>
#include <window/picwindow.h>
#include <windowmanager.h>
#include <opencv.h>

FaceView::FaceView(std::string iconPathName, ViewInfo& viewInfo)
:View(iconPathName, viewInfo)
{
}


int FaceView::process(struct CookEvent& cookEvent)
{
	OpenCV opencv("/OpenCV/haarcascades/haarcascade_frontalface_alt.xml");

	PicWindow *picwindow = (PicWindow *)getParentWindow();
	Mat imageIn = imread(picwindow->getPicFileName());



	double scale = (double)(getParentWindow()->getWindowInfo().iHeight - getParentWindow()->getWindowInfo().iHeight / 8) / (double)imageIn.rows;
	int targetWidth = scale * imageIn.cols;
	if (targetWidth > getParentWindow()->getWindowInfo().iWidth) {
		scale = (double)getParentWindow()->getWindowInfo().iWidth / (double)imageIn.cols;
	}
	

    Size dsize = Size(imageIn.cols * scale, imageIn.rows * scale);
    Mat imageOut = Mat(dsize, CV_8UC3);
	
    resize(imageIn, imageOut, dsize);

	opencv.recognition(imageOut);
	
	Window * window = new PicWindow(imageOut);
	WindowManager::getWindowManager()->addWindow(window);
	WindowManager::getWindowManager()->addTask(window, NEW_WINDOW);
	
	return 0;	
}






