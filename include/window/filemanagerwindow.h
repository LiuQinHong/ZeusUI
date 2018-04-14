#ifndef __FILEMANAGERWINDOW_H__
#define __FILEMANAGERWINDOW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class FileManagerWindow;

#include <window/window.h>

class FileManagerWindow : public Window{
public:
	FileManagerWindow();
	FileManagerWindow(const std::string strDirPath);
	void clearContentArea(void);	
	void drawWindowPixelData(void);
	static int isDir(char *strFilePath);
	static int isRegFile(char *strFilePath);
	static int isPicFile(char *strFilePath);
	static int isMusicFile(char *strFilePath);
	static int isTextFile(char *strFilePath);	
	int scanDir(const std::string& dirPath);
	int setMaxFileViewSum(void);
	int getMaxFileViewSum(void);
	int getCurFileViewSum(void);
	void setCurFileViewSum(int iCurFileViewSum);
	int getCurPageCount(void);
	void incCurPageCount(void);
	void decCurPageCount(void);
	bool isEndPage(void);
	void setEndPageFlag(bool mIsEnd);

	std::string& getCurDirName(void);
	void setCurDirName(const std::string& strCurDirName);
	virtual int onProcess(struct CookEvent& cookEvent);
private:
	int mMaxFileViewSum;	//一页最多可以显示多少个文件
	int mCurFileViewSum;	//当前显示多少个文件
	int mCurPageCount;			//当前页数	
	std::string mCurDirName;
	bool isEnd;
};



#endif //__FILEMANAGERWINDOW_H__



