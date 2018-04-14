#include <calibrate.h>
#include <device/framebuffer.h>
#include <textparser.h>
#include <input/inputreader.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define ABS(X) ((X) < 0 ? (-1 * (X)) : (X))


Calibrate *Calibrate::calibrate = NULL;
pthread_mutex_t Calibrate::tMutex  = PTHREAD_MUTEX_INITIALIZER;

Calibrate* Calibrate::getCalibrate(void)
{
	if (NULL == calibrate) {		
		pthread_mutex_lock(&tMutex);
		if (NULL == calibrate) {
			calibrate = new Calibrate();
		}
		pthread_mutex_unlock(&tMutex);
	}

	return calibrate;
}


Calibrate::Calibrate() 
{
	mHasCalibrated = false;
	mtCalibrateInfo.mInterchangeXY = false;
}


/* 返回0表示成功, 返回-1表示没有找到文件，需要执行 startCalibrate 函数 */
int Calibrate::readConfigSetCalibrateInfo(void)
{
	if (access("/etc/calibrateInfo", F_OK) < 0) {
		/* 文件不存在 */
		startCalibrate();
	}
	else {
		int iFd = open("/etc/calibrateInfo", O_RDWR);
		if (iFd < 0) {
			printf("open /etc/calibrateInfo error!\n");
			return -1;
		}
		mHasCalibrated = true;
		read(iFd, &mtCalibrateInfo, sizeof(struct CalibrateInfo));		
	}

	return 0;
}

void Calibrate::showUsageToLCD(void)
{
	int iFontSize = 16;
	int iX = 0;
	int iY = 0;
	int iLen = 0;
	int iWidth = 0;
	int iHeight = 0;

	FrameBuffer::getFrameBuffer()->FbCleanScreen(0);	
	TextParser *textParser = new TextParser();
	
	iLen = strlen("此处为触摸屏校准程序");
	iWidth  = iLen / 3 * iFontSize;
	iHeight = iFontSize;
	iX = (FrameBuffer::getFrameBuffer()->getScreenInfo().iXres - iWidth) / 2;
	iY = iY + iHeight;
	textParser->putStrToLCD(iX, iY, "此处为触摸屏校准程序", iLen);\


	iLen = strlen("请按屏幕显示步骤准确的按下五个提示点");
	iWidth  = iLen / 3 * iFontSize;
	iHeight = iFontSize;
	iX = (FrameBuffer::getFrameBuffer()->getScreenInfo().iXres - iWidth) / 2;
	iY = iY + iHeight;
	textParser->putStrToLCD(iX, iY, "请按屏幕显示步骤准确的按下五个提示点", iLen);

	delete textParser;
	
}

bool Calibrate::isCalibrated(void)
{
	return mHasCalibrated;
}

struct CalibrateInfo& Calibrate::getCalibrateInfo(void)
{
	return mtCalibrateInfo;
}

void Calibrate::showTargetToLCD(int iX, int iY, int& iXL, int& iYL)
{
	int iR = 8;
	int iRet;
	struct CookEvent cookEvent[128];
	
	FrameBuffer::getFrameBuffer()->FbFastCircle(iX, iY, iR, 0x0000CD);

	while (1) {
		iRet = InputReader::getInputReader()->readOnce(cookEvent, 128);
		if (iRet <= 0)
			continue;

		if (cookEvent->iType == TOUCHSCREEN) {
			if (cookEvent->iPressure == 1) {
				/* 按下 */
				iXL = cookEvent->iX;
				iYL = cookEvent->iY;
			}
			else {
				break;
			}
		}

	}

	FrameBuffer::getFrameBuffer()->FbFastCircle(iX, iY, iR, 0);
	
}


void Calibrate::startCalibrate(void)
{
	int iX = 0;
	int iY = 0;
	int iOffset = 50;

	struct PointDesc tPoint[5];
	struct PointDesc tPoint_L[5];

	memset(&mtCalibrateInfo, 0, sizeof(struct CalibrateInfo));
	showUsageToLCD();
	
	/* 左上角 */
	iX = iOffset;
	iY = iOffset;
	tPoint[0].iX = iX;
	tPoint[0].iY = iY;
	showTargetToLCD(iX, iY, tPoint_L[0].iX, tPoint_L[0].iY);


	iX = FrameBuffer::getFrameBuffer()->getScreenInfo().iXres - iOffset;
	iY = iOffset;
	/* 右上角 */
	tPoint[1].iX = iX;
	tPoint[1].iY = iY;
	showTargetToLCD(iX, iY, tPoint_L[1].iX, tPoint_L[1].iY);
	

	if (ABS(tPoint_L[1].iX - tPoint_L[0].iX) < ABS(tPoint_L[1].iY - tPoint_L[0].iY)) {
		/* 说明X Y坐标应该互换   */
		mtCalibrateInfo.mInterchangeXY = true;
		int iTmp;
		iTmp = tPoint_L[0].iX;
		tPoint_L[0].iX = tPoint_L[0].iY;
		tPoint_L[0].iY = iTmp;

		iTmp = tPoint_L[1].iX;
		tPoint_L[1].iX = tPoint_L[1].iY;
		tPoint_L[1].iY = iTmp;
		
		iX = FrameBuffer::getFrameBuffer()->getScreenInfo().iXres - iOffset;
		iY = FrameBuffer::getFrameBuffer()->getScreenInfo().iYres - iOffset;
		/* 右下角 */
		tPoint[2].iX = iX;
		tPoint[2].iY = iY;
		showTargetToLCD(iX, iY, tPoint_L[2].iY, tPoint_L[2].iX);


		iX = iOffset;
		iY = FrameBuffer::getFrameBuffer()->getScreenInfo().iYres - iOffset;
		/* 左下角 */
		tPoint[3].iX = iX;
		tPoint[3].iY = iY;
		showTargetToLCD(iX, iY, tPoint_L[3].iY, tPoint_L[3].iX);


		iX = FrameBuffer::getFrameBuffer()->getScreenInfo().iXres >> 1;
		iY = FrameBuffer::getFrameBuffer()->getScreenInfo().iYres >> 1;
		/* 中心 */
		tPoint[4].iX = iX;
		tPoint[4].iY = iY;
		showTargetToLCD(iX, iY, tPoint_L[4].iY, tPoint_L[4].iX);			
		
	}
	else {
		iX = FrameBuffer::getFrameBuffer()->getScreenInfo().iXres - iOffset;
		iY = FrameBuffer::getFrameBuffer()->getScreenInfo().iYres - iOffset;
		/* 右下角 */
		tPoint[2].iX = iX;
		tPoint[2].iY = iY;
		showTargetToLCD(iX, iY, tPoint_L[2].iX, tPoint_L[2].iY);


		iX = iOffset;
		iY = FrameBuffer::getFrameBuffer()->getScreenInfo().iYres - iOffset;
		/* 左下角 */
		tPoint[3].iX = iX;
		tPoint[3].iY = iY;
		showTargetToLCD(iX, iY, tPoint_L[3].iX, tPoint_L[3].iY);


		iX = FrameBuffer::getFrameBuffer()->getScreenInfo().iXres >> 1;
		iY = FrameBuffer::getFrameBuffer()->getScreenInfo().iYres >> 1;
		/* 中心 */
		tPoint[4].iX = iX;
		tPoint[4].iY = iY;
		showTargetToLCD(iX, iY, tPoint_L[4].iX, tPoint_L[4].iY);		
	}


	int aS_L[4];
	int aS[4];

	/* 逻辑值计算四个边 */
	
	aS_L[0] = tPoint_L[1].iX - tPoint_L[0].iX;
	aS_L[1] = tPoint_L[2].iY - tPoint_L[1].iY;
	aS_L[2] = tPoint_L[2].iX - tPoint_L[3].iX;
	aS_L[3] = tPoint_L[3].iY - tPoint_L[0].iY;

	/* 物理值计算四个边 */
	aS[0] = tPoint[1].iX - tPoint[0].iX;
	aS[1] = tPoint[2].iY - tPoint[1].iY;
	aS[2] = tPoint[2].iX - tPoint[3].iX;
	aS[3] = tPoint[3].iY - tPoint[0].iY;

	mtCalibrateInfo.iKx = ((double)(aS_L[0] + aS_L[2]) / 2) / aS[0];
	mtCalibrateInfo.iKy = ((double)(aS_L[1] + aS_L[3]) / 2) / aS[1];
	mtCalibrateInfo.iXLc = tPoint_L[4].iX;
	mtCalibrateInfo.iYLc = tPoint_L[4].iX;
	mtCalibrateInfo.iXc = tPoint[4].iX;
	mtCalibrateInfo.iYc = tPoint[4].iY;	
	mHasCalibrated = true;

	if (!access("/etc/calibrateInfo", F_OK)) {
		/* 文件存在，删掉 */
		remove("/etc/calibrateInfo");
	}

	int iFd = open("/etc/calibrateInfo", O_RDWR | O_CREAT);
	if (iFd < 0) {
		printf("open /etc/calibrateInfo error!\n");
		return;
	}

	write(iFd, &mtCalibrateInfo, sizeof(struct CalibrateInfo));
	
}

