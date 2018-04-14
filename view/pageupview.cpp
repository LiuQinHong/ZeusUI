#include <view/pageupview.h>
#include <windowmanager.h>
#include <window/filemanagerwindow.h>
#include <dirent.h>
#include <textparser.h>
#include <view/dirview.h>
#include <view/picview.h>
#include <view/textview.h>
#include <view/unknownview.h>

PageUpView::PageUpView(std::string iconPathName, ViewInfo& viewInfo)
:View(iconPathName, viewInfo)
{
}


int PageUpView::process(struct CookEvent& cookEvent)
{
	FileManagerWindow* pfileManagerWindow =	(FileManagerWindow*)getParentWindow();
	std::string& strCurDirName = pfileManagerWindow->getCurDirName();
	
	DIR *pDir;
	struct dirent *file;
	struct WindowInfo& windowInfo = pfileManagerWindow->getWindowInfo();
	TextParser *textParser = new TextParser();
	int iStrLen = 0;
	int iFontX = 0;
	int iFontY = 0;

	View* view = NULL;		
	ViewInfo viewInfo;
	int iX = 10;
	int iY = windowInfo.iHeight / 8;
	char strTmp[256];

	int iCount = 0;
	int iCurFileViewSum = 0;
	
	if (pfileManagerWindow->getCurPageCount() == 1) {
		/* 已经是第一页了，不需要处理 */
		delete textParser;
		return 0;
	}

	/* 更新标记 */
	pfileManagerWindow->setEndPageFlag(false);

	/* 更新当前页数 */
	pfileManagerWindow->decCurPageCount();

	/* 删除文件图标 */
	for (int i = 0; i < pfileManagerWindow->getCurFileViewSum(); i++) {
		pfileManagerWindow->delViewBack();
	}

	/* 清空内容区 */
	pfileManagerWindow->clearContentArea();
	pfileManagerWindow->showAllFixView();

	if(!(pDir = opendir(strCurDirName.c_str()))) {
		printf("error opendir %s!!!/n", strCurDirName.c_str());
		delete textParser;		
		return -1;
	}
	
	viewInfo.iWidth = windowInfo.iHeight / 4;
	viewInfo.iHeight = viewInfo.iWidth;

	while((file = readdir(pDir)) != NULL) {
		if(strncmp(file->d_name, ".", 1) == 0)
			continue;


		/* 判断有没有超出空间 */
		if ((iX + viewInfo.iWidth) > windowInfo.iWidth) {
			iX = 10;
			
			iY += viewInfo.iHeight;
			iY += 10;
			if ((iY + viewInfo.iHeight) > windowInfo.iHeight) {
				break;
			}
		}

		memset(strTmp, 0, 256);
		if (!strcmp(strCurDirName.c_str() , "/")) {
			sprintf(strTmp, "%s%s", strCurDirName.c_str(), file->d_name);						
		}
		else {
			sprintf(strTmp, "%s/%s", strCurDirName.c_str(), file->d_name);
		}

		/* 滑过前面的文件 */
		iCount++;
		if (iCount <= (pfileManagerWindow->getCurPageCount() - 1) * pfileManagerWindow->getMaxFileViewSum())
			continue;

		iCurFileViewSum++;
		pfileManagerWindow->setCurFileViewSum(iCurFileViewSum);

		viewInfo.iX = iX;
		viewInfo.iY = iY;
		viewInfo.iCareEventType = TOUCHSCREEN;

		if (pfileManagerWindow->isDir(strTmp)) {
			/* 目录 */			
			view = new DirView(DIR_VIEW_ICON, viewInfo);
		}
		else if (pfileManagerWindow->isRegFile(strTmp)) {
			/* 常规文件 */
			if (pfileManagerWindow->isPicFile(strTmp)) {
				view = new PicView(PIC_VIEW_ICON, viewInfo);			
			}
			else if (pfileManagerWindow->isTextFile(strTmp)) {
				view = new TextView(TEXT_VIEW_ICON, viewInfo);			
			}
			else {
				/* 未知文件 */
				view = new UnknownView(UNKNOWN_VIEW_ICON, viewInfo);
			}
		}
		else {
			/* 字符设备，块设备先不处理 */
			view = new UnknownView(UNKNOWN_VIEW_ICON, viewInfo);			
		}
		
		view->setTargetFileName(strTmp);			
		view->setParentWindow(pfileManagerWindow);
		
		iStrLen = strlen(file->d_name);
		if ((iStrLen << 3) > viewInfo.iWidth) {
			iStrLen = viewInfo.iWidth >> 3;
		}
		
		iFontX = (viewInfo.iWidth - (iStrLen << 3)) / 2;
		iFontY = viewInfo.iHeight / 4 * 3;
		textParser->putStr(view->getViewInfoRef().tZoomPixelDatas, iFontX, iFontY, file->d_name, iStrLen, 0xFFFFFFFF);
		view->showViewToWindow();
		
		pfileManagerWindow->addView(view);
		
		
		iX += viewInfo.iWidth;
		iX += 10;

	}
	closedir(pDir);
	delete textParser;

	WindowManager::getWindowManager()->addTask(getParentWindow(), REFRESH_WINDOW);
	
	return 0;
	
}


