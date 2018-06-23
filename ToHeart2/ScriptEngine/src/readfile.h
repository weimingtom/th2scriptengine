#pragma once

#ifndef _LEAFLIB_CREADFILE
#define _LEAFLIB_CREADFILE

#include <vector>
using namespace std;

struct file_info{
	TCHAR	fname[31];
	char	bCompress;
	int		pack_size;
	int		seekPoint;
};

struct stream_info{
	int		num;
	int		seekPoint;
};

struct arc_file_info{
	HANDLE		handle;
	int			fileCount;
	TCHAR		dirName[24];
	file_info	*pack_file;
	vector<stream_info>	streamInfo;
	int					streamMax;
	arc_file_info()
	{
		streamMax = 0;
		pack_file = NULL;
	}
};

class ClReadFile
{
public:
	ClReadFile();
	~ClReadFile();
public:
	int OpenPackFile(LPCTSTR packName,int packNum=-1);
	int ReadPackFile(int arcFileNum, LPCTSTR decFile, char **fileBuf);
	int ReadPackFileNum(int arcFileNum, int num, char **fileBuf);
	int StreamOpenFile(int arcFileNum,LPCTSTR decFile,int &size);
	int StreamOpenFileNum(int arcFileNum,int num,int &size);
	ClResult StreamCloseFile(int arcFileNum,int streamNum);
	int StreamReadFile(int arcFileNum,int streamNum,char *read_buf,int size);
	ClResult StreamSeekFile(int arcFileNum,int streamNum,int seekSize,int moveMethod);
	ClResult ExtractFile(int srcFileNum, LPCTSTR decFile, LPCTSTR newFile);
	TCHAR *GetFileName(int arcFileNum,int num);
	TCHAR *GetArcDirName(int arcFileNum){return ArcFile[arcFileNum].dirName;}
	int DecodePackFile(int read_size, BYTE *readFile, char **fileBuf);
	int SearchFile(int arcFileNum, LPCTSTR decFile,BOOL errcheck=FALSE);
	int SequentialSearchFile(int arcFileNum, LPCTSTR decFile);
	int GetSeekPoint(int arcFileNum,int num)
	{ 
		return ArcFile[arcFileNum].pack_file[num].seekPoint; 
	}
private:
	vector<arc_file_info>	ArcFile;
	int						openFileNum;
};

int ReadPackFileFromMem(TCHAR *decFile,char *packMem,char **fileBuf);
extern ClReadFile	*readFile;

//#define _LEAFLIB_CREADFILE
#endif  _LEAFLIB_CREADFILE
