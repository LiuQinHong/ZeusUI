#include <view/cameraview.h>
#include <window/camerawindow.h>
#include <windowmanager.h>
#include <pthread.h>

CameraView::CameraView(std::string iconPathName, ViewInfo& viewInfo)
:View(iconPathName, viewInfo)
{
}

int CameraView::process(struct CookEvent& cookEvent)
{
	CameraWindow * window = new CameraWindow(this->getTargetFileName());
	WindowManager::getWindowManager()->addWindow(window);
	WindowManager::getWindowManager()->addTask(window, NEW_WINDOW);
	
	window->startCameraDevice();
	return 0;	
}



