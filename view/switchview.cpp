#include <view/switchview.h>
#include <window/qqwindow.h>
#include <windowmanager.h>
#include <qqlink.h>

SwitchView::SwitchView(std::string iconPathName, ViewInfo& viewInfo)
:View(iconPathName, viewInfo)
{
}


int SwitchView::process(struct CookEvent& cookEvent)
{
	if (QQlink::getQQlink()->isConnect()) {
		/* 已经与手机QQ连上，则断开连接 */
		QQlink::getQQlink()->exitDevice();
		QQlink::getQQlink()->setConnect(false);
		changeIconImage(SWITCHCLOSE_VIEW_ICON);

	}
	else {
		/* 没有与手机QQ连上，则进行连接 */
		QQlink::getQQlink()->initDevice();
		QQlink::getQQlink()->initDataPoint();
		QQlink::getQQlink()->initFileTransfer();
		//QQlink::getQQlink()->setConnect(true);
		changeIconImage(SWITCHOPEN_VIEW_ICON);
	}

	showViewToWindow();
	WindowManager::getWindowManager()->addTask(getParentWindow(), REFRESH_WINDOW);
	
	return 0;	
}





