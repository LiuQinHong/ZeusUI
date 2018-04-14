#ifndef __MUSICWINDOW_H__
#define __MUSICWINDOW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class MusicWindow;

#include <window/window.h>
#include <textparser.h>
#include <music/mp3parser.h>
#include <id3tag/id3tag.h>


struct PicDesc {
	char *pcPicData;
	unsigned int iLength;
	enum pic_type type;
};

class MusicWindow : public Window{
public:	
	MusicWindow(const std::string& strTargetFileName);
	~MusicWindow();
	virtual void drawWindowPixelData(void);
	virtual int onProcess(struct CookEvent& cookEvent);
private:
	std::string strMusicFileName;
	MusicParser *mMusicParser;
	struct PicDesc mPicDesc;
	MusicWindow();
};



#endif //__MUSICWINDOW_H__




