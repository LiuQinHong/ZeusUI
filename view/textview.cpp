#include <view/textview.h>
#include <window/textwindow.h>
#include <windowmanager.h>

TextView::TextView(std::string iconPathName, ViewInfo& viewInfo)
:View(iconPathName, viewInfo)
{
}


int TextView::process(struct CookEvent& cookEvent)
{
	Window * window = new TextWindow(this->getTargetFileName());
	WindowManager::getWindowManager()->addWindow(window);
	WindowManager::getWindowManager()->addTask(window, NEW_WINDOW);
	return 0;
	
}


