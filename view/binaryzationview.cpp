#include <view/binaryzationview.h>
#include <windowmanager.h>
#include <opencv.h>
#include <window/picwindow.h>

BinaryzationView::BinaryzationView(std::string iconPathName, ViewInfo& viewInfo)
:View(iconPathName, viewInfo)
{
}


int BinaryzationView::process(struct CookEvent& cookEvent)
{
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


	Mat grayImage, Image; 
	cvtColor(imageOut, grayImage, COLOR_BGR2GRAY);		
	threshold(grayImage, Image, 128, 255, CV_THRESH_BINARY_INV);


	//printf("rows = %d\n", Image.rows); //h
	//printf("cols = %d\n", Image.cols); //w
	//printf("elemSize = %d\n", Image.elemSize()); //bpp / 8

	
	Window * window = new PicWindow(Image);
	WindowManager::getWindowManager()->addWindow(window);
	WindowManager::getWindowManager()->addTask(window, NEW_WINDOW);

	return 0;	
}





