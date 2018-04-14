#include <qqlink.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>


int   Product_id=0;
char  Pubkey_file[128]={0};
char  GUID_file[128]={0};
char  Licence_file[128]={0};





static bool file_is_exist(const std::string& path)
{
	int  ret = 0;
	
	ret = access(path.c_str(), F_OK);
	if (!ret)
	    return true;

	
	return  false;
}



static int dev_config_init(void)
{
	FILE    *fp;
	char 	*line = NULL;
    size_t  len = 0;
    ssize_t read;
	char    buf[128]={0};	

 
	if(!file_is_exist("/etc/qq_iot/demo_bind/config")) {
		printf("[error]open config failed...\n");
		return 1;
	}
	
	fp = fopen("/etc/qq_iot/demo_bind/config", "rb");
	if (!fp) {
	    return 1;   
	}
	
	read = getline(&line, &len, fp);  
	memset(buf, 0, 128);
	strncpy(buf, line, read - 1);
	buf[read]='\0';
	Product_id = atoi(buf);
	//printf("Product_id =%d\n", Product_id);	
	
	
	read = getline(&line, &len, fp);
	memset(buf, 0, 128);
	strncpy(buf, line, read - 1);
	buf[read]='\0';
	sprintf(Pubkey_file, "/etc/qq_iot/demo_bind/%s", buf);
	//printf("Pubkey_file =%s\n", Pubkey_file);
	
	if(!file_is_exist(Pubkey_file) ) {
		printf("[error]access %sfailed...\n", Pubkey_file);
		return 1;
	}

	
	read = getline(&line, &len, fp);
	memset(buf, 0, 128);
	strncpy(buf, line, read - 1);
	buf[read]='\0';
	sprintf(GUID_file, "/etc/qq_iot/demo_bind/%s", buf);
	//printf("GUID_file = %s\n", GUID_file);	
	
	if(!file_is_exist(Pubkey_file)) {
		printf("[error]access %sfailed...\n", Pubkey_file);
		return 1;
	}
	
	read = getline(&line, &len, fp);
	memset(buf,0,128);
	strncpy(buf,line,read-1);
	buf[read]='\0';
	sprintf(Licence_file, "/etc/qq_iot/demo_bind/%s", buf);
	//printf("Licence_file = %s\n", Licence_file);

	if(!file_is_exist(Pubkey_file)) {
		printf("[error]access %sfailed...\n",Pubkey_file);
		return 1;
	}

   if (line)
	   free(line);
   
	fclose(fp);
    return 0;
}


/**
 * 辅助函数：SDK的log输出回调
 * SDK内部调用改log输出函数，有助于开发者调试程序
 */
static void log_func(int level, const char* module, int line, const char* message)
{
	printf("%s\n", message);
}


/**
 * 登录完成的通知，errcode为0表示登录成功，其余请参考全局的错误码表
 */
static void on_login_complete(int errcode) {
    printf("on_login_complete | code[%d]\n", errcode);
} 
 
/**
 * 在线状态变化通知， 状态（status）取值为 11 表示 在线， 取值为 21 表示  离线
 * old是前一个状态，new是变化后的状态（当前）
 */
static void on_online_status(int iOld, int iNew)
{
	//int err =0;

    printf("old status: %d\n",iOld);
    printf("new status: %d\n",iNew);
    printf("online status: %s\n", 11 == iNew ? "true" : "false");

	QQlink::getQQlink()->setConnect(11 == iNew ? true : false);

}





/**
 * 辅助函数: 从文件读取buffer
 * 这里用于读取 license 和 guid
 * 这样做的好处是不用频繁修改代码就可以更新license和guid
 */
static bool readBufferFromFile(char *pPath, unsigned char *pBuffer, int nInSize, int *pSizeUsed) 
{
	if (!pPath || !pBuffer)
		return false;

	int uLen = 0;
	FILE * file = fopen(pPath, "rb");
	if (!file)
	    return false;

	fseek(file, 0L, SEEK_END);
	uLen = ftell(file);
	fseek(file, 0L, SEEK_SET);

	if (0 == uLen || nInSize < uLen) {
		printf("invalide file or buffer size is too small...\n");
        fclose(file);
		return false;
	}

	*pSizeUsed = fread(pBuffer, 1, uLen, file);
	if (pBuffer[uLen-1] == 0x0a) {
		*pSizeUsed = uLen - 1;
		pBuffer[uLen-1] = '\0';
	}

	//printf("len:%d, ulen:%d\n",uLen, *pSizeUsed);
	fclose(file);
	return true;
}

void QQlink::logOn()
{
	tx_set_log_func(log_func, 1, 1);
}
    
void QQlink::logOff()
{
	tx_set_log_func(NULL, 0, 0);
}	


/**
 * SDK初始化
 * 例如：
 * （1）填写设备基本信息
 * （2）打算监听哪些事件，事件监听的原理实际上就是设置各类消息的回调函数，
 * 	例如设置Datapoint消息通知回调：
 * 	开发者应该定义如下的 my_on_receive_data_point 函数，将其赋值tx_data_point_notify对象中对应的函数指针，并初始化：
 *
 * 			tx_data_point_notify msgOnRecv= {0};
 * 			msgOnRecv.on_receive_data_point = my_on_receive_data_point;
 * 			tx_init_data_point(&msgOnRecv);
 *
 * 	那么当SDK内部的一个线程收到对方发过来的DataPoint消息后（通过服务器转发），将同步调用 msgOnRecv.on_receive_data_point 
 */
int QQlink::initDevice(void)
{
	if(dev_config_init()) {
		return false;
	}
	
	// 读取 license
	unsigned char license[256] = {0};
	int nLicenseSize = 0;

	if (!readBufferFromFile(Licence_file, license, sizeof(license), &nLicenseSize)) {
		printf("[error]get license from file failed...\n");
		return false;
	}

	// 读取guid
	unsigned char guid[32] = {0};
	int nGUIDSize = 0;
	if(!readBufferFromFile(GUID_file, guid, sizeof(guid), &nGUIDSize)) {
		printf("[error]get guid from file failed...\n");
		return false;
	}

    unsigned char svrPubkey[256] = {0};
    int nPubkeySize = 0;
    if (!readBufferFromFile(Pubkey_file, svrPubkey, sizeof(svrPubkey), &nPubkeySize))
    {
        printf("[error]get svrPubkey from file failed...\n");
        return false;
    }

    // 设备的基本信息
    tx_device_info info = {0};
    info.os_platform            = (char *)"Linux";

    info.device_name            = (char *)"demo1";
    info.device_serial_number   = (char *)guid;
    info.device_license         = (char *)license;
    info.product_version        = 1;
    info.network_type			= network_type_wifi;
    info.product_id             = Product_id;
    info.server_pub_key         = (char *)svrPubkey;

	// 设备登录、在线状态、消息等相关的事件通知
	// 注意事项：
	// 如下的这些notify回调函数，都是来自硬件SDK内部的一个线程，所以在这些回调函数内部的代码一定要注意线程安全问题
	// 比如在on_login_complete操作某个全局变量时，一定要考虑是不是您自己的线程也有可能操作这个变量
    tx_device_notify notify      = {0};
    notify.on_login_complete     = on_login_complete;
    notify.on_online_status      = on_online_status;
    notify.on_binder_list_change = NULL;

    // SDK初始化目录，写入配置、Log输出等信息
    // 为了了解设备的运行状况，存在上传异常错误日志 到 服务器的必要
    // system_path：SDK会在该目录下写入保证正常运行必需的配置信息
    // system_path_capicity：是允许SDK在该目录下最多写入多少字节的数据（最小大小：10K，建议大小：100K）
    // app_path：用于保存运行中产生的log或者crash堆栈
    // app_path_capicity：同上，（最小大小：300K，建议大小：1M）
    // temp_path：可能会在该目录下写入临时文件
    // temp_path_capicity：这个参数实际没有用的，可以忽略
    tx_init_path init_path = {0};
    init_path.system_path = (char *)"./";
    init_path.system_path_capicity = 100 * 1024;
    init_path.app_path = (char *)"./";
    init_path.app_path_capicity = 1024 * 1024;
    init_path.temp_path = (char *)"./";
    init_path.temp_path_capicity = 10 * 1024;

    // 设置log输出函数，如果不想打印log，则无需设置。
    // 建议开发在开发调试阶段开启log，在产品发布的时候禁用log。
    //tx_set_log_func(log_func, 1, 1);

    // 初始化SDK，若初始化成功，则内部会启动一个线程去执行相关逻辑，该线程会持续运行，直到收到 exit 调用
	int ret = tx_init_device(&info, &notify, &init_path);
	if (err_null == ret) {
		printf(" >>> tx_init_device success\n");
	}
	else {
		printf(" >>> tx_init_device failed [%d]\n", ret);
		return false;
	}

	return true;
}


void QQlink::exitDevice(void)
{
	tx_exit_device();
}





// 处理设备关心的控制端指令
static void on_receive_datapoint(unsigned long long from_id, tx_data_point * data_points, int data_points_count)
{

	/*步骤1：先打印出来看看*/
	int i = 0;
	printf("=================================\n");
	printf("on_receive_data_point\n");
	printf("=================================\n");
	
	while (i < data_points_count) 
	{
    	printf("data_points[i].id = %u \n",data_points[i].id);
#if 0
		if (data_points[i].id== LED1_ID) {//100002892
			on_receive_led1_status(from_id, data_points[i]);
		}
		
		if (data_points[i].id== LED2_ID) {//100003251
			on_receive_led2_status(from_id, data_points[i]);
		}
		
		if (data_points[i].id== BRIGHTNESS_ID) {//100002895
			on_receive_led_light(from_id, data_points[i]);
		}
		
		
		if (data_points[i].id== TEMPER_SWITCH_ID) {//100003271
			on_receive_temper_switch(from_id, data_points[i]);
		}		
#endif		
		++i;
	}
	
}


QQlink *QQlink::qqlink = NULL;
pthread_mutex_t QQlink::tMutex  = PTHREAD_MUTEX_INITIALIZER;


QQlink::QQlink()
{
	mConnect = false;
}


QQlink* QQlink::getQQlink(void)
{
	if (NULL == qqlink) {		
		pthread_mutex_lock(&tMutex);
		if (NULL == qqlink) {
			qqlink = new QQlink();
		}
		pthread_mutex_unlock(&tMutex);
	}

	return qqlink;
}



void QQlink::initDataPoint(void)
{
	//挂接datapoint的处理函数
	tx_data_point_notify dataPointNotify = {0};
	dataPointNotify.on_receive_data_point = on_receive_datapoint;

	// 初始化datapoint
	tx_init_data_point(&dataPointNotify);

}




static void cb_on_transfer_progress(unsigned long long transfer_cookie, unsigned long long transfer_progress, unsigned long long max_transfer_progress)
{
    printf("========> on file progress %f%%\n", transfer_progress * 100.0 / max_transfer_progress);
}

// 收到C2C transfer通知
static void cb_on_recv_file(unsigned long long transfer_cookie, const tx_ccmsg_inst_info * inst_info, const tx_file_transfer_info * tran_info)
{
}

/**
 * 文件传输完成后的结果通知，成功接收到文件后，厂商调用自己的接口播放语音文件
 */
static void cb_on_transfer_complete(unsigned long long transfer_cookie, int err_code, tx_file_transfer_info* tran_info)
{
    printf("================ontransfer complete=====transfer_cookie == %lld ====================\n", transfer_cookie);
    // 这个 transfer_cookie 是SDK内部执行文件传输（接收或发送） 任务 保存的一个标记，在回调完这个函数后，transfer_cookie 将失效
    // 可以根据 transfer_cookie 查询文件的信息

    //在完成回调里直接传回 tran_info
    // tx_file_transfer_info ftInfo = {0};
    // tx_query_transfer_info(transfer_cookie, &ftInfo);

    printf("errcode %d, bussiness_name [%s], file path [%s]\n", err_code, tran_info->bussiness_name,  tran_info->file_path);
    printf("===============================================================================\n");
    if(err_code == 0) {
        //如果时语音文件，则播放，否则不处理
        if(!strcmp(tran_info->bussiness_name, BUSINESS_NAME_AUDIO_MSG) == 0) {
            //do_play(tran_info->file_path);
        }
        else
        {
        }
    }
    
}


void QQlink::initFileTransfer(void)
{
    //收到文件（一般是手q向设备发送的语音文件）的事件通知
    tx_file_transfer_notify fileTransferNotify = {0};
    fileTransferNotify.on_transfer_complete = cb_on_transfer_complete;
    fileTransferNotify.on_transfer_progress = cb_on_transfer_progress;
    fileTransferNotify.on_file_in_come 		= cb_on_recv_file;
    tx_init_file_transfer(fileTransferNotify, (char *)"/tmp");
	
}



//callback: 用于确认消息是否送达了QQ物联后台服务器
static void  tx_text_msg_callback  (unsigned int cookie, int err_code)
{
	printf("tx_send_text_msg, cookie[%u] ret[%d]\n", cookie, err_code);
}


void QQlink::sendTextMsg(char *pcText)
{
	tx_send_text_msg(1, pcText, tx_text_msg_callback, NULL, NULL, 0);
	tx_send_text_msg(1, pcText, tx_text_msg_callback, NULL, NULL, 0);
}




static void on_send_alarm_file_progress(const unsigned int cookie, unsigned long long transfer_progress, unsigned long long max_transfer_progress)
{
    printf("on_send_alarm_file_progress, cookie[%u]\n", cookie);
    printf("on_send_alarm_file_progress, %lld/%lld\n", max_transfer_progress, transfer_progress);

}

static void on_send_alarm_msg_ret(const unsigned int cookie, int err_code)
{
    printf("on_send_alarm_msg_ret, cookie[%u] ret[%d]\n", cookie, err_code);
}


void QQlink::sendPicMsg(structuring_msg& msg)
{
    msg.msg_id = 2;   					//假设您已经在物联官网上配置触发器值为2时发送图文报警
	
    tx_send_msg_notify notify = {0};
    notify.on_file_transfer_progress = on_send_alarm_file_progress;
    notify.on_send_structuring_msg_ret = on_send_alarm_msg_ret;
    tx_send_structuring_msg(&msg, &notify, 0);	
}


void QQlink::sendAudioMsg(structuring_msg& msg)
{
    msg.msg_id = 3;   //假设您已经在物联官网上配置触发器值为3时发送语音报警

    tx_send_msg_notify notify = {0};
    notify.on_file_transfer_progress = on_send_alarm_file_progress;
    notify.on_send_structuring_msg_ret = on_send_alarm_msg_ret;
    tx_send_structuring_msg(&msg, &notify, 0);	
}


void QQlink::sendVideoMsg(structuring_msg& msg)
{
#if 0
    // 发送视频的结构化消息
    structuring_msg msg = {0};
    msg.file_path = "/res/test.264";
#endif
    msg.msg_id = 4;   //假设您已经在物联官网上配置触发器值为4时发送视频报警

    msg.title = (char *)"视频警告";
    msg.digest = (char *)"收到视频报警";
    msg.guide_words = (char *)"点击查看";

    tx_send_msg_notify notify = {0};
    notify.on_file_transfer_progress = on_send_alarm_file_progress;
    notify.on_send_structuring_msg_ret = on_send_alarm_msg_ret;
    tx_send_structuring_msg(&msg, &notify, 0);	
}


bool QQlink::isConnect(void)
{
	return mConnect;
}

void QQlink::setConnect(bool isConnect)
{
	mConnect = isConnect;
}


