#include <view/musicmanagerview.h>
#include <window/filemanagerwindow.h>
#include <windowmanager.h>

MusicManagerView::MusicManagerView(std::string iconPathName, ViewInfo& viewInfo)
:View(iconPathName, viewInfo)
{
}


int MusicManagerView::process(struct CookEvent& cookEvent)
{
	Window * window = new FileManagerWindow("/res/music");
	WindowManager::getWindowManager()->addWindow(window);

	WindowManager::getWindowManager()->addTask(window, NEW_WINDOW);
	
	return 0;	
}







