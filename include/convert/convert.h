
#ifndef __CONVERT_H__
#define __CONVERT_H__

class Convert;


#include <linux/videodev2.h>
#include <window/camerawindow.h>



class Convert {
public:
	void setStrName(const std::string& strName);
	std::string& getStrName(void);
    virtual int isSupport(int iPixelFormatIn, int iPixelFormatOut) = 0;
    virtual int convert(struct VideoBuf& tVideoBufIn, struct VideoBuf& tVideoBufOut) = 0;
    virtual int convertExit(struct VideoBuf& tVideoBufOut) = 0;
	
private:
	std::string mStrName;
};



class Mjpeg2rgb : public Convert {
public:	
    virtual int isSupport(int iPixelFormatIn, int iPixelFormatOut);
    virtual int convert(struct VideoBuf& tVideoBufIn, struct VideoBuf& tVideoBufOut);
    virtual int convertExit(struct VideoBuf& tVideoBufOut);	
};


class Rgb2rgb : public Convert {
public:	
    virtual int isSupport(int iPixelFormatIn, int iPixelFormatOut);
    virtual int convert(struct VideoBuf& tVideoBufIn, struct VideoBuf& tVideoBufOut);
    virtual int convertExit(struct VideoBuf& tVideoBufOut);	
};


class Yuv2rgb : public Convert {
public:	
	Yuv2rgb();
	~Yuv2rgb();
    virtual int isSupport(int iPixelFormatIn, int iPixelFormatOut);
    virtual int convert(struct VideoBuf& tVideoBufIn, struct VideoBuf& tVideoBufOut);
    virtual int convertExit(struct VideoBuf& tVideoBufOut);	
};

#endif /* __CONVERT_H__ */

