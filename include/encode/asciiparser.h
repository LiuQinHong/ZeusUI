#ifndef __ASCII_PARSER_H__
#define __ASCII_PARSER_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class AsciiParser;

#include <iostream>
#include <encode/encodeparser.h>

class AsciiParser  : public EncodeParser{
public:
	AsciiParser();
	AsciiParser(std::string encodeParserName, Fonts *pFonts);
	virtual bool isSupport(char *pcBufHead, char *pcBufEnd);
	virtual int getCodeFromBuf(char *pcBufStart, char *pcBufEnd, unsigned int& iCode);
};


#endif // __ASCII_PARSER_H__





