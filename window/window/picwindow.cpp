#include <window/picwindow.h>
#include <device/framebuffer.h>
#include <input/inputreader.h>
#include <windowmanager.h>

#include <view/closeview.h>
#include <view/facerecognitionview.h>
#include <view/shareview.h>
#include <view/binaryzationview.h>

#include <pic/picparser.h>
#include <pic/jpgparser.h>
#include <pic/pngparser.h>
#include <pic/bmpparser.h>

PicWindow::PicWindow(){}


PicWindow::PicWindow(const std::string& strTargetFileName)
:Window(strTargetFileName), strPicFileName(strTargetFileName)
{
	setRemovable(true);
	struct WindowInfo& windowInfo = getWindowInfo();
	
	windowInfo.iWidth = FrameBuffer::getFrameBuffer()->getScreenInfo().iXres / 4 * 3;
	windowInfo.iHeight = windowInfo.iWidth / 3 * 2;

	windowInfo.iX = (FrameBuffer::getFrameBuffer()->getScreenInfo().iXres - windowInfo.iWidth) / 2;
	windowInfo.iY = (FrameBuffer::getFrameBuffer()->getScreenInfo().iYres - windowInfo.iHeight) / 2;

	windowInfo.iCareEventType = TOUCHSCREEN;

	/* 画出这个窗口 */
	drawWindowPixelData();


	View* view;
	ViewInfo viewInfo;

	/* close view */
	viewInfo.iX = windowInfo.iWidth - windowInfo.iHeight / 8;
	viewInfo.iY = 0;
	viewInfo.iWidth = windowInfo.iHeight / 8;
	viewInfo.iHeight = viewInfo.iWidth;
	viewInfo.iCareEventType = TOUCHSCREEN;
	view = new CloseView(CLOSE_VIEW_ICON, viewInfo);
	view->setParentWindow(this);
	view->showViewToWindow();
	addView(view);

	/* face_recognitionview view */
	viewInfo.iX = 0;
	viewInfo.iY = 0;
	viewInfo.iWidth = windowInfo.iHeight / 8;
	viewInfo.iHeight = viewInfo.iWidth;
	viewInfo.iCareEventType = TOUCHSCREEN;
	view = new FaceView(FACE_VIEW_ICON, viewInfo);
	view->setParentWindow(this);
	view->showViewToWindow();
	addView(view);


	/* share view */
	viewInfo.iX += windowInfo.iHeight / 8;
	viewInfo.iY = 0;
	viewInfo.iWidth = windowInfo.iHeight / 8;
	viewInfo.iHeight = viewInfo.iWidth;
	viewInfo.iCareEventType = TOUCHSCREEN;
	view = new ShareView(SHARE_VIEW_ICON, viewInfo);
	view->setParentWindow(this);
	view->showViewToWindow();
	addView(view);


	/* binaryzation view */
	viewInfo.iX += windowInfo.iHeight / 8;
	viewInfo.iY = 0;
	viewInfo.iWidth = windowInfo.iHeight / 8;
	viewInfo.iHeight = viewInfo.iWidth;
	viewInfo.iCareEventType = TOUCHSCREEN;
	view = new BinaryzationView(BINARYZATION_VIEW_ICON, viewInfo);
	view->setParentWindow(this);
	view->showViewToWindow();
	addView(view);
	
	
}


PicWindow::PicWindow(Mat& image)
:Window(), mImage(image)
{
	setRemovable(true);
	struct WindowInfo& windowInfo = getWindowInfo();
	
	windowInfo.iWidth = FrameBuffer::getFrameBuffer()->getScreenInfo().iXres / 4 * 3;
	windowInfo.iHeight = windowInfo.iWidth / 3 * 2;

	windowInfo.iX = (FrameBuffer::getFrameBuffer()->getScreenInfo().iXres - windowInfo.iWidth) / 2;
	windowInfo.iY = (FrameBuffer::getFrameBuffer()->getScreenInfo().iYres - windowInfo.iHeight) / 2;

	windowInfo.iCareEventType = TOUCHSCREEN;

	/* 画出这个窗口 */

	drawWindowPixelData();


	View* view;
	ViewInfo viewInfo;

	/* close view */
	viewInfo.iX = windowInfo.iWidth - windowInfo.iHeight / 8;
	viewInfo.iY = 0;
	viewInfo.iWidth = windowInfo.iHeight / 8;
	viewInfo.iHeight = viewInfo.iWidth;
	viewInfo.iCareEventType = TOUCHSCREEN;
	view = new CloseView(CLOSE_VIEW_ICON, viewInfo);
	view->setParentWindow(this);
	view->showViewToWindow();
	addView(view);	
}


void PicWindow::drawWindowPixelData(void)
{
	struct WindowInfo& windowInfo = getWindowInfo();
	windowInfo.tWindowCurPixelDatas.iBpp = FrameBuffer::getFrameBuffer()->getScreenInfo().iBpp;
	windowInfo.tWindowCurPixelDatas.iWidth = windowInfo.iWidth;
	windowInfo.tWindowCurPixelDatas.iHeight = windowInfo.iHeight;
	windowInfo.tWindowCurPixelDatas.iLineBytes = windowInfo.iWidth * windowInfo.tWindowCurPixelDatas.iBpp / 8;
	windowInfo.tWindowCurPixelDatas.iTotalBytes = windowInfo.tWindowCurPixelDatas.iLineBytes * windowInfo.tWindowCurPixelDatas.iHeight;
	windowInfo.tWindowCurPixelDatas.pcPixelDatas = new char[windowInfo.tWindowCurPixelDatas.iTotalBytes];

	/* 先绘制顶部的功能栏 */
	FrameBuffer::getFrameBuffer()->FbDrawRectangle(windowInfo.tWindowCurPixelDatas,\
	0, 0, windowInfo.tWindowCurPixelDatas.iWidth, windowInfo.iHeight / 8, 0);

	/* 绘制内容区 */
	FrameBuffer::getFrameBuffer()->FbDrawRectangle(windowInfo.tWindowCurPixelDatas,\
	0, windowInfo.iHeight / 8, windowInfo.tWindowCurPixelDatas.iWidth, windowInfo.tWindowCurPixelDatas.iHeight - windowInfo.iHeight / 10, 0xFFFFFFFF);



	/* 绘制底部一条边 */
	FrameBuffer::getFrameBuffer()->FbDrawLine(windowInfo.tWindowCurPixelDatas, \
		0, windowInfo.iHeight - 1, windowInfo.iWidth, windowInfo.iHeight -1 , 0);
	
	/* 绘制右边一条边 */
	FrameBuffer::getFrameBuffer()->FbDrawLine(windowInfo.tWindowCurPixelDatas, \
		windowInfo.iWidth - 1, 0, windowInfo.iWidth - 1, windowInfo.iHeight, 0);

	/* 绘制左边一条边 */
	FrameBuffer::getFrameBuffer()->FbDrawLine(windowInfo.tWindowCurPixelDatas, \
		0, 0, 0, windowInfo.iHeight, 0);
	

	/* 显示图片 */
	if (!strPicFileName.empty()) {
		showPic();
	}
	else {
		showPic(mImage);
	}

		
}


int PicWindow::showPic(void)
{
	int iRet;
	PicParser *picParser= NULL;
	TextParser *textParser = new TextParser();

	showInfoTitleBar(strPicFileName, textParser);

	if (!strcmp(PicParser::getFilenameExt(strPicFileName), ".jpg")) {		
		picParser = new JpgParser();
	}
	else if (!strcmp(PicParser::getFilenameExt(strPicFileName), ".png")) {
		picParser = new PngParser();		

	}
	else if (!strcmp(PicParser::getFilenameExt(strPicFileName), ".bmp")) {	
		picParser = new BmpParser();
	}

	/* 设置要得到的图像的数据格式 */
	tPixelDatas.iBpp = FrameBuffer::getFrameBuffer()->getScreenInfo().iBpp;
	tPixelDatas.iWidth = 0;
	tPixelDatas.iHeight = this->getWindowInfo().iHeight - this->getWindowInfo().iHeight / 8 - 1;
	//tPixelDatas.iLineBytes = mViewInfo.iWidth * tPixelDatas.iBpp / 8;
	//tPixelDatas.iTotalBytes = tPixelDatas.iLineBytes * tPixelDatas.iHeight;
	tPixelDatas.pcPixelDatas = NULL;  //由 picParser->getZoomPixelData 分配
	iRet = picParser->getZoomPixelData(strPicFileName, tPixelDatas);
	if (iRet < 0) {
		delete picParser;
		delete textParser;
		return -1;
	}

	int iX = (this->getWindowInfo().iWidth - tPixelDatas.iWidth) / 2;
	int iY = ((this->getWindowInfo().iHeight - this->getWindowInfo().iHeight / 8) - tPixelDatas.iHeight) / 2 \
				+ this->getWindowInfo().iHeight / 8;

	FrameBuffer::getFrameBuffer()->FbVideoMemFusion(iX, iY, this->getWindowInfo().tWindowCurPixelDatas, tPixelDatas);

	delete picParser;
	delete textParser;
	delete tPixelDatas.pcPixelDatas;

	return 0;
}




int PicWindow::showPic(Mat& image)
{
	
	tPixelDatas.iBpp = FrameBuffer::getFrameBuffer()->getScreenInfo().iBpp;
	tPixelDatas.iHeight = image.rows;
	tPixelDatas.iWidth = image.cols;
	tPixelDatas.iLineBytes = tPixelDatas.iWidth * tPixelDatas.iBpp / 8;
	tPixelDatas.iTotalBytes = tPixelDatas.iLineBytes * tPixelDatas.iHeight;
	tPixelDatas.pcPixelDatas = new char[tPixelDatas.iTotalBytes];	

	if (image.isContinuous()) {
		//printf("isContinuous yes!\n");
		FrameBuffer::getFrameBuffer()->CovertBGR2RGB(tPixelDatas.iWidth, tPixelDatas.iHeight, \
			image.elemSize() * 8, FrameBuffer::getFrameBuffer()->getScreenInfo().iBpp, \
			(char *)image.ptr<uchar>(0), tPixelDatas.pcPixelDatas);
		
	}
	else {
		printf("isContinuous no!\n");
		return 0;
	}
	
	int iX = (this->getWindowInfo().iWidth - tPixelDatas.iWidth) / 2;
	int iY = ((this->getWindowInfo().iHeight - this->getWindowInfo().iHeight / 8) - tPixelDatas.iHeight) / 2 \
				+ this->getWindowInfo().iHeight / 8;


	FrameBuffer::getFrameBuffer()->FbVideoMemFusion(iX, iY, this->getWindowInfo().tWindowCurPixelDatas, tPixelDatas);

	delete tPixelDatas.pcPixelDatas;
	return 0;
}


std::string& PicWindow::getPicFileName(void)
{
	return strPicFileName;
}



int PicWindow::onProcess(struct CookEvent& cookEvent)
{
	int iRet = 0;


	if (isMoved()) {
		if (cookEvent.iPressure == 0) {
			setMoved(false);
		}
		
		moveWindow(cookEvent.iX, cookEvent.iY);
		return 0;
	}

	iRet = dispatchEvent(cookEvent);
	if (iRet < 0) {
		/* 没有支持view */	
		if (isRemovable() && isLongPress(cookEvent.tTime)) {
			/* 这个window可移动并且长按了 */
			setMoved(true); 
		}	
	}
		
	return iRet;
}



