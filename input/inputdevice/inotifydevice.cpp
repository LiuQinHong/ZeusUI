#include <inputdevice/inotifydevice.h>
#include <inputdevice/eventdevice.h>
#include <input/inputreader.h>
#include <sys/inotify.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

//std::list<DirectoryPathDesc*> InotifyDevice::directoryPathDescList;
InotifyDevice *InotifyDevice::inotifyDevice = NULL;
pthread_mutex_t InotifyDevice::tMutex  = PTHREAD_MUTEX_INITIALIZER;

InotifyDevice::InotifyDevice()
{
	int iFd = inotify_init();
	setFd(iFd);
	setDeviceName("InotifyDevice");
	mINotifyFd = iFd;
}


InotifyDevice::~InotifyDevice()
{
	for(std::list<DirectoryPathDesc*>::iterator list_iter = directoryPathDescList.begin(); 
		list_iter != directoryPathDescList.end(); list_iter++) {
		directoryPathDescList.remove((*list_iter));
		delete *list_iter;
	}
}


int InotifyDevice::addWatchDirectory(std::string directoryPath)
{
	/* add watch */
	int iWd;
	iWd = inotify_add_watch(mINotifyFd, directoryPath.c_str(), IN_DELETE | IN_CREATE);
	if (iWd < 0) {
		fprintf(stderr, "inotify_add_watch for %s error !\n", directoryPath.c_str());
		return -1;
	}
	
	DirectoryPathDesc *directoryPathDesc = new DirectoryPathDesc();
	directoryPathDesc->iWd = iWd;
	directoryPathDesc->directoryPath = directoryPath;
	directoryPathDescList.push_back(directoryPathDesc);

	return 0;
}

int InotifyDevice::delWatchDirectory(std::string directoryPath)
{
	DirectoryPathDesc* tmp = NULL;

	std::list<DirectoryPathDesc*>::iterator list_iter;
	for(list_iter = directoryPathDescList.begin(); 
		list_iter != directoryPathDescList.end(); list_iter++) {
		if ((*list_iter)->directoryPath.compare(directoryPath) == 0) {
			tmp = (*list_iter);
			directoryPathDescList.remove((*list_iter));
			break;
		}
	}

	if (tmp == NULL) {
		return 0;
	}
	inotify_rm_watch(mINotifyFd, tmp->iWd);
	delete tmp;

	return 0;
}


int InotifyDevice::addWatchDirectory_R(std::string directoryPath)
{
	/* add watch */
	int iWd;
	iWd = inotify_add_watch(mINotifyFd, directoryPath.c_str(), IN_DELETE | IN_CREATE);
	if (iWd < 0) {
		fprintf(stderr, "inotify_add_watch for %s error !\n", directoryPath.c_str());
		return -1;
	}
	
	DirectoryPathDesc *directoryPathDesc = new DirectoryPathDesc();
	directoryPathDesc->iWd = iWd;
	directoryPathDesc->directoryPath = directoryPath;
	directoryPathDescList.push_back(directoryPathDesc);

	DIR *pDir;
	struct dirent *file;

	if(!(pDir = opendir(directoryPath.c_str()))) {
		printf("error opendir %s!!!/n", directoryPath.c_str());
		return -1;
	}

	while((file = readdir(pDir)) != NULL) {
		if(strncmp(file->d_name, ".", 1) == 0)
			continue;

		char strTmp[256];			
		snprintf(strTmp, 256, "%s/%s", directoryPath.c_str(), file->d_name);
		strTmp[255] = '\0';
		if (isDir(strTmp)) {
			addWatchDirectory_R(strTmp);
			continue;
			
		}
		
	}
	
	closedir(pDir);

	return 0;
}

int InotifyDevice::delWatchDirectory_R(std::string directoryPath)
{
	DirectoryPathDesc* tmp = NULL;

	std::list<DirectoryPathDesc*>::iterator list_iter;
	for(list_iter = directoryPathDescList.begin(); 
		list_iter != directoryPathDescList.end(); list_iter++) {
		if ((*list_iter)->directoryPath.compare(directoryPath) == 0) {
			tmp = (*list_iter);
			directoryPathDescList.remove((*list_iter));
			break;
		}
	}

		
	if (tmp == NULL) {
		return 0;
	}
	inotify_rm_watch(mINotifyFd, tmp->iWd);
	delete tmp;


	DIR *pDir;
	struct dirent *file;

	if(!(pDir = opendir(directoryPath.c_str()))) {
		printf("error opendir %s!!!/n", directoryPath.c_str());
		return -1;
	}

	while((file = readdir(pDir)) != NULL) {
		if(strncmp(file->d_name, ".", 1) == 0)
			continue;
		char strTmp[256];			
		snprintf(strTmp, 256, "%s/%s", directoryPath.c_str(), file->d_name);
		strTmp[255] = '\0';
		if (isDir(strTmp)) {
			delWatchDirectory_R(strTmp);
			continue;
			
		}
		
	}
	
	closedir(pDir);	

	return 0;
}


void InotifyDevice::printAllWatchDirectory(void)
{
	printf("Is to monitor the following directories:\n");
	for(std::list<DirectoryPathDesc*>::iterator list_iter = directoryPathDescList.begin(); 
		list_iter != directoryPathDescList.end(); list_iter++) {
		printf("%s\n", (*list_iter)->directoryPath.c_str());
	}
}


int InotifyDevice::processCreate(int iWd, char *fileName, struct CookEvent* cookEvent)
{
	std::list<DirectoryPathDesc*>::iterator list_iter;
	char strTmp[256];
	int iRet = 0;
	struct CookEvent* cev = cookEvent;

	for(list_iter = directoryPathDescList.begin(); 
		list_iter != directoryPathDescList.end(); list_iter++) {
		if ((*list_iter)->iWd == iWd) 
			break;
	}

	memset(strTmp, 0, 256);
	snprintf(strTmp, 256, "%s/%s", (char *)(*list_iter)->directoryPath.c_str(), fileName);

	if (isDir(strTmp)) {
		cev->iType = ADDDIR;
		gettimeofday(&cev->tTime, NULL);
		cev->filePath = strTmp;
		iRet++;
		
		addWatchDirectory_R(strTmp);
	}
	else if (isRegFile(strTmp)) {
		/* 常规文件 */
		cev->iType = ADDFILE;
		gettimeofday(&cev->tTime, NULL);
		cev->filePath = strTmp;
		iRet++;
	
	}
	else if (isCharDevice(strTmp)) {
		cev->iType = ADDDEVICE;
		gettimeofday(&cev->tTime, NULL);
		cev->filePath = strTmp;
		iRet++;
		
		if (strncmp(fileName, "event", 5) == 0) {
			/* 如果是输入设备的话就加入device链表，并监听它 */
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
	else if (isBlkDevice(strTmp)) {
		/* 块设备 */
		cev->iType = ADDDEVICE;
		gettimeofday(&cev->tTime, NULL);
		cev->filePath = strTmp;
		iRet++;
	
	}

	return iRet;
}

int InotifyDevice::processRemove(int iWd, char *fileName, struct CookEvent* cookEvent)
{
	char strTmp[256];
	
	std::list<DirectoryPathDesc*>::iterator directoryPathDesc_list_iter;

	for(directoryPathDesc_list_iter = directoryPathDescList.begin(); 
		directoryPathDesc_list_iter != directoryPathDescList.end(); directoryPathDesc_list_iter++) {
		if ((*directoryPathDesc_list_iter)->iWd == iWd) 
			break;
	}


	memset(strTmp, 0, 256);
	snprintf(strTmp, 256, "%s/%s", (*directoryPathDesc_list_iter)->directoryPath.c_str(), fileName);

	/* 先处理字符设备和块设备 */
	if (0 == InputReader::getInputReader()->delDevice(strTmp)) {
		//printf("del device : %s\n", strTmp);
		cookEvent->filePath = strTmp;
		cookEvent->iType = DELDEVICE;
		gettimeofday(&cookEvent->tTime, NULL);
		return 1; //表示有一个处理事件		
	}

	/* 一般处理 */

	if (!strncmp(strTmp, "/dev/", 5)) {
		cookEvent->iType = DELDEVICE;
	}
	else {
		cookEvent->iType = DELFILE | DELDIR; //两种都有可能，交给上层分辨		
	}

	cookEvent->filePath = strTmp;
	gettimeofday(&cookEvent->tTime, NULL);
	return 1; //表示有一个处理事件

}

InotifyDevice* InotifyDevice::getInotifyDevice(void)
{
	if (NULL == inotifyDevice) {		
		pthread_mutex_lock(&tMutex);
		if (NULL == inotifyDevice) {
			inotifyDevice = new InotifyDevice();
		}
		pthread_mutex_unlock(&tMutex);
	}

	return inotifyDevice;
}


int InotifyDevice::process(int iFd, struct CookEvent* cookEvent, int iSize)
{
	int res;
	int iRet = 0;
	char event_buf[512];
	int event_size;
	int event_pos = 0;
	struct inotify_event *event;	
	struct CookEvent* cev = cookEvent;

	res = read(iFd, event_buf, sizeof(event_buf));
	if(res < (int)sizeof(*event)) {
		if(errno == EINTR)
			return 0;
		printf("could not get event, %s\n", strerror(errno));
		return -1;
	}

	while(res >= (int)sizeof(*event)) {
		event = (struct inotify_event *)(event_buf + event_pos);
		//printf("%d: %08x \"%s\"\n", event->wd, event->mask, event->len ? event->name : "");
		if(event->len) {
			if(event->mask & IN_CREATE) {
				iRet += processCreate(event->wd, event->name, cev);
				cev = cookEvent + iRet;
				//printAllDevice();
			} else {
				iRet += processRemove(event->wd, event->name, cev);
				cev = cookEvent + iRet;
				//printAllDevice();
			}
		}
		event_size = sizeof(*event) + event->len;
		res -= event_size;
		event_pos += event_size;
	}
	return iRet;
}


