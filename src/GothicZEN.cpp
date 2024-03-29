#include "GothicTools.h"

#include "Archiver.h"
#include "World.h"

enum
{
	ARG_PROG_NAME,
	ARG_FILE_IN,
	ARG_VERSION_IN,
	ARG_OPTIONAL_FILE_OUT,
	ARG_OPTIONAL_VERSION_OUT,
	ARG_OPTIONAL_0,
	ARG_OPTIONAL_1,
	ARG_OPTIONAL_2,
	ARG_NUM,
};

int main(int argc, const char **argv)
{
#ifdef _CRTDBG_MAP_ALLOC
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
#endif

	if (argc < ARG_OPTIONAL_FILE_OUT)
	{
		printf("GothicZEN v0.5 by withmorten\n\n");
		printf("GothicZEN can convert a compiled ZEN to another Gothic version, or remove LOD polygons when saving to the same version\n\n");
		printf("usage:\n");
		printf("GothicZEN <filename in> <version in> <filename out> <version out> [xzen] [nodelod] [decompile]\n\n");
		printf("version in/out can be 101 (1.01e), 108 (1.08k) or 130 (1.30) / 260 (2.6f)\n\n");
		printf("GothicZEN works with compiled (ASCII) and uncompiled ZENs\n");
		printf("GothicZEN keeps existing lightmaps, but after converting a ZEN you should \"Compile Light\" in the Spacer just in case\n\n");
		printf("for 1.08k, \"xzen\" can be appended to save mesh as GRM mesh format which uses 32bit vertex indices, but requires GRMFixes_DLL to load ingame\n\n");
		printf("\"decompile\" will strip the binary meshAndTree chunk and save just the ASCII data\n\n");
		printf("1.01e and 1.08k outdoor meshes contain LOD polygons - GothicZEN will automatically remove these when converting to any version\n");
		printf("\"nodelod\" can be appended to keep these LOD polygons\n\n");
		printf("the order for these two optional arguments does not matter\n\n");
		printf("1.08k ZENs are fully compatible with 1.04d (104) and 1.12f (112) (and any other Gothic version between those two versions)\n");
		printf("as long as you don't use XZENs ...\n");

		return zOK;
	}

	if (argc > ARG_OPTIONAL_0)
	{
		if (!stricmp(argv[ARG_OPTIONAL_0], "xzen")) xZenOut = TRUE;
		if (!stricmp(argv[ARG_OPTIONAL_0], "decompile")) decompile = TRUE;

		if (argc > ARG_OPTIONAL_1)
		{
			if (!stricmp(argv[ARG_OPTIONAL_1], "xzen")) xZenOut = TRUE;
			if (!stricmp(argv[ARG_OPTIONAL_1], "decompile")) decompile = TRUE;

			if (argc > ARG_OPTIONAL_2)
			{
				if (!stricmp(argv[ARG_OPTIONAL_2], "xzen")) xZenOut = TRUE;
				if (!stricmp(argv[ARG_OPTIONAL_2], "decompile")) decompile = TRUE;
			}
		}
	}

	if (!SetInVersions(atoi(argv[ARG_VERSION_IN])))
	{
		return zERROR;
	}

	if (argc > ARG_OPTIONAL_VERSION_OUT)
	{
		if (!SetOutVersions(atoi(argv[ARG_OPTIONAL_VERSION_OUT])))
		{
			return zERROR;
		}
	}

	if (argc > ARG_OPTIONAL_0)
	{
		if (!stricmp(argv[ARG_OPTIONAL_0], "nodelod")) deLOD = FALSE;

		if (argc > ARG_OPTIONAL_1)
		{
			if (!stricmp(argv[ARG_OPTIONAL_1], "nodelod")) deLOD = FALSE;

			if (argc > ARG_OPTIONAL_2)
			{
				if (!stricmp(argv[ARG_OPTIONAL_2], "nodelod")) deLOD = FALSE;
			}
		}
	}

	zCWorld *world = zNEW(zCWorld);

	zCArchiver arc;
	arc.OpenFile(argv[ARG_FILE_IN]);

	if (arc.ReadHeader() && world->LoadZEN(arc))
	{
		arc.CloseFile();

		if (argc > ARG_OPTIONAL_VERSION_OUT)
		{
			arc.OpenFile(argv[ARG_OPTIONAL_FILE_OUT], TRUE);
			arc.mode = zARC_MODE_ASCII;
			arc.WriteHeader();

			world->SaveZEN(arc);

			arc.CloseFile();

			printf("Zen saved\n");
		}
		else
		{
			printf("Zen OK\n");
		}
	}
	else
	{
		printf("Zen could not be loaded\n");

		return zERROR;
	}

	zDELETE(world);

	return zOK;
}
