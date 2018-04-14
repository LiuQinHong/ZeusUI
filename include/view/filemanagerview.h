#ifndef __FILEMANAGERVIEW_H__
#define __FILEMANAGERVIEW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class FileManagerView;


#include <view/view.h>

#define FILEMANAGER_VIEW_ICON "/res/view/filemanagerview.png"

class FileManagerView : public View{
public:	
	FileManagerView(std::string iconPathName, ViewInfo& viewInfo);
	virtual int process(struct CookEvent& cookEvent);
private:
	FileManagerView();
};



#endif //__FILEMANAGERVIEW_H__



