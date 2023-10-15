#ifdef _WIN32
#include <direct.h>
#else
#include <limits.h>
#define _MAX_PATH PATH_MAX
#define _mkdir(path) mkdir(path, S_IRWXU)
#endif

#include "Disk.h"

int mkdir_p(const char *path)
{
	char _path[_MAX_PATH];
	char *p;

	errno = 0;

	if (strlen(path) + 1 > _MAX_PATH)
	{
		errno = ENAMETOOLONG;
		return -1;
	}

	strcpy(_path, path);

	for (p = _path + 1; *p; p++)
	{
		if (*p == '/')
		{
			*p = '\0';

			if (_mkdir(_path) != 0)
			{
				if (errno != EEXIST && (errno == EACCES && !strchr(_path, ':')))
				{
					return -1;
				}
			}

			*p = '/';
		}
	}

	if (_mkdir(_path) != 0)
	{
		if (errno != EEXIST)
		{
			return -1;
		}
	}

	return 0;
}

FILE *fopen_r(const char *complete_path, const char *folder_path, const char *mode)
{
	if (mkdir_p(folder_path))
	{
		printf("Couldn't create directory %s\n", folder_path);

		return NULL;
	}

	return fopen(complete_path, mode);
}

zFILE::zFILE(const zSTRING &filepath, bool32 w)
{
	complete_path = filepath;
	unixify_path(complete_path);

	size_t dotPos = complete_path.rfind('.');
	size_t namePos = complete_path.rfind('/') + 1;

	extension = complete_path.substr(dotPos + 1, complete_path.size() - dotPos).c_str();
	filename = complete_path.substr(namePos, (complete_path.size() - namePos) - (complete_path.size() - dotPos)).c_str();
	basename = (filename + zSTRING(".") + extension).c_str();
	if (namePos) path = complete_path.substr(0, namePos - 1).c_str();

	write = w;

	const char *mode = (write) ? "wb" : "rb";

	if (write && path.Length()) file_handle = fopen_r(complete_path.ToChar(), path.ToChar(), mode);
	else file_handle = fopen(complete_path.ToChar(), mode);

	if (!file_handle)
	{
		printf("Could not open %s, mode %s\n", complete_path.ToChar(), mode);
		exit(zERROR);
	}

	struct stat st;
	stat(complete_path.ToChar(), &st);
	size = st.st_size;
}

void zFILE::ReadLine(zSTRING &s, bool32 trimTabs)
{
	//printf("ReadLine pos: %d\n", Pos());

	char file_buffer[zFILE_MAXCHARS + 1];
	bool32 finished = FALSE;
	s.Clear();

	do
	{
		if (fgets(file_buffer, zFILE_MAXCHARS, file_handle) == NULL)
		{
			finished = TRUE;
		}
		else
		{
			//printf("file_buffer: %s\n", file_buffer);
			s += zSTRING(file_buffer);

			while (s.Length() > 0 && (s.GetLastChar() == '\n' || s.GetLastChar() == '\r'))
			{
				if (s.GetLastChar() == '\n')
				{
					finished = TRUE;
				}

				s.DeleteRight(1);
			}
		}
	}
	while (!finished);

	//printf("ReadResult pos: %d, %s\n", Pos(), s.ToChar());

	if (trimTabs)
	{
		s.TrimLeft('\t');
	}
}

char *fgets_z(char *_Buffer, int _MaxCount, FILE *_Stream)
{
	int _Char;
	char *_Backup = _Buffer;

	for (_MaxCount--; _MaxCount > 0; _MaxCount--)
	{
		_Char = fgetc(_Stream);

		if (_Char == EOF)
		{
			*(_Buffer++) = '\0';

			break;
		}

		*(_Buffer++) = (char)_Char;

		if (_MaxCount == 1)
		{
			*_Buffer = '\0';
		}

		if (_Char == '\0')
		{
			break;
		}
	}

	return _Backup;
}

void zFILE::Read(zSTRING &s, bool32 trimTabs)
{
	char file_buffer[zFILE_MAXCHARS];
	s.Clear();

	s += zSTRING(fgets_z(file_buffer, zFILE_MAXCHARS, file_handle));

	if (trimTabs)
	{
		s.TrimLeft('\t');
	}
}

void zFILE::WriteLine(const zSTRING &s, bool32 crLf)
{
	fputs(s.ToChar(), file_handle);

	if (crLf) fputc('\r', file_handle);
	fputc('\n', file_handle);
}

void zFILE::WriteLineIndented(uint32 numTabs, const zSTRING &s, bool32 crLf)
{
	for (uint32 i = 0; i < numTabs; i++)
	{
		fputc('\t', file_handle);
	}

	WriteLine(s, crLf);
}

zCFileBIN::zCFileBIN(const zSTRING &fname, bool32 write)
{
	externalFile = FALSE;
	file = zNEW(zFILE)(fname, write);

	Init();
}

zCFileBIN::zCFileBIN(zFILE *inFile)
{
	externalFile = TRUE;
	file = inFile;

	Init();
}

zCFileBIN::~zCFileBIN()
{
	if (!externalFile) zDELETE(file);
}

void zCFileBIN::Init()
{
	lastStart = -1;
	nextStart = -1;
}

void zCFileBIN::BinOpenChunk(uint16 &id, int32 &len)
{
	file->Read(&id, sizeof(id));

	if (file->Eof())
	{
		id = 0;
		return;
	}

	file->Read(&len, sizeof(len));

	nextStart = file->Pos() + len;
}

void zCFileBIN::BinSkipChunk()
{
	if (nextStart >= file->Size())
	{
		return;
	}

	file->Seek(nextStart);
}

void zCFileBIN::BinStartChunk(const uint16 id)
{
	BinEndChunk();

	int32 len = 0;
	lastStart = file->Pos();
	file->Write(&id, sizeof(id));
	file->Write(&len, sizeof(len));
}

void zCFileBIN::BinEndChunk()
{
	if (lastStart != -1)
	{
		int32 actPos = file->Pos();
		int32 len = actPos - lastStart - 6;

		file->Seek(lastStart + sizeof(uint16));
		file->Write(&len, sizeof(len));
		file->Seek(actPos);
	}
}

struct file_time_info
{
	int64_t atime;
	int64_t mtime;
};

void get_file_time_info(file_time_info *ti, char *filepath)
{
	struct stat st;

	stat(filepath, &st);

	ti->atime = st.st_atime;
	ti->mtime = st.st_mtime;
}

bool32 zDATE::IsFileNewer(const zSTRING &one, const zSTRING &two)
{
	file_time_info t1, t2;

	get_file_time_info(&t1, one.ToChar());
	get_file_time_info(&t2, two.ToChar());

	if (t2.mtime > t1.mtime) return TRUE;

	return FALSE;
}

void zDATE::SetTime(zFILE &file)
{
	file_time_info ti;

	get_file_time_info(&ti, file.complete_path.ToChar());

	ti.mtime += 1 * 60 * 60; // hack because Gothic seems to be one hour off with timestamps

	tm *_tm = localtime(&ti.mtime);

	year = _tm->tm_year + 1900;
	month = _tm->tm_mon + 1;
	day = _tm->tm_mday;
	hour = _tm->tm_hour;
	minute = _tm->tm_min;
	second = _tm->tm_sec;
}
