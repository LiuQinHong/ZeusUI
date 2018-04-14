#include <view/returnview.h>
#include <windowmanager.h>
#include <textparser.h>
#include <window/filemanagerwindow.h>
#include <string.h>

ReturnView::ReturnView(std::string iconPathName, ViewInfo& viewInfo)
:View(iconPathName, viewInfo)
{
}


int ReturnView::process(struct CookEvent& cookEvent)
{
	FileManagerWindow *fileManagerWindow = (FileManagerWindow *)getParentWindow();

	if (!fileManagerWindow->getCurDirName().compare("/")) {
		/* 当前处于根目录，没有上级了 */
		return 0;
	}
	
	
	/* 多出一个目录view这里处理 */
	for (int i = 0; i < fileManagerWindow->getCurFileViewSum() + 1; i++) {
		fileManagerWindow->delViewBack();
	}

	/* 清空内容区 */
	fileManagerWindow->clearContentArea();
	fileManagerWindow->showAllFixView();


	int iLen = strrchr(fileManagerWindow->getCurDirName().c_str(), '/') - fileManagerWindow->getCurDirName().c_str();
	std::string strDirName(fileManagerWindow->getCurDirName(), 0, iLen);
	if (iLen == 0) {
		/* 表明上一个目录是"/" */
		strDirName = '/';
	}

	fileManagerWindow->scanDir(strDirName);

	WindowManager::getWindowManager()->addTask(getParentWindow(), REFRESH_WINDOW);
	return 0;
}



