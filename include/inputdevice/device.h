#ifndef __DEVICE_H__
#define __DEVICE_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class Device;

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

class Device {
public:
	int isRegFile(char *strFilePath);
	int isDir(char *strFilePath);	
	int isCharDevice(char *strFilePath);
	int isBlkDevice(char *strFilePath);	
	void setFd(int iFd);
	int getFd(void);
	void setDeviceName(std::string deviceName);	
	std::string& getDeviceName(void);
	void setStat(struct stat tStat);	
	struct stat& getStat(void);
	
	virtual int process(int iFd, struct CookEvent* cookEvent, int iSize) = 0;

private:
	int mFd;
	std::string mDeviceName;
	struct stat mStat;	
};



#endif // __DEVICE_H__








