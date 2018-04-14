#include <inputdevice/eventdevice.h>
#include <input/inputreader.h>
#include <string.h>
#include <sys/types.h>
#include <linux/input.h>
#include <calibrate.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


static int g_iLastX;
static int g_iLastY;
static int g_iLastPressure;

static void CookTouchEvent(struct input_event& iev, struct CookEvent* cookEvent)
{
	Calibrate *calibrate = Calibrate::getCalibrate();
	struct CalibrateInfo& calibrateInfo = calibrate->getCalibrateInfo();
	switch (iev.code) {
		case ABS_X:
		{		
			if (calibrate->isCalibrated()) {
				if (calibrateInfo.mInterchangeXY) {
					/* 应该互换X Y 所以这里把本来赋给X的值赋给Y*/
					g_iLastY = ((iev.value - calibrateInfo.iYLc) / calibrateInfo.iKy) + calibrateInfo.iYc;
				}
				else {
					g_iLastX = ((iev.value - calibrateInfo.iXLc) / calibrateInfo.iKx) + calibrateInfo.iXc;
				}				
			}
			else {
				/* 没有校准的话就返回原值 */
				g_iLastX = iev.value;
			}
			break;
		}
	
		case ABS_Y:
		{
			if (calibrate->isCalibrated()) {
				if (calibrateInfo.mInterchangeXY) {
					/* 应该互换X Y 所以这里把本来赋给Y的值赋给X*/
					g_iLastX = ((iev.value - calibrateInfo.iXLc) / calibrateInfo.iKx) + calibrateInfo.iXc;				
				}
				else {
					g_iLastY = ((iev.value - calibrateInfo.iYLc) / calibrateInfo.iKy) + calibrateInfo.iYc;
				}
			}
			else {
				/* 没有校准的话就返回原值 */
				g_iLastY = iev.value;
			}
			break;	
		}
	
		case ABS_PRESSURE:
		{
			g_iLastPressure = iev.value;
			break;	
		}
	
		case BTN_TOUCH:
		{
			//printf("itouch = %d\n", iev.value);		
			break;			
		}

		default :
		{
			printf("Not About Knowing iev.code : %d\n", iev.code);
		}
	}


}


static void CookKeyEvent(struct input_event& iev, struct CookEvent* cookEvent)
{

}


static int isCharDevice_Static(char *strFilePath)
{
    struct stat tStat;


    if ((stat(strFilePath, &tStat) == 0) && S_ISCHR(tStat.st_mode)) {
        return 1;
    }
    else {
        return 0;
    }
}


int EventDevice::scanDevice(std::string devicePath)
{
	DIR *pDir;
	struct dirent *file;

	if(!(pDir = opendir(devicePath.c_str()))) {
		printf("error opendir %s!!!/n", devicePath.c_str());
		return -1;
	}

	while((file = readdir(pDir)) != NULL) {
		if(strncmp(file->d_name, ".", 1) == 0)
			continue;

		char strTmp[256];			
		snprintf(strTmp, 256, "%s/%s", devicePath.c_str(), file->d_name);
		strTmp[255] = '\0';
		if (isCharDevice_Static(strTmp)) {
			if (strncmp(file->d_name, "event", 5) == 0) {
				int iFd = open(strTmp, O_RDWR);
				if (iFd < 0) {
					fprintf(stderr, "open device %s error\n", strTmp);
					return -1;
				}
				
				EventDevice *device = new EventDevice();
				device->setFd(iFd);
				device->setDeviceName(strTmp);
				InputReader::getInputReader()->addDevice(device);	
				
			}
		}

	}
	closedir(pDir);

	return 0;

}


int EventDevice::process(int iFd, struct CookEvent* cookEvent, int iSize)
{
	
	struct input_event readBuffer[256];
	size_t capacity = 256;
	struct CookEvent* cev = cookEvent;
	int iRet = 0;

	int32_t readSize = read(iFd, readBuffer,sizeof(struct input_event) * capacity);
	if (readSize == 0 || (readSize < 0 && errno == ENODEV)) {
		/* 这个设备已经被移除，但是还没有处理inotifiy事件 */
		//printf("Device was removed before INotify noticed.\n");
		return -1;
	} else if (readSize < 0) {
		if (errno != EAGAIN && errno != EINTR) {
			fprintf(stderr, "could not get event (errno=%d)", errno);
			return -1;
		}
	} else if ((readSize % sizeof(struct input_event)) != 0) {
		fprintf(stderr, "could not get event (wrong size: %d)", readSize);
		return -1;
	}
		
	size_t count = size_t(readSize) / sizeof(struct input_event);
	for (size_t i = 0; i < count; i++) {
		struct input_event& iev = readBuffer[i];

		switch (iev.type) {
			case EV_ABS :
			{
				/* 触摸屏事件 */
				CookTouchEvent(iev, cev);
				break;
			}

			case EV_KEY :
			{
				/* 按键类事件 */
				CookKeyEvent(iev, cev);
				break;
			}

			case EV_SYN :
			{
				/* 同步事件 */
				if (iev.code == SYN_REPORT) {
					/* 单点触摸屏的事件完整 */
					cev->iX = g_iLastX;
					cev->iY = g_iLastY;
					cev->iPressure = g_iLastPressure;
					cev->iType = TOUCHSCREEN;
					gettimeofday(&cev->tTime, NULL);
					cev++;					
					iRet++;
					if (iRet == iSize) {
						/* 已经得到了iSize个CookEvent */
						return iRet;
					}

				}

			}

			default :
			{
				
			}
		}
		
	}

	return iRet;
}


