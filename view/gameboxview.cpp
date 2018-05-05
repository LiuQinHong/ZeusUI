#include <view/gameboxview.h>
#include <window/filemanagerwindow.h>


GameBoxView::GameBoxView(std::string iconPathName, ViewInfo& viewInfo)
:View(iconPathName, viewInfo)
{
}


int GameBoxView::process(struct CookEvent& cookEvent)
{	
	
	Window * window = new FileManagerWindow("/res/nes");
	WindowManager::getWindowManager()->addWindow(window);

	WindowManager::getWindowManager()->addTask(window, NEW_WINDOW);
	return 0;	
}






