#ifndef __SNDPCM_H__
#define __SNDPCM_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class SndPcm;

#include <iostream>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <alsa/asoundlib.h>


class SndPcm {
public:
	int initSndPcm(int iSampleRate, int iChannels);
	void exitSndPcm(void);
	void writeSndPcm(char *pcFrameBuffer);

	snd_pcm_uframes_t& getFrames(void);
	int getFrameSize(void);
	int getSampleRate(void);
	int getChannels(void);
	char *getFrameBuffer(void);
	static SndPcm* getSndPcm(void);
private:
	static SndPcm *sndpcm;
	static pthread_mutex_t tMutex;
	SndPcm();
	snd_pcm_t *mHandle;
	snd_pcm_uframes_t mFrames;
	int mFrameSize;	
	char *mFrameBuffer;
	int mSampleRate;
	int mChannels;

};


#endif // __SNDPCM_H__





