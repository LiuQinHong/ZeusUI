#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "K6502.h"
#include <stdlib.h>
#include <errno.h>
#include <linux/input.h>
#include <dirent.h>
#include <string.h>
#include <sys/epoll.h>




//#define USB_JS_DEV "/dev/usb_as_joypad"
#define USB_JS_DEV "/dev/event1"

static int g_iEpollFd;


static int isCharDevice(char *Path)
{
    struct stat tStat;


    if ((stat(Path, &tStat) == 0) && S_ISCHR(tStat.st_mode)) {
        return 1;
    }
    else {
        return 0;
    }
}

static int addEpollFd(int iFd)
{
	int result;
    struct epoll_event eventItem;
    memset(&eventItem, 0, sizeof(eventItem));
    eventItem.events = EPOLLIN;
    eventItem.data.fd = iFd;
    result = epoll_ctl(g_iEpollFd, EPOLL_CTL_ADD, iFd, &eventItem);
	return result;

}

static int delEpollFd(int iFd)
{
	return epoll_ctl(g_iEpollFd, EPOLL_CTL_DEL, iFd, NULL);
}




int ScanDevice(const char *Path)
{
	DIR *pDir;
	struct dirent *file;

	pDir = opendir(Path);
	if(!pDir) {
		printf("error opendir %s!!!/n", Path);
		return -1;
	}

	while((file = readdir(pDir)) != NULL) {
		if(strncmp(file->d_name, ".", 1) == 0)
			continue;

		char strTmp[256];			
		snprintf(strTmp, 256, "%s/%s", Path, file->d_name);
		strTmp[255] = '\0';
		if (isCharDevice(strTmp)) {
			if (strncmp(file->d_name, "event", 5) == 0) {
				int iFd = open(strTmp, O_RDWR);
				if (iFd < 0) {
					fprintf(stderr, "open device %s error\n", strTmp);
					continue;
				}
				addEpollFd(iFd);			
			}
		}

	}
	closedir(pDir);

	return 0;

}



static int USBjoypadDevInit(void)
{

	g_iEpollFd = epoll_create(16);
	ScanDevice("/dev");	
	return 0;
}

static int USBjoypadDevExit(void)
{
	close(g_iEpollFd);
	return 0;
}

static int joypadGet(int iFd, DWORD *pdwKeyPad1, DWORD *pdwKeyPad2)
{
	/**
	 * FC手柄 bit 键位对应关系 真实手柄中有一个定时器，处理 连A  连B 
	 * 0  1   2       3       4    5      6     7
	 * A  B   Select  Start  Up   Down   Left  Right
	 */
	//因为 USB 手柄每次只能读到一位键值 所以要有静态变量保存上一次的值
	static DWORD joypad1 = 0;
	static DWORD joypad2 = 0;
	struct input_event evKey;	
	int count;
	int i;

	count = read(iFd, &evKey, sizeof(struct input_event));
	for(i = 0; i < (int)count / sizeof(struct input_event); i++) {
		if(EV_KEY == evKey.type) {
			
			switch (evKey.code) {
				case KEY_ESC :	//退出游戏
				{
					if (evKey.value != 0) {
						/* 按下 */
					}
					else {
						/* 松开   */
						USBjoypadDevExit();
						exit(0);						
					}
					break;
				}

				case KEY_W :	//上
				{
					if (evKey.value != 0) {
						/* 按下 */
						joypad1 |= 1<<4;
					}
					else {
						/* 松开   */
						joypad1 &= ~(1<<4);
					}
					break;
				}
				
				case KEY_S :	//下
				{
					if (evKey.value != 0) {
						/* 按下 */
						joypad1 |= 1<<5;
					}
					else {
						/* 松开	*/
						joypad1 &= ~(1<<5);
					}
					break;
				}
				
				case KEY_A :	//左
				{
					if (evKey.value != 0) {
						/* 按下 */
						joypad1 |= 1<<6;
					}
					else {
						/* 松开	*/
						joypad1 &= ~(1<<6);
					}
					break;
				}

				case KEY_D :	//右
				{
					if (evKey.value != 0) {
						/* 按下 */
						joypad1 |= 1<<7;
					}
					else {
						/* 松开	*/
						joypad1 &= ~(1<<7);
					}
					break;
				}

				case KEY_I :	//选择
				{
					if (evKey.value != 0) {
						/* 按下 */
						joypad1 |= 1<<2;
					}
					else {
						/* 松开	*/
						joypad1 &= ~(1<<2);
					}
					break;
				}

				
				case KEY_O :	//确定/暂停/开始
				{
					if (evKey.value != 0) {
						/* 按下 */
						joypad1 |= 1<<3;
					}
					else {
						/* 松开	*/
						joypad1 &= ~(1<<3);
					}
					break;
				}

				
				case KEY_K :	//连A，跳跃
				{
					if (evKey.value != 0) {
						/* 按下 */
						joypad1 |= 1<<0;
					}
					else {
						/* 松开	*/
						joypad1 &= ~(1<<0);
					}
					break;
				}

				
				case KEY_J :	//连B，攻击
				{
					if (evKey.value != 0) {
						/* 按下 */
						joypad1 |= 1<<1;
					}
					else {
						/* 松开	*/
						joypad1 &= ~(1<<1);
					}
					break;
				}

				/* 玩家2 */
				case KEY_UP :	//上
				{
					if (evKey.value != 0) {
						/* 按下 */
						joypad2 |= 1<<4;
					}
					else {
						/* 松开   */
						joypad2 &= ~(1<<4);
					}
					break;
				}
				
				case KEY_DOWN :	//下
				{
					if (evKey.value != 0) {
						/* 按下 */
						joypad2 |= 1<<5;
					}
					else {
						/* 松开	*/
						joypad2 &= ~(1<<5);
					}
					break;
				}
				
				case KEY_LEFT :	//左
				{
					if (evKey.value != 0) {
						/* 按下 */
						joypad2 |= 1<<6;
					}
					else {
						/* 松开	*/
						joypad2 &= ~(1<<6);
					}
					break;
				}

				case KEY_RIGHT :	//右
				{
					if (evKey.value != 0) {
						/* 按下 */
						joypad2 |= 1<<7;
					}
					else {
						/* 松开	*/
						joypad2 &= ~(1<<7);
					}
					break;
				}

				case KEY_KP4 :	//选择
				{
					if (evKey.value != 0) {
						/* 按下 */
						joypad2 |= 1<<2;
					}
					else {
						/* 松开	*/
						joypad2 &= ~(1<<2);
					}
					break;
				}

				
				case KEY_KP5 :	//确定/暂停/开始
				{
					if (evKey.value != 0) {
						/* 按下 */
						joypad2 |= 1<<3;
					}
					else {
						/* 松开	*/
						joypad2 &= ~(1<<3);
					}
					break;
				}

				
				case KEY_KP2 :	//连A，跳跃
				{
					if (evKey.value != 0) {
						/* 按下 */
						joypad2 |= 1<<0;
					}
					else {
						/* 松开	*/
						joypad2 &= ~(1<<0);
					}
					break;
				}

				
				case KEY_KP1 :	//连B，攻击
				{
					if (evKey.value != 0) {
						/* 按下 */
						joypad2 |= 1<<1;
					}
					else {
						/* 松开	*/
						joypad2 &= ~(1<<1);
					}
					break;
				}

			}
		}

	}

	*pdwKeyPad1 = joypad1;
	*pdwKeyPad2 = joypad2;
	return 0;
}

static int USBjoypadGet(DWORD *pdwKeyPad1, DWORD *pdwKeyPad2)
{
	int i;
	
	static const int EPOLL_MAX_EVENTS = 16;
	struct epoll_event mPendingEventItems[EPOLL_MAX_EVENTS];

	int pollResult = epoll_wait(g_iEpollFd, mPendingEventItems, EPOLL_MAX_EVENTS, -1);
	for (i = 0; i < pollResult; i++) {
		if (mPendingEventItems[i].events & EPOLLERR || mPendingEventItems[i].events & EPOLLHUP\
			|| mPendingEventItems[i].events & EPOLLRDHUP) {
			delEpollFd(mPendingEventItems[i].data.fd);
			close(mPendingEventItems[i].data.fd);
		}
		else if (mPendingEventItems[i].events == EPOLLIN) {
			joypadGet(mPendingEventItems[i].data.fd, pdwKeyPad1, pdwKeyPad2);
		}
	}

	return 0;
}



void InitJoypadInput(void)
{
	USBjoypadDevInit();
}

int GetJoypadInput(DWORD *pdwKeyPad1, DWORD *pdwKeyPad2)
{

	USBjoypadGet(pdwKeyPad1, pdwKeyPad2);

	return 0;
}




