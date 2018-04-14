#include <view/qqview.h>
#include <window/qqwindow.h>
#include <windowmanager.h>


QQView::QQView(std::string iconPathName, ViewInfo& viewInfo)
:View(iconPathName, viewInfo)
{
}


int QQView::process(struct CookEvent& cookEvent)
{
	Window * window = new QQWindow();
	WindowManager::getWindowManager()->addWindow(window);
	WindowManager::getWindowManager()->addTask(window, NEW_WINDOW);

	return 0;	
}





