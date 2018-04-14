#include <textparser.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



TextParser::TextParser()
{
	mtTextFileInfo.pcFileDataStart = NULL;
	mpEncodeParser = NULL;
	mpFonts = NULL;
	mpFrameBuffer = NULL;
	mtFontInfo.iFontSize = 8;
}


TextParser::~TextParser()
{
	if (mtTextFileInfo.pcFileDataStart) {
		delete mtTextFileInfo.pcFileDataStart;
	}
}

int TextParser::openTextInit(const std::string& fileName)
{
	struct stat fileStat;
	int iFd;

	iFd = open(fileName.c_str(), O_RDONLY);
	if (iFd < 0) {
		fprintf(stderr, "open %s error!\n", fileName.c_str());
		return -1;
	}

	if (fstat(iFd, &fileStat)) {
		fprintf(stderr, "can't get fstat\n");
		close(iFd);
		return -1;
	}

	mFileName = fileName;
	mtTextFileInfo.iFileSize = fileStat.st_size;
	mtTextFileInfo.pcFileDataStart = new char[mtTextFileInfo.iFileSize];
	if (mtTextFileInfo.pcFileDataStart == NULL) {
		fprintf(stderr, "can't get fstat\n");
		close(iFd);
		return -1;		
	}

	read(iFd, mtTextFileInfo.pcFileDataStart, mtTextFileInfo.iFileSize);
	mtTextFileInfo.pcFileDataEnd = mtTextFileInfo.pcFileDataStart + mtTextFileInfo.iFileSize;
	close(iFd);


	mpEncodeParser = EncodeParser::selectEncodeParser(mtTextFileInfo.pcFileDataStart, mtTextFileInfo.pcFileDataEnd);
	if (mpEncodeParser == NULL) {
		delete mtTextFileInfo.pcFileDataStart;
		mtTextFileInfo.pcFileDataStart = NULL;
		return -1;
	}
	//printf("select %s parser\n", mpEncodeParser->getEncodeParserName().c_str());

	/* 默认值 */
	mtFontInfo.iBackcolor = 0;
	mtFontInfo.iFontColor = 0xFFFFFFFF;
	mtFontInfo.iFontSize = mpEncodeParser->getFonts()->getFontSize();

	mPoslist.push_back(mtTextFileInfo.pcFileDataStart);

	return 0;
}



void TextParser::releaseText(void)
{
	if (mtTextFileInfo.pcFileDataStart) {
		mtTextFileInfo.pcFileDataStart = NULL;
		delete mtTextFileInfo.pcFileDataStart;
	}
}



void TextParser::showText(void)
{
	unsigned int iCode;
	int iCount = 1;
	int iConsumeSize;
	FontBitMap tFontBitMap;
	tFontBitMap.iCurOriginX = 0;
	tFontBitMap.iCurOriginY = 30;
	
	char *pcFileDataStart = mtTextFileInfo.pcFileDataStart;
	char *pcFileDataEnd = mtTextFileInfo.pcFileDataEnd;
	
	while (1) {
		iConsumeSize = mpEncodeParser->getCodeFromBuf(pcFileDataStart, pcFileDataEnd, iCode);
		if (!iConsumeSize) {
			return;
		}
		pcFileDataStart += iConsumeSize;

		if (iCode == '\n') {		
			tFontBitMap.iCurOriginX = 0;
			tFontBitMap.iCurOriginY += mtFontInfo.iFontSize;
			if (tFontBitMap.iCurOriginY > FrameBuffer::getFrameBuffer()->getScreenInfo().iYres) {
				return;
			}
			continue;
		}
		else if (iCode == '\r') {
			continue;
		}
		else if (iCode == '\t') {
			iCode = ' ';
			iCount = 4;
		}

		while (iCount--) {
			mpEncodeParser->getFonts()->getFontBitmap(iCode, tFontBitMap);	
			FrameBuffer::getFrameBuffer()->FbDrawBitMap(tFontBitMap);
			
			tFontBitMap.iCurOriginX = tFontBitMap.iNextOriginX;
			tFontBitMap.iCurOriginY = tFontBitMap.iNextOriginY;
			if ((tFontBitMap.iCurOriginX + mtFontInfo.iFontSize) > FrameBuffer::getFrameBuffer()->getScreenInfo().iXres) {
				tFontBitMap.iCurOriginX = 0;
				tFontBitMap.iCurOriginY += mtFontInfo.iFontSize;
			}
			
			if (tFontBitMap.iCurOriginY > FrameBuffer::getFrameBuffer()->getScreenInfo().iYres) {
				return;
			}

		}
		iCount = 1;
	}




}



int TextParser::showText(struct PixelDatas& tPixelDatas, int iX, int iY, unsigned int dwColor, bool isDowm)
{
	unsigned int iCode;
	int iCount = 1;
	int iConsumeSize = 0;
	FontBitMap tFontBitMap;

	
	tFontBitMap.iCurOriginX = iX;
	tFontBitMap.iCurOriginY = iY;

	if (isDowm) {
		/* 下一页 */
		if (isTheLastPage()) {
			/* 最后一页 */
			return 0;
		}

		while (1) {
			iConsumeSize = mpEncodeParser->getCodeFromBuf(mPosStart, mtTextFileInfo.pcFileDataEnd, iCode);
			if (!iConsumeSize) {
				mPoslist.push_back(mPosStart);
				return 0;
			}
			mPosStart += iConsumeSize;

			if (iCode == '\n') {		
				tFontBitMap.iCurOriginX = iX;
				tFontBitMap.iCurOriginY += mtFontInfo.iFontSize;
				if (tFontBitMap.iCurOriginY > tPixelDatas.iHeight) {
					mPoslist.push_back(mPosStart);
					return 0;
				}
				continue;
			}
			else if (iCode == '\r') {
				continue;
			}
			else if (iCode == '\t') {
				iCode = ' ';
				iCount = 4;
			}

			while (iCount--) {
				mpEncodeParser->getFonts()->getFontBitmap(iCode, tFontBitMap);	
				FrameBuffer::getFrameBuffer()->FbDrawBitMap(tPixelDatas, tFontBitMap, dwColor);
				
				tFontBitMap.iCurOriginX = tFontBitMap.iNextOriginX;
				tFontBitMap.iCurOriginY = tFontBitMap.iNextOriginY;
				if ((tFontBitMap.iCurOriginX + mtFontInfo.iFontSize) > tPixelDatas.iWidth) {
					tFontBitMap.iCurOriginX = 0;
					tFontBitMap.iCurOriginY += mtFontInfo.iFontSize;
				}
				
				if (tFontBitMap.iCurOriginY > tPixelDatas.iHeight) {
					mPoslist.push_back(mPosStart);					
					return 0;
				}

			}
			iCount = 1;
		}
	}
	else {
		/* 上一页 */	
		if (isFirstPage()) {
			/* 第一页 */
			return 0;
		}
		
		mPoslist.pop_back();
		mPoslist.pop_back();		
		mPosStart = mPoslist.back();
		while (1) {
			iConsumeSize = mpEncodeParser->getCodeFromBuf(mPosStart, mtTextFileInfo.pcFileDataEnd, iCode);
			if (!iConsumeSize) {
				mPoslist.push_back(mPosStart);
				return 0;
			}
			mPosStart += iConsumeSize;

			if (iCode == '\n') {		
				tFontBitMap.iCurOriginX = iX;
				tFontBitMap.iCurOriginY += mtFontInfo.iFontSize;
				if (tFontBitMap.iCurOriginY > tPixelDatas.iHeight) {
					mPoslist.push_back(mPosStart);
					return 0;
				}
				continue;
			}
			else if (iCode == '\r') {
				continue;
			}
			else if (iCode == '\t') {
				iCode = ' ';
				iCount = 4;
			}

			while (iCount--) {
				mpEncodeParser->getFonts()->getFontBitmap(iCode, tFontBitMap);	
				FrameBuffer::getFrameBuffer()->FbDrawBitMap(tPixelDatas, tFontBitMap, dwColor);
				
				tFontBitMap.iCurOriginX = tFontBitMap.iNextOriginX;
				tFontBitMap.iCurOriginY = tFontBitMap.iNextOriginY;
				if ((tFontBitMap.iCurOriginX + mtFontInfo.iFontSize) > tPixelDatas.iWidth) {
					tFontBitMap.iCurOriginX = 0;
					tFontBitMap.iCurOriginY += mtFontInfo.iFontSize;
				}
				
				if (tFontBitMap.iCurOriginY > tPixelDatas.iHeight) {
					mPoslist.push_back(mPosStart);					
					return 0;
				}

			}
			iCount = 1;
		}	
	}
	
	return 0;
}



int TextParser::showText(struct PixelDatas& tPixelDatas, char *pcBufStart, char *pcBufEnd, int iX, int iY, unsigned int dwColor, bool isDowm)
{
	unsigned int iCode;
	int iCount = 1;
	int iConsumeSize = 0;
	FontBitMap tFontBitMap;

	
	tFontBitMap.iCurOriginX = iX;
	tFontBitMap.iCurOriginY = iY;

	if (isDowm) {
		/* 下一页 */
		if (isTheLastPage()) {
			/* 最后一页 */
			return 0;
		}

		while (1) {
			iConsumeSize = mpEncodeParser->getCodeFromBuf(pcBufStart, pcBufEnd, iCode);
			if (!iConsumeSize) {
				mPoslist.push_back(mPosStart);
				return 0;
			}
			mPosStart += iConsumeSize;

			if (iCode == '\n') {		
				tFontBitMap.iCurOriginX = iX;
				tFontBitMap.iCurOriginY += mtFontInfo.iFontSize;
				if (tFontBitMap.iCurOriginY > tPixelDatas.iHeight) {
					mPoslist.push_back(mPosStart);
					return 0;
				}
				continue;
			}
			else if (iCode == '\r') {
				continue;
			}
			else if (iCode == '\t') {
				iCode = ' ';
				iCount = 4;
			}

			while (iCount--) {
				mpEncodeParser->getFonts()->getFontBitmap(iCode, tFontBitMap);	
				FrameBuffer::getFrameBuffer()->FbDrawBitMap(tPixelDatas, tFontBitMap, dwColor);
				
				tFontBitMap.iCurOriginX = tFontBitMap.iNextOriginX;
				tFontBitMap.iCurOriginY = tFontBitMap.iNextOriginY;
				if ((tFontBitMap.iCurOriginX + mtFontInfo.iFontSize) > tPixelDatas.iWidth) {
					tFontBitMap.iCurOriginX = 0;
					tFontBitMap.iCurOriginY += mtFontInfo.iFontSize;
				}
				
				if (tFontBitMap.iCurOriginY > tPixelDatas.iHeight) {
					mPoslist.push_back(mPosStart);					
					return 0;
				}

			}
			iCount = 1;
		}
	}
	else {
		/* 上一页 */	
		if (isFirstPage()) {
			/* 第一页 */
			return 0;
		}
		
		mPoslist.pop_back();
		mPoslist.pop_back();		
		mPosStart = mPoslist.back();
		while (1) {
			iConsumeSize = mpEncodeParser->getCodeFromBuf(mPosStart, mtTextFileInfo.pcFileDataEnd, iCode);
			if (!iConsumeSize) {
				mPoslist.push_back(mPosStart);
				return 0;
			}
			mPosStart += iConsumeSize;

			if (iCode == '\n') {		
				tFontBitMap.iCurOriginX = iX;
				tFontBitMap.iCurOriginY += mtFontInfo.iFontSize;
				if (tFontBitMap.iCurOriginY > tPixelDatas.iHeight) {
					mPoslist.push_back(mPosStart);
					return 0;
				}
				continue;
			}
			else if (iCode == '\r') {
				continue;
			}
			else if (iCode == '\t') {
				iCode = ' ';
				iCount = 4;
			}

			while (iCount--) {
				mpEncodeParser->getFonts()->getFontBitmap(iCode, tFontBitMap);	
				FrameBuffer::getFrameBuffer()->FbDrawBitMap(tPixelDatas, tFontBitMap, dwColor);
				
				tFontBitMap.iCurOriginX = tFontBitMap.iNextOriginX;
				tFontBitMap.iCurOriginY = tFontBitMap.iNextOriginY;
				if ((tFontBitMap.iCurOriginX + mtFontInfo.iFontSize) > tPixelDatas.iWidth) {
					tFontBitMap.iCurOriginX = 0;
					tFontBitMap.iCurOriginY += mtFontInfo.iFontSize;
				}
				
				if (tFontBitMap.iCurOriginY > tPixelDatas.iHeight) {
					mPoslist.push_back(mPosStart);					
					return 0;
				}

			}
			iCount = 1;
		}	
	}
	
	return 0;
}



bool TextParser::isFirstPage(void)
{
	char *PosTmp = mPoslist.back();
	mPoslist.pop_back();
	if (mPoslist.back() == mtTextFileInfo.pcFileDataStart) {
		/* 已经是第一页了 */
		mPoslist.push_back(PosTmp);
		return true;			
	}
	
	mPoslist.push_back(PosTmp);
	return false;

}

bool TextParser::isTheLastPage(void)
{
	mPosStart = mPoslist.back();
	if (mPosStart == mtTextFileInfo.pcFileDataEnd) {
		/* 已经读到最后了 */
		return true;
	}

	return false;
	
}


void TextParser::setFont(FontInfo& tFontInfo)
{

	if (mpEncodeParser) {
		if (!mpEncodeParser->getFonts()->setFontSize(tFontInfo.iFontSize))
			mtFontInfo = tFontInfo;
	}	
}

FontInfo& TextParser::getFont(void)
{
	return mtFontInfo;
}

void TextParser::putStrToLCD(int iX, int iY, const char *pcStrBufStart, int iLen)
{
	unsigned int iCode;
	int iCount = 1;	
	int iConsumeSize;

	FontBitMap tFontBitMap;
	tFontBitMap.iCurOriginX = iX;
	tFontBitMap.iCurOriginY = iY;
	
	char *pcStrStart = (char *)pcStrBufStart;
	char *pcStrEnd = (char *)pcStrBufStart + iLen;
	EncodeParser *pEncodeParser = EncodeParser::selectEncodeParser(pcStrStart, pcStrEnd);
	if (pEncodeParser == NULL) {
		return ;
	}
	
	while (1) {
		iConsumeSize = pEncodeParser->getCodeFromBuf(pcStrStart, pcStrEnd, iCode);
		if (!iConsumeSize) {
			return;
		}
		pcStrStart += iConsumeSize;


		if (iCode == '\t') {
			iCode = ' ';
			iCount = 4;
		}

		while (iCount--) {
			pEncodeParser->getFonts()->getFontBitmap(iCode, tFontBitMap);	
			FrameBuffer::getFrameBuffer()->FbDrawBitMap(tFontBitMap);
			
			tFontBitMap.iCurOriginX = tFontBitMap.iNextOriginX;
			tFontBitMap.iCurOriginY = tFontBitMap.iNextOriginY;
			if ((tFontBitMap.iCurOriginX + mtFontInfo.iFontSize) > FrameBuffer::getFrameBuffer()->getScreenInfo().iXres) {
				tFontBitMap.iCurOriginX = 0;
				tFontBitMap.iCurOriginY += mtFontInfo.iFontSize;
			}
			
			if (tFontBitMap.iCurOriginY > FrameBuffer::getFrameBuffer()->getScreenInfo().iYres) {
				return;
			}

		}
		iCount = 1;
	}
	
}



void TextParser::putStr(struct PixelDatas& tDestPixelData, int iX, int iY, const char *pcStrBufStart, int iLen, unsigned int dwColor)
{
	unsigned int iCode;
	int iCount = 1;	
	int iConsumeSize;

	FontBitMap tFontBitMap;
	tFontBitMap.iCurOriginX = iX;
	tFontBitMap.iCurOriginY = iY;
	
	char *pcStrStart = (char *)pcStrBufStart;
	char *pcStrEnd = (char *)pcStrBufStart + iLen;
	EncodeParser *pEncodeParser = EncodeParser::selectEncodeParser(pcStrStart, pcStrEnd);
	if (pEncodeParser == NULL) {
		return ;
	}
	
	while (1) {
		iConsumeSize = pEncodeParser->getCodeFromBuf(pcStrStart, pcStrEnd, iCode);
		if (!iConsumeSize) {
			return;
		}
		pcStrStart += iConsumeSize;


		if (iCode == '\t') {
			iCode = ' ';
			iCount = 4;
		}

		while (iCount--) {
			pEncodeParser->getFonts()->getFontBitmap(iCode, tFontBitMap);	
			FrameBuffer::getFrameBuffer()->FbDrawBitMap(tDestPixelData, tFontBitMap, dwColor);
			
			tFontBitMap.iCurOriginX = tFontBitMap.iNextOriginX;
			tFontBitMap.iCurOriginY = tFontBitMap.iNextOriginY;
			if ((tFontBitMap.iCurOriginX + mtFontInfo.iFontSize) > tDestPixelData.iWidth) {
				tFontBitMap.iCurOriginX = iX;
				tFontBitMap.iCurOriginY += mtFontInfo.iFontSize;
			}
			
			if (tFontBitMap.iCurOriginY > tDestPixelData.iHeight) {
				return;
			}

		}
		iCount = 1;
	}
	
}



void TextParser::putStr(struct PixelDatas& tDestPixelData, int iX, int iY, const char *pcStrBufStart, int iLen, unsigned int dwColor, const std::string& strEncodeName)
{
	unsigned int iCode;
	int iCount = 1;	
	int iConsumeSize;

	FontBitMap tFontBitMap;
	tFontBitMap.iCurOriginX = iX;
	tFontBitMap.iCurOriginY = iY;
	
	char *pcStrStart = (char *)pcStrBufStart;
	char *pcStrEnd = (char *)pcStrBufStart + iLen;
	EncodeParser *pEncodeParser = EncodeParser::selectEncodeParser(strEncodeName);
	if (pEncodeParser == NULL) {
		printf("EncodeParser::selectEncodeParser(strEncodeName) error!\n");
		return ;
	}
	
	while (1) {
		iConsumeSize = pEncodeParser->getCodeFromBuf(pcStrStart, pcStrEnd, iCode);
		if (!iConsumeSize) {
			return;
		}
		pcStrStart += iConsumeSize;


		if (iCode == '\t') {
			iCode = ' ';
			iCount = 4;
		}

		while (iCount--) {
			pEncodeParser->getFonts()->getFontBitmap(iCode, tFontBitMap);	
			FrameBuffer::getFrameBuffer()->FbDrawBitMap(tDestPixelData, tFontBitMap, dwColor);
			
			tFontBitMap.iCurOriginX = tFontBitMap.iNextOriginX;
			tFontBitMap.iCurOriginY = tFontBitMap.iNextOriginY;
			if ((tFontBitMap.iCurOriginX + mtFontInfo.iFontSize) > tDestPixelData.iWidth) {
				tFontBitMap.iCurOriginX = iX;
				tFontBitMap.iCurOriginY += mtFontInfo.iFontSize;
			}
			
			if (tFontBitMap.iCurOriginY > tDestPixelData.iHeight) {
				return;
			}

		}
		iCount = 1;
	}
	
}


EncodeParser *TextParser::getEncodeParser(void)
{
	return mpEncodeParser;
}

Fonts *TextParser::getFonts(void)
{
	return mpFonts;
}

