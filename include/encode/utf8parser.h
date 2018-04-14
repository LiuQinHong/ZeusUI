#ifndef __UTF_8_PARSER_H__
#define __UTF_8_PARSER_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class Utf8Parser;

#include <iostream>
#include <encode/encodeparser.h>

class Utf8Parser : public EncodeParser {
public:
	Utf8Parser();
	Utf8Parser(std::string encodeParserName, Fonts *pFonts);
	virtual bool isSupport(char *pcBufHead, char *pcBufEnd);
	virtual int getCodeFromBuf(char *pcBufStart, char *pcBufEnd, unsigned int& iCode);
};


#endif // __UTF_8_PARSER_H__




