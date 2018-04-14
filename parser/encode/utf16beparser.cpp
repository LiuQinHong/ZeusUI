#include <encode/utf16beparser.h>
#include <string.h>


Utf16BeParser::Utf16BeParser(){}
Utf16BeParser::Utf16BeParser(std::string encodeParserName, Fonts *pFonts)
{
	setEncodeParserName(encodeParserName);
	setFonts(pFonts);
}


bool Utf16BeParser::isSupport(char *pcBufHead, char *pcBufEnd)
{
	const char aStrUtf16be[] = {0xFE, 0xFF, 0};
	
	if (strncmp((const char*)pcBufHead, aStrUtf16be, 2) == 0) {
		/* UTF-16 big endian */
		return true;
	}
	else {
		return false;
	}
}


int Utf16BeParser::getCodeFromBuf(char *pcBufStart, char *pcBufEnd, unsigned int& iCode)
{
	if (pcBufStart + 1 < pcBufEnd) {
		iCode = (((unsigned int)pcBufStart[1])<<8) + (unsigned int)pcBufStart[0];
		return 2;
	}
	else {
		/* 读完 */
		return 0;
	}
}


