#include <input/inputreader.h>
#include <inputdevice/inotifydevice.h>
#include <inputdevice/eventdevice.h>
#include <sys/epoll.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <errno.h>
#include <sys/time.h>


//std::list<Device*> InputReader::deviceList;

InputReader *InputReader::inputReader = NULL;
pthread_mutex_t InputReader::tMutex  = PTHREAD_MUTEX_INITIALIZER;

InputReader* InputReader::getInputReader(void)
{
	if (NULL == inputReader) {		
		pthread_mutex_lock(&tMutex);
		if (NULL == inputReader) {
			inputReader = new InputReader();
		}
		pthread_mutex_unlock(&tMutex);
	}

	return inputReader;
}


InputReader::InputReader()
{
	/* epoll_create */
    mEpollFd = epoll_create(16);
}

InputReader::~InputReader()
{
	close(mEpollFd);
}

void InputReader::addDevice(Device* device)
{
	deviceList.push_back(device);
	addEpollFd(device->getFd());
}

int InputReader::delDevice(char *pcDevicePath)
{
	std::list<Device*>::iterator deviceList_iter;
	Device *deviceTmp = NULL; 		
	for(deviceList_iter = deviceList.begin(); deviceList_iter != deviceList.end(); deviceList_iter++) {
		if ((*deviceList_iter)->getDeviceName().compare(pcDevicePath) == 0) {
			deviceTmp = (*deviceList_iter);
			deviceList.remove((*deviceList_iter));		
			delEpollFd(deviceTmp->getFd());
			close(deviceTmp->getFd());
			delete deviceTmp;
			return 0;
		}
	}
	
	return 1; //没有这个设备在链表中
}

Device* InputReader::findDevice(char *pcDevicePath)
{
	for(std::list<Device*>::iterator deviceList_iter = deviceList.begin(); deviceList_iter != deviceList.end(); deviceList_iter++) {
		if ((*deviceList_iter)->getDeviceName().compare(pcDevicePath) == 0) {
			return (*deviceList_iter);
		}
	}

	return NULL;
}
Device* InputReader::findDevice(int iFd)
{
	for(std::list<Device*>::iterator deviceList_iter = deviceList.begin(); deviceList_iter != deviceList.end(); deviceList_iter++) {
		if ((*deviceList_iter)->getFd() == iFd) {
			return (*deviceList_iter);
		}
	}
	return NULL;	
}


int InputReader::addEpollFd(int iFd)
{
	int result;
    struct epoll_event eventItem;
    memset(&eventItem, 0, sizeof(eventItem));
    eventItem.events = EPOLLIN;
    eventItem.data.fd = iFd;
    result = epoll_ctl(mEpollFd, EPOLL_CTL_ADD, iFd, &eventItem);
	return result;

}

int InputReader::delEpollFd(int iFd)
{
	return epoll_ctl(mEpollFd, EPOLL_CTL_DEL, iFd, NULL);
}


void InputReader::printAllDevice(void)
{
	printf("printAllDevice start\n");
	std::list<Device*>::iterator deviceList_iter;
	for(deviceList_iter = deviceList.begin(); deviceList_iter != deviceList.end(); deviceList_iter++) {
			std::cout<<"have device : "<<(*deviceList_iter)->getDeviceName()<<std::endl;
	}
	printf("printAllDevice end\n");	
}

int InputReader::readOnce(struct CookEvent* cookEvent, int iSize)
{
	int i;
	// Maximum number of signalled FDs to handle at a time.
	 static const int EPOLL_MAX_EVENTS = 16;
	
	 // The array of pending epoll events and the index of the next event to be handled.
	 struct epoll_event mPendingEventItems[EPOLL_MAX_EVENTS];
	int pollResult = epoll_wait(mEpollFd, mPendingEventItems, EPOLL_MAX_EVENTS, -1);
	for (i = 0; i < pollResult; i++) {
		
		if (mPendingEventItems[i].events & EPOLLERR || mPendingEventItems[i].events & EPOLLHUP\
			|| mPendingEventItems[i].events & EPOLLRDHUP) {
			printf("EPOLLERR | EPOLLHUP | EPOLLRDHUP 0x%x\n", mPendingEventItems[i].events);
			delEpollFd(mPendingEventItems[i].data.fd);
			close(mPendingEventItems[i].data.fd);
		}
		else if (mPendingEventItems[i].events == EPOLLIN) {
			/* 从Device链表中找到对应项调用process函数 */
			Device* device = findDevice(mPendingEventItems[i].data.fd);
			if (device == NULL) {
				fprintf(stderr, "no this device in list\n");
				continue;
			}
			return device->process(mPendingEventItems[i].data.fd, cookEvent, iSize);
			
		}
	}
	

	return 0;
}

