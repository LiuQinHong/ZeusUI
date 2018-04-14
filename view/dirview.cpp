#include <view/dirview.h>
#include <window/filemanagerwindow.h>
#include <windowmanager.h>
#include <textparser.h>

DirView::DirView(std::string iconPathName, ViewInfo& viewInfo)
:View(iconPathName, viewInfo)
{
}


int DirView::process(struct CookEvent& cookEvent)
{
	FileManagerWindow *fileManagerWindow = (FileManagerWindow *)getParentWindow();

	fileManagerWindow->removeView(this);
	
	
	/* 因为前面去掉了一个view,因此这里从1开始 */
	for (int i = 1; i < fileManagerWindow->getCurFileViewSum(); i++) {
		fileManagerWindow->delViewBack();
	}

	/* 清空内容区 */
	fileManagerWindow->clearContentArea();
	fileManagerWindow->showAllFixView();

	/* 这个view由return处理 */
	fileManagerWindow->addView(this);
	
	/* 设置view关心触摸屏事件 */
	this->cancelCareEvent(TOUCHSCREEN);
	fileManagerWindow->scanDir(getTargetFileName());

	WindowManager::getWindowManager()->addTask(getParentWindow(), REFRESH_WINDOW);
	return 0;
}



