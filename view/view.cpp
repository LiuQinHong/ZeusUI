#include <view/view.h>

#include <pic/jpgparser.h>
#include <pic/bmpparser.h>
#include <pic/pngparser.h>

View::View()
{
}

View::View(const std::string& iconPathName, ViewInfo& viewInfo)
: mIconPathName(iconPathName), mViewInfo(viewInfo) //注意 初始化顺序要和类成员的申明顺序一样
{
	mPicParser = NULL;

	if (!strcmp(PicParser::getFilenameExt(iconPathName), ".jpg")) {		
		mPicParser = new JpgParser();
	}
	else if (!strcmp(PicParser::getFilenameExt(iconPathName), ".png")) {
		mPicParser = new PngParser();		

	}
	else if (!strcmp(PicParser::getFilenameExt(iconPathName), ".bmp")) {		
		mPicParser = new BmpParser();
	}

	/* 设置要得到的图像的数据格式 */
	mViewInfo.tZoomPixelDatas.iBpp = FrameBuffer::getFrameBuffer()->getScreenInfo().iBpp;
	mViewInfo.tZoomPixelDatas.iWidth = mViewInfo.iWidth;
	mViewInfo.tZoomPixelDatas.iHeight = mViewInfo.iHeight;
	mViewInfo.tZoomPixelDatas.iLineBytes = mViewInfo.iWidth * mViewInfo.tZoomPixelDatas.iBpp / 8;
	mViewInfo.tZoomPixelDatas.iTotalBytes = mViewInfo.tZoomPixelDatas.iLineBytes * mViewInfo.tZoomPixelDatas.iHeight;
	mViewInfo.tZoomPixelDatas.pcPixelDatas = NULL;  //由 mPicParser->getZoomPixelData 分配
	mPicParser->getZoomPixelData(mIconPathName, mViewInfo.tZoomPixelDatas);		


	/* 设置原始数据 */
	mViewInfo.tOrgPixelDatas.iBpp = mViewInfo.tZoomPixelDatas.iBpp;
	mViewInfo.tOrgPixelDatas.iWidth = mViewInfo.tZoomPixelDatas.iWidth;
	mViewInfo.tOrgPixelDatas.iHeight = mViewInfo.tZoomPixelDatas.iHeight;
	mViewInfo.tOrgPixelDatas.iLineBytes = mViewInfo.tZoomPixelDatas.iLineBytes;	
	mViewInfo.tOrgPixelDatas.iTotalBytes = mViewInfo.tZoomPixelDatas.iTotalBytes;
	mViewInfo.tOrgPixelDatas.pcPixelDatas = new char[mViewInfo.tOrgPixelDatas.iTotalBytes];

	mViewInfo.mSelected = false;
	mViewInfo.mRemovable = false;
	mViewInfo.mPressed = false;
	mViewInfo.mRemoved = true;
	
}


View::~View()
{
	if (mPicParser)
		delete mPicParser;

	if (mViewInfo.tZoomPixelDatas.pcPixelDatas) {
		delete mViewInfo.tZoomPixelDatas.pcPixelDatas;
	}
	
	if (mViewInfo.tOrgPixelDatas.pcPixelDatas) {
		delete mViewInfo.tOrgPixelDatas.pcPixelDatas;
	}

	
}
ViewInfo& View::getViewInfoRef(void)
{
	return mViewInfo;
}

std::string& View::getIconPathName(void)
{
	return mIconPathName;
}

void View::showViewToWindow(void)
{
	FrameBuffer::getFrameBuffer()->FbVideoMemFusion(mViewInfo.iX, mViewInfo.iY, \
		mParentWindow->getWindowInfo().tWindowCurPixelDatas, mViewInfo.tZoomPixelDatas);
}

void View::setCareEvent(int eventType)
{
	mViewInfo.iCareEventType |= eventType;
}
void View::cancelCareEvent(int eventType)
{
	mViewInfo.iCareEventType &= ~eventType;

}

bool View::isCareThisEvent(int eventType)
{
	return ((mViewInfo.iCareEventType & eventType) != 0);
}

void View::setParentWindow(Window *parentWindow)
{
	mParentWindow = parentWindow;
}

Window *View::getParentWindow(void)
{
	return mParentWindow;
}


void View::changeIconImage(const std::string& iconPathName)
{
	if (mPicParser)
		delete mPicParser;

	if (mViewInfo.tZoomPixelDatas.pcPixelDatas) {
		delete mViewInfo.tZoomPixelDatas.pcPixelDatas;
	}

	mPicParser = NULL;
	mIconPathName = iconPathName;

	if (!strcmp(PicParser::getFilenameExt(iconPathName), ".jpg")) {		
		mPicParser = new JpgParser();
	}
	else if (!strcmp(PicParser::getFilenameExt(iconPathName), ".png")) {
		mPicParser = new PngParser();		

	}
	else if (!strcmp(PicParser::getFilenameExt(iconPathName), ".bmp")) {		
		mPicParser = new BmpParser();
	}

	/* 设置要得到的图像的数据格式 */
	mViewInfo.tZoomPixelDatas.pcPixelDatas = NULL;  //由 mPicParser->getZoomPixelData 分配
	mPicParser->getZoomPixelData(mIconPathName, mViewInfo.tZoomPixelDatas);

}



int View::reversalPixelData(void)
{
	int i;
	int iTotalBytes = mViewInfo.tZoomPixelDatas.iTotalBytes;
	char *pcSrcPtr = mViewInfo.tZoomPixelDatas.pcPixelDatas;
	char cTmp = '\0';



	for (i = iTotalBytes >> 3; i > 0; i--) {
		cTmp = ~(*pcSrcPtr);
		*pcSrcPtr++ = cTmp;
		cTmp = ~(*pcSrcPtr);
		*pcSrcPtr++ = cTmp;	
		cTmp = ~(*pcSrcPtr);
		*pcSrcPtr++ = cTmp;		
		cTmp = ~(*pcSrcPtr);
		*pcSrcPtr++ = cTmp;		
		cTmp = ~(*pcSrcPtr);
		*pcSrcPtr++ = cTmp;		
		cTmp = ~(*pcSrcPtr);
		*pcSrcPtr++ = cTmp;		
		cTmp = ~(*pcSrcPtr);
		*pcSrcPtr++ = cTmp;		
		cTmp = ~(*pcSrcPtr);
		*pcSrcPtr++ = cTmp;

	}

	if (iTotalBytes & 1 << 2) {
		cTmp = ~(*pcSrcPtr);
		*pcSrcPtr++ = cTmp;
		cTmp = ~(*pcSrcPtr);
		*pcSrcPtr++ = cTmp;	
		cTmp = ~(*pcSrcPtr);
		*pcSrcPtr++ = cTmp;		
		cTmp = ~(*pcSrcPtr);
		*pcSrcPtr++ = cTmp;	
	}

	if (iTotalBytes & 1 << 1) {
		cTmp = ~(*pcSrcPtr);
		*pcSrcPtr++ = cTmp;
		cTmp = ~(*pcSrcPtr);
		*pcSrcPtr++ = cTmp;	
	}

	if (iTotalBytes & 1)
		cTmp = ~(*pcSrcPtr);
		*pcSrcPtr++ = cTmp;	

	return 0;
	
}




/* 判断是否按下过 */
bool View::isPressed(void)
{
	return mViewInfo.mPressed;
}
void View::setPressed(bool pressed)
{
	mViewInfo.mPressed = pressed;
}



bool View::isSupport(int iX, int iY)
{

	int iAbsoluteX = mViewInfo.iX + mParentWindow->getWindowInfo().iX; //得到view的绝对坐标
	int iAbsoluteY = mViewInfo.iY + mParentWindow->getWindowInfo().iY; //得到view的绝对坐标;

	if ((iX > iAbsoluteX) && (iX < (iAbsoluteX + mViewInfo.iWidth)) && \
		(iY > iAbsoluteY) && (iY < (iAbsoluteY + mViewInfo.iHeight))) {
		
		return true;
	}

	return false;
}



void View::setTargetFileName(const std::string& strTargetFileName)
{
	mTargetFileName = strTargetFileName;
}

std::string& View::getTargetFileName(void)
{
	return mTargetFileName;
}



int View::onProcess(struct CookEvent& cookEvent)
{

	if (isPressed()) {
		/* 之前按下过 */
		if (cookEvent.iPressure == 0) {
			/* 按下过后松开 */
			setPressed(false);
			/* 反转view的图像数据 */
			reversalPixelData();
			showViewToWindow();
			WindowManager::getWindowManager()->addTask(getParentWindow(), REFRESH_WINDOW);
			process(cookEvent);
		}
		return 0;
	}
	else {
		setPressed(true);
		/* 反转view的图像数据 */
		reversalPixelData();
		showViewToWindow();
		WindowManager::getWindowManager()->addTask(getParentWindow(), REFRESH_WINDOW);
	}

	
	return 0;	
}




