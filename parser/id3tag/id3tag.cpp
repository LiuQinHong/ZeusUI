#include <id3tag/id3tag.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>


static T_FrameID g_tFrameID[] = {
	{"AENC", "Audio encryption", ID3_INFO_TYPE_TEXT},
	{"APIC", "Attached picture", ID3_INFO_TYPE_PIC},
	{"ASPI", "Audio seek point index", ID3_INFO_TYPE_TEXT},
	{"COMM", "Comments", ID3_INFO_TYPE_TEXT},
	{"COMR", "Commercial frame", ID3_INFO_TYPE_TEXT},
	{"ENCR", "Encryption method registration", ID3_INFO_TYPE_TEXT},
	{"EQU2", "Equalisation (2)", ID3_INFO_TYPE_TEXT},
	{"EQUA", "Equalization", ID3_INFO_TYPE_TEXT},
	{"ETCO", "Event timing codes", ID3_INFO_TYPE_TEXT},
	{"GEOB", "General encapsulated object", ID3_INFO_TYPE_TEXT},
	{"GRID", "Group identification registration", ID3_INFO_TYPE_TEXT},
	{"IPLS", "Involved people list", ID3_INFO_TYPE_TEXT},
	{"LINK", "Linked information", ID3_INFO_TYPE_TEXT},
	{"MCDI", "Music CD identifier", ID3_INFO_TYPE_TEXT},
	{"MLLT", "MPEG location lookup table", ID3_INFO_TYPE_TEXT},
	{"OWNE", "Ownership frame", ID3_INFO_TYPE_TEXT},
	{"PRIV", "Private frame", ID3_INFO_TYPE_TEXT},
	{"PCNT", "Play counter", ID3_INFO_TYPE_TEXT},
	{"POPM", "Popularimeter", ID3_INFO_TYPE_TEXT},
	{"POSS", "Position synchronisation frame", ID3_INFO_TYPE_TEXT},
	{"RBUF", "Recommended buffer size", ID3_INFO_TYPE_TEXT},
	{"RVA2", "Relative volume adjustment (2)", ID3_INFO_TYPE_TEXT},
	{"RVAD", "Relative volume adjustment", ID3_INFO_TYPE_TEXT},
	{"RVRB", "Reverb", ID3_INFO_TYPE_TEXT},
	{"SEEK", "Seek frame", ID3_INFO_TYPE_TEXT},
	{"SIGN", "Signature frame", ID3_INFO_TYPE_TEXT},
	{"SYLT", "Synchronized lyric/text", ID3_INFO_TYPE_TEXT},
	{"SYTC", "Synchronized tempo codes", ID3_INFO_TYPE_TEXT},
	{"TALB", "Album/Movie/Show title", ID3_INFO_TYPE_TEXT},
	{"TBPM", "BPM (beats per minute)", ID3_INFO_TYPE_TEXT},
	{"TCOM", "Composer", ID3_INFO_TYPE_TEXT},
	{"TCON", "Content type", ID3_INFO_TYPE_TEXT},
	{"TCOP", "Copyright message", ID3_INFO_TYPE_TEXT},
	{"TDAT", "Date", ID3_INFO_TYPE_TEXT},
	{"TDEN", "Encoding time", ID3_INFO_TYPE_TEXT},
	{"TDLY", "Playlist delay", ID3_INFO_TYPE_TEXT},
	{"TDOR", "Original release time", ID3_INFO_TYPE_TEXT},
	{"TDRC", "Recording time", ID3_INFO_TYPE_TEXT},
	{"TDRL", "Release time", ID3_INFO_TYPE_TEXT},
	{"TDTG", "Tagging time", ID3_INFO_TYPE_TEXT},
	{"TIPL", "Involved people list", ID3_INFO_TYPE_TEXT},
	{"TENC", "Encoded by", ID3_INFO_TYPE_TEXT},
	{"TEXT", "Lyricist/Text writer", ID3_INFO_TYPE_TEXT},
	{"TFLT", "File type", ID3_INFO_TYPE_TEXT},
	{"TIME", "Time", ID3_INFO_TYPE_TEXT},
	{"TIT1", "Content group description", ID3_INFO_TYPE_TEXT},
	{"TIT2", "Title/songname/content description", ID3_INFO_TYPE_TEXT},
	{"TIT3", "Subtitle/Description refinement", ID3_INFO_TYPE_TEXT},
	{"TKEY", "Initial key", ID3_INFO_TYPE_TEXT},
	{"TLAN", "Language(s)", ID3_INFO_TYPE_TEXT},
	{"TLEN", "Length", ID3_INFO_TYPE_TEXT},
	{"TMCL", "Musician credits list", ID3_INFO_TYPE_TEXT},
	{"TMED", "Media type", ID3_INFO_TYPE_TEXT},
	{"TMOO", "Mood", ID3_INFO_TYPE_TEXT},
	{"TOAL", "Original album/movie/show title", ID3_INFO_TYPE_TEXT},
	{"TOFN", "Original filename", ID3_INFO_TYPE_TEXT},
	{"TOLY", "Original lyricist(s)/text writer(s)", ID3_INFO_TYPE_TEXT},
	{"TOPE", "Original artist(s)/performer(s)", ID3_INFO_TYPE_TEXT},
	{"TORY", "Original release year", ID3_INFO_TYPE_TEXT},
	{"TOWN", "File owner/licensee", ID3_INFO_TYPE_TEXT},
	{"TPE1", "Lead performer(s)/Soloist(s)", ID3_INFO_TYPE_TEXT},
	{"TPE2", "Band/orchestra/accompaniment", ID3_INFO_TYPE_TEXT},
	{"TPE3", "Conductor/performer refinement", ID3_INFO_TYPE_TEXT},
	{"TPE4", "Interpreted, remixed, or otherwise modified by", ID3_INFO_TYPE_TEXT},
	{"TPOS", "Part of a set", ID3_INFO_TYPE_TEXT},
	{"TPRO", "Produced notice", ID3_INFO_TYPE_TEXT},
	{"TPUB", "Publisher", ID3_INFO_TYPE_TEXT},
	{"TRCK", "Track number/Position in set", ID3_INFO_TYPE_TEXT},
	{"TRDA", "Recording dates", ID3_INFO_TYPE_TEXT},
	{"TRSN", "Internet radio station name", ID3_INFO_TYPE_TEXT},
	{"TRSO", "Internet radio station owner", ID3_INFO_TYPE_TEXT},
	{"TSIZ", "Size", ID3_INFO_TYPE_TEXT},
	{"TSOA", "Album sort order", ID3_INFO_TYPE_TEXT},
	{"TSOP", "Performer sort order", ID3_INFO_TYPE_TEXT},
	{"TSOT", "Title sort order", ID3_INFO_TYPE_TEXT},
	{"TSRC", "ISRC (international standard recording code)", ID3_INFO_TYPE_TEXT},
	{"TSSE", "Software/Hardware and settings used for encoding", ID3_INFO_TYPE_TEXT},
	{"TSST", "Set subtitle", ID3_INFO_TYPE_TEXT},
	{"TXXX", "User defined text information", ID3_INFO_TYPE_TEXT},
	{"TYER", "Year", ID3_INFO_TYPE_TEXT},
	{"UFID", "Unique file identifier", ID3_INFO_TYPE_TEXT},
	{"USER", "Terms of use", ID3_INFO_TYPE_TEXT},
	{"USLT", "Unsynchronized lyric/text transcription", ID3_INFO_TYPE_TEXT},
	{"WCOM", "Commercial information", ID3_INFO_TYPE_TEXT},
	{"WCOP", "Copyright/Legal infromation", ID3_INFO_TYPE_TEXT},
	{"WOAF", "Official audio file webpage", ID3_INFO_TYPE_TEXT},
	{"WOAR", "Official artist/performer webpage", ID3_INFO_TYPE_TEXT},
	{"WOAS", "Official audio source webpage", ID3_INFO_TYPE_TEXT},
	{"WORS", "Official internet radio station homepage", ID3_INFO_TYPE_TEXT},
	{"WPAY", "Payment", ID3_INFO_TYPE_TEXT},
	{"WPUB", "Official publisher webpage", ID3_INFO_TYPE_TEXT},
	{"WXXX", "User defined URL link", ID3_INFO_TYPE_TEXT}

};


int Id3Tag::id3TagOpen(const std::string& strFileName)
{

	mStrFileName = strFileName;
	mId3File = id3_file_open(strFileName.c_str(), ID3_FILE_MODE_READONLY);
	if (mId3File == NULL) {
		printf("id3_file_open for %s error!\n", strFileName.c_str());
		return -1;
	}


	mId3Tag = id3_file_tag(mId3File);
	if (mId3Tag == NULL) {
		printf("%s no tag\n", strFileName.c_str());
		return -1;
	}

	return 0;
}

void Id3Tag::id3TagClose(void)
{
	id3_file_close(mId3File);
	for(std::list<struct id3_Info*>::iterator id3_Info_iter = id3_Info_list.begin(); id3_Info_iter!= id3_Info_list.end(); ++id3_Info_iter) {
		delete *id3_Info_iter;
	}
}

void Id3Tag::addId3Info(struct id3_Info *ptId3Info)
{	
	id3_Info_list.push_back(ptId3Info);
}


std::list<struct id3_Info *>& Id3Tag::parser(void)
{
	struct id3_frame const *frame;
	union id3_field const *field;
	enum id3_field_textencoding textencoding = ID3_FIELD_TEXTENCODING_ISO_8859_1;
	struct id3_Info *id3Info;
	unsigned int iSum = sizeof(g_tFrameID) / sizeof(g_tFrameID[0]);

	for (unsigned int j = 0; j < iSum; j++) {
		frame = id3_tag_findframe(mId3Tag, g_tFrameID[j].frameID, 0);
		if (frame == NULL) {
			continue;
		}

		id3Info = new id3_Info();

		id3Info->ptFrameID	= &g_tFrameID[j];
		id3Info->data		= NULL;
		id3Info->length		= 0;
		
		for (unsigned int i = 0; i < frame->nfields; i++) {
			field = id3_frame_field(frame, i);
			if (field == NULL) {
				continue;
			}
		
			switch (field->type) {
				case ID3_FIELD_TYPE_TEXTENCODING:
				{
					textencoding = id3_field_gettextencoding(field);
					id3Info->textencoding = textencoding;
					break;
				}
				case ID3_FIELD_TYPE_LATIN1:
				{
					id3_latin1_t const *latin1;
					latin1 = id3_field_getlatin1(field);

					if (g_tFrameID[j].type == ID3_INFO_TYPE_PIC) {
						if (!strcmp("image/jpeg", (const char*)latin1) || !strcmp("image/jpg", (const char*)latin1)) {
							/* jpg */
							id3Info->picType = PIC_TYPE_JPG;
						}
						else if (!strcmp("image/png", (const char*)latin1)) {
							/* png */
							id3Info->picType = PIC_TYPE_PNG;
						}
						else if (!strcmp("image/bmp", (const char*)latin1)) {
							/* bmp */
							id3Info->picType = PIC_TYPE_BMP;
						}

					}
					else if (g_tFrameID[j].type == ID3_INFO_TYPE_TEXT) {
						id3Info->length = strlen((const char *)latin1);
						id3Info->data = new unsigned char[id3Info->length + 1];					
						strncpy((char *)id3Info->data, (char *)latin1, id3Info->length);
						id3Info->data[id3Info->length] = '\0';
					}
					
					break;
				}
				case ID3_FIELD_TYPE_STRINGFULL:
				{
					id3_ucs4_t const *ucs4;
					ucs4 = id3_field_getfullstring(field);
					
					switch (textencoding) {
						case ID3_FIELD_TEXTENCODING_ISO_8859_1:
						{							
							id3_latin1_t *latin1;
							latin1 = id3_ucs4_latin1duplicate(ucs4);
							if (latin1 == NULL)
								break;
							
							id3Info->length = strlen("163 key(Don't modify)");//strlen((const char *)latin1);
							id3Info->data = new unsigned char[id3Info->length + 1];
							strncpy((char *)id3Info->data, (char *)latin1, id3Info->length);
							id3Info->data[id3Info->length] = '\0';

							free(latin1);
							break;
						}
						case ID3_FIELD_TEXTENCODING_UTF_16:
						{							
							id3_utf16_t *utf16;
							utf16 = id3_ucs4_utf16duplicate(ucs4);
							if (utf16 == NULL)
								break;

							id3Info->length = strlen((const char *)utf16);					
							id3Info->data = new unsigned char[id3Info->length + 1];
							strncpy((char *)id3Info->data, (char *)utf16, id3Info->length);
							id3Info->data[id3Info->length] = '\0';

							free(utf16);
							break;
						}
						case ID3_FIELD_TEXTENCODING_UTF_16BE:
						{
							id3_utf16_t *utf16;
							utf16 = id3_ucs4_utf16duplicate(ucs4);
							if (utf16 == NULL)
								break;
							
							id3Info->length = strlen((const char *)utf16);					
							id3Info->data = new unsigned char[id3Info->length + 1];
							strncpy((char *)id3Info->data, (char *)utf16, id3Info->length);
							id3Info->data[id3Info->length] = '\0';

							free(utf16);
							break;
						}
						case ID3_FIELD_TEXTENCODING_UTF_8:
						{
							id3_utf8_t *utf8;
							utf8 = id3_ucs4_utf8duplicate(ucs4);
							if (utf8 == NULL)
								break;
					
							id3Info->length = strlen((char *)utf8);					
							id3Info->data = new unsigned char[id3Info->length + 1];
							strncpy((char *)id3Info->data, (char *)utf8, id3Info->length);
							id3Info->data[id3Info->length] = '\0';

							free(utf8);

							break;
						}
						
					}

					
					break;
				}
				case ID3_FIELD_TYPE_STRINGLIST:
				{
					unsigned int nstrings;
					id3_ucs4_t const *ucs4;
					
					nstrings = id3_field_getnstrings(field);
					for (unsigned int j = 0; j < nstrings; ++j) {
						ucs4 = id3_field_getstrings(field, j);
						assert(ucs4);


						switch (textencoding) {
							case ID3_FIELD_TEXTENCODING_ISO_8859_1:
							{							
								id3_latin1_t *latin1;
								latin1 = id3_ucs4_latin1duplicate(ucs4);
								if (latin1 == NULL)
									break;
								
								id3Info->length = strlen("163 key(Don't modify)");//strlen((const char *)latin1);
								id3Info->data = new unsigned char[id3Info->length + 1];
								strncpy((char *)id3Info->data, (char *)latin1, id3Info->length);
								id3Info->data[id3Info->length] = '\0';
						
								free(latin1);
								break;
							}
							case ID3_FIELD_TEXTENCODING_UTF_16:
							{							
								id3_utf16_t *utf16;
								utf16 = id3_ucs4_utf16duplicate(ucs4);
								if (utf16 == NULL)
									break;
						
								id3Info->length = strlen((const char *)utf16);					
								id3Info->data = new unsigned char[id3Info->length + 1];
								strncpy((char *)id3Info->data, (char *)utf16, id3Info->length);
								id3Info->data[id3Info->length] = '\0';
						
								free(utf16);
								break;
							}
							case ID3_FIELD_TEXTENCODING_UTF_16BE:
							{
								id3_utf16_t *utf16;
								utf16 = id3_ucs4_utf16duplicate(ucs4);
								if (utf16 == NULL)
									break;
								
								id3Info->length = strlen((const char *)utf16);					
								id3Info->data = new unsigned char[id3Info->length + 1];
								strncpy((char *)id3Info->data, (char *)utf16, id3Info->length);
								id3Info->data[id3Info->length] = '\0';
						
								free(utf16);
								break;
							}
							case ID3_FIELD_TEXTENCODING_UTF_8:
							{
								id3_utf8_t *utf8;
								utf8 = id3_ucs4_utf8duplicate(ucs4);
								if (utf8 == NULL)
									break;
						
								id3Info->length = strlen((char *)utf8); 				
								id3Info->data = new unsigned char[id3Info->length + 1];
								strncpy((char *)id3Info->data, (char *)utf8, id3Info->length);
								id3Info->data[id3Info->length] = '\0';
						
								free(utf8);						
								break;
							}
							
						}
						
					}

					break;
				}
				case ID3_FIELD_TYPE_BINARYDATA:
				{					
					id3_byte_t const *data;
					id3_length_t length;
					data = id3_field_getbinarydata(field, &length);
					if (data == NULL) {
						printf("no data!\n");
						break;
					}

					id3Info->length = length;					
					id3Info->data = new unsigned char[id3Info->length];
					memcpy((char *)id3Info->data, (char *)data, id3Info->length);
					
					break;
				}
				default:
				{
					
				}
				
			}
			
		}

		addId3Info(id3Info);
		
	}	


	return id3_Info_list;
}




