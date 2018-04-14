#include <device/sndpcm.h>


SndPcm *SndPcm::sndpcm = NULL;
pthread_mutex_t SndPcm::tMutex  = PTHREAD_MUTEX_INITIALIZER;



SndPcm* SndPcm::getSndPcm(void)
{
	if (NULL == sndpcm) {		
		pthread_mutex_lock(&tMutex);
		if (NULL == sndpcm) {
			sndpcm = new SndPcm();
		}
		pthread_mutex_unlock(&tMutex);
	}

	return sndpcm;
}


SndPcm::SndPcm()
{
}



int SndPcm::initSndPcm(int iSampleRate,int iChannels)
{
	int iRet;
	unsigned int val;
	int dir=0;
	//snd_pcm_uframes_t frames;
	snd_pcm_uframes_t periodsize;
	snd_pcm_hw_params_t *hw_params;//硬件信息和PCM流配置

	mSampleRate = iSampleRate;
	mChannels = iChannels;

	//1. 打开PCM，最后一个参数为0意味着标准配置
	iRet = snd_pcm_open(&mHandle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (iRet < 0) {
		perror("snd_pcm_open");
		exit(1);
	}

	//2. 分配snd_pcm_hw_params_t结构体
	iRet = snd_pcm_hw_params_malloc(&hw_params);
	if (iRet < 0) {
		perror("snd_pcm_hw_params_malloc");
		exit(1);
	}
	
	//3. 初始化hw_params
	iRet = snd_pcm_hw_params_any(mHandle, hw_params);
	if (iRet < 0) {
		perror("snd_pcm_hw_params_any");
		exit(1);
	}
	
	//4. 初始化访问权限
	iRet = snd_pcm_hw_params_set_access(mHandle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
	if (iRet < 0) {
		perror("snd_pcm_hw_params_set_access");
		exit(1);
	}
	
	//5. 初始化采样格式SND_PCM_FORMAT_S16_LE,16位
	iRet = snd_pcm_hw_params_set_format(mHandle, hw_params, SND_PCM_FORMAT_S16_LE);
	if (iRet < 0) {
		perror("snd_pcm_hw_params_set_format");
		exit(1);
	}

	//6. 设置采样率，如果硬件不支持我们设置的采样率，将使用最接近的
	//val = 44100,有些录音采样频率固定为8KHz
	val = iSampleRate;
	iRet = snd_pcm_hw_params_set_rate_near(mHandle, hw_params, &val, &dir);
	if (iRet < 0) {
		perror("snd_pcm_hw_params_set_rate_near");
		exit(1);
	}

	//7. 设置通道数量
	iRet = snd_pcm_hw_params_set_channels(mHandle, hw_params, iChannels);
	if (iRet < 0) {
		perror("snd_pcm_hw_params_set_channels");
		exit(1);
	}

	/* Set period size to 32 frames. */
	mFrames = 32;
	periodsize = mFrames ;
	iRet = snd_pcm_hw_params_set_buffer_size_near(mHandle, hw_params, &periodsize);
	if (iRet < 0) {
		 printf("Unable to set buffer size %li : %s\n", mFrames * 2, snd_strerror(iRet));
	}
	
	periodsize /= 2;
	iRet = snd_pcm_hw_params_set_period_size_near(mHandle, hw_params, &periodsize, 0);
	if (iRet < 0) {
		printf("Unable to set period size %li : %s\n", periodsize,	snd_strerror(iRet));
	}

	//8. 设置hw_params
	iRet = snd_pcm_hw_params(mHandle, hw_params);
	if (iRet < 0) {
		perror("snd_pcm_hw_params");
		exit(1);
	}

	
	 /* Use a buffer large enough to hold one period */
	snd_pcm_hw_params_get_period_size(hw_params, &mFrames, &dir);

	mFrameSize = mFrames * 2 * mChannels; /* 2 bytes/sample, 2 channels */
	mFrameBuffer = new char[mFrameSize];

	snd_pcm_hw_params_free (hw_params);

	return 0;


}

void SndPcm::exitSndPcm(void)
{
	delete mFrameBuffer;
	snd_pcm_drain(mHandle);
	snd_pcm_close(mHandle);
}

void SndPcm::writeSndPcm(char *pcFrameBuffer)
{
	int iRet;
	//9. 写音频数据到PCM设备
	iRet = snd_pcm_writei(mHandle, pcFrameBuffer, mFrames);
	if (iRet < 0) {
		if (iRet == -EPIPE) {
			  /* EPIPE means underrun */
			  fprintf(stderr, "underrun occurred\n");
			  //完成硬件参数设置，使设备准备好
			  snd_pcm_prepare(mHandle);
		}
		else if (iRet < 0) {
			  fprintf(stderr, "error from writei: %s\n", snd_strerror(iRet));
		}		
	}

}

snd_pcm_uframes_t& SndPcm::getFrames(void)
{
	return mFrames;
}

int SndPcm::getFrameSize(void)
{
	return mFrameSize;
}

int SndPcm::getSampleRate(void)
{
	return mSampleRate;
}

int SndPcm::getChannels(void)
{
	return mChannels;
}


char *SndPcm::getFrameBuffer(void)
{
	return mFrameBuffer;
}


