#ifndef __INOTIFY_DEVICE_H__
#define __INOTIFY_DEVICE_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
struct DirectoryPathDesc;
class InotifyDevice;

#include <inputdevice/device.h>
#include <iostream>
#include <list>

struct DirectoryPathDesc {
	int iWd;
	std::string directoryPath;
};


class InotifyDevice : public Device{
public:
	virtual ~InotifyDevice();
	int addWatchDirectory(std::string directoryPath);
	int delWatchDirectory(std::string directoryPath);
	int addWatchDirectory_R(std::string directoryPath);	
	int delWatchDirectory_R(std::string directoryPath);
	void printAllWatchDirectory(void);
	int processCreate(int iWd, char *fileName, struct CookEvent* cookEvent);	
	int processRemove(int iWd, char *fileName, struct CookEvent* cookEvent);
	
	
	virtual int process(int iFd, struct CookEvent* cookEvent, int iSize);	
	static InotifyDevice* getInotifyDevice(void);
	
private:
	int mINotifyFd;
	std::list<DirectoryPathDesc*> directoryPathDescList;

	static InotifyDevice *inotifyDevice;
	static pthread_mutex_t tMutex;
	InotifyDevice();

};


#endif // __INOTIFY_DEVICE_H__







