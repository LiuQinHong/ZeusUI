#include <window/camerawindow.h>
#include <device/framebuffer.h>
#include <input/inputreader.h>
#include <windowmanager.h>
#include <view/closeview.h>
#include <pic/picparser.h>
#include <pic/jpgparser.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <poll.h>
#include <pic/jpgparser.h>


static void *CameraThreadFunction(void *pVoid);
static int g_aiSupportedFormats[] = {V4L2_PIX_FMT_YUYV, V4L2_PIX_FMT_MJPEG, V4L2_PIX_FMT_JPEG, V4L2_PIX_FMT_RGB565};

CameraWindow::CameraWindow(){}

CameraWindow::~CameraWindow()
{
	stopCameraDevice();
	exitCameraDevice();
}

CameraWindow::CameraWindow(const std::string& strDeviceName)
:Window(), mStrDeviceName(strDeviceName)
{
	setRemovable(true);
	struct WindowInfo& windowInfo = getWindowInfo();

	initCameraDevice();

	windowInfo.iX = (FrameBuffer::getFrameBuffer()->getScreenInfo().iXres - windowInfo.iWidth) / 2;
	windowInfo.iY = (FrameBuffer::getFrameBuffer()->getScreenInfo().iYres - windowInfo.iHeight) / 2;

	windowInfo.iCareEventType = TOUCHSCREEN;

	/* 画出这个窗口 */
	drawWindowPixelData();


	View* view;
	ViewInfo viewInfo;

	/* close view */
	viewInfo.iX = windowInfo.iWidth - windowInfo.iHeight / 8;
	viewInfo.iY = 0;
	viewInfo.iWidth = windowInfo.iHeight / 8;
	viewInfo.iHeight = viewInfo.iWidth;
	viewInfo.iCareEventType = TOUCHSCREEN;
	view = new CloseView(CLOSE_VIEW_ICON, viewInfo);
	view->setParentWindow(this);
	view->showViewToWindow();
	addView(view);
	
}


static void *CameraThreadFunction(void *pVoid)
{
	int iTopLeftX;
	int iTopLeftY;
	int iRet;
    int iLcdBpp = FrameBuffer::getFrameBuffer()->getScreenInfo().iBpp;

	float k;

	struct VideoBuf *ptVideoBufCur;
	struct CameraThread *ptCameraThread = (struct CameraThread *)pVoid;
	CameraWindow *window = (CameraWindow *)ptCameraThread->window;
	Convert *convert = ptCameraThread->convert;
	int iPixelFormatOfDisp = ptCameraThread->iPixelFormatOfDisp;
	int iPixelFormatOfVideo = ptCameraThread->iPixelFormatOfVideo;

	struct VideoBuf tVideoBuf;
	struct VideoBuf tZoomBuf;
	JpgParser jpgparser;


	memset(&tVideoBuf, 0, sizeof(tVideoBuf));
	memset(&window->getConvertBuf(), 0, sizeof(window->getConvertBuf()));
	window->getConvertBuf().iPixelFormat	 = iPixelFormatOfDisp;
	window->getConvertBuf().tPixelDatas.iBpp = iLcdBpp;
	
	memset(&tZoomBuf, 0, sizeof(tZoomBuf));



	for (;;) {
		pthread_testcancel();

		/* 读入摄像头数据 */
		iRet = window->getFrame(tVideoBuf);
		if (iRet) {
			printf("GetFrame for /dev/video0 error!\n");
			return NULL;
		}
		ptVideoBufCur = &tVideoBuf;

		if (iPixelFormatOfVideo != iPixelFormatOfDisp) {
			/* 转换为RGB */
			iRet = convert->convert(tVideoBuf, window->getConvertBuf());
			if (iRet) {
				printf("Convert for /dev/video0 error!\n");
				return NULL;
			}			 
			ptVideoBufCur = &window->getConvertBuf();
		}

		/* 如果图像分辨率大于LCD, 缩放 */
		if (ptVideoBufCur->tPixelDatas.iWidth > window->getWindowInfo().iWidth || \
			ptVideoBufCur->tPixelDatas.iHeight > window->getWindowInfo().iHeight) {
			/* 确定缩放后的分辨率 */
			/* 把图片按比例缩放到VideoMem上, 居中显示
			 * 1. 先算出缩放后的大小
			 */
			k = (float)ptVideoBufCur->tPixelDatas.iHeight / ptVideoBufCur->tPixelDatas.iWidth;
			tZoomBuf.tPixelDatas.iWidth  = window->getWindowInfo().iWidth;
			tZoomBuf.tPixelDatas.iHeight = window->getWindowInfo().iWidth * k;
			if ( tZoomBuf.tPixelDatas.iHeight > window->getWindowInfo().iHeight) {
				tZoomBuf.tPixelDatas.iWidth  = window->getWindowInfo().iHeight / k;
				tZoomBuf.tPixelDatas.iHeight = window->getWindowInfo().iHeight;
			}
			tZoomBuf.tPixelDatas.iBpp		 = iLcdBpp;
			tZoomBuf.tPixelDatas.iLineBytes  = tZoomBuf.tPixelDatas.iWidth * tZoomBuf.tPixelDatas.iBpp / 8;
			tZoomBuf.tPixelDatas.iTotalBytes = tZoomBuf.tPixelDatas.iLineBytes * tZoomBuf.tPixelDatas.iHeight;

			if (!tZoomBuf.tPixelDatas.pcPixelDatas) {
				tZoomBuf.tPixelDatas.pcPixelDatas = new char[tZoomBuf.tPixelDatas.iTotalBytes];
			}
			jpgparser.zoomPic(tZoomBuf.tPixelDatas, ptVideoBufCur->tPixelDatas);
			ptVideoBufCur = &tZoomBuf;
		}

		iTopLeftX = (window->getWindowInfo().iWidth - ptVideoBufCur->tPixelDatas.iWidth) / 2;
		iTopLeftY = window->getWindowInfo().iHeight / 8;

		FrameBuffer::getFrameBuffer()->FbVideoMemFusion(iTopLeftX, iTopLeftY, \
			window->getWindowInfo().tWindowCurPixelDatas, ptVideoBufCur->tPixelDatas);
		
		WindowManager::getWindowManager()->addTask(window, REFRESH_WINDOW);

			
		iRet = window->putFrame(tVideoBuf);
		if (iRet) {
			printf("PutFrame for /dev/video0 error!\n");
			return NULL;
		}

		if (tZoomBuf.tPixelDatas.pcPixelDatas) {
			delete tZoomBuf.tPixelDatas.pcPixelDatas;
			tZoomBuf.tPixelDatas.pcPixelDatas = NULL;
		}
	}	
}


static int isSupportThisFormat(int iPixelFormat)
{
    unsigned int i;
    for (i = 0; i < sizeof(g_aiSupportedFormats)/sizeof(g_aiSupportedFormats[0]); i++) {
        if (g_aiSupportedFormats[i] == iPixelFormat)
            return 1;
    }
    return 0;
}

int CameraWindow::initCameraDevice(void)
{
	int i;
	int iFd;
	int iError;
	struct v4l2_capability tV4l2Cap;
	struct v4l2_fmtdesc tFmtDesc;
	struct v4l2_format	tV4l2Fmt;
	struct v4l2_requestbuffers tV4l2ReqBuffs;
	struct v4l2_buffer tV4l2Buf;

	int iLcdWidth = FrameBuffer::getFrameBuffer()->getScreenInfo().iXres;
	int iLcdHeigt = FrameBuffer::getFrameBuffer()->getScreenInfo().iYres;

	iFd = open(mStrDeviceName.c_str(), O_RDWR);
	if (iFd < 0) {
		printf("can not open %s\n", mStrDeviceName.c_str());
		return -1;
	}
	mtVideoDevice.iFd = iFd;

	//iError = ioctl(iFd, VIDIOC_QUERYCAP, &tV4l2Cap);
	memset(&tV4l2Cap, 0, sizeof(struct v4l2_capability));
	iError = ioctl(iFd, VIDIOC_QUERYCAP, &tV4l2Cap);
	if (iError) {
		printf("Error opening device %s: unable to query device.\n", mStrDeviceName.c_str());
		goto err_exit;
	}

	if (!(tV4l2Cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		printf("%s is not a video capture device\n", mStrDeviceName.c_str());
		goto err_exit;
	}

	if (tV4l2Cap.capabilities & V4L2_CAP_STREAMING) {
		printf("%s supports streaming i/o\n", mStrDeviceName.c_str());
	}
	
	if (tV4l2Cap.capabilities & V4L2_CAP_READWRITE) {
		printf("%s supports read i/o\n", mStrDeviceName.c_str());
	}

	memset(&tFmtDesc, 0, sizeof(tFmtDesc));
	tFmtDesc.index = 0;
	tFmtDesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	
	while ((iError = ioctl(iFd, VIDIOC_ENUM_FMT, &tFmtDesc)) == 0) {
		if (isSupportThisFormat(tFmtDesc.pixelformat)) {
			mtVideoDevice.iPixelFormat = tFmtDesc.pixelformat;
			break;
		}
		tFmtDesc.index++;
	}

	if (!mtVideoDevice.iPixelFormat) {
		printf("can not support the format of this device\n");
		goto err_exit;		  
	}

	
	/* set format in */
	memset(&tV4l2Fmt, 0, sizeof(struct v4l2_format));
	tV4l2Fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	tV4l2Fmt.fmt.pix.pixelformat = mtVideoDevice.iPixelFormat;
	tV4l2Fmt.fmt.pix.width		 = iLcdWidth;
	tV4l2Fmt.fmt.pix.height 	 = iLcdHeigt;
	tV4l2Fmt.fmt.pix.field		 = V4L2_FIELD_ANY;


	/* 如果驱动程序发现无法支持某些参数(比如分辨率),
	 * 它会调整这些参数, 并且返回给应用程序
	 */
	iError = ioctl(iFd, VIDIOC_S_FMT, &tV4l2Fmt); 
	if (iError) {
		printf("Unable to set format\n");
		goto err_exit;		  
	}
	mtVideoDevice.iWidth  = tV4l2Fmt.fmt.pix.width;
	mtVideoDevice.iHeight = tV4l2Fmt.fmt.pix.height;

	/* 根据返回的长宽计算window的大小 */
	getWindowInfo().iWidth = mtVideoDevice.iWidth;
	getWindowInfo().iHeight = mtVideoDevice.iHeight + mtVideoDevice.iHeight / 8;


	printf("%s %s %d : width = %d height = %d\n", __FILE__, __FUNCTION__, __LINE__, mtVideoDevice.iWidth, mtVideoDevice.iHeight);

	/* request buffers */
	memset(&tV4l2ReqBuffs, 0, sizeof(struct v4l2_requestbuffers));
	tV4l2ReqBuffs.count = NB_BUFFER;
	tV4l2ReqBuffs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	tV4l2ReqBuffs.memory = V4L2_MEMORY_MMAP;

	iError = ioctl(iFd, VIDIOC_REQBUFS, &tV4l2ReqBuffs);
	if (iError) {
		printf("Unable to allocate buffers.\n");
		goto err_exit;		  
	}
	
	mtVideoDevice.iVideoBufCnt = tV4l2ReqBuffs.count;
	if (tV4l2Cap.capabilities & V4L2_CAP_STREAMING) {
		/* map the buffers */
		for (i = 0; i < mtVideoDevice.iVideoBufCnt; i++) {
			memset(&tV4l2Buf, 0, sizeof(struct v4l2_buffer));
			tV4l2Buf.index = i;
			tV4l2Buf.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
			tV4l2Buf.memory = V4L2_MEMORY_MMAP;
			iError = ioctl(iFd, VIDIOC_QUERYBUF, &tV4l2Buf);
			if (iError) {
				printf("Unable to query buffer.\n");
				goto err_exit;
			}

			mtVideoDevice.iVideoBufMaxLen = tV4l2Buf.length;
			mtVideoDevice.pucVideBuf[i] = (char *)mmap(0 /* start anywhere */ ,
					  tV4l2Buf.length, PROT_READ, MAP_SHARED, iFd,
					  tV4l2Buf.m.offset);
			if (mtVideoDevice.pucVideBuf[i] == MAP_FAILED)  {
				printf("Unable to map buffer\n");
				goto err_exit;
			}
		}		 

		/* Queue the buffers. */
		for (i = 0; i < mtVideoDevice.iVideoBufCnt; i++)  {
			memset(&tV4l2Buf, 0, sizeof(struct v4l2_buffer));
			tV4l2Buf.index = i;
			tV4l2Buf.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			tV4l2Buf.memory = V4L2_MEMORY_MMAP;
			iError = ioctl(iFd, VIDIOC_QBUF, &tV4l2Buf);
			if (iError) {
				printf("Unable to queue buffer.\n");
				goto err_exit;
			}
		}
		
	}
	else if (tV4l2Cap.capabilities & V4L2_CAP_READWRITE) {
		
		/* read(fd, buf, size) */
		mtVideoDevice.iVideoBufCnt  = 1;
		/* 在这个程序所能支持的格式里, 一个象素最多只需要4字节 */
		mtVideoDevice.iVideoBufMaxLen = mtVideoDevice.iWidth * mtVideoDevice.iHeight * 4;
		mtVideoDevice.pucVideBuf[0] = new char[mtVideoDevice.iVideoBufMaxLen];
	}

	return 0;
	
err_exit:	 
	close(iFd);
	return -1;	  
}




int CameraWindow::exitCameraDevice(void)
{
	int i;
	struct v4l2_capability tV4l2Cap;

	memset(&tV4l2Cap, 0, sizeof(struct v4l2_capability));
	ioctl(mtVideoDevice.iFd, VIDIOC_QUERYCAP, &tV4l2Cap);

	if (tV4l2Cap.capabilities & V4L2_CAP_STREAMING) {
		/* map the buffers */	
		for (i = 0; i < mtVideoDevice.iVideoBufCnt; i++) {
			if (mtVideoDevice.pucVideBuf[i]) {
				munmap(mtVideoDevice.pucVideBuf[i], mtVideoDevice.iVideoBufMaxLen);
				mtVideoDevice.pucVideBuf[i] = NULL;
			}
		}	
		
	}
	else if (tV4l2Cap.capabilities & V4L2_CAP_READWRITE) {
		/* read(fd, buf, size) */
		delete mtVideoDevice.pucVideBuf[0];
		mtVideoDevice.pucVideBuf[0] = NULL;
	}


	close(mtVideoDevice.iFd);
	return 0;
}

    
static int V4l2GetFrameForStreaming(struct VideoDevice& tVideoDevice, struct VideoBuf& tVideoBuf)
{
    struct pollfd tFds[1];
    int iRet;
    struct v4l2_buffer tV4l2Buf;
            
    /* poll */
    tFds[0].fd     = tVideoDevice.iFd;
    tFds[0].events = POLLIN;

    iRet = poll(tFds, 1, -1);
    if (iRet <= 0) {
        printf("poll error!\n");
        return -1;
    }
    
    /* VIDIOC_DQBUF */
    memset(&tV4l2Buf, 0, sizeof(struct v4l2_buffer));
    tV4l2Buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    tV4l2Buf.memory = V4L2_MEMORY_MMAP;
    iRet = ioctl(tVideoDevice.iFd, VIDIOC_DQBUF, &tV4l2Buf);
    if (iRet < 0)  {
    	printf("Unable to dequeue buffer.\n");
    	return -1;
    }
    tVideoDevice.iVideoBufCurIndex = tV4l2Buf.index;

    tVideoBuf.iPixelFormat        = tVideoDevice.iPixelFormat;
    tVideoBuf.tPixelDatas.iWidth  = tVideoDevice.iWidth;
    tVideoBuf.tPixelDatas.iHeight = tVideoDevice.iHeight;
    tVideoBuf.tPixelDatas.iBpp    = (tVideoDevice.iPixelFormat == V4L2_PIX_FMT_YUYV) ? 16 : \
                                        (tVideoDevice.iPixelFormat == V4L2_PIX_FMT_MJPEG) ? 0 :  \
                                        (tVideoDevice.iPixelFormat == V4L2_PIX_FMT_RGB565) ? 16 :  \
                                        0;
    tVideoBuf.tPixelDatas.iLineBytes    = tVideoDevice.iWidth * tVideoBuf.tPixelDatas.iBpp / 8;
    tVideoBuf.tPixelDatas.iTotalBytes   = tV4l2Buf.bytesused;
    tVideoBuf.tPixelDatas.pcPixelDatas = tVideoDevice.pucVideBuf[tV4l2Buf.index];    
    return 0;
}



static int V4l2GetFrameForReadWrite(struct VideoDevice& tVideoDevice, struct VideoBuf& tVideoBuf)
{
    int iRet;

    iRet = read(tVideoDevice.iFd, tVideoDevice.pucVideBuf[0], tVideoDevice.iVideoBufMaxLen);
    if (iRet <= 0)
    {
        return -1;
    }
    
    tVideoBuf.iPixelFormat        = tVideoDevice.iPixelFormat;
    tVideoBuf.tPixelDatas.iWidth  = tVideoDevice.iWidth;
    tVideoBuf.tPixelDatas.iHeight = tVideoDevice.iHeight;
    tVideoBuf.tPixelDatas.iBpp    = (tVideoDevice.iPixelFormat == V4L2_PIX_FMT_YUYV) ? 16 : \
                                        (tVideoDevice.iPixelFormat == V4L2_PIX_FMT_MJPEG) ? 0 :  \
                                        (tVideoDevice.iPixelFormat == V4L2_PIX_FMT_RGB565)? 16 : \
                                          0;
    tVideoBuf.tPixelDatas.iLineBytes    = tVideoDevice.iWidth * tVideoBuf.tPixelDatas.iBpp / 8;
    tVideoBuf.tPixelDatas.iTotalBytes   = iRet;
    tVideoBuf.tPixelDatas.pcPixelDatas = tVideoDevice.pucVideBuf[0];    
    
    return 0;
}



int CameraWindow::getFrame(struct VideoBuf& tVideoBuf)
{
	struct v4l2_capability tV4l2Cap;

	memset(&tV4l2Cap, 0, sizeof(struct v4l2_capability));
	ioctl(mtVideoDevice.iFd, VIDIOC_QUERYCAP, &tV4l2Cap);

	if (tV4l2Cap.capabilities & V4L2_CAP_STREAMING) {
		/* map the buffers */	
		V4l2GetFrameForStreaming(mtVideoDevice, tVideoBuf);
	}
	else if (tV4l2Cap.capabilities & V4L2_CAP_READWRITE) {
		/* read(fd, buf, size) */
		V4l2GetFrameForReadWrite(mtVideoDevice, tVideoBuf);
	}
	
	return 0;
}





static int V4l2PutFrameForStreaming(struct VideoDevice& tVideoDevice, struct VideoBuf& tVideoBuf)
{
    /* VIDIOC_QBUF */
    struct v4l2_buffer tV4l2Buf;
    int iError;
    
	memset(&tV4l2Buf, 0, sizeof(struct v4l2_buffer));
	tV4l2Buf.index  = tVideoDevice.iVideoBufCurIndex;
	tV4l2Buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	tV4l2Buf.memory = V4L2_MEMORY_MMAP;
	iError = ioctl(tVideoDevice.iFd, VIDIOC_QBUF, &tV4l2Buf);
	if (iError)  {
	    printf("Unable to queue buffer.\n");
	    return -1;
	}
    return 0;
}

static int V4l2PutFrameForReadWrite(struct VideoDevice& tVideoDevice, struct VideoBuf& tVideoBuf)
{
    return 0;
}


int CameraWindow::putFrame(struct VideoBuf& tVideoBuf)
{
	struct v4l2_capability tV4l2Cap;

	memset(&tV4l2Cap, 0, sizeof(struct v4l2_capability));
	ioctl(mtVideoDevice.iFd, VIDIOC_QUERYCAP, &tV4l2Cap);

	if (tV4l2Cap.capabilities & V4L2_CAP_STREAMING) {
		/* map the buffers */	
		V4l2PutFrameForStreaming(mtVideoDevice, tVideoBuf);
	}
	else if (tV4l2Cap.capabilities & V4L2_CAP_READWRITE) {
		/* read(fd, buf, size) */
		V4l2PutFrameForReadWrite(mtVideoDevice, tVideoBuf);
	}
	
	return 0;	
}

int CameraWindow::getFormat(void)
{	
    return mtVideoDevice.iPixelFormat;
}

int CameraWindow::startCameraDevice(void)
{
	int iType = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int iError;
	int iPixelFormatOfVideo;
	int iPixelFormatOfDisp;
	
	iPixelFormatOfVideo = getFormat();
    iPixelFormatOfDisp = (FrameBuffer::getFrameBuffer()->getScreenInfo().iBpp == 16) ? V4L2_PIX_FMT_RGB565 : \
						(FrameBuffer::getFrameBuffer()->getScreenInfo().iBpp == 32) ?  V4L2_PIX_FMT_RGB32 : 0;
	
	mConvert = selectVideoConvertForFormats(iPixelFormatOfVideo, iPixelFormatOfDisp);
	if (NULL == mConvert) {
		printf("%s %s %d : can not support this format convert\n", __FILE__, __FUNCTION__, __LINE__);
		return -1;		
	}

	iError = ioctl(mtVideoDevice.iFd, VIDIOC_STREAMON, &iType);
	if (iError) {
		printf("Unable to start capture.\n");
		return -1;
	}


	mCameraThread.iPixelFormatOfDisp  = iPixelFormatOfDisp;
	mCameraThread.iPixelFormatOfVideo = iPixelFormatOfVideo;
	mCameraThread.ptVideoDevice       = &mtVideoDevice;
	mCameraThread.window			  = this;
	mCameraThread.convert			  = mConvert;
	pthread_create(&mCameraThread.tTreadID, NULL, CameraThreadFunction, &mCameraThread);

	
	return 0;
}


int CameraWindow::stopCameraDevice(void)
{
	int iType = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int iError;
	
	mConvert->convertExit(mtConvertBuf);
	
	iError = ioctl(mtVideoDevice.iFd, VIDIOC_STREAMOFF, &iType);
	if (iError) {
		printf("Unable to stop capture.\n");
		return -1;
	}
	return 0;
}

Convert* CameraWindow::selectVideoConvertForFormats(int iPixelFormatIn, int iPixelFormatOut)
{
	mConvert = NULL;
	mConvert = new Mjpeg2rgb();
	if (mConvert->isSupport(iPixelFormatIn, iPixelFormatOut)) {
		return mConvert;
	}
	delete mConvert;
	mConvert = NULL;

	mConvert = new Rgb2rgb();
	if (mConvert->isSupport(iPixelFormatIn, iPixelFormatOut)) {
		return mConvert;
	}
	delete mConvert;
	mConvert = NULL;

	mConvert = new Yuv2rgb();
	if (mConvert->isSupport(iPixelFormatIn, iPixelFormatOut)) {
		return mConvert;
	}
	delete mConvert;
	mConvert = NULL;	

	return mConvert;
}

struct VideoBuf& CameraWindow::getConvertBuf(void)
{
	return mtConvertBuf;
}


void CameraWindow::drawWindowPixelData(void)
{
	struct WindowInfo& windowInfo = getWindowInfo();
	windowInfo.tWindowCurPixelDatas.iBpp = FrameBuffer::getFrameBuffer()->getScreenInfo().iBpp;
	windowInfo.tWindowCurPixelDatas.iWidth = windowInfo.iWidth;
	windowInfo.tWindowCurPixelDatas.iHeight = windowInfo.iHeight;
	windowInfo.tWindowCurPixelDatas.iLineBytes = windowInfo.iWidth * windowInfo.tWindowCurPixelDatas.iBpp / 8;
	windowInfo.tWindowCurPixelDatas.iTotalBytes = windowInfo.tWindowCurPixelDatas.iLineBytes * windowInfo.tWindowCurPixelDatas.iHeight;
	windowInfo.tWindowCurPixelDatas.pcPixelDatas = new char[windowInfo.tWindowCurPixelDatas.iTotalBytes];

	/* 先绘制顶部的功能栏 */
	FrameBuffer::getFrameBuffer()->FbDrawRectangle(windowInfo.tWindowCurPixelDatas,\
	0, 0, windowInfo.tWindowCurPixelDatas.iWidth, windowInfo.iHeight / 8, 0);

	/* 绘制内容区 */
	FrameBuffer::getFrameBuffer()->FbDrawRectangle(windowInfo.tWindowCurPixelDatas,\
	0, windowInfo.iHeight / 8, windowInfo.tWindowCurPixelDatas.iWidth, windowInfo.tWindowCurPixelDatas.iHeight - windowInfo.iHeight / 10, 0xFFFFFFFF);



	/* 绘制底部一条边 */
	FrameBuffer::getFrameBuffer()->FbDrawLine(windowInfo.tWindowCurPixelDatas, \
		0, windowInfo.iHeight - 1, windowInfo.iWidth, windowInfo.iHeight -1 , 0);
	
	/* 绘制右边一条边 */
	FrameBuffer::getFrameBuffer()->FbDrawLine(windowInfo.tWindowCurPixelDatas, \
		windowInfo.iWidth - 1, 0, windowInfo.iWidth - 1, windowInfo.iHeight, 0);

	/* 绘制左边一条边 */
	FrameBuffer::getFrameBuffer()->FbDrawLine(windowInfo.tWindowCurPixelDatas, \
		0, 0, 0, windowInfo.iHeight, 0);
			
}


int CameraWindow::onProcess(struct CookEvent& cookEvent)
{
	int iRet = 0;


	if (isMoved()) {
		if (cookEvent.iPressure == 0) {
			setMoved(false);
		}
		
		moveWindow(cookEvent.iX, cookEvent.iY);
		return 0;
	}

	iRet = dispatchEvent(cookEvent);
	if (iRet < 0) {
		/* 没有支持view */	
		if (isRemovable() && isLongPress(cookEvent.tTime)) {
			/* 这个window可移动并且长按了 */
			setMoved(true); 
		}	
	}
		
	return iRet;
}




