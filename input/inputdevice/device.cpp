#include <inputdevice/device.h>


int Device::isRegFile(char *strFilePath)
{
    struct stat tStat;

    if ((stat(strFilePath, &tStat) == 0) && S_ISREG(tStat.st_mode)) {
        return 1;
    }
    else {
        return 0;
    }
}


int Device::isDir(char *strFilePath)
{
    struct stat tStat;

    if ((stat(strFilePath, &tStat) == 0) && S_ISDIR(tStat.st_mode)) {
        return 1;
    }
    else {
        return 0;
    }
}


/* 判断是否为字符设备 */
int Device::isCharDevice(char *strFilePath)
{
    struct stat tStat;


    if ((stat(strFilePath, &tStat) == 0) && S_ISCHR(tStat.st_mode)) {
        return 1;
    }
    else {
        return 0;
    }
}

/* 判断是否为块设备 */
int Device::isBlkDevice(char *strFilePath)
{
    struct stat tStat;

    if ((stat(strFilePath, &tStat) == 0) && S_ISBLK(tStat.st_mode)) {
        return 1;
    }
    else {
        return 0;
    }
}


void Device::setFd(int iFd)
{
	mFd = iFd;
}

int Device::getFd(void)
{
	return mFd;
}

void Device::setDeviceName(std::string deviceName)
{
	mDeviceName = deviceName;
}

std::string& Device::getDeviceName(void)
{
	return mDeviceName;
}

void Device::setStat(struct stat tStat)
{
	mStat = tStat;
}

struct stat& Device::getStat(void)
{
	return mStat;
}


