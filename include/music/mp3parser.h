#ifndef __MP3PARSER_H__
#define __MP3PARSER_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class Mp3Parser;

#include <music/musicparser.h>
#include <mad.h>
#include <iostream>

#define MPEG_BUFSZ	40000	/* 2.5 s at 128 kbps; 1 s at 320 kbps */


struct PriData {
	char *pcFrameBuffer;
	Mp3Parser *mp3parser;
	char *data;
	unsigned int length;

	bool isOpen;
	int iFd;
	std::string strFileName;
};


class Mp3Parser : public MusicParser {
public:
	Mp3Parser(const std::string& strFileName);	
	virtual ~Mp3Parser();
	bool isInited(void);
	void setInited(bool isInited);
	bool isStop(void);
	void setStop(bool isStop);	
	virtual void play(void);
	virtual void suspend(void);
	virtual void stop(void);

private:	
	struct mad_decoder mDecoder;
	struct PriData mPriData;
	bool mIsDecoded;
	bool mIsInited;
	bool mIsStop;
	Mp3Parser();	
};


#endif // __MP3PARSER_H__




