#include <window/qqwindow.h>
#include <device/framebuffer.h>
#include <input/inputreader.h>
#include <windowmanager.h>
#include <view/closeview.h>
#include <view/switchview.h>
#include <qqlink.h>

QQWindow::QQWindow()
:Window()
{
	setRemovable(true);
	struct WindowInfo& windowInfo = getWindowInfo();
	
	windowInfo.iWidth = FrameBuffer::getFrameBuffer()->getScreenInfo().iXres / 5 * 2;
	windowInfo.iHeight = windowInfo.iWidth;

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


	/* switch view */
	viewInfo.iWidth = windowInfo.iWidth / 2;
	viewInfo.iHeight = viewInfo.iWidth;

	viewInfo.iX = (windowInfo.iWidth - viewInfo.iWidth) / 2;
	viewInfo.iY = (windowInfo.iHeight - viewInfo.iHeight) / 2;
	viewInfo.iCareEventType = TOUCHSCREEN;
	if (QQlink::getQQlink()->isConnect()) {
		view = new SwitchView(SWITCHOPEN_VIEW_ICON, viewInfo);
	}
	else {
		view = new SwitchView(SWITCHCLOSE_VIEW_ICON, viewInfo);		
	}
		
	view->setParentWindow(this);
	view->showViewToWindow();
	addView(view);	


}


void QQWindow::drawWindowPixelData(void)
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
	0, windowInfo.iHeight / 8, windowInfo.tWindowCurPixelDatas.iWidth, windowInfo.tWindowCurPixelDatas.iHeight - windowInfo.iHeight / 10, 0);
		
}


int QQWindow::onProcess(struct CookEvent& cookEvent)
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



