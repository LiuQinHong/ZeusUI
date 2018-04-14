#ifndef __ENCODEPARSER_H__
#define __ENCODEPARSER_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class EncodeParser;

#include <iostream>
#include <list>
#include <fonts/fonts.h>


#define ENCODE_ASCII	"ascii"
#define ENCODE_UTF8		"utf8"
#define ENCODE_UTF16BE	"utf16Be"
#define ENCODE_UTF16LE 	"utf16Le"

class EncodeParser {
public:
	EncodeParser();
	EncodeParser(std::string encodeParserName, Fonts *pFonts);
	static EncodeParser* selectEncodeParser(char *pcBufHead, char *pcBufEnd);
	static EncodeParser* selectEncodeParser(const std::string& strEncodeName);
	static void addAllEncodeParser(void);
	std::string& getEncodeParserName(void);
	void setEncodeParserName(std::string encodeParserName);
	Fonts *getFonts(void);
	void setFonts(Fonts *pFonts);
	virtual bool isSupport(char *pcBufHead, char *pcBufEnd) = 0;
	virtual int getCodeFromBuf(char *pcBufStart, char *pcBufEnd, unsigned int& iCode) = 0;
	
private:
	static std::list<EncodeParser*> encodeParserList;
	std::string mEncodeParserName;
	Fonts *mpFonts;
};


#endif // __ENCODEPARSER_H__



