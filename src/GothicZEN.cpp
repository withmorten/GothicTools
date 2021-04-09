#include "GothicTools.h"

int main(int argc, const char **argv)
{
	if (argc < 4)
	{
		printf("GothicZEN v0.4 by withmorten\n\n");
		printf("GothicZEN can convert a compiled ZEN to another Gothic version, or remove LOD polygons when saving to the same version\n\n");
		printf("usage:\n");
		printf("GothicZEN <version in> <version out> <filename in> <filename out> [xzen] [nodelod] [uncompiled]\n\n");
		printf("version in/out can be 101 (1.01e), 108 (1.08k) or 130 (1.30) / 26 (2.6f)\n\n");
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

	if (argc > 5)
	{
		if (!stricmp(argv[5], "xzen")) xZenOut = TRUE;
		if (!stricmp(argv[5], "decompile")) deCompile = TRUE;

		if (argc > 6)
		{
			if (!stricmp(argv[6], "xzen")) xZenOut = TRUE;
			if (!stricmp(argv[6], "decompile")) deCompile = TRUE;

			if (argc > 7)
			{
				if (!stricmp(argv[7], "xzen")) xZenOut = TRUE;
				if (!stricmp(argv[7], "decompile")) deCompile = TRUE;
			}
		}
	}

	switch (atoi(argv[1])) // version in
	{
	case 101:
		meshAndBspVersionIn = BSPMESH_VERSION_GOTHIC_1_01;
		meshVersionIn = MESH_VERSION_GOTHIC_1_01;
		bspVersionIn = BSP_VERSION_GOTHIC_1_01;
		materialVersionIn = MATERIAL_VERSION_SUM_GOTHIC_1_01;

		deLod = TRUE;

		break;
	case 104:
	case 108:
	case 112:
		meshAndBspVersionIn = BSPMESH_VERSION_GOTHIC_1_04;
		meshVersionIn = MESH_VERSION_GOTHIC_1_04;
		bspVersionIn = BSP_VERSION_GOTHIC_1_04;
		materialVersionIn = MATERIAL_VERSION_SUM_GOTHIC_1_04;

		deLod = TRUE;

		break;
	case 130:
	case 26:
		meshAndBspVersionIn = BSPMESH_VERSION_GOTHIC_1_30;
		meshVersionIn = MESH_VERSION_GOTHIC_1_30;
		bspVersionIn = BSP_VERSION_GOTHIC_1_30;
		materialVersionIn = MATERIAL_VERSION_SUM_GOTHIC_1_30;

		break;
	default:
		printf("Unrecognized version in, needs to be 101, 104/108/112 or 130/26\n");

		return zERROR;

		break;
	}

	switch (atoi(argv[2])) // version out
	{
	case 101:
		meshAndBspVersionOut = BSPMESH_VERSION_GOTHIC_1_01;
		meshVersionOut = MESH_VERSION_GOTHIC_1_01;
		bspVersionOut = BSP_VERSION_GOTHIC_1_01;
		materialVersionOut = MATERIAL_VERSION_SUM_GOTHIC_1_01;

		xZenOut = FALSE;

		break;
	case 104:
	case 108:
	case 112:
		meshAndBspVersionOut = BSPMESH_VERSION_GOTHIC_1_04;
		meshVersionOut = MESH_VERSION_GOTHIC_1_04;
		bspVersionOut = BSP_VERSION_GOTHIC_1_04;
		materialVersionOut = MATERIAL_VERSION_SUM_GOTHIC_1_04;

		break;
	case 130:
	case 26:
		meshAndBspVersionOut = BSPMESH_VERSION_GOTHIC_1_30;
		meshVersionOut = MESH_VERSION_GOTHIC_1_30;
		bspVersionOut = BSP_VERSION_GOTHIC_1_30;
		materialVersionOut = MATERIAL_VERSION_SUM_GOTHIC_1_30;

		break;
	default:
		printf("Unrecognized version out, needs to be 101, 104/108/112 or 130/26\n");

		return zERROR;

		break;
	}

	if (argc > 5)
	{
		if (!stricmp(argv[5], "nodelod")) deLod = FALSE;

		if (argc > 6)
		{
			if (!stricmp(argv[6], "nodelod")) deLod = FALSE;

			if (argc > 7)
			{
				if (!stricmp(argv[7], "nodelod")) deLod = FALSE;
			}
		}
	}

	oCWorld *world = zNEW(oCWorld);

	if (world->LoadZEN(argv[3])) // filename in
	{
		if (argc > 4)
		{
			world->SaveZEN(argv[4]); // filename out

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
