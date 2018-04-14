#ifndef __CALIBRATE_H__
#define __CALIBRATE_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
struct CalibrateInfo;
struct PointDesc;
class Calibrate;


#include <iostream>

#define CALIBRATEINFOCONFIG "/etc/CalibrateInfo"


/* X = ( XL - XLC ) / KX + XC 
 * Y = ( YL - YLC ) / KY + YC 
 */
struct CalibrateInfo {
	bool mInterchangeXY; //是否应该互换XY坐标
	double iKx;
	double iKy;
	int iXLc;	//LCD逻辑坐标中心作为校正基准点
	int iYLc;	//LCD逻辑坐标中心作为校正基准点
	int iXc;	//LCD物理坐标中心
	int iYc;	//LCD物理坐标中心
};

struct PointDesc {
	int iX;
	int iY;
};
class Calibrate {
public:
	/* 返回0表示成功, 返回-1表示没有找到文件，需要执行 startCalibrate 函数 */
	int readConfigSetCalibrateInfo(void);
	void startCalibrate(void);
	void showUsageToLCD(void);
	void showTargetToLCD(int iX, int iY, int& iXL, int& iYL);
	bool isCalibrated(void);
	struct CalibrateInfo& getCalibrateInfo(void);
	static Calibrate* getCalibrate(void);
	
private:
	struct CalibrateInfo mtCalibrateInfo;
	bool mHasCalibrated;
	static Calibrate *calibrate;
	static pthread_mutex_t tMutex;
	Calibrate();

};



#endif // __CALIBRATE_H__


