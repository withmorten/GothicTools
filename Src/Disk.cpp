#include "GothicTools.h"

#ifdef _WIN32
#include <direct.h>
#else
#include <limits.h>
#define _MAX_PATH PATH_MAX
#define _mkdir(path) mkdir(path, S_IRWXU)
#endif

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

zFILE::zFILE(const zSTRING &filepath, zBOOL w)
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

void zFILE::ReadLine(zSTRING &s)
{
	char file_buffer[zFILE_MAXCHARS + 1];
	zBOOL finished = FALSE;
	s.Clear();

	do
	{
		if (fgets(file_buffer, zFILE_MAXCHARS, file_handle) == NULL)
		{
			finished = TRUE;
		}
		else
		{
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
}

inline char *fgets_z(char *_Buffer, int _MaxCount, FILE *_Stream)
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

void zFILE::Read(zSTRING &s)
{
	char file_buffer[zFILE_MAXCHARS];
	s.Clear();

	s += zSTRING(fgets_z(file_buffer, zFILE_MAXCHARS, file_handle));
}

void zFILE::WriteLine(const zSTRING &s, zBOOL crLf)
{
	fputs(s.ToChar(), file_handle);

	if (crLf) fputc('\r', file_handle);
	fputc('\n', file_handle);
}

void zFILE::WriteLineIndented(zUINT numTabs, const zSTRING &s, zBOOL crLf)
{
	for (zUINT i = 0; i < numTabs; i++)
	{
		fputc('\t', file_handle);
	}

	WriteLine(s, crLf);
}

zCFileBIN::zCFileBIN(const zSTRING &fname, zBOOL write)
{

	file = zNEW(zFILE)(fname, write);
	lastStart = -1;
	nextStart = -1;
}

void zCFileBIN::BinOpenChunk(uint16 &id, zLONG &len)
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

	zINT len = 0;
	lastStart = file->Pos();
	file->Write(&id, sizeof(id));
	file->Write(&len, sizeof(len));
}

void zCFileBIN::BinEndChunk()
{
	if (lastStart != -1)
	{
		zINT actPos = file->Pos();
		zINT len = actPos - lastStart - 6;

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

zBOOL zDATE::IsFileNewer(const zSTRING &one, const zSTRING &two)
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
