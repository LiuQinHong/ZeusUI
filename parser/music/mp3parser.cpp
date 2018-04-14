#include <music/mp3parser.h>

#include <device/sndpcm.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <mad.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include <sys/wait.h>
#include <id3tag/id3tag.h>


static enum mad_flow output(void *data, struct mad_header const *header, struct mad_pcm *pcm);
static enum mad_flow input(void *data, struct mad_stream *stream);
static enum mad_flow error(void *data, struct mad_stream *stream, struct mad_frame *frame);
static enum mad_flow message(void *data, void *cmd, unsigned int *index);


Mp3Parser::Mp3Parser()
{

}

Mp3Parser::Mp3Parser(const std::string& strFileName)
:MusicParser(strFileName)
{

	mPriData.mp3parser			= this;
	mPriData.pcFrameBuffer		= NULL;
	mPriData.isOpen				= false;
	mPriData.strFileName 		= strFileName;

	mIsDecoded = false;
	mIsStop = false;
	mIsInited = false;


	/* configure input, output, and error functions */
	mad_decoder_init(&mDecoder, &mPriData,
	input, 0 /* header */, 0 /* filter */, output,
	error, message /* message */);
}

Mp3Parser::~Mp3Parser()
{
}

bool Mp3Parser::isInited(void)
{
	return mIsInited;
}

void Mp3Parser::setInited(bool isInited)
{
	mIsInited = isInited;
}

bool Mp3Parser::isStop(void)
{
	return mIsStop;
}

void Mp3Parser::setStop(bool isStop)
{
	mIsStop = isStop;
}



static enum mad_flow message(void *data, void *cmd, unsigned int *index)
{
	return MAD_FLOW_STOP;	
}


static enum mad_flow error(void *data, struct mad_stream *stream, struct mad_frame *frame)
{
	fprintf(stderr, "decoding error 0x%04x (%s)\n",	stream->error, mad_stream_errorstr(stream));

	/* return MAD_FLOW_BREAK here to stop decoding (and propagate an error) */
	return MAD_FLOW_CONTINUE;
}


static enum mad_flow input(void *data, struct mad_stream *stream)
{
	struct PriData *ptPriData = (struct PriData *)data;
	int iLen;
	int iRet;

	
	if (ptPriData->mp3parser->isStop()) {
		close(ptPriData->iFd);
		delete ptPriData->data;
		return MAD_FLOW_STOP;
	}


	if (!ptPriData->isOpen) {
		struct ID3V2_Head tID3V2_Head;
		int iFd;

		iFd = open(ptPriData->strFileName.c_str(), O_RDONLY);
		if (iFd < 0) {
			printf("open %s error!\n", ptPriData->strFileName.c_str());
			return MAD_FLOW_STOP;	
		}
		read(iFd, &tID3V2_Head, 10);
		unsigned long iLen = (tID3V2_Head.acSize[0]&0x7f)*0x200000
		  +(tID3V2_Head.acSize[1]&0x7f)*0x4000
		  +(tID3V2_Head.acSize[2]&0x7f)*0x80
		  +(tID3V2_Head.acSize[3]&0x7f) + 10; // 10是标签本身的长度
		
		lseek(iFd, iLen, SEEK_SET);
		ptPriData->data	= new char[MPEG_BUFSZ];
		ptPriData->length = 0;		
		ptPriData->isOpen = true;		
		ptPriData->iFd = iFd;
	}


	if (stream->next_frame) {
		ptPriData->length = &ptPriData->data[ptPriData->length] - (char*)stream->next_frame;
		memmove(ptPriData->data, stream->next_frame, ptPriData->length);
	}

	iLen = MPEG_BUFSZ - ptPriData->length;

	iRet = read(ptPriData->iFd, ptPriData->data + ptPriData->length, iLen);
	if (iRet < iLen) {
		iLen = iRet;
		ptPriData->mp3parser->setStop(true);
	}
	ptPriData->length += iLen;


	mad_stream_buffer(stream, (const unsigned char*)ptPriData->data, ptPriData->length);

	return MAD_FLOW_CONTINUE;

}


static inline signed int scale(mad_fixed_t sample)
{
	/* round */
	sample += (1L << (MAD_F_FRACBITS - 16));


	/* clip */
	if (sample >= MAD_F_ONE)
		sample = MAD_F_ONE - 1;
	else if (sample < -MAD_F_ONE)
		sample = -MAD_F_ONE;

	/* quantize */
	return sample >> (MAD_F_FRACBITS + 1 - 16);
}


static enum mad_flow output(void *data, struct mad_header const *header, struct mad_pcm *pcm)
{
	struct PriData *ptPriData = (struct PriData *)data;
	unsigned int nchannels;
	long int nsamples,samplerate;
	mad_fixed_t const *left_ch, *right_ch;
	static int i=0;
	char buf;
	/* pcm->samplerate contains the sampling frequency */


	nchannels = pcm->channels;
	nsamples  = pcm->length;	/* 这个不是采样位，而一帧的数据长度12*3(采样)*32(子带)=1152*/
	left_ch   = pcm->samples[0];
	right_ch  = pcm->samples[1];
	samplerate= pcm->samplerate;


	if(!ptPriData->mp3parser->isInited()) {
		SndPcm::getSndPcm()->initSndPcm(samplerate, nchannels);
		ptPriData->pcFrameBuffer = SndPcm::getSndPcm()->getFrameBuffer();
		ptPriData->mp3parser->setInited(true);
	}

	while (nsamples--) {
		signed int sample;

		/* output sample(s) in 16-bit signed little-endian PCM */
		sample = scale(*left_ch++);

		buf=(sample >> 0) & 0xff;
		ptPriData->pcFrameBuffer[i] = buf;
		i++;

		buf=(sample >> 8) & 0xff;
		ptPriData->pcFrameBuffer[i] = buf;
		i++;

		if (nchannels == 2) {
			sample = scale(*right_ch++);

			buf=(sample >> 0) & 0xff;
			ptPriData->pcFrameBuffer[i] = buf;
			i++;

			buf=(sample >> 8) & 0xff;
			ptPriData->pcFrameBuffer[i] = buf;
			i++;

		}
		
		if(i == SndPcm::getSndPcm()->getFrameSize()) {
			i = 0;
			SndPcm::getSndPcm()->writeSndPcm(ptPriData->pcFrameBuffer);
		}


	}

	return MAD_FLOW_CONTINUE;
}


void Mp3Parser::play(void)
{
	if (!mIsDecoded) {
		/* start decoding */
		mad_decoder_run(&mDecoder, MAD_DECODER_MODE_ASYNC);//MAD_DECODER_MODE_SYNC, MAD_DECODER_MODE_ASYNC
		mIsDecoded = true;
	}

}

void Mp3Parser::suspend(void)
{
	
}

void Mp3Parser::stop(void)
{
	unsigned int i = 1;
	int iStatus;
	int ret;


	/* WNOHANG 若pid指定的子进程没有结束，则waitpid()函数返回0，不予以等待。若结束，则返回该子进程的ID	 */
	ret = waitpid(mDecoder.async.pid, &iStatus, WNOHANG);

	if (ret == 0) {
		mad_decoder_message(&mDecoder, &mPriData, &i);		
	}

	/* release the decoder */
	mad_decoder_finish(&mDecoder);
	
}


