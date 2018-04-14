#include <view/picview.h>
#include <window/picwindow.h>
#include <windowmanager.h>

PicView::PicView(std::string iconPathName, ViewInfo& viewInfo)
:View(iconPathName, viewInfo)
{
}


int PicView::process(struct CookEvent& cookEvent)
{
	Window * window = new PicWindow(this->getTargetFileName());
	WindowManager::getWindowManager()->addWindow(window);

	WindowManager::getWindowManager()->addTask(window, NEW_WINDOW);
	return 0;
}


