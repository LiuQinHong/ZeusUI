#include <windowmanager.h>
#include <input/inputreader.h>
#include <stdio.h>

WindowManager *WindowManager::windowManager = new WindowManager();

WindowManager* WindowManager::getWindowManager(void)
{
	return windowManager;
}


WindowManager::WindowManager()
{
	mIsUpdate = false;
	mCurProcessingWindow = NULL;
	mNextPoint.iX = 20;
	mNextPoint.iY = 20;

	mtWindowManagerPixelDatas.iBpp = FrameBuffer::getFrameBuffer()->getScreenInfo().iBpp;
	mtWindowManagerPixelDatas.iWidth = FrameBuffer::getFrameBuffer()->getScreenInfo().iXres;
	mtWindowManagerPixelDatas.iHeight = FrameBuffer::getFrameBuffer()->getScreenInfo().iYres;
	mtWindowManagerPixelDatas.iLineBytes = FrameBuffer::getFrameBuffer()->getScreenInfo().iLineWidth;
	mtWindowManagerPixelDatas.iTotalBytes = mtWindowManagerPixelDatas.iLineBytes * mtWindowManagerPixelDatas.iHeight;
	mtWindowManagerPixelDatas.pcPixelDatas = new char[mtWindowManagerPixelDatas.iTotalBytes];
	FrameBuffer::getFrameBuffer()->FbSetColour(mtWindowManagerPixelDatas, BACKGROUND_COLOR);


    mThreadPool = new ThreadPool();
    mThreadPool->set_pool_size(1);
	mThreadPool->start();
	
}

WindowManager::~WindowManager()
{
	if (mtWindowManagerPixelDatas.pcPixelDatas) {
		delete(mtWindowManagerPixelDatas.pcPixelDatas);
	}
}

void showFunc(void *data) 
{
    TaskData *td = (TaskData *) data;


	switch (td->taskType) {
		case NEW_WINDOW :
		{
			td->window->showWindow();		
			break;
		}
		case CLOSE_WINDOW :
		{
			delete td->window;
			WindowManager::getWindowManager()->clear();
			WindowManager::getWindowManager()->showAllWindow();
			break;
		}
		case MOVE_WINDOW :
		{
			WindowManager::getWindowManager()->clear();
			WindowManager::getWindowManager()->showAllWindow();	
			break;
		}
		case REFRESH_WINDOW :
		{
			td->window->showWindow();		
			break;
		}

	}
		

	WindowManager::getWindowManager()->showWindowManager();
    delete td;
}


void WindowManager::addTask(Window *window, enum TaskType taskType)
{
	TaskData *tdata = new TaskData();
	tdata->window = window;
	tdata->taskType = taskType;
	
	Task *task = new Task(showFunc, tdata);

	mThreadPool->add_task(task);
}


void WindowManager::addWindow(Window *pWindow)
{
	/* 设置一个最大level值 */
	pWindow->setLevel(~0);
	windowList.push_back(pWindow);
	sortWindow();
}

void WindowManager::removeWindow(Window *pWindow)
{
	windowList.remove(pWindow);
}


static bool compareWindowLevel(Window* first, Window* second)
{
	if (first->getWindowInfo().iLevel > second->getWindowInfo().iLevel) {
		return true;
	}

	return false;
}


/* 让window以iLevel的值从大到小排列 */
void WindowManager::sortWindow(void)
{	
	windowList.sort(compareWindowLevel);

	int iSize = (int)windowList.size();
	for(std::list<Window*>::iterator windowlist_iter = windowList.begin(); windowlist_iter != windowList.end(); windowlist_iter++) {
		/* 全部统一设置level */
		(*windowlist_iter)->setLevel(iSize--);
	}	
}


int WindowManager::dispatchTouchEvent(struct CookEvent& cookEvent)
{
	Window *windowTmp = NULL;



	if (mCurProcessingWindow) {
		  /* 当前有处理中的window */
		  /* 我们直接找到当前正在处理的window, 将事件发给它 */		  
		  mCurProcessingWindow->onProcess(cookEvent);

		  if (cookEvent.iPressure == 0) {
			  /* 松开 */
			  mCurProcessingWindow->setMoved(false);
			  mCurProcessingWindow = NULL;
		  }
		  goto done;
	}



	if (mCurProcessingWindow == NULL) {
		/* 当前没有处理中的window, 且触摸屏按下 */
		/* 根据level从大到小顺序查找关心这个事件的window */	
		for(std::list<Window*>::iterator windowlist_iter = windowList.begin(); windowlist_iter != windowList.end(); windowlist_iter++) {
			windowTmp = (*windowlist_iter);
			if (windowTmp->isCareThisEvent(cookEvent.iType) && windowTmp->isSupport(cookEvent.iX, cookEvent.iY)) {
				/* 这个window关心这个事件, 并且触点在window内 */
				mCurProcessingWindow = windowTmp;
				windowTmp->setFirstEvent(cookEvent);
				windowTmp->onProcess(cookEvent);

				if (isTopWindow(windowTmp)) {
					/* 原来就是最顶端 */
				}
				else {
					/* 如果本来不是顶端则置顶 */
					windowTmp->placeTop();					
					WindowManager::getWindowManager()->addTask(windowTmp, MOVE_WINDOW);

				}
				
				goto done;
			}
			else {
				/* 这个window不关心或则无法处理这个事件 */
				/* 找下一个window */	
				continue;
			}
		}		
	}

done:

	return 0;
}

int WindowManager::dispatchNetEvent(struct CookEvent& cookEvent)
{
	return 0;
}

int WindowManager::dispatchFileEvent(struct CookEvent& cookEvent)
{
	return 0;
}

int WindowManager::dispatchDeviceEvent(struct CookEvent& cookEvent)
{
	Window *windowTmp = NULL;

	for(std::list<Window*>::iterator windowlist_iter = windowList.begin(); windowlist_iter != windowList.end(); windowlist_iter++) {
		windowTmp = (*windowlist_iter);
		if (windowTmp->isCareThisEvent(cookEvent.iType)) {
			/* 这个window关心这个事件 */
			windowTmp->onProcess(cookEvent);
	
			if (windowTmp->getWindowInfo().iLevel == windowList.size()) {
				/* 原来就是最顶端 */
			}
			else {
				/* 如果本来不是顶端则置顶 */
				windowTmp->placeTop();					
				WindowManager::getWindowManager()->addTask(windowTmp, MOVE_WINDOW);
			
			}
			

			continue;
		}
		else {
			/* 这个window不关心或则无法处理这个事件 */
			/* 找下一个window */	
			continue;
		}
	}	

	return 0;
}


struct PixelDatas& WindowManager::getWindowManagerPixelDatas(void)
{
	return mtWindowManagerPixelDatas;
}


void WindowManager::clear(void)
{
	FrameBuffer::getFrameBuffer()->FbSetColour(mtWindowManagerPixelDatas, BACKGROUND_COLOR);	
}


/* 以level值从小到大刷新 */
void WindowManager::showAllWindow(void)
{
	Window *windowTmp = NULL;
	int iSize = (int)windowList.size();
	std::list<Window*>::iterator windowlist_iter = windowList.end();
	windowlist_iter--;

	/* 从小到大 */
	for (int i = 0; i < iSize; i++, windowlist_iter--) {
		windowTmp = (*windowlist_iter);
		windowTmp->showWindow();
	}

}


void WindowManager::showWindowManager(void)
{
	FrameBuffer::getFrameBuffer()->FbVideoMemFusion(0, 0, \
		FrameBuffer::getFrameBuffer()->getFrameBufferPixelData(), \
		mtWindowManagerPixelDatas);

}

struct tPoint& WindowManager::getNextPoint(void)
{
	return mNextPoint;
}

void WindowManager::updatePoint(void)
{
	mNextPoint.iX += 20;
	mNextPoint.iY += 20;

	if (mNextPoint.iX > (FrameBuffer::getFrameBuffer()->getScreenInfo().iXres / 4)) {
		mNextPoint.iX = 20;
		mNextPoint.iY = 20;
	}
}


bool WindowManager::isTopWindow(Window *window)
{
	return (window->getWindowInfo().iLevel == windowList.size());
}


int WindowManager::onProcess(struct CookEvent *cookEvent, int iSize)
{
	int i;
	

	for (i = 0; i < iSize; i++) {
		
		switch (cookEvent[i].iType) {
			case TOUCHSCREEN : 
			{
				return dispatchTouchEvent(cookEvent[i]);
				break;
			}

			case NETWORKINPUT : 
			{
				return dispatchNetEvent(cookEvent[i]);			
				break;
			}
			
			case DELFILE | DELDIR :
			case ADDFILE : 
			case DELFILE : 
			case ADDDIR : 
			case DELDIR : 				
			{
				return dispatchFileEvent(cookEvent[i]);
				break;				
			}

			case ADDDEVICE : 				
			case DELDEVICE : 
			{
				return dispatchDeviceEvent(cookEvent[i]);
				break;
			}
			case KEYEVENT :
			{
				//printf("key code = %d\n", cookEvent[i].iKey);
				break;
			}
			default:
			{
			}
		}			
	}

	return 0;
}


