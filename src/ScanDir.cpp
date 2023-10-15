#include "ScanDir.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <errno.h>
#include <fts.h>
#endif

void zCScanDir::ScanDirectory(const zSTRING &dir)
{
#ifdef _WIN32
	WIN32_FIND_DATA fd;
	HANDLE h = INVALID_HANDLE_VALUE;

	zSTRING path = dir + "/*.*";
	unixify_path(path);

	h = FindFirstFile(path.ToChar(), &fd);

	if (h == INVALID_HANDLE_VALUE)
	{
		printf("Could not open %s\n", path.ToChar());
		exit(zERROR);
	}

	do
	{
		zSTRING cFileName(fd.cFileName);

		if (cFileName == "." || cFileName == "..") continue;

		path = dir + "/" + cFileName;
		unixify_path(path);

		cFileName.Upper();

		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			ScanDirectory(path);
		}
		else
		{
			files.InsertEnd(path);
		}
	}
	while(FindNextFile(h, &fd));
#else
	FTS *fts = NULL;
	FTSENT *ent = NULL;

	char *fts_argv[2];

	fts_argv[0] = dir.ToChar();
	fts_argv[1] = NULL;

	fts = fts_open((char * const *)fts_argv, FTS_PHYSICAL | FTS_NOCHDIR, NULL);

	if (errno)
	{
		printf("Could not open %s\n", dir.ToChar());
	}

	while ((ent = fts_read(fts)) != NULL)
	{
		// No errno checking because fts_read adds random backslashes into pathnames, resulting in "No such dir"
		// fopen() will fail later on if the file cannot actually be opened and the program will exit
		if (ent->fts_info & FTS_F)
		{
			zSTRING path = ent->fts_path;
			unixify_path(path);

			files.InsertEnd(path);
		}
	}

	fts_close(fts);
#endif
}

zSTRING zCScanDir::Find(const zSTRING &s)
{
	zSTRING search = s;
	search.Upper();

	for (int32 i = 0; i < files.numInArray; i++)
	{
		zSTRING file = files[i];
		file.Upper();

		if (file.Contains(search)) return files[i];
	}

	return zSTR_EMPTY;
}
