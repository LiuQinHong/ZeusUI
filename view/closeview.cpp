#include <view/closeview.h>
#include <windowmanager.h>

CloseView::CloseView(std::string iconPathName, ViewInfo& viewInfo)
:View(iconPathName, viewInfo)
{
}


int CloseView::process(struct CookEvent& cookEvent)
{
	WindowManager::getWindowManager()->removeWindow(getParentWindow());
	WindowManager::getWindowManager()->addTask(getParentWindow(), CLOSE_WINDOW);

	return 0;	
}





