#include <window/textwindow.h>
#include <device/framebuffer.h>
#include <input/inputreader.h>
#include <windowmanager.h>
#include <view/closeview.h>

TextWindow::TextWindow(){}

TextWindow::~TextWindow()
{
	if (mTextParser) {
		delete mTextParser;		
	}
}


TextWindow::TextWindow(const std::string& strTargetFileName)
:Window(strTargetFileName), strTextFileName(strTargetFileName)
{
	setRemovable(true);
	mTextParser = NULL;
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


void TextWindow::clearContentArea(void)
{
	FrameBuffer::getFrameBuffer()->FbDrawRectangle(getWindowInfo().tWindowCurPixelDatas, \
			0, getWindowInfo().iHeight / 8, getWindowInfo().iWidth, getWindowInfo().iHeight - getWindowInfo().iHeight / 8, 0xFFFFFFFF);
}

void TextWindow::drawWindowPixelData(void)
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
	

	/* 显示文档 */
	showText();
		
}


int TextWindow::showText(void)
{

	mTextParser = new TextParser();
	
	mTextParser->openTextInit(strTextFileName);
	mTextParser->showText(this->getWindowInfo().tWindowCurPixelDatas, \
			1, this->getWindowInfo().iHeight / 8 + mTextParser->getFont().iFontSize, 0, true);
	
	showInfoTitleBar(strTextFileName, mTextParser);
	
	return 0;
}


bool TextWindow::isHalfBottom(struct CookEvent& cookEvent)
{
	if (cookEvent.iY > (this->getWindowInfo().iY + this->getWindowInfo().iHeight / 2)) {
		return true;
	}

	return false;
}


int TextWindow::onProcess(struct CookEvent& cookEvent)
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

		if (cookEvent.iPressure == 0) {
			/* 松开 */
			if (isHalfBottom(cookEvent)) {
				if (mTextParser->isTheLastPage()) {
					return 0;
				}
				clearContentArea();
				mTextParser->showText(this->getWindowInfo().tWindowCurPixelDatas, \
						1, this->getWindowInfo().iHeight / 8 + mTextParser->getFont().iFontSize, 0, true);
				WindowManager::getWindowManager()->addTask(this, REFRESH_WINDOW);
			}
			else {
				if (mTextParser->isFirstPage()) {
					return 0;
				}
				clearContentArea();
				mTextParser->showText(this->getWindowInfo().tWindowCurPixelDatas, \
						1, this->getWindowInfo().iHeight / 8 + mTextParser->getFont().iFontSize, 0, false);
				
				WindowManager::getWindowManager()->addTask(this, REFRESH_WINDOW);
			}
		}
		
	}
		
	return iRet;
}




