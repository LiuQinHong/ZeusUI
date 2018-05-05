#include <window/filemanagerwindow.h>
#include <device/framebuffer.h>
#include <input/inputreader.h>
#include <windowmanager.h>
#include <view/closeview.h>
#include <view/dirview.h>
#include <view/returnview.h>
#include <view/pageupview.h>
#include <view/pagedowmview.h>
#include <view/picview.h>
#include <view/textview.h>
#include <view/musicview.h>
#include <view/gameview.h>
#include <view/unknownview.h>
#include <dirent.h>
#include <pic/picparser.h>

FileManagerWindow::FileManagerWindow()
:Window()
{
	setRemovable(true);
	struct WindowInfo& windowInfo = getWindowInfo();
	
	windowInfo.iWidth = FrameBuffer::getFrameBuffer()->getScreenInfo().iXres / 5 * 3;
	windowInfo.iHeight = windowInfo.iWidth / 3 * 2;

	struct tPoint& point = WindowManager::getWindowManager()->getNextPoint();
	windowInfo.iX = point.iX;
	windowInfo.iY = point.iY;
	WindowManager::getWindowManager()->updatePoint();

	windowInfo.iCareEventType = TOUCHSCREEN | ADDFILE | DELFILE | ADDDIR | DELDIR;

	/* 画出这个窗口 */
	drawWindowPixelData();

	View* view;
	ViewInfo viewInfo;

	/* close view */
	viewInfo.iX = windowInfo.iWidth - windowInfo.iHeight / 8;
	viewInfo.iY = 0;
	viewInfo.iWidth = windowInfo.iHeight / 8;
	viewInfo.iHeight = viewInfo.iWidth;
	viewInfo.iCareEventType = TOUCHSCREEN;
	view = new CloseView(CLOSE_VIEW_ICON, viewInfo);
	view->setParentWindow(this);
	view->showViewToWindow();
	addFixView(view);
	addView(view);

	/* return view */
	viewInfo.iX = 0;
	viewInfo.iY = 0;
	viewInfo.iWidth = windowInfo.iHeight / 8;
	viewInfo.iHeight = viewInfo.iWidth;
	viewInfo.iCareEventType = TOUCHSCREEN;
	view = new ReturnView(RETURN_VIEW_ICON, viewInfo);
	view->setParentWindow(this);
	view->showViewToWindow();
	addFixView(view);
	addView(view);

	/* up view */
	viewInfo.iX = windowInfo.iWidth - windowInfo.iHeight / 4 - 1;
	viewInfo.iY = (((windowInfo.iHeight - windowInfo.iHeight / 8) / 2) - windowInfo.iHeight / 4) / 2 + windowInfo.iHeight / 8;
	viewInfo.iWidth = windowInfo.iHeight / 4;
	viewInfo.iHeight = viewInfo.iWidth;
	viewInfo.iCareEventType = TOUCHSCREEN;
	view = new PageUpView(PAGEUP_VIEW_ICON, viewInfo);
	view->setParentWindow(this);
	view->showViewToWindow();
	addFixView(view);
	addView(view);
	

	/* dowm view */
	viewInfo.iX = windowInfo.iWidth - windowInfo.iHeight / 4 - 1;
	viewInfo.iY = (((windowInfo.iHeight - windowInfo.iHeight / 8) / 2) - windowInfo.iHeight / 4) / 2 + windowInfo.iHeight / 8 + ((windowInfo.iHeight - windowInfo.iHeight / 8) / 2);
	viewInfo.iWidth = windowInfo.iHeight / 4;
	viewInfo.iHeight = viewInfo.iWidth;
	viewInfo.iCareEventType = TOUCHSCREEN;
	view = new PageDowmView(PAGEDOWM_VIEW_ICON, viewInfo);
	view->setParentWindow(this);
	view->showViewToWindow();
	addFixView(view);
	addView(view);


	/* 扫描文件 */
	setMaxFileViewSum();
	scanDir("/");
	
	
}



FileManagerWindow::FileManagerWindow(const std::string strDirPath)
:Window()
{
	setRemovable(true);
	struct WindowInfo& windowInfo = getWindowInfo();
	
	windowInfo.iWidth = FrameBuffer::getFrameBuffer()->getScreenInfo().iXres / 5 * 3;
	windowInfo.iHeight = windowInfo.iWidth / 3 * 2;

	struct tPoint& point = WindowManager::getWindowManager()->getNextPoint();
	windowInfo.iX = point.iX;
	windowInfo.iY = point.iY;
	WindowManager::getWindowManager()->updatePoint();

	windowInfo.iCareEventType = TOUCHSCREEN | ADDFILE | DELFILE | ADDDIR | DELDIR;

	/* 画出这个窗口 */
	drawWindowPixelData();

	View* view;
	ViewInfo viewInfo;

	/* close view */
	viewInfo.iX = windowInfo.iWidth - windowInfo.iHeight / 8;
	viewInfo.iY = 0;
	viewInfo.iWidth = windowInfo.iHeight / 8;
	viewInfo.iHeight = viewInfo.iWidth;
	viewInfo.iCareEventType = TOUCHSCREEN;
	view = new CloseView(CLOSE_VIEW_ICON, viewInfo);
	view->setParentWindow(this);
	view->showViewToWindow();
	addFixView(view);
	addView(view);

	/* return view */
	viewInfo.iX = 0;
	viewInfo.iY = 0;
	viewInfo.iWidth = windowInfo.iHeight / 8;
	viewInfo.iHeight = viewInfo.iWidth;
	viewInfo.iCareEventType = TOUCHSCREEN;
	view = new ReturnView(RETURN_VIEW_ICON, viewInfo);
	view->setParentWindow(this);
	view->showViewToWindow();
	addFixView(view);
	addView(view);

	/* up view */
	viewInfo.iX = windowInfo.iWidth - windowInfo.iHeight / 4 - 1;
	viewInfo.iY = (((windowInfo.iHeight - windowInfo.iHeight / 8) / 2) - windowInfo.iHeight / 4) / 2 + windowInfo.iHeight / 8;
	viewInfo.iWidth = windowInfo.iHeight / 4;
	viewInfo.iHeight = viewInfo.iWidth;
	viewInfo.iCareEventType = TOUCHSCREEN;
	view = new PageUpView(PAGEUP_VIEW_ICON, viewInfo);
	view->setParentWindow(this);
	view->showViewToWindow();
	addFixView(view);	
	addView(view);

	/* dowm view */
	viewInfo.iX = windowInfo.iWidth - windowInfo.iHeight / 4 - 1;
	viewInfo.iY = (((windowInfo.iHeight - windowInfo.iHeight / 8) / 2) - windowInfo.iHeight / 4) / 2 + windowInfo.iHeight / 8 + ((windowInfo.iHeight - windowInfo.iHeight / 8) / 2);
	viewInfo.iWidth = windowInfo.iHeight / 4;
	viewInfo.iHeight = viewInfo.iWidth;
	viewInfo.iCareEventType = TOUCHSCREEN;
	view = new PageDowmView(PAGEDOWM_VIEW_ICON, viewInfo);
	view->setParentWindow(this);
	view->showViewToWindow();
	addFixView(view);
	addView(view);


	/* 扫描文件 */
	setMaxFileViewSum();
	scanDir(strDirPath);
	
	
}


int FileManagerWindow::isRegFile(char *strFilePath)
{
    struct stat tStat;

    if ((stat(strFilePath, &tStat) == 0) && S_ISREG(tStat.st_mode)) {
        return 1;
    }
    else {
        return 0;
    }
}


int FileManagerWindow::isPicFile(char *strFilePath)
{
	if ((!strcmp(PicParser::getFilenameExt(strFilePath), ".jpg")) \
		|| (!strcmp(PicParser::getFilenameExt(strFilePath), ".png")) \
		|| (!strcmp(PicParser::getFilenameExt(strFilePath), ".bmp"))) {	
		/* 图片文件 */
		return 1;
	}

	return 0;
}


int FileManagerWindow::isMusicFile(char *strFilePath)
{
	if (!strcmp(PicParser::getFilenameExt(strFilePath), ".mp3")) {
		/* MP3文件 */
		return 1;
	}

	return 0;
}


int FileManagerWindow::isTextFile(char *strFilePath)
{
	TextParser *textParser = new TextParser();

	if (!textParser->openTextInit(strFilePath)) {
		/* 是文档文件 */
		delete textParser;
		return 1;
	}

	delete textParser;
	return 0;
	
}

int FileManagerWindow::isNesGame(char *strFilePath)
{
	if (!strcmp(PicParser::getFilenameExt(strFilePath), ".nes")) {
		/* NES文件 */
		return 1;
	}

	return 0;	
}

int FileManagerWindow::isDir(char *strFilePath)
{
    struct stat tStat;

    if ((stat(strFilePath, &tStat) == 0) && S_ISDIR(tStat.st_mode)) {
        return 1;
    }
    else {
        return 0;
    }
}

int FileManagerWindow::scanDir(const std::string& dirPath)
{
	DIR *pDir;
	struct dirent *file;
	struct WindowInfo& windowInfo = getWindowInfo();
	TextParser *textParser = new TextParser();
	int iStrLen = 0;
	int iFontX = 0;
	int iFontY = 0;

	View* view = NULL;		
	ViewInfo viewInfo;
	int iX = 10;
	int iY = windowInfo.iHeight / 8;
	char strTmp[256];			

	mCurFileViewSum = 0;
	mCurPageCount = 1;
	isEnd = false;


	/* 显示当前所在目录名称 */
	setCurDirName(dirPath);	
	showInfoTitleBar(dirPath, textParser);


	if(!(pDir = opendir(dirPath.c_str()))) {
		printf("error opendir %s!!!/n", dirPath.c_str());
		return -1;
	}
	
	viewInfo.iWidth = windowInfo.iHeight / 4;
	viewInfo.iHeight = viewInfo.iWidth;
	while(1) {
		file = readdir(pDir);
		if (file == NULL) {
			isEnd = true;
			return 0;
		}
		
		if(strncmp(file->d_name, ".", 1) == 0)
			continue;


		/* 判断是否超出空间 */
		if ((iX + viewInfo.iWidth) > windowInfo.iWidth) {
			iX = 10;
			
			iY += viewInfo.iHeight;
			iY += 10;
			if ((iY + viewInfo.iHeight) > windowInfo.iHeight) {
				break;
			}
		}


		memset(strTmp, 0, 256);
		if (!strcmp(dirPath.c_str() , "/")) {
			sprintf(strTmp, "%s%s", dirPath.c_str(), file->d_name);						
		}
		else {
			sprintf(strTmp, "%s/%s", dirPath.c_str(), file->d_name);
		}

		mCurFileViewSum++;
		viewInfo.iX = iX;
		viewInfo.iY = iY;
		viewInfo.iCareEventType = TOUCHSCREEN;

		if (isDir(strTmp)) {
			/* 目录 */		
			view = new DirView(DIR_VIEW_ICON, viewInfo);
		}
		else if (isRegFile(strTmp)) {
			/* 常规文件 */
			if (isPicFile(strTmp)) {
				view = new PicView(PIC_VIEW_ICON, viewInfo);			
			}
			else if (isMusicFile(strTmp)) {
				view = new MusicView(MUSIC_VIEW_ICON, viewInfo);			
			}
			else if (isNesGame(strTmp)) {
				view = new GameView(GAME_VIEW_ICON, viewInfo);
			}
			else if (isTextFile(strTmp)) {
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
		view->setParentWindow(this);
		
		iStrLen = strlen(file->d_name);
		if ((iStrLen << 3) > viewInfo.iWidth) {
			iStrLen = viewInfo.iWidth >> 3;
		}

		iFontX = (viewInfo.iWidth - (iStrLen << 3)) / 2;
		iFontY = viewInfo.iHeight / 4 * 3;
		textParser->putStr(view->getViewInfoRef().tZoomPixelDatas, iFontX, iFontY, file->d_name, iStrLen, 0xFFFFFFFF);
		view->showViewToWindow();

		addView(view);

		
		iX += viewInfo.iWidth;
		iX += 10;
		
	}

	closedir(pDir);
	delete textParser;
	return 0;

}

/* 计算一页最多可以显示多少个view */
int FileManagerWindow::setMaxFileViewSum(void)
{
	mMaxFileViewSum = 0;
	int iX = 10;
	int iY = getWindowInfo().iHeight / 8;
	int iWidth = getWindowInfo().iHeight / 4;
	int iHeight = iWidth;
	
	while (1) {
			mMaxFileViewSum++;
			iX += iWidth;
			iX += 10;
			if ((iX + iWidth) > getWindowInfo().iWidth) {
				iX = 10;
				
				iY += iHeight;
				iY += 10;
				if ((iY + iHeight) > getWindowInfo().iHeight) {
					break;
				}
			}			
	}

	return 0;
}


int FileManagerWindow::getMaxFileViewSum(void)
{
	return mMaxFileViewSum;
}

int FileManagerWindow::getCurFileViewSum(void)
{
	return mCurFileViewSum;
}

void FileManagerWindow::setCurFileViewSum(int iCurFileViewSum)
{
	mCurFileViewSum = iCurFileViewSum;
}


int FileManagerWindow::getCurPageCount(void)
{
	return mCurPageCount;	
}


void FileManagerWindow::incCurPageCount(void)
{
	mCurPageCount++;
}

void FileManagerWindow::decCurPageCount(void)
{
	mCurPageCount--;
}

bool FileManagerWindow::isEndPage(void)
{
	return isEnd;
}

void FileManagerWindow::setEndPageFlag(bool mIsEnd)
{
	isEnd = mIsEnd;
}


std::string& FileManagerWindow::getCurDirName(void)
{
	return mCurDirName;
}

void FileManagerWindow::setCurDirName(const std::string& strCurDirName)
{
	mCurDirName = strCurDirName;
}

void FileManagerWindow::clearContentArea(void)
{
	FrameBuffer::getFrameBuffer()->FbDrawRectangle(getWindowInfo().tWindowCurPixelDatas, \
			0, getWindowInfo().iHeight / 8, getWindowInfo().iWidth, getWindowInfo().iHeight - getWindowInfo().iHeight / 8, 0xFFFFFFFF);
}


void FileManagerWindow::drawWindowPixelData(void)
{
	struct WindowInfo& windowInfo = getWindowInfo();
	windowInfo.tWindowCurPixelDatas.iBpp = FrameBuffer::getFrameBuffer()->getScreenInfo().iBpp;
	windowInfo.tWindowCurPixelDatas.iWidth = windowInfo.iWidth;
	windowInfo.tWindowCurPixelDatas.iHeight = windowInfo.iHeight;
	windowInfo.tWindowCurPixelDatas.iLineBytes = windowInfo.iWidth * windowInfo.tWindowCurPixelDatas.iBpp / 8;
	windowInfo.tWindowCurPixelDatas.iTotalBytes = windowInfo.tWindowCurPixelDatas.iLineBytes * windowInfo.tWindowCurPixelDatas.iHeight;
	windowInfo.tWindowCurPixelDatas.pcPixelDatas = new char[windowInfo.tWindowCurPixelDatas.iTotalBytes];

	/* 先绘制顶部的功能栏 */
	FrameBuffer::getFrameBuffer()->FbDrawRectangle(windowInfo.tWindowCurPixelDatas,\
	0, 0, windowInfo.tWindowCurPixelDatas.iWidth, windowInfo.iHeight / 8, 0);

	/* 绘制内容区 */
	FrameBuffer::getFrameBuffer()->FbDrawRectangle(windowInfo.tWindowCurPixelDatas,\
	0, windowInfo.iHeight / 8, windowInfo.tWindowCurPixelDatas.iWidth, windowInfo.tWindowCurPixelDatas.iHeight - windowInfo.iHeight / 10, 0xFFFFFFFF);



	/* 绘制底部一条边 */
	FrameBuffer::getFrameBuffer()->FbDrawLine(windowInfo.tWindowCurPixelDatas, \
		0, windowInfo.iHeight - 1, windowInfo.iWidth, windowInfo.iHeight -1 , 0);
	
	/* 绘制右边一条边 */
	FrameBuffer::getFrameBuffer()->FbDrawLine(windowInfo.tWindowCurPixelDatas, \
		windowInfo.iWidth - 1, 0, windowInfo.iWidth - 1, windowInfo.iHeight, 0);

	/* 绘制左边一条边 */
	FrameBuffer::getFrameBuffer()->FbDrawLine(windowInfo.tWindowCurPixelDatas, \
		0, 0, 0, windowInfo.iHeight, 0);
	
}


int FileManagerWindow::onProcess(struct CookEvent& cookEvent)
{
	int iRet = 0;


	if (isMoved()) {
		if (cookEvent.iPressure == 0) {
			setMoved(false);
		}
		
		moveWindow(cookEvent.iX, cookEvent.iY);
		return 0;
	}

	iRet = dispatchEvent(cookEvent);
	if (iRet < 0) {
		/* 没有支持view */	
		if (isRemovable() && isLongPress(cookEvent.tTime)) {
			/* 这个window可移动并且长按了 */
			setMoved(true); 
		}	
	}
		
	return iRet;
}


