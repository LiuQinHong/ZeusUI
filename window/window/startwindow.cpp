#include <window/startwindow.h>
#include <device/framebuffer.h>
#include <input/inputreader.h>
#include <view/settingview.h>
#include <view/filemanagerview.h>
#include <view/cameraview.h>
#include <view/gameview.h>
#include <view/musicmanagerview.h>
#include <view/qqview.h>
#include <string.h>

StartWindow::StartWindow()
:Window()
{
	struct WindowInfo& windowInfo = getWindowInfo();


	windowInfo.iWidth = FrameBuffer::getFrameBuffer()->getScreenInfo().iXres;
	windowInfo.iHeight = FrameBuffer::getFrameBuffer()->getScreenInfo().iYres / 8;
	windowInfo.iX = 0;
	windowInfo.iY = FrameBuffer::getFrameBuffer()->getScreenInfo().iYres - windowInfo.iHeight;
	windowInfo.iCareEventType = TOUCHSCREEN | NETWORKINPUT | ADDDEVICE | DELDEVICE;
	

	/* 画出这个窗口 */
	drawWindowPixelData();


	View* view;
	int iOffset = windowInfo.iHeight / 3;


	ViewInfo viewInfo;
	viewInfo.iX = 0;
	viewInfo.iY = 0;
	viewInfo.iWidth = windowInfo.iHeight;
	viewInfo.iHeight = windowInfo.iHeight;
	viewInfo.iCareEventType = TOUCHSCREEN;
	view = new SettingView(SETTING_VIEW_ICON, viewInfo);
	view->setParentWindow(this);
	view->showViewToWindow();
	addView(view);

	
	viewInfo.iX = viewInfo.iX + viewInfo.iWidth + iOffset;
	viewInfo.iY = 0;
	viewInfo.iWidth = windowInfo.iHeight;
	viewInfo.iHeight = windowInfo.iHeight;
	viewInfo.iCareEventType = TOUCHSCREEN;
	view = new FileManagerView(FILEMANAGER_VIEW_ICON, viewInfo);
	view->setParentWindow(this);
	view->showViewToWindow();
	addView(view);	


	viewInfo.iX = viewInfo.iX + viewInfo.iWidth + iOffset;
	viewInfo.iY = 0;
	viewInfo.iWidth = windowInfo.iHeight;
	viewInfo.iHeight = windowInfo.iHeight;
	viewInfo.iCareEventType = TOUCHSCREEN;
	view = new GameView(GAME_VIEW_ICON, viewInfo);
	view->setParentWindow(this);
	view->showViewToWindow();
	addView(view);	


	viewInfo.iX = viewInfo.iX + viewInfo.iWidth + iOffset;
	viewInfo.iY = 0;
	viewInfo.iWidth = windowInfo.iHeight;
	viewInfo.iHeight = windowInfo.iHeight;
	viewInfo.iCareEventType = TOUCHSCREEN;
	view = new MusicManagerView(MUSICMANAGER_VIEW_ICON, viewInfo);
	view->setParentWindow(this);
	view->showViewToWindow();
	addView(view);	


	viewInfo.iX = viewInfo.iX + viewInfo.iWidth + iOffset;
	viewInfo.iY = 0;
	viewInfo.iWidth = windowInfo.iHeight;
	viewInfo.iHeight = windowInfo.iHeight;
	viewInfo.iCareEventType = TOUCHSCREEN;
	view = new QQView(QQ_VIEW_ICON, viewInfo);
	view->setParentWindow(this);
	view->showViewToWindow();
	addView(view);	
}



void StartWindow::drawWindowPixelData(void)
{
	struct WindowInfo& windowInfo = getWindowInfo();
	windowInfo.tWindowCurPixelDatas.iBpp = FrameBuffer::getFrameBuffer()->getScreenInfo().iBpp;
	windowInfo.tWindowCurPixelDatas.iWidth = windowInfo.iWidth;
	windowInfo.tWindowCurPixelDatas.iHeight = windowInfo.iHeight;
	windowInfo.tWindowCurPixelDatas.iLineBytes = windowInfo.iWidth * windowInfo.tWindowCurPixelDatas.iBpp / 8;
	windowInfo.tWindowCurPixelDatas.iTotalBytes = windowInfo.tWindowCurPixelDatas.iLineBytes * windowInfo.tWindowCurPixelDatas.iHeight;
	windowInfo.tWindowCurPixelDatas.pcPixelDatas = new char[windowInfo.tWindowCurPixelDatas.iTotalBytes];

	memset(windowInfo.tWindowCurPixelDatas.pcPixelDatas, 0, windowInfo.tWindowCurPixelDatas.iTotalBytes);
}


void StartWindow::recalculateViewLocation(void)
{
	int iX = 0;
	struct WindowInfo& windowInfo = getWindowInfo();	
	int iOffset = windowInfo.iHeight / 3;
	View* view = NULL;

	for(std::list<View*>::iterator viewlist_iter = getViewList().begin(); viewlist_iter!= getViewList().end(); ++viewlist_iter) {
		view = (*viewlist_iter);
		view->getViewInfoRef().iX = iX;
		iX += (view->getViewInfoRef().iWidth + iOffset);
	}	
}


int StartWindow::onProcess(struct CookEvent& cookEvent)
{
	int iRet = 0;
	
	iRet = dispatchEvent(cookEvent);
	if (!iRet) {
		/* view已经处理过这个事件了 */
		return 0;
	}


	View* view;
	struct WindowInfo& windowInfo = getWindowInfo();	
	ViewInfo viewInfo;
	int iOffset = windowInfo.iHeight / 3;


	/* 这里window做一些兜底工作 */
	if (cookEvent.iType & ADDDEVICE) {
		if (!strncmp(cookEvent.filePath.c_str(), "/dev/video", strlen("/dev/video"))) {
			viewInfo.iX = getViewSum() * (windowInfo.iHeight + iOffset);
			viewInfo.iY = 0;
			viewInfo.iWidth = windowInfo.iHeight;
			viewInfo.iHeight = windowInfo.iHeight;
			viewInfo.iCareEventType = TOUCHSCREEN;
			view = new CameraView(CAMERAVIEW_VIEW_ICON, viewInfo);
			view->setTargetFileName(cookEvent.filePath);
			view->setParentWindow(this);
			view->showViewToWindow();
			addView(view);
			
			WindowManager::getWindowManager()->addTask(this, REFRESH_WINDOW);
			iRet = 0;
		}
	}
	else if (cookEvent.iType & DELDEVICE) {
		
		view = getViewForTargetFileName(cookEvent.filePath);
		if (view == NULL) {
			return 0;
		}

		/* 恢复window的数据 */
		//restoreWindow();
		delView(view);
		recalculateViewLocation();
		showAllView();
		
		WindowManager::getWindowManager()->addTask(this, REFRESH_WINDOW);

		iRet = 0;
	}

	return iRet;
}

