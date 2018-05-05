#ifndef __INPUT_READER_H__
#define __INPUT_READER_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
struct CookEvent;
class InputReader;

#include <iostream>
#include <list>
#include <sys/time.h>
#include <sys/stat.h>
#include <inputdevice/device.h>

#define TOUCHSCREEN		0x00000001
#define NETWORKINPUT	0x00000002
#define ADDFILE			0x00000004
#define DELFILE			0x00000008
#define ADDDIR			0x00000010
#define DELDIR			0x00000020
#define ADDDEVICE		0x00000040
#define DELDEVICE		0x00000080
#define KEYEVENT		0x00000100


struct CookEvent {
	struct timeval tTime;
	std::string filePath;
	int iType;
	int iX;
	int iY;
	int iKey;
	int iPressure;
};



class InputReader {
public:
	~InputReader();
	void addDevice(Device* device);
	int delDevice(char *pcDevicePath);
	Device* findDevice(char *pcDevicePath);
	Device* findDevice(int iFd);
	void printAllDevice(void);
	
	int addEpollFd(int iFd);
	int delEpollFd(int iFd);

	int readOnce(struct CookEvent* cookEvent, int iSize);	
	static InputReader* getInputReader(void);
	
private:
	std::list<Device*> deviceList;
	int mEpollFd;
	static InputReader *inputReader;
	static pthread_mutex_t tMutex;
	InputReader();	 //把构造函数私有化，避免使用构造函数构造另一个实例化对象
};



#endif // __INPUT_READER_H__







