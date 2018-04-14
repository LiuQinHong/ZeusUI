#include <encode/asciiparser.h>
#include <string.h>

AsciiParser::AsciiParser(){}
AsciiParser::AsciiParser(std::string encodeParserName, Fonts *pFonts)
{
	setEncodeParserName(encodeParserName);
	setFonts(pFonts);
}


bool AsciiParser::isSupport(char *pcBufHead, char *pcBufEnd)
{
	const char aStrUtf8[]	 = {0xEF, 0xBB, 0xBF, 0};
	const char aStrUtf16le[] = {0xFF, 0xFE, 0};
	const char aStrUtf16be[] = {0xFE, 0xFF, 0};
	const char aexeProgram1[] = {0x7F, 0x45, 0x4c, 0x46};
	const char aexeProgram2[] = {0x74, 0x74, 0x63, 0x66};


	
	if (strncmp((const char*)pcBufHead, aStrUtf8, 3) == 0) {
		/* UTF-8 */
		return false;
	}
	else if (strncmp((const char*)pcBufHead, aStrUtf16le, 2) == 0) {
		/* UTF-16 little endian */
		return false;
	}
	else if (strncmp((const char*)pcBufHead, aStrUtf16be, 2) == 0) {
		/* UTF-16 big endian */
		return false;
	}
	else if (strncmp((const char*)pcBufHead, aexeProgram1, 4) == 0) {
		/* UTF-16 big endian */
		return false;
	}	
	else if (strncmp((const char*)pcBufHead, aexeProgram2, 4) == 0) {
		/* UTF-16 big endian */
		return false;
	}
	else {
		return true;
	}
}


int AsciiParser::getCodeFromBuf(char *pcBufStart, char *pcBufEnd, unsigned int& iCode)
{
	char *pucBuf = pcBufStart;
	unsigned int  c = *pucBuf;
	
	if ((pucBuf < pcBufEnd) && (c < 0x80)) {
		/* 返回ASCII码 */
		iCode = c;
		return 1;
	}

	if (((pucBuf + 1) < pcBufEnd) && (c >= 0xA1)) {
		/* 返回GBK码 */
		iCode = pucBuf[0] + (((unsigned int)pucBuf[1])<<8);
		return 2;
	}

	if (pucBuf < pcBufEnd) {
		/* 可能文件有损坏, 但是还是返回一个码, 即使它是错误的 */
		iCode = (unsigned int)c;
		return 1;
	}
	else {
		/* 文件处理完毕 */
		return 0;
	}
}



