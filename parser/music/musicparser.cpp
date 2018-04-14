#include <music/musicparser.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>

MusicParser::MusicParser(){}

MusicParser::MusicParser(const std::string strFileName)
:mStrFileName(strFileName)
{

}


MusicParser::~MusicParser()
{

}

std::string& MusicParser::getFileName(void)
{
	return mStrFileName;
}

