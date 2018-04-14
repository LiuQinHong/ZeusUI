#ifndef __UTF_16_BE_PARSER_H__
#define __UTF_16_BE_PARSER_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class Utf16BeParser;

#include <iostream>
#include <encode/encodeparser.h>

class Utf16BeParser          : public EncodeParser {
public:
	Utf16BeParser();
	Utf16BeParser(std::string encodeParserName, Fonts *pFonts);
	virtual bool isSupport(char *pcBufHead, char *pcBufEnd);
	virtual int getCodeFromBuf(char *pcBufStart, char *pcBufEnd, unsigned int& iCode);
};


#endif // __UTF_16_BE_PARSER_H__




