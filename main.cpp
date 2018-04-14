#include <iostream>
#include <stdlib.h>
#include <input/inputreader.h>
#include <calibrate.h>
#include <inputdevice/eventdevice.h>
#include <inputdevice/inotifydevice.h>
#include <windowmanager.h>
#include <window/startwindow.h>



static void initWindowManager(void)
{
	Window *window = new StartWindow();
	WindowManager::getWindowManager()->addWindow(window);
	WindowManager::getWindowManager()->addTask(window, NEW_WINDOW);
	
}

int main(int argc, char **argv)
{
	struct CookEvent cookEvent[128];
	int iRet;

	/* EncodeParser init */
	EncodeParser::addAllEncodeParser();
	/* inotify init */
	InputReader::getInputReader()->addDevice(InotifyDevice::getInotifyDevice());
	EventDevice::scanDevice("/dev");
	InotifyDevice::getInotifyDevice()->addWatchDirectory("/dev");
	Calibrate::getCalibrate()->readConfigSetCalibrateInfo();

	initWindowManager();
	while (1) {
		iRet = InputReader::getInputReader()->readOnce(cookEvent, 128);
		if (iRet <= 0) {
			continue;
		}
		/* 传给 WindowManager 分发给关心这些事件的window */
		WindowManager::getWindowManager()->onProcess(cookEvent, iRet);
	}


	
	
	return 0;
}

