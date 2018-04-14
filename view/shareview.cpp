#include <view/shareview.h>
#include <windowmanager.h>
#include <qqlink.h>
#include <window/filemanagerwindow.h>

ShareView::ShareView(std::string iconPathName, ViewInfo& viewInfo)
:View(iconPathName, viewInfo)
{
}


int ShareView::process(struct CookEvent& cookEvent)
{
	structuring_msg msg = {0};

	msg.file_path = (char *)getParentWindow()->getFileName().c_str();

	if (!QQlink::getQQlink()->isConnect()) {
		/* 没有与QQ连接上 */
		
		return 0;
	}


	if (FileManagerWindow::isPicFile((char *)getParentWindow()->getFileName().c_str())) {
		/* 图片 */
		msg.thumb_path = (char *)getParentWindow()->getFileName().c_str();
		msg.title = (char *)"图片分享";
		msg.digest = (char *)"来自嵌入式设备的图片分享";
		msg.guide_words = (char *)"点击查看";
		
		QQlink::getQQlink()->sendPicMsg(msg);
	}
	else if (FileManagerWindow::isMusicFile((char *)getParentWindow()->getFileName().c_str())) {
		/* mp3 */	
		msg.title = (char *)"语音分享";
		msg.digest = (char *)"来自嵌入式设备的语音分享";
		msg.guide_words = (char *)"点击查看";
		QQlink::getQQlink()->sendAudioMsg(msg);
	}



	return 0;	
}





