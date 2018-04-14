#ifndef __TX_WIFI_SYNC_H__
#define __TX_WIFI_SYNC_H__

#include "TXSDKCommonDef.h"

CXX_EXTERN_BEGIN

/*
 *                      SmartLink API简介
 * SmartLink API有两套接口，目前推荐使用2.0接口，2.0接口简单，1.0接口使用复杂不推荐使用；
 * 三部分构成：common API：公共部分，1.0、2.0接口均使用；
             2.0接口API：2.0接口部分；
             1.0接口API：1.0接口部分；
 */

////////////////////////////////////////////////////////////////////////////////////////
//     
//        			    			common API
//
////////////////////////////////////////////////////////////////////////////////////////

/** init_wifi_sync & start_smartlink的返回值使用此enum定义
 */
enum wifisyncerror {
	QLERROR_INIT_SUCCESS        = 0,         //suc
	QLERROR_MEMORY_ALLOC  		= 1,        //内存分配失败
	QLERROR_NOTIFY_NULL		  	= 2,        //FUNC_NOTIFY回调函数为空
	QLERROR_PARAM_KEY       	= 3,        //key传入为NULL
	QLERROR_PARAM_KEY_LEN   	= 4,        //key长度不合法
	QLERROR_OPENSSL_LOAD_FAILED = 5,        //加载openssl库失败
	QLERROR_HOP_NULL			= 6,		//跳频回调函数为空
	QLERROR_SZIFNAME_INVALID	= 7,		//szifname不合法
	QLERROR_WIFICHIP_NOTSUPPORT = 8,		//此WIFI CHIP 2.0接口不支持
	QLERROR_INIT_OTHER		    = 9,		//其他错误
};

/** fill_80211_frame的返回值使用此enum定义
 */
enum fill80211relust {
	QLERROR_SUCCESS 		= 0,		//解析成功
	QLERROR_HOP				= 1,		//收到此返回值表示当前信道锁定有误，立马切换下一个信道
	QLERROR_LOCK    		= 2,        //收到此返回值表示是当前信道，锁定

	//下面这些返回值供调试使用，切信道，锁信道根据上面三个值确定,BCAST表示Braodcast，MCAST表示Multicast
	QLERROR_OTHER			= 3,    //其它错误
	QLERROR_DECRYPT_FAILED  = 4,    //解密出错
	QLERROR_NEED_INIT     	= 5,    //wifi sync没有初始化
	QLERROR_VERSION        	= 6,    //SDK版本不对应
	QLERROR_START_FRAME     = 7, 	//无效的包

    //Broadcast relative
    QLERROR_BCAST_NOT_FRAME	= 8, 	//不是广播包
    QLERROR_BCAST_CALC_C	= 9,	//成功计算C值
    QLERROR_BCAST_ONE_DATA	= 10,	//解析到一个广播数据

    //Multicast relative
    QLERROR_MCAST_NOT_FRAME	= 11,  //不是组播包
    QLERROR_MCAST_ONE_DATA	= 12,  //解析到一个组播数据
};

#define QLMAX_SSID_LEN      128
#define QLMAX_PSWD_LEN      128
#define QLMAX_IP_LEN        16

typedef struct
{
	char                sz_ssid[QLMAX_SSID_LEN];      // AP账号名称
	char                sz_password[QLMAX_PSWD_LEN];  // AP密码
    char                sz_ip[QLMAX_IP_LEN];          // 发送端IP地址，设备连接AP后反馈给发送端ack时用到
    unsigned short      sh_port;                      // 发送端端口，  设备连接AP后反馈给发送端ack时用到
    unsigned char       sz_bssid[6];                  // AP的48位MAC地址， 主要用于连接隐藏SSID广播的AP，厂商可根据实际情况选择是否使用
	                                                  // 如果设备可以扫描到beacon帧并通过beancon中的信息连接AP则可忽略此成员
} tx_wifi_sync_param;


/**  跳