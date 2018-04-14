#ifndef __TEXTWINDOW_H__
#define __TEXTWINDOW_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class TextWindow;

#include <window/window.h>
#include <textparser.h>

class TextWindow : public Window{
public:	
	TextWindow(const std::string& strTargetFileName);
	virtual ~TextWindow();
	int showText(void);
	bool isHalfBottom(struct CookEvent& cookEvent);
	void clearContentArea(void);	
	virtual void drawWindowPixelData(void);
	virtual int onProcess(struct CookEvent& cookEvent);
private:
	std::string strTextFileName;
	TextParser *mTextParser;
	TextWindow();
};



#endif //__TEXTWINDOW_H__




