#include <window/musicwindow.h>
#include <device/framebuffer.h>
#include <input/inputreader.h>
#include <windowmanager.h>
#include <view/closeview.h>
#include <view/shareview.h>

#include <pic/picparser.h>
#include <pic/jpgparser.h>
#include <pic/pngparser.h>
#include <pic/bmpparser.h>


MusicWindow::MusicWindow(){}

MusicWindow::~MusicWindow()
{

	mMusicParser->stop();

	delete mMusicParser;
}


MusicWindow::MusicWindow(const std::string& strTargetFileName)
:Window(strTargetFileName), strMusicFileName(strTargetFileName)
{
	setRemovable(true);
	struct WindowInfo& windowInfo = getWindowInfo();
	
	windowInfo.iWidth = FrameBuffer::getFrameBuffer()->getScreenInfo().iXres / 5 * 3;
	windowInfo.iHeight = windowInfo.iWidth / 3 * 2;
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


	/* share view */
	viewInfo.iX = 0;
	viewInfo.iY = 0;
	viewInfo.iWidth = windowInfo.iHeight / 8;
	viewInfo.iHeight = viewInfo.iWidth;
	viewInfo.iCareEventType = TOUCHSCREEN;
	view = new ShareView(SHARE_VIEW_ICON, viewInfo);
	view->setParentWindow(this);
	view->showViewToWindow();
	addView(view);


	mMusicParser = new Mp3Parser(strTargetFileName);
	mMusicParser->play();

}



void MusicWindow::drawWindowPixelData(void)
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


	TextParser * textParser = new TextParser();

	showInfoTitleBar(strMusicFileName, textParser);

#if 1
	Id3Tag *id3Tag = new Id3Tag();
	id3Tag->id3TagOpen(strMusicFileName);
	std::list<struct id3_Info *>& id3_Info_list = id3Tag->parser();


	struct id3_Info *id3Info = NULL;
	int iX = 0;
	int iY = windowInfo.iHeight / 8 + 20;

	for(std::list<struct id3_Info *>::iterator id3_Info_list_iter = id3_Info_list.begin(); id3_Info_list_iter!= id3_Info_list.end(); ++id3_Info_list_iter) {
		id3Info = *id3_Info_list_iter;
	
		if (id3Info->ptFrameID->type == ID3_INFO_TYPE_PIC) {
			/* 图片数据 */
			mPicDesc.pcPicData = (char *)id3Info->data;
			mPicDesc.iLength = id3Info->length;
			mPicDesc.type = id3Info->picType;

			
		}
		else if (id3Info->ptFrameID->type == ID3_INFO_TYPE_TEXT) {

			switch (id3Info->textencoding) {
				case ID3_FIELD_TEXTENCODING_ISO_8859_1:
				{				
					textParser->putStr(getWindowInfo().tWindowCurPixelDatas, iX, iY, \
						(const char*)id3Info->data, id3Info->length, 0, ENCODE_ASCII);
					iY += 20; 
					
					break;
				}
				case ID3_FIELD_TEXTENCODING_UTF_16:
				{							
					textParser->putStr(getWindowInfo().tWindowCurPixelDatas, iX, iY, \
						(const char*)id3Info->data, id3Info->length, 0, ENCODE_UTF16LE);
					iY += 20; 

					break;
				}
				case ID3_FIELD_TEXTENCODING_UTF_16BE:
				{			
					textParser->putStr(getWindowInfo().tWindowCurPixelDatas, iX, iY, \
						(const char*)id3Info->data, id3Info->length, 0, ENCODE_UTF16BE);
					iY += 20; 

					break;
				}
				case ID3_FIELD_TEXTENCODING_UTF_8:
				{
					textParser->putStr(getWindowInfo().tWindowCurPixelDatas, iX, iY, \
						(const char*)id3Info->data, id3Info->length, 0, ENCODE_UTF8);			

					iY += 20; 
					break;
				}
				
			}			

		}

		delete id3Info->data;
 	}	

	id3Tag->id3TagClose();
	delete id3Tag;
#endif


	delete textParser;
}


int MusicWindow::onProcess(struct CookEvent& cookEvent)
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



