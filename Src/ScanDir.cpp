#include "GothicTools.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#endif

zVOID zCScanDir::ScanDirectory(const zSTRING &dir, const zSTRING &ext)
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
			ScanDirectory(path, ext);
		}
		else
		{
			if (ext == zSTR_EMPTY || cFileName.EndsWith(ext))
			{
				files.InsertEnd(path);
			}
		}
	}
	while(FindNextFile(h, &fd));
#else
#endif
}

zSTRING zCScanDir::Find(const zSTRING &s)
{
	zSTRING search = s;
	search.Upper();

	for (zINT i = 0; i < files.numInArray; i++)
	{
		zSTRING file = files[i];
		file.Upper();

		if (file.Contains(search)) return files[i];
	}

	return zSTR_EMPTY;
}
