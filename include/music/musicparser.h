#ifndef __MUSICPARSER_H__
#define __MUSICPARSER_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class MusicParser;

#include <iostream>

class MusicParser{
public:
	MusicParser(const std::string strFileName);
	virtual ~MusicParser();
	MusicParser();
	std::string& getFileName(void);

	virtual void play(void) = 0;
	virtual void suspend(void) = 0;
	virtual void stop(void) = 0;

private:	
	int mFd;
	int mFileSize;
	std::string mStrFileName;
};


#endif // __MUSICPARSER_H__




