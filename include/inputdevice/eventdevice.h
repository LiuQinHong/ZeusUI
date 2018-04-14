#ifndef __EVENT_DEVICE_H__
#define __EVENT_DEVICE_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class EventDevice;

#include <inputdevice/device.h>

class EventDevice : public Device         {
public:
	static int scanDevice(std::string devicePath);
	
	virtual int process(int iFd, struct CookEvent* cookEvent, int iSize);
};



#endif // __EVENT_DEVICE_H__









