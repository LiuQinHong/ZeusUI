#ifndef __ID3TAG_H__
#define __ID3TAG_H__

/* 头文件做前置声明防止互相包含产生未定义错误 */
class Id3Tag;

#include <id3tag.h>
#include <iostream>
#include <pic/picparser.h>
#include <list>


struct id3_file {
  FILE *iofile;
  enum id3_file_mode mode;
  char *path;

  int flags;

  struct id3_tag *primary;

  unsigned int ntags;
  struct filetag *tags;
};


enum id3_info_type {
  ID3_INFO_TYPE_PIC		= 0x00,
  ID3_INFO_TYPE_TEXT	= 0x01,
};

enum pic_type {
  PIC_TYPE_JPG		= 0X00,
  PIC_TYPE_PNG		= 0X01,
  PIC_TYPE_BMP		= 0X02
};


typedef struct tFrameID {
	const char *frameID;
	const char *Desc;
	enum id3_info_type type;	
}T_FrameID, *PT_FrameID;



struct id3_Info{
	enum pic_type picType;
	enum id3_field_textencoding textencoding;
	PT_FrameID ptFrameID;
	unsigned char *data;
	unsigned int length;
};



struct ID3V2_Head {
	unsigned char acFileID[3];
	unsigned char version[2];
	unsigned char flag[1];
	unsigned char acSize[4];
};


class Id3Tag {
public:	
	int id3TagOpen(const std::string& strFileName);
	void id3TagClose(void);

	std::list<struct id3_Info *>& parser(void);
	void addId3Info(struct id3_Info *ptId3Info);

	
private:
	std::string mStrFileName;
	std::list<struct id3_Info *> id3_Info_list;
	struct id3_file *mId3File;
	struct id3_tag *mId3Tag;
};



#endif //__ID3TAG_H__







