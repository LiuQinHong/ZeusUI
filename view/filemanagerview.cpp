#include <view/filemanagerview.h>
#include <window/filemanagerwindow.h>
#include <windowmanager.h>


FileManagerView::FileManagerView(std::string iconPathName, ViewInfo& viewInfo)
:View(iconPathName, viewInfo)
{
}


int FileManagerView::process(struct CookEvent& cookEvent)
{

	Window * window = new FileManagerWindow();
	WindowManager::getWindowManager()->addWindow(window);

	WindowManager::getWindowManager()->addTask(window, NEW_WINDOW);
	
	return 0;	
}


