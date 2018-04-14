#include <encode/utf8parser.h>
#include <string.h>



Utf8Parser::Utf8Parser(){}
Utf8Parser::Utf8Parser(std::string encodeParserName, Fonts *pFonts)
{
	setEncodeParserName(encodeParserName);	
	setFonts(pFonts);
}


bool Utf8Parser::isSupport(char *pcBufHead, char *pcBufEnd)
{
	char* start = pcBufHead;  
	char* end   = pcBufEnd;

	const char aStrUtf8[]	 = {0xEF, 0xBB, 0xBF, 0};
	
	if (strncmp((const char*)pcBufHead, aStrUtf8, 3) == 0) {
		/* This is UTF-8 */
		return true;
	}

	while (start < end) {		
		if ((unsigned int)*start < 0x80) {
			/* (10000000): 值小于0x80的为ASCII字符 */
			start++;  
		}
		else if ((unsigned int)*start < (0xC0)) { 
			/* (11000000): 值介于0x80与0xC0之间的为无效UTF-8字符 */
			return false;  
		}  
		else if ((unsigned int)*start < (0xE0)) {
			/* (11100000): 此范围内为2字节UTF-8字符 */
			if ((start[1] & (0xC0)) != 0x80) {
				return false;
			}
			return true;
		}  
		else if ((unsigned int)*start < (0xF0)) {
			/* (11110000): 此范围内为3字节UTF-8字符 */				
			if ((start[1] & (0xC0)) != 0x80 || (start[2] & (0xC0)) != 0x80) {  
				return false; 
			}
			return true;
		}  
		else {  
			return false;
		}  
	}  

	return true;
}


static int GetPreOneBits(char ucVal)
{
	int i;
	int j = 0;
	
	for (i = 7; i >= 0; i--) {
		if (!(ucVal & (1<<i)))
			break;
		else
			j++;
	}
	return j;

}


int Utf8Parser::getCodeFromBuf(char *pcBufStart, char *pcBufEnd, unsigned int& iCode)
{

	int i;	
	int iNum;
	unsigned char ucVal;
	unsigned int dwSum = 0;

	if (pcBufStart >= pcBufEnd) {
		/* 读完 */
		return 0;
	}

	ucVal = pcBufStart[0];
	iNum  = GetPreOneBits(pcBufStart[0]);

	if ((pcBufStart + iNum) > pcBufEnd) {
		/* 读完 */
		return 0;
	}

	if (iNum == 0) {
		/* ASCII */
		iCode = pcBufStart[0];
		return 1;
	}
	else {
		ucVal = ucVal << iNum;
		ucVal = ucVal >> iNum;
		dwSum += ucVal;
		for (i = 1; i < iNum; i++) {
			ucVal = pcBufStart[i] & 0x3f;
			dwSum = dwSum << 6;
			dwSum += ucVal; 		
		}
		iCode = dwSum;
		
		return iNum;
	}
}


