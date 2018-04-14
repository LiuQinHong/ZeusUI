#ifndef __WINDOWMANAGER_H__
#define __WINDOWMANAGER_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class WindowManager;


#include <iostream>
#include <list>
#include <view/view.h>
#include <pic/picparser.h>
#include <window/window.h>
#include <threadpool.h>


#define BACKGROUND_COLOR 0x36648B

void showFunc(void *data);

enum TaskType{
	NEW_WINDOW		= 0,
	CLOSE_WINDOW	= 1,
	MOVE_WINDOW		= 2,
	REFRESH_WINDOW	= 3
};

struct TaskData {
	enum TaskType taskType;
    Window *window;
};


struct tPoint {
	int iX;
	int iY;
};

class WindowManager {
public:
	~WindowManager();
	void addTask(Window *window, enum TaskType taskType);
	void addWindow(Window *pWindow);
	void removeWindow(Window *pWindow);
	void sortWindow(void);
	int dispatchTouchEvent(struct CookEvent& cookEvent);
	int dispatchNetEvent(struct CookEvent& cookEvent);
	int dispatchFileEvent(struct CookEvent& cookEvent);
	int dispatchDeviceEvent(struct CookEvent& cookEvent);
	struct PixelDatas& getWindowManagerPixelDatas(void);
	void clear(void);
	void showAllWindow(void);
	void showWindowManager(void);
	struct tPoint& getNextPoint(void);
	void updatePoint(void);
	int onProcess(struct CookEvent *cookEvent, int iSize);	
	static WindowManager* getWindowManager(void);
private:
	Window *mCurProcessingWindow;
	std::list<Window*> windowList;
	struct PixelDatas mtWindowManagerPixelDatas;
	static WindowManager *windowManager;
	struct tPoint mNextPoint;
	bool mIsUpdate;
	ThreadPool *mThreadPool;
	WindowManager();

};



#endif //__WINDOWMANAGER_H__
