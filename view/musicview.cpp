#include <view/musicview.h>
#include <window/musicwindow.h>
#include <windowmanager.h>

MusicView::MusicView(std::string iconPathName, ViewInfo& viewInfo)
:View(iconPathName, viewInfo)
{
}


int MusicView::process(struct CookEvent& cookEvent)
{

	Window * window = new MusicWindow(getTargetFileName());

	WindowManager::getWindowManager()->addWindow(window);	
	WindowManager::getWindowManager()->addTask(window, NEW_WINDOW);

	return 0;	
}






