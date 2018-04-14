#include <encode/encodeparser.h>
#include <encode/utf8parser.h>
#include <encode/utf16beparser.h>
#include <encode/utf16leparser.h>
#include <encode/asciiparser.h>
#include <fonts/freetype.h>
#include <fonts/asciifont.h>

std::list<EncodeParser*> EncodeParser::encodeParserList;


EncodeParser::EncodeParser()
{

}

EncodeParser::EncodeParser(std::string encodeParserName, Fonts *pFonts)
:mEncodeParserName(encodeParserName), mpFonts(pFonts)
{

}


EncodeParser* EncodeParser::selectEncodeParser(char *pcBufHead, char *pcBufEnd)
{

	for(std::list<EncodeParser*>::iterator list_iter = encodeParserList.begin(); 
		list_iter != encodeParserList.end(); list_iter++) {
		/* 遍历所有解码器 */
		if ((*list_iter)->isSupport(pcBufHead, pcBufEnd)) {
			return *list_iter;
		}
	}

	/* 没有找到合适的解码器 */
	return NULL;
}


EncodeParser* EncodeParser::selectEncodeParser(const std::string& strEncodeName)
{
	EncodeParser* encodeParser= NULL;

	for(std::list<EncodeParser*>::iterator list_iter = encodeParserList.begin(); 
		list_iter != encodeParserList.end(); list_iter++) {
		/* 遍历所有解码器 */
		encodeParser = (*list_iter);
		if (!encodeParser->getEncodeParserName().compare(strEncodeName)) {
			return encodeParser;
		}
		
	}

	/* 没有找到合适的解码器 */
	return NULL;
}


/* 当要添加新的编码器时在这里添加 */
void EncodeParser::addAllEncodeParser(void)
{

	AsciiParser *asciiParser = new AsciiParser(ENCODE_ASCII, AsciiFont::getAsciiFont());
	encodeParserList.push_back(asciiParser);

	Utf8Parser *utf8Parser = new Utf8Parser(ENCODE_UTF8, Freetype::getFreetype());
	encodeParserList.push_back(utf8Parser);

	Utf16BeParser *utf16BeParser = new Utf16BeParser(ENCODE_UTF16BE, Freetype::getFreetype());
	encodeParserList.push_back(utf16BeParser);

	Utf16LeParser *utf16LeParser = new Utf16LeParser(ENCODE_UTF16LE, Freetype::getFreetype());
	encodeParserList.push_back(utf16LeParser);

}

std::string& EncodeParser::getEncodeParserName(void)
{
	return mEncodeParserName;
}

void EncodeParser::setEncodeParserName(std::string encodeParserName)
{
	mEncodeParserName = encodeParserName;
}

Fonts *EncodeParser::getFonts(void)
{
	return mpFonts;
}

void EncodeParser::setFonts(Fonts *pFonts)
{
	mpFonts = pFonts;
}

