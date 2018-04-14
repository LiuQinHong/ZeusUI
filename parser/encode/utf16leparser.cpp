#include <encode/utf16leparser.h>
#include <string.h>

Utf16LeParser::Utf16LeParser(){}
Utf16LeParser::Utf16LeParser(std::string encodeParserName, Fonts *pFonts)
{
	setEncodeParserName(encodeParserName);
	setFonts(pFonts);
}

bool Utf16LeParser::isSupport(char *pcBufHead, char *pcBufEnd)
{
	const char aStrUtf16le[] = {0xFF, 0xFE, 0};
	if (strncmp((const char *)pcBufHead, aStrUtf16le, 2) == 0) {
		/* UTF-16 little endian */
		return true;
	}
	else {
		return false;
	}
}


int Utf16LeParser::getCodeFromBuf(char *pcBufStart, char *pcBufEnd, unsigned int& iCode)
{
	if (pcBufStart + 1 < pcBufEnd)
	{
		iCode = (((unsigned int)pcBufStart[1])<<8) + (unsigned int)pcBufStart[0];
		return 2;
	}
	else {
		/* 读完 */
		return 0;
	}
}


