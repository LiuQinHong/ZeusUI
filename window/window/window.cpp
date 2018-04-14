#include <window/window.h>
#include <input/inputreader.h>
#include <windowmanager.h>

Window::Window()
{
	mtWindowInfo.mSelected = false;
	mtWindowInfo.mRemovable = false;
	mtWindowInfo.mPressed = false;
	mtWindowInfo.mMoved = false;
	mCurView = NULL;
}

Window::Window(const std::string& strTargetFileName)
:strFileName(strTargetFileName)
{
	mtWindowInfo.mSelected = false;
	mtWindowInfo.mRemovable = false;
	mtWindowInfo.mPressed = false;
	mtWindowInfo.mMoved = false;
	mCurView = NULL;
}


Window::~Window()
{
	delAllView();
	if (getWindowInfo().tWindowCurPixelDatas.pcPixelDatas) {
		delete getWindowInfo().tWindowCurPixelDatas.pcPixelDatas;
	}

}

struct WindowInfo& Window::getWindowInfo(void)
{
	return mtWindowInfo;
}

/* 置顶 */
void Window::placeTop(void)
{
	setLevel(~0);
	WindowManager::getWindowManager()->sortWindow();
}

void Window::setLevel(unsigned int iLevel)
{
	mtWindowInfo.iLevel = iLevel;
}

void Window::setPoint(int iX, int iY, int iWidth, int iHeight)
{
	mtWindowInfo.iX = iX;
	mtWindowInfo.iY = iY;
	mtWindowInfo.iWidth = iWidth;
	mtWindowInfo.iHeight = iHeight;
}

void Window::setWindowID(int iWindowID)
{
	mtWindowInfo.iWindowID = iWindowID;
}


void Window::setCareEvent(int eventType)
{
	mtWindowInfo.iCareEventType |= eventType;
}

void Window::cancelCareEvent(int eventType)
{
	mtWindowInfo.iCareEventType &= ~eventType;	
}

bool Window::isCareThisEvent(int eventType)
{
	return ((mtWindowInfo.iCareEventType & eventType) != 0);
}

int Window::getViewSum(void)
{
	int iRet = 0;
	
	for(std::list<View*>::iterator viewlist_iter = viewList.begin(); viewlist_iter!= viewList.end(); ++viewlist_iter) {
		iRet++;
	}

	return iRet;
}


void Window::addView(View *pView)
{
	viewList.push_back(pView);
}

void Window::addFixView(View *pView)
{
	fixviewList.push_back(pView);
}

void Window::showAllFixView(void)
{
	for(std::list<View*>::iterator viewlist_iter = fixviewList.begin(); viewlist_iter!= fixviewList.end(); ++viewlist_iter) {
		(*viewlist_iter)->showViewToWindow();
	}
}

void Window::printAllView(void)
{
	for(std::list<View*>::iterator viewlist_iter = viewList.begin(); viewlist_iter!= viewList.end(); ++viewlist_iter) {
		printf("name is %s\n", (*viewlist_iter)->getTargetFileName().c_str());
	}

}
void Window::showAllView(void)
{
	for(std::list<View*>::iterator viewlist_iter = viewList.begin(); viewlist_iter!= viewList.end(); ++viewlist_iter) {
		(*viewlist_iter)->showViewToWindow();
	}
	
}


void Window::delAllView(void)
{	
	for(std::list<View*>::iterator viewlist_iter = viewList.begin(); viewlist_iter!= viewList.end(); ++viewlist_iter) {
		delete *viewlist_iter;
	}

	viewList.clear();

}

void Window::delView(View *pView)
{	
	viewList.remove(pView);
	delete pView;
}


/* 从list中除去但是不delete */
void Window::removeView(View *pView)
{
	viewList.remove(pView);
}


void Window::delViewBack(void)
{	
	View* view = viewList.back();

	delView(view);
}


View *Window::getViewForTargetFileName(std::string& strTargetFileName)
{
	for(std::list<View*>::iterator viewlist_iter = viewList.begin(); viewlist_iter!= viewList.end(); ++viewlist_iter) {
		if (!strTargetFileName.compare((*viewlist_iter)->getTargetFileName())) {
			return (*viewlist_iter);
		}
	}

	return NULL;
}

std::string& Window::getFileName(void)
{
	return strFileName;
}


std::list<View*>& Window::getViewList(void)
{
	return viewList;
}


int Window::windowAndViewMerge(View *pView)
{
	return 0;
}


void Window::showInfoTitleBar(const std::string& Info, TextParser *textParser)
{
	struct WindowInfo& windowInfo = getWindowInfo();
	int iStrLen = 0;
	int iFontX = 0;
	int iFontY = 0;

	/* 先处理掉之前显示的目录名 */
	FrameBuffer::getFrameBuffer()->FbDrawRectangle(windowInfo.tWindowCurPixelDatas, \
			windowInfo.iHeight / 8, 0, windowInfo.iWidth - windowInfo.iHeight / 4, \
			windowInfo.iHeight / 8, 0);


	iStrLen = strlen(Info.c_str());
	if ((iStrLen << 3) > ((windowInfo.iWidth) - windowInfo.iHeight / 8)) {
		iStrLen = ((windowInfo.iWidth) - windowInfo.iHeight / 8) >> 3;
	}

	iFontX = (((windowInfo.iWidth) - windowInfo.iHeight / 8) - (iStrLen << 3)) / 2;
	iFontY = (windowInfo.iHeight / 8) / 4 * 3;
	textParser->putStr(windowInfo.tWindowCurPixelDatas, iFontX, iFontY, Info.c_str(), iStrLen, 0xFFFFFFFF);	
	setUpdateStatus(true);
}




void Window::showWindow(void)
{

	FrameBuffer::getFrameBuffer()->FbVideoMemFusion(mtWindowInfo.iX, mtWindowInfo.iY,\
		WindowManager::getWindowManager()->getWindowManagerPixelDatas(), \
		mtWindowInfo.tWindowCurPixelDatas);

	setUpdateStatus(false);		
}





void Window::moveWindow(int iX, int iY)
{

	int iDx = iX - mFirstEvent.iX;
	int iDy = iY - mFirstEvent.iY;
	
	mFirstEvent.iX = iX;
	mFirstEvent.iY = iY;
	
	mtWindowInfo.iX += iDx;
	mtWindowInfo.iY += iDy;

	if (mtWindowInfo.iX < 0) {
		mtWindowInfo.iX = 0;
	}

	if (mtWindowInfo.iY < 0) {
		mtWindowInfo.iY = 0;
	}

	WindowManager::getWindowManager()->addTask(this, MOVE_WINDOW);	
}


bool Window::isRemovable(void)
{
	return mtWindowInfo.mRemovable;
}

void Window::setRemovable(bool removable)
{
	mtWindowInfo.mRemovable = removable;
}

bool Window::isMoved(void)
{
	return mtWindowInfo.mMoved;
}

void Window::setMoved(bool Moved)
{
	mtWindowInfo.mMoved = Moved;
}


bool Window::isSupport(int iX, int iY)
{

	if ((iX > mtWindowInfo.iX) && (iX < (mtWindowInfo.iX + mtWindowInfo.iWidth)) && \
		(iY > mtWindowInfo.iY) && (iY < (mtWindowInfo.iY + mtWindowInfo.iHeight))) {
		return true;
	}

	return false;
}

bool Window::isUpdateStatus(void)
{
	return mIsUpdate;
}

void Window::setUpdateStatus(bool isUpdate)
{
	mIsUpdate = isUpdate;
}

void Window::setFirstEvent(struct CookEvent& cookEvent)
{
	mFirstEvent = cookEvent;
}

bool Window::isLongPress(struct timeval tTime)
{
	if ((tTime.tv_sec - mFirstEvent.tTime.tv_sec) >= 2) {
		return true;
	}

	return false;
}

struct CookEvent& Window::getFirstEvent(void)
{
	return mFirstEvent;
}

int Window::dispatchEvent(struct CookEvent& cookEvent)
{
	View* view;

	if (mCurView) {
		/* 当前存在正在处理的view */
		mCurView->onProcess(cookEvent);

		if (cookEvent.iPressure == 0) {
			/* 松开 */
			mCurView = NULL;		
		}
		return 0;
	}

	for(std::list<View*>::iterator viewlist_iter = viewList.begin(); viewlist_iter != viewList.end(); viewlist_iter++) {
		view = (*viewlist_iter);
		if (view->isCareThisEvent(cookEvent.iType) && view->isSupport(cookEvent.iX, cookEvent.iY)) {
			/* 这个view关心这个事件,并且能处理这个事件 */
			mCurView = view;
			view->onProcess(cookEvent);	
		}
		else {
			/* 这个view不关心或不能处理这个事件 */
			/* 找下一个view */
			continue;
		}
	}

	/* 没有view支持这个事件 */
	return -1;
}



