#ifndef __IPCAMERA_H__
#define __IPCAMERA_H__

#include "TXSDKCommonDef.h"
#include "TXDeviceSDK.h"

CXX_EXTERN_BEGIN


/////////////////////////////////////////////////////////////////
//
//         摄像头 清晰度调整 & 云台控制 
//
/////////////////////////////////////////////////////////////////


// 视频清晰度 定义
enum definition {
	def_low		= 1,	//低
	def_middle	= 2,	//中
	def_high	= 3,	//高
};


// 云台(PTZ)旋转方向
enum rotate_direction {
	rotate_direction_left	= 1,	//左
	rotate_direction_right	= 2,	//右
	rotate_direction_up		= 3,	//上
	rotate_direction_down	= 4,	//下
};

// 云台(PTZ)旋转角度范围
enum rotate_degree {
	//水平角度范围
	rotate_degree_h_min = 0,
	rotate_degree_h_max = 360,

	//垂直角度范围
	rotate_degree_v_min = 0,
	rotate_degree_v_max = 180,
};

// 回调处理函数，需要产商实现。
typedef struct _tx_ipcamera_notify {
	/**
	 * 接口说明:	ipcamera清晰度调整回调，调用后返回摄像头的当前分辨率
	 * param : definition :  要调整的清晰度，具体取值参见 enum definition
	 * param : cur_definition：暂时保留，没有用到
	 * param : cur_definition_length : 暂时保留，没有用到。
	*/
	int (*on_set_definition)(int definition, char *cur_definition, int cur_definition_length);

	/**
	 * 接口说明：ipcamera云台控制回调
	 * param ：rotate_direction : 云台调整方向，具体取值可以参见 enum rotate_direction
	 * param ：rotate_degree ： 云台调整角度，具体取值可以参见 enum rotate_degree
	 * return : 0代表调整失败， 非0代表调整成功。
	*/
	int (*on_control_rotate)(int rotate_direction, int rotate_degree);

	/**
	 * 接口说明：ipcamera变倍控制回调
	 * param zoom			 :  放大或者缩小, 负值表缩小，负值代表放大。
	 * return : 0代表调整失败， 非0代表调整成功。
	*/
	int (*on_control_zoom)(int zoom);
} tx_ipcamera_notify;


/**
 * 接口说明： 设置ipcamera相关的回调
 * param: notify IPCamera交互回调对象。
 */
SDK_API int tx_ipcamera_set_callback(tx_ipcamera_notify *notify);



/////////////////////////////////////////////////////////////////
//
//         ipcamera互联相关接口: 历史视频回看
//
/////////////////////////////////////////////////////////////////

/**
  *  QQ物联历史视频回看方案：
  *
  *      |-----------|                                        |