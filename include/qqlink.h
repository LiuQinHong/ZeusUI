#ifndef __QQLINK_H__
#define __QQLINK_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class QQlink;


#include <TXSDK/TXSDKCommonDef.h>
#include <TXSDK/TXDeviceSDK.h>
#include <TXSDK/TXDataPoint.h>
#include <TXSDK/TXDataPoint.h>
#include <TXSDK/TXFileTransfer.h>
#include <TXSDK/TXMsg.h>
#include <TXSDK/TXAudioVideo.h>
#include <pthread.h>


class QQlink {
public:	
	int initDevice(void);	
	void exitDevice(void);
	void logOn();
	void logOff();

	void initDataPoint(void);
	void initFileTransfer(void);
	static QQlink* getQQlink(void);

	void sendTextMsg(char *pcText);	
	void sendPicMsg(structuring_msg& msg);
	void sendAudioMsg(structuring_msg& msg);
	void sendVideoMsg(structuring_msg& msg);

	bool isConnect(void);
	void setConnect(bool isConnect);
	
private:
	QQlink();
	bool mConnect;
	static QQlink *qqlink;
	static pthread_mutex_t tMutex;
	
};



#endif //__QQLINK_H__







