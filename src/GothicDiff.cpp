#include "GothicTools.h"

#include "Archiver.h"
#include "World.h"
#include "WayNet.h"
#include "Visual.h"
#include "Vob.h"
#include "ObjectRegistry.h"

enum
{
	ARG_PROG_NAME,
	ARG_FILE_IN_1,
	ARG_VERSION_IN_1,
	ARG_FILE_IN_2,
	ARG_VERSION_IN_2,
	ARG_OPTIONAL_MATCHES_FILE,
	ARG_NUM,
};

// #define SPECIAL_VOB_DEBUG

int main(int argc, const char **argv)
{
#ifdef _CRTDBG_MAP_ALLOC
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
#endif

#if 1
	if (argc < ARG_OPTIONAL_MATCHES_FILE)
	{
		printf("GothicDiff v0.1 by withmorten\n");
		printf("usage:\n");
		printf("GothicDiff <filename in 1> <version in 1> <filename in 2> <version in 2> [<matches file>]\n\n");

		return zOK;
	}

	int32 version1 = atoi(argv[ARG_VERSION_IN_1]);
	int32 version2 = atoi(argv[ARG_VERSION_IN_2]);

	zCWorld *world1 = zNEW(zCWorld);
	zCArchiver arc1;

	if (!SetInVersions(version1)) return zERROR;

	arc1.OpenFile(argv[ARG_FILE_IN_1]);
	arc1.ReadHeader();
	if (!world1->LoadZEN(arc1)) return zERROR;
	arc1.CloseFile();

	printf("Zen 1 ok\n");

	if (!SetInVersions(version2)) return zERROR;

	zCWorld *world2 = zNEW(zCWorld);
	zCArchiver arc2;

	arc2.OpenFile(argv[ARG_FILE_IN_2]);
	arc2.ReadHeader();
	if (!world2->LoadZEN(arc2)) return zERROR;
	arc2.CloseFile();

	printf("Zen 2 ok\n");

	int32 not_found1 = 0;
	int32 not_found2 = 0;

	zCArchiver arc1_diff;
	zCArchiver arc2_diff;

	arc1_diff.mode = zARC_MODE_ASCII_DIFF;
	arc2_diff.mode = zARC_MODE_ASCII_DIFF;
	
	arc1_diff.OpenFile(zSTRING(argv[ARG_FILE_IN_1]) + ".DIFF", TRUE);
	arc2_diff.OpenFile(zSTRING(argv[ARG_FILE_IN_2]) + ".DIFF", TRUE);

	// version2 wins, if higher
	// because there is no logic for discarding higher version values for comparison.
	SetOutVersions(version2 > version1 ? version2 : version1);

	zCObjectMatches matches;

	if (argc > ARG_OPTIONAL_MATCHES_FILE)
	{
		matches.Load(argv[ARG_OPTIONAL_MATCHES_FILE]);

		printf("loaded %d matches\n", matches.objectMatches.numInArray + matches.wayMatches.numInArray);
	}
	
	// VobTree
	{
		// first check is for hash equality (fast)
		// if not found by that, second check is for "flaky" equality (slow), which takes into consideration some special logic for fields
		for (int32 i = 0; i < world1->vobs.numInArray; i++)
		{
			zCVob *vob1 = world1->vobs[i];

			for (int32 j = 0; j < world2->vobs.numInArray; j++)
			{
				zCVob *vob2 = world2->vobs[j];

				if (!vob2->found && vob1->IsHashEqual(vob2))
				{
					vob1->found = TRUE;
					vob2->found = TRUE;

					break;
				}
			}

			if (!vob1->found)
			{
				for (int32 j = 0; j < world2->vobs.numInArray; j++)
				{
					zCVob *vob2 = world2->vobs[j];

					if (!vob2->found && vob1->IsEqual(vob2))
					{
						vob1->found = TRUE;
						vob2->found = TRUE;

						break;
					}
				}
			}
		}

		// TODO ShowVisual seems to be set to TRUE after loading some visuals?
		// zCVobLensFlare::Unarchive sets it to TRUE after Unarchive
		// ^ this is handled now, 104 doesn't set it to TRUE, but 108 does ...
		// now why is not TRUE in original WORLD.ZEN? what kind of ancient version did they write the world with?
		// most likely a Spacer version around 1.04/1.05 .. probably 1.05, since that removes stateTarget/stateNum

		// TODO so FIRE_SMOKE.pfx vobs are actually super flaky ...
		// even when selecting them in the Spacer their pos will differ on different selections :(
		// interestingly, how do these things stay in proper position???
		// and what about lights in their vobtree? unfortunately i don't visualise this at all in the vob diffing ...

		// TODO gothic 1 which focusNames were ok and which were not? I think named ones were supposed to be bad ...

		// now write the not found objects to the diff archive, if they weren't in the loaded matches file ...
		for (int32 i = 0; i < world1->vobs.numInArray; i++)
		{
			zCVob *vob1 = world1->vobs[i];

			if (!vob1->found && !matches.IsInObjectMatches1(vob1->chunk.objectIndex))
			{
				arc1_diff.WriteObject(vob1);

				not_found1++;
			}
		}

		for (int32 j = 0; j < world2->vobs.numInArray; j++)
		{
			zCVob *vob2 = world2->vobs[j];

			if (!vob2->found && !matches.IsInObjectMatches2(vob2->chunk.objectIndex))
			{
				arc2_diff.WriteObject(vob2);

				not_found2++;
			}
		}

		printf("VobTree not found: %d, %d\n", not_found1, not_found2);

		not_found1 = 0;
		not_found2 = 0;
	}

	// WayNet
	{
		// Waypoints
		{
			// first check is for hash equality (fast)
			// if not found by that, second check is for "flaky" equality (slow), which takes into consideration some special logic for fields
			for (int32 i = 0; i < world1->wayNet->numWaypoints; i++)
			{
				zCWaypoint *wp1 = world1->wayNet->waypoints[i];

				for (int32 j = 0; j < world2->wayNet->numWaypoints; j++)
				{
					zCWaypoint *wp2 = world2->wayNet->waypoints[j];

					if (!wp2->found && wp1->IsHashEqual(wp2))
					{
						wp1->found = TRUE;
						wp2->found = TRUE;

						break;
					}
				}

				if (!wp1->found)
				{
					for (int32 j = 0; j < world2->wayNet->numWaypoints; j++)
					{
						zCWaypoint *wp2 = world2->wayNet->waypoints[j];

						if (!wp2->found && wp1->IsEqual(wp2))
						{
							wp1->found = TRUE;
							wp2->found = TRUE;

							break;
						}
					}
				}
			}

			// now write the not found objects to the diff archive, if they weren't in the loaded matches file ...
			for (int32 i = 0; i < world1->wayNet->numWaypoints; i++)
			{
				zCWaypoint *wp1 = world1->wayNet->waypoints[i];

				if (!wp1->found && !matches.IsInObjectMatches1(wp1->chunk.objectIndex))
				{
					arc1_diff.WriteObject(wp1);

					not_found1++;
				}
			}

			for (int32 j = 0; j < world2->wayNet->numWaypoints; j++)
			{
				zCWaypoint *wp2 = world2->wayNet->waypoints[j];

				if (!wp2->found && !matches.IsInObjectMatches2(wp2->chunk.objectIndex))
				{
					arc2_diff.WriteObject(wp2);

					not_found2++;
				}
			}

			printf("WayNet Waypoints not found: %d, %d\n", not_found1, not_found2);

			not_found1 = 0;
			not_found2 = 0;
		}

		// Ways
		{
			// first check is for hash equality (fast)
			// if not found by that, second check is for "flaky" equality (slow), which takes into consideration some special logic for fields
			for (int32 i = 0; i < world1->wayNet->numWays; i++)
			{
				zCWay *way1 = world1->wayNet->ways[i];

				for (int32 j = 0; j < world2->wayNet->numWays; j++)
				{
					zCWay *way2 = world2->wayNet->ways[j];

					if (!way2->found && way1->IsHashEqual(way2))
					{
						way1->found = TRUE;
						way2->found = TRUE;

						break;
					}
				}

				if (!way1->found)
				{
					for (int32 j = 0; j < world2->wayNet->numWays; j++)
					{
						zCWay *way2 = world2->wayNet->ways[j];

						if (!way2->found && way1->IsEqual(way2))
						{
							way1->found = TRUE;
							way2->found = TRUE;

							break;
						}
					}
				}
			}

			// now write the not found objects to the diff archive, if they weren't in the loaded matches file ...
			for (int32 i = 0; i < world1->wayNet->numWays; i++)
			{
				zCWay *way1 = world1->wayNet->ways[i];

				if (!way1->found && !matches.IsInWayMatches1(way1->wayIndex))
				{
					arc1_diff.WriteObject(way1->left);
					arc1_diff.WriteObject(way1->right);

					not_found1++;
				}
			}

			for (int32 j = 0; j < world2->wayNet->numWays; j++)
			{
				zCWay *way2 = world2->wayNet->ways[j];

				if (!way2->found && !matches.IsInWayMatches2(way2->wayIndex))
				{
					arc2_diff.WriteObject(way2->left);
					arc2_diff.WriteObject(way2->right);

					not_found2++;
				}
			}

			printf("WayNet Ways not found: %d, %d\n", not_found1, not_found2);
		}
	}
	
	zDELETE(world1);
	zDELETE(world2);
#endif

#if 0
	SetInVersions(GOTHIC_VERSION_108);
	SetOutVersions(GOTHIC_VERSION_108);

	zCArchiver arc1;
	zCArchiver arc2;

	zCWorld *world1 = zNEW(zCWorld);
	zCWorld *world2 = zNEW(zCWorld);

	arc1.OpenFile("C:\\Gothic\\Gothic\\_work\\DATA\\Worlds\\WORLD_VANILLA.ZEN");
	arc1.ReadHeader();
	if (!world1->LoadZEN(arc1)) return 1;
	arc1.CloseFile();

	arc2.OpenFile("C:\\Gothic\\Gothic Switch\\_work\\DATA\\Worlds\\WORLD.ZEN");
	arc2.ReadHeader();
	if (!world2->LoadZEN(arc2)) return 1;
	arc2.CloseFile();

#if 0
	zCWorld *world3 = zNEW(zCWorld);
	zCArchiver arc3;

	arc3.OpenFile("G:\\Program Files\\PiranhaBytes\\Gothic_clean\\_work\\DATA\\Worlds\\_work\\SURFACE.ZEN");
	arc3.ReadHeader();
	if (!world3->LoadZEN(arc3)) return 1;
	arc3.CloseFile();

	world3->compiled = FALSE;
	arc3.mode = zARC_MODE_ASCII_DIFF;
	arc3.OpenFile("C:\\Gothic\\Gothic Switch\\_work\\DATA\\Worlds\\_work\\SURFACE.ZEN.DIFF", TRUE);
	arc3.WriteHeader();
	world3->SaveZEN(arc3);
	arc3.CloseFile();

	zDELETE(world3);
#endif

#ifdef SPECIAL_VOB_DEBUG
	zCVobLight *foo1 = NULL;

	for (int32 i = 0; i < world1->vobs.numInArray; i++)
	{
		zCVob *vob = world1->vobs[i];

		if (vob->chunk.objectIndex == 65)
		{
			foo1 = (zCVobLight *)vob;

			break;
		}
	}
	zCVobLight *foo2 = NULL;

	for (int32 i = 0; i < world2->vobs.numInArray; i++)
	{
		zCVob *vob = world2->vobs[i];

		if (vob->chunk.objectIndex == 72)
		{
			foo2 = (zCVobLight *)vob;

			break;
		}
	}
#endif

	for (int32 i = 0; i < world1->vobs.numInArray; i++)
	{
		zCVob *vob1 = world1->vobs[i];

		for (int32 j = 0; j < world2->vobs.numInArray; j++)
		{
			zCVob *vob2 = world2->vobs[j];

			if (!vob2->found && vob1->IsHashEqual(vob2))
			{
				vob1->found = TRUE;
				vob2->found = TRUE;

				break;
			}
		}

		if (!vob1->found)
		{
#ifdef SPECIAL_VOB_DEBUG
			if (vob1 == foo1)
			{
				__nop();
			}
#endif

			for (int32 j = 0; j < world2->vobs.numInArray; j++)
			{
				zCVob *vob2 = world2->vobs[j];

#ifdef SPECIAL_VOB_DEBUG
				if (vob1 == foo1 && vob2 == foo2)
				{
					__nop();
				}
#endif

				if (!vob2->found && vob1->IsEqual(vob2))
				{
					vob1->found = TRUE;
					vob2->found = TRUE;

					break;
				}
			}
		}
	}
	
	int32 not_found1 = 0;
	int32 not_found2 = 0;

	zCObjectMatches matches;

#if 1
	matches.Insert(  169,   176, "oCMobInter -> oCMobSwitch");
	matches.Insert(  182,   189, "ORCDRUM_1 +focusName");
	matches.Insert(  184,   191, "ORCDRUM_1 +focusName");
	matches.Insert(  189,   196, "ORCDRUM_1 +focusName");
	matches.Insert(  191,   198, "ORCDRUM_1 +focusName");
	matches.Insert(  304,   311, "oCMobInter -> oCMobSwitch");
	matches.Insert(  306,   313, "EVT_AM_LOB_GATE_BIG +sfx");
	matches.Insert(  378,   385, "oCMobSwitch +focusName");
	matches.Insert(  409,   416, "CHESTBIG_OCCHESTLARGE -> CHESTBIG_OCCHESTLARGELOCKED");
	matches.Insert(  411,   418, "oCMobContainer focusName");
	matches.Insert(  413,   420, "oCMobContainer focusName");
	matches.Insert(  423,   430, "BOOK_BLUE +focusName");
	matches.Insert(  425,   432, "BOOK_BLUE +focusName");
	matches.Insert(  427,   434, "oCMobContainer focusName");
	matches.Insert(  445,   452, "oCMobInter -> oCMobSwitch");
	matches.Insert(  455,   462, "CHESTSMALL_OCCHESTSMALLLOCKED -> CHESTSMALL_OCCHESTSMALL");
	matches.Insert(  457,   464, "CHESTSMALL_OCCHESTSMALLLOCKED -> CHESTSMALL_OCCHESTSMALL");
	matches.Insert(  459,   466, "CHESTSMALL_OCCHESTSMALLLOCKED -> CHESTSMALL_OCCHESTSMALL");
	matches.Insert(  533,   539, "CHESTSMALL_OCCHESTSMALLLOCKED -> CHESTSMALL_OCCHESTSMALL");
	matches.Insert(  535,   541, "CHESTSMALL_OCCHESTSMALLLOCKED -> CHESTSMALL_OCCHESTSMALL");
	matches.Insert(  541,   547, "CHESTSMALL_OCCHESTSMALLLOCKED -> CHESTSMALL_OCCHESTSMALL");
	matches.Insert(  580,   586, "DT_BOOKSHELF_V2 cdDyn, staticVob -> TRUE");
	matches.Insert(  581,   587, "DT_BOOKSHELF_V2 cdDyn, staticVob -> TRUE");
	matches.Insert(  582,   588, "OC_LOB_CAGE cdDyn, staticVob -> TRUE");
	matches.Insert(  584,   590, "OC_LOB_CAGE cdDyn, staticVob -> TRUE");
	matches.Insert(  641,   647, "DT_BOOKSHELF_V2 cdDyn, staticVob -> TRUE");
	matches.Insert(  642,   648, "DT_BOOKSHELF_V2 cdDyn, staticVob -> TRUE");
	matches.Insert(  643,   649, "DT_BOOKSHELF_V2 cdDyn, staticVob -> TRUE");
	matches.Insert(  644,   650, "DT_BOOKSHELF_V2 cdDyn, staticVob -> TRUE");
	matches.Insert(  667,   673, "OW_FOCUSPLATTFORM cdDyn, staticVob -> TRUE");
	matches.Insert(  878,   884, "CHEST trafoOSToWSPos OOB");
	matches.Insert(  880,   886, "CHEST contains");
	matches.Insert(  884,   890, "CHESTBIG_OCCHESTMEDIUMLOCKED -> CHESTBIG_OCCHESTMEDIUM");
	matches.Insert(  886,   892, "oCMobContainer focusName");
	matches.Insert(  889,   895, "ITMISWORDRAW trafoOSToWSPos OOB");
	matches.Insert(  907,   913, "zCVob -> oCMobFire");
	matches.Insert(  942,   948, "oCMobContainer focusName");
	matches.Insert( 1080,  1085, "CHESTBIG_OCCHESTLARGE -> CHESTBIG_OCCHESTLARGELOCKED");
	matches.Insert( 1082,  1087, "CHESTBIG_OCCHESTLARGE -> CHESTBIG_OCCHESTLARGELOCKED");
	matches.Insert( 1115,  1120, "BENCH_1_NC +focusName");
	matches.Insert( 1117,  1122, "zCVob -> oCMobFire");
	matches.Insert( 1119,  1124, "zCVob -> oCMobFire");
	matches.Insert( 1129,  1134, "zCVob -> oCMobFire");
	matches.Insert( 1190,  1195, "BENCH_1_OC +focusName");
	matches.Insert( 1192,  1197, "oCMobContainer +ownerGuild");
	matches.Insert( 1210,  1215, "zCVob -> oCMobFire");
	matches.Insert( 1212,  1217, "zCVob -> oCMobFire");
	matches.Insert( 1218,  1223, "zCVob -> oCMobFire");
	matches.Insert( 1235,  1240, "oCMobContainer +ownerGuild, focusName");
	matches.Insert( 1567,  1572, "ORCDRUM_1 +focusName");
	matches.Insert( 1569,  1574, "ORCDRUM_1 +focusName");
	matches.Insert( 1572,  1577, "ORCDRUM_1 +focusName");
	matches.Insert( 1577,  1582, "ORCDRUM_1 +focusName");
	matches.Insert( 2081,  2086, "EVT_OW_LOCKOUT_TRIGGER, oCMobInter -> oCMobSwitch, +triggerTarget");
	matches.Insert( 2085,  2090, "EVT_OW_STARTLOCKOUT_MOVER +sfx");
	matches.Insert( 2087,  2092, "DT_SKELETON_V01 trafoOSToWSRot"); // TODO IsEqual? it's not visually different
	matches.Insert( 2088,  2093, "zCVob trafoOSToWSRot, trafoOSToWSPos"); // TODO IsEqual? it's not visually different
	matches.Insert( 2090,  2095, "zCVob trafoOSToWSRot, trafoOSToWSPos"); // TODO IsEqual? it's not visually different
	matches.Insert( 2091,  2096, "DT_SKELETON_V01_HEAD trafoOSToWSRot, trafoOSToWSPos"); // TODO IsEqual? it's not visually different
	matches.Insert( 2483,  2488, "FIRE_SMOKE trafoOSToWSRot, trafoOSToWSPos");
	matches.Insert( 2564,  2569, "FIRE_SMOKE trafoOSToWSRot, trafoOSToWSPos");
	matches.Insert( 2578,  2583, "FIRE_SMOKE trafoOSToWSRot, trafoOSToWSPos");
	matches.Insert( 2586,  2591, "FIRE_SMOKE trafoOSToWSRot, trafoOSToWSPos");
	matches.Insert( 2595,  2600, "FIRE_SMOKE trafoOSToWSRot, trafoOSToWSPos");
	matches.Insert( 2600,  2605, "FIRE_SMOKE trafoOSToWSRot, trafoOSToWSPos");
	matches.Insert( 2723,  2728, "BENCH_1_OC cdDyn, staticVob -> TRUE");
	matches.Insert( 2725,  2730, "BENCH_1_OC cdDyn, staticVob -> TRUE");
	matches.Insert( 2727,  2732, "oCMobFire useWithItem");
	matches.Insert( 3527,  3532, "EVT_CASTEL_FLOOR_5 +sfx");
	matches.Insert( 3529,  3534, "EVT_CASTEL_FLOOR_4 +sfx");
	matches.Insert( 3531,  3536, "EVT_CASTEL_FLOOR_3 +sfx");
	matches.Insert( 3533,  3538, "EVT_CASTEL_FLOOR_2 +sfx");
	matches.Insert( 3535,  3540, "EVT_CASTEL_FLOOR_1 +sfx, numCanBeActivated");
	matches.Insert( 3537,  3542, "EVT_CASTEL_PLATE +sfx");
	matches.Insert( 3543,  3548, "EVT_CASTLE_TRIGGER_LIST_1 numCanBeActivated");
	matches.Insert( 5478,  5483, "OW_TREE_V2, oCMOB -> zCVob");
	matches.Insert( 5637,  5642, "EVT_CASTLE_LIFT_01, +sfx");
	matches.Insert( 5641,  5646, "EVT_CASTLE_LIFT_02, +sfx");
	matches.Insert( 6005,  6010, "zCVobLight, -lensflareFX");
	matches.Insert( 6075,  6080, "MIN_LOB_COUPLESTONES trafoOSToWSPos OOB");
	matches.Insert( 6146,  6151, "OC_LOB_STONEBROKEN trafoOSToWSPos OOB");
	matches.Insert( 6268,  6273, "EVT_NC_LOCKOUT_MOVER_01 +sfx");
	matches.Insert( 6269,  6274, "EVT_NC_LOCKOUT_TRIGGER_01, oCMobInter -> oCMobSwitch, +triggerTarget");
	matches.Insert( 6290,  6295, "FIRE_SMOKE trafoOSToWSRot, trafoOSToWSPos");
	matches.Insert( 6299,  6304, "FIRE_SMOKE trafoOSToWSRot, trafoOSToWSPos");
	matches.Insert( 6330,  6335, "zCVobLight trafoOSToWSPos ORIGIN");
	matches.Insert( 6331,  6336, "zCVobLight trafoOSToWSPos ORIGIN");
	matches.Insert( 6332,  6337, "zCVobLight trafoOSToWSPos ORIGIN");
	matches.Insert( 6333,  6338, "zCVobLight trafoOSToWSPos ORIGIN");
	matches.Insert( 6334,  6339, "zCVobLight trafoOSToWSPos ORIGIN");
	matches.Insert( 6335,  6340, "zCVobLight trafoOSToWSPos ORIGIN");
	matches.Insert( 6342,  6347, "zCVobLight trafoOSToWSPos ORIGIN");
	matches.Insert( 6343,  6348, "zCVobLight trafoOSToWSPos ORIGIN");
	matches.Insert( 6344,  6349, "zCVobLight trafoOSToWSPos ORIGIN");
	matches.Insert( 6345,  6350, "zCVobLight trafoOSToWSPos ORIGIN");
	matches.Insert( 6346,  6351, "zCVobLight trafoOSToWSPos ORIGIN");
	matches.Insert( 6347,  6352, "zCVobLight trafoOSToWSPos ORIGIN");
	matches.Insert( 6407,  6412, "OW_O_LAKEHUT cdDyn -> TRUE");
	matches.Insert( 6786,  6791, "BENCH_1_NC +focusName");
	matches.Insert( 6788,  6793, "BENCH_1_NC +focusName");
	matches.Insert( 7003,  7008, "CHESTBIG_OCCHESTLARGE -> CHESTBIG_OCCHESTLARGELOCKED");
	matches.Insert( 7082,  7087, "EVT_SURF_ORCSECRET_01 +sfx");
	matches.Insert( 7083,  7088, "EVT_SURF_ORCSECRET_02 +sfx");
	matches.Insert( 7088,  7093, "ORC_E_COLUMN trafoOSToWSPos OOB");
	matches.Insert( 7197,  7202, "oCMobContainer +ownerGuild");
	matches.Insert( 7200,  7205, "BENCH_1_OC +focusName");
	matches.Insert( 7390,  7395, "zCVob -> oCMobFire");
	matches.Insert( 7704,  7709, "oCMobInter -> oCMobSwitch");
	matches.Insert( 8063,  8068, "LADDER_10 +focusName");
	matches.Insert( 8065,  8070, "LADDER_8 +focusName");
	matches.Insert( 8067,  8072, "LADDER_10 +focusName");
	matches.Insert( 8069,  8074, "CHESTBIG_OCCHESTLARGE -> CHESTBIG_OCCHESTLARGELOCKED");
	matches.Insert( 8073,  8078, "oCMobInter -> oCMobSwitch, -ownerGuild");
	matches.Insert( 8092,  8097, "oCMobContainer focusName");
	matches.Insert( 8179,  8184, "oCMobContainer focusName");
	matches.Insert( 8229,  8234, "oCMobContainer focusName");
	matches.Insert( 8233,  8238, "CHESTBIG_OCCHESTMEDIUMLOCKED -> CHESTBIG_OCCHESTMEDIUM");
	matches.Insert( 8244,  8249, "oCMobContainer focusName");
	matches.Insert( 8246,  8251, "oCMobContainer focusName");
	matches.Insert( 8273,  8278, "oCMobContainer focusName");
	matches.Insert( 8275,  8280, "oCMobContainer focusName");
	matches.Insert( 8277,  8282, "oCMobContainer focusName");
	matches.Insert( 8279,  8284, "oCMobContainer focusName");
	matches.Insert( 8281,  8286, "oCMobInter +focusName");
	matches.Insert( 8283,  8288, "oCMobInter +focusName, triggerTarget");
	matches.Insert( 8288,  8293, "GEWANDTHEITSAMULETT2 trafoOSToWSPos OOB");
	matches.Insert( 8299,  8304, "oCMobInter focusName");
	matches.Insert( 8305,  8310, "zCVob -> oCMobFire");
	matches.Insert( 8336,  8341, "CHESTSMALL_OCCHESTSMALLLOCKED -> CHESTSMALL_OCCHESTSMALL");
	matches.Insert( 8338,  8343, "CHESTSMALL_OCCHESTSMALLLOCKED -> CHESTSMALL_OCCHESTSMALL");
	matches.Insert( 8340,  8345, "ORCDRUM_1 +focusName");
	matches.Insert( 8371,  8376, "MOBSISEULE trafoOSToWSRot, trafoOSToWSPos FLIP");
	matches.Insert( 8389,  8394, "DT_BOOKSHELF_V2 cdDyn, staticVob -> TRUE");
	matches.Insert( 8390,  8395, "DT_BOOKSHELF_V2 cdDyn, staticVob -> TRUE");
	matches.Insert( 8391,  8396, "DT_BOOKSHELF_V2 cdDyn, staticVob -> TRUE");
	matches.Insert( 8393,  8398, "oCMobContainer focusName");
	matches.Insert( 8489,  8673, "BOOK_BLUE +focusName");
	matches.Insert( 8491,  8671, "BOOK_BLUE +focusName");
	matches.Insert( 8493,  8669, "BOOK_BLUE +focusName");
	matches.Insert( 8690,  9044, "oCMobSwitch +focusName");
	matches.Insert( 8737,  8999, "CHESTBIG_OCCHESTLARGELOCKED -> CHESTBIG_OCCHESTLARGE");
	matches.Insert( 8803,  8931, "oCMobContainer locked -> TRUE");
	matches.Insert( 8813,  8921, "CHESTSMALL_OCCHESTSMALLLOCKED -> CHESTSMALL_OCCHESTSMALL");
	matches.Insert( 8821,  8913, "CHESTSMALL_OCCHESTSMALLLOCKED -> CHESTSMALL_OCCHESTSMALL");
	matches.Insert( 8873,  8860, "oCMobFire +useWithItem");
	matches.Insert( 8879,  8856, "oCMobFire +useWithItem");
	matches.Insert( 8887,  8848, "oCMobFire +useWithItem");
	matches.Insert( 8889,  8846, "oCMobFire +useWithItem");
	matches.Insert( 8896,  8839, "oCMobFire +useWithItem");
	matches.Insert( 8903,  8832, "oCMobFire +useWithItem");
	matches.Insert( 8911,  8824, "oCMobFire +useWithItem");
	matches.Insert( 8924,  8811, "oCMobFire +useWithItem");
	matches.Insert( 8926,  8809, "oCMobFire +useWithItem");
	matches.Insert( 8953,  8782, "oCMobFire +useWithItem");
	matches.Insert( 8955,  8780, "oCMobFire +useWithItem");
	matches.Insert( 8972,  8762, "oCMobFire +useWithItem");
	matches.Insert( 8974,  8760, "oCMobFire +useWithItem");
	matches.Insert( 8976,  8758, "oCMobFire +useWithItem");
	matches.Insert( 8978,  8756, "oCMobFire +useWithItem");
	matches.Insert( 9002,  8733, "OC_LOB_GATE_BIG +sfx");
	matches.Insert( 9003,  8731, "zCVobSound sndName");
	matches.Insert( 9004,  8732, "zCVobSound sndName");
	matches.Insert( 9005,  8729, "zCVobSound sndName");
	matches.Insert( 9006,  8730, "zCVobSound sndName");
	matches.Insert( 9007,  8728, "zCVobSound sndName");
	matches.Insert( 9008,  8726, "zCVobSound sndName");
	matches.Insert( 9009,  8727, "zCVobSound sndName");
	matches.Insert( 9010,  8725, "zCVobSound sndName");
	matches.Insert( 9033,  9101, "CAULDRON_OC focusName, +useWithItem");
	matches.Insert( 9044,  9112, "CAULDRON_OC +focusName, +useWithItem");
	matches.Insert( 9062,  9130, "BENCH_THRONE +focusName");
	matches.Insert( 9133,  9201, "OW_LOB_WOODPLANKS_V2 trafoOSToWSPos");
	matches.Insert( 9183,  9251, "OC_DECORATE_V1 trafoOSToWSPos");
	matches.Insert( 9212,  9280, "zCVobLight trafoOSToWSPos");
	matches.Insert( 9288,  9356, "oCMobContainer keyInstance");
	matches.Insert( 9393,  9461, "NC_LOB_TABLE2 trafoOSToWSPos OOB");
	matches.Insert( 9700,  9768, "CHAIR_1_NC +focusname");
	matches.Insert( 9759,  9827, "oCMobFire useWithItem");
	matches.Insert( 9761,  9829, "oCMobFire useWithItem");
	matches.Insert( 9763,  9831, "oCMobFire useWithItem");
	matches.Insert( 9808,  9876, "NC_LOB_LOG3 trafoOSToWSPos");
	matches.Insert( 9809,  9877, "NC_LOB_LOG3 trafoOSToWSPos");
	matches.Insert( 9810,  9878, "NC_LOB_LOG3 trafoOSToWSPos");
	matches.Insert(10170, 10238, "EVT_NC_MAINGATE01_MOVER +sfx");
	matches.Insert(10171, 10239, "oCMobInter -> oCMobSwitch");
	matches.Insert(10467, 10535, "NC_LOB_LIGHTER trafoOSToWSPos");
	matches.Insert(10469, 10537, "OC_BARRELS_REIHE trafoOSToWSPos");
	matches.Insert(10527, 10595, "CHESTBIG_OCCHESTMEDIUMLOCKED -> CHESTBIG_OCCHESTMEDIUM");
	matches.Insert(10533, 10601, "CHESTBIG_OCCHESTMEDIUMLOCKED -> CHESTBIG_OCCHESTMEDIUM");
	matches.Insert(10543, 10611, "oCMobContainer -keyInstance +pickLockStr");
	matches.Insert(10545, 10613, "CHESTBIG_OCCHESTMEDIUMLOCKED -> CHESTBIG_OCCHESTMEDIUM");
	matches.Insert(10547, 10615, "CHESTBIG_OCCHESTMEDIUMLOCKED -> CHESTBIG_OCCHESTMEDIUM");
	matches.Insert(10549, 10617, "CHESTBIG_OCCHESTMEDIUMLOCKED -> CHESTBIG_OCCHESTMEDIUM");
	matches.Insert(10551, 10619, "CHESTBIG_OCCHESTMEDIUMLOCKED -> CHESTBIG_OCCHESTMEDIUM");
	matches.Insert(10561, 10629, "CHESTBIG_OCCHESTMEDIUMLOCKED -> CHESTBIG_OCCHESTMEDIUM");
	matches.Insert(10645, 10713, "BSSHARP_OC +useWithItem");
	matches.Insert(10685, 11915, "CHAIR_THRONE +focusName");
	matches.Insert(10713, 11889, "zCVob -> oCMobFire");
	matches.Insert(10727, 11875, "oCMobInter focusName");
	matches.Insert(10729, 11873, "oCMobInter focusName");
	matches.Insert(10731, 11871, "oCMobInter focusName");
	matches.Insert(10831, 11772, "SMOKE_WATERPIPE trafoOSToWSPos");
	matches.Insert(10854, 11749, "BSCOOL_OC +focusName");
	matches.Insert(10892, 11712, "oCMobInter focusName");
	matches.Insert(10984, 11621, "oCMobBed -> oCMobDoor, +onStateFunc SLEEPABIT");
	matches.Insert(10986, 11619, "oCMobBed -> oCMobDoor, +onStateFunc SLEEPABIT");
	matches.Insert(10988, 11617, "oCMobBed -> oCMobDoor, +onStateFunc SLEEPABIT");
	matches.Insert(10990, 11615, "oCMobBed -> oCMobDoor, +onStateFunc SLEEPABIT");
	matches.Insert(10992, 11613, "oCMobBed -> oCMobDoor, +onStateFunc SLEEPABIT");
	matches.Insert(10994, 11611, "oCMobBed -> oCMobDoor, +onStateFunc SLEEPABIT");
	matches.Insert(10996, 11609, "oCMobBed -> oCMobDoor, +onStateFunc SLEEPABIT");
	matches.Insert(10998, 11607, "oCMobBed -> oCMobDoor, +onStateFunc SLEEPABIT");
	matches.Insert(11000, 11605, "oCMobBed -> oCMobDoor, +onStateFunc SLEEPABIT");
	matches.Insert(11002, 11603, "oCMobBed -> oCMobDoor, +onStateFunc SLEEPABIT");
	matches.Insert(11406, 11200, "FLAM_A0 trafoOSToWSPos");
	matches.Insert(11412, 11194, "FLAM_A0 trafoOSToWSPos");
	matches.Insert(11415, 11191, "FLAM_A0 trafoOSToWSPos");
	matches.Insert(11424, 11182, "FLAM_A0 trafoOSToWSPos");
	matches.Insert(11427, 11179, "FLAM_A0 trafoOSToWSPos");
	matches.Insert(11430, 11176, "FLAM_A0 trafoOSToWSPos");
	matches.Insert(11433, 11173, "FLAM_A0 trafoOSToWSPos");
	matches.Insert(11446, 11161, "FLAM_A0 trafoOSToWSPos");
	matches.Insert(11449, 11158, "FLAM_A0 trafoOSToWSPos");
	matches.Insert(11455, 11152, "FLAM_A0 trafoOSToWSPos");
	matches.Insert(11458, 11149, "FLAM_A0 trafoOSToWSPos");
	matches.Insert(11462, 11145, "FLAM_A0 trafoOSToWSPos");
	matches.Insert(11508, 11098, "oCMobInter focusName");
	matches.Insert(11510, 11096, "oCMobInter focusName");
	matches.Insert(11516, 11090, "oCMobInter +useWithItem");
	matches.Insert(11586, 11019, "oCMobInter focusname");
	matches.Insert(11588, 11017, "oCMobInter focusname");
	matches.Insert(11598, 11007, "BSFIRE_OC +focusname");
	matches.Insert(11600, 11005, "BSANVIL_OC +focusname");
	matches.Insert(11602, 11003, "BSANVIL_OC +focusname");
	matches.Insert(11604, 11001, "BSANVIL_OC +focusname");
	matches.Insert(11606, 10999, "BSCOOL_OC +focusname");
	matches.Insert(11608, 10997, "BSCOOL_OC +focusname");
	matches.Insert(11646, 10959, "BSFIRE_OC +focusname");
	matches.Insert(11656, 10947, "BSSHARP_OC +focusName");
	matches.Insert(11658, 10945, "BSSHARP_OC +focusName");
	matches.Insert(11660, 10943, "BSCOOL_OC +focusName");
	matches.Insert(11720, 10883, "oCMobInter focusName");
	matches.Insert(11722, 10881, "oCMobInter focusName");
	matches.Insert(11756, 10847, "BSCOOL_OC +focusName");
	matches.Insert(11821, 10779, "oCMobContainer focusName");
	matches.Insert(11848, 10751, "oCMobContainer focusName");
	matches.Insert(11835, 10764, "zCVob -> oCMobFire");
	matches.Insert(12100, 16665, "oCMobDoor keyInstance");
	matches.Insert(12136, 16633, "OC_STABLE_PLANKS cdDyn, staticVob -> TRUE");
	matches.Insert(12161, 16609, "OC_STABLE_PLANKS cdDyn, staticVob -> TRUE");
	matches.Insert(12482, 16290, "OW_DIRTDECAL trafoOSToWSPos");
	matches.Insert(12484, 16288, "OW_DIRTDECAL trafoOSToWSPos");
	matches.Insert(13134, 15643, "OC_FIREWOOD_V1 cdDyn -> TRUE");
	matches.Insert(13135, 15642, "OC_SACK_V01 cdDyn -> TRUE");
	matches.Insert(13142, 15635, "OC_SACK_V03 cdDyn -> TRUE");
	matches.Insert(13143, 15634, "OC_SACK_V03 cdDyn -> TRUE");
	matches.Insert(13144, 15633, "OC_SACK_V03 cdDyn -> TRUE");
	matches.Insert(13145, 15632, "OC_SACK_V03 cdDyn -> TRUE");
	matches.Insert(13146, 15631, "OC_CHICKENSTICK_CHICKEN_V01 cdDyn -> TRUE");
	matches.Insert(13147, 15630, "OC_CHICKENSTICK_CHICKEN_V01 cdDyn -> TRUE");
	matches.Insert(13148, 15629, "OC_CHICKENSTICK_V02 cdDyn -> TRUE");
	matches.Insert(13225, 15553, "OC_LOB_FLAG_SMALL trafoOSToWSPos OOB");
	matches.Insert(13226, 15552, "OC_LOB_FLAG_SMALL trafoOSToWSPos OOB");
	matches.Insert(13245, 15533, "OW_LOB_BUSH_V5 cdDyn -> FALSE");
	matches.Insert(13779, 14999, "BARRELO_OC focusName");
	matches.Insert(13783, 14996, "OC_LOB_GATE_SMALL3 +sfx");
	matches.Insert(13785, 14995, "OC_LOB_GATE_SMALL3 +sfx");
	matches.Insert(13786, 14993, "OC_LOB_GATE_SMALL3 +sfx");
	matches.Insert(13795, 14984, "OC_LOB_GATE_BIG +sfx");
	matches.Insert(14128, 14650, "CHESTBIG_OCCHESTLARGE -> CHESTBIG_OCCHESTLARGELOCKED");
	matches.Insert(14140, 14638, "oCMobContainer contains");
	matches.Insert(14142, 14636, "CHESTBIG_OCCHESTLARGE -> CHESTBIG_OCCHESTLARGELOCKED");
	matches.Insert(14180, 14598, "oCMobContainer contains");
	matches.Insert(14198, 14580, "CHESTBIG_OCCHESTMEDIUMLOCKED -> CHESTBIG_OCCHESTMEDIUM");
	matches.Insert(14200, 14578, "oCMobContainer contains");
	matches.Insert(14268, 14510, "oCMobContainer contains");
	matches.Insert(14282, 14496, "CHESTBIG_OCCRATELARGE -> CHESTBIG_OCCRATELARGELOCKED");
	matches.Insert(14286, 14492, "CHESTBIG_OCCRATELARGE -> CHESTBIG_OCCRATELARGELOCKED");
	matches.Insert(14288, 14490, "CHESTBIG_OCCRATELARGE -> CHESTBIG_OCCRATELARGELOCKED");
	matches.Insert(14290, 14488, "CHESTBIG_OCCRATELARGE -> CHESTBIG_OCCRATELARGELOCKED");
	matches.Insert(14294, 14484, "CHESTBIG_OCCRATELARGE -> CHESTBIG_OCCRATELARGELOCKED");
	matches.Insert(14296, 14482, "CHESTBIG_OCCRATELARGE -> CHESTBIG_OCCRATELARGELOCKED");
	matches.Insert(14764, 14012, "OC_LOB_PLANKBROKEN_SMALL trafoOSToWSPos");
	matches.Insert(15072, 13704, "EVT_OC_MAINGATE01_01 +sfx");
	matches.Insert(15079, 13697, "EVT_OC_MAINGATE02_02 +sfx");
	matches.Insert(15146,    -1, "-TORCH.pfx");
	matches.Insert(15150,    -1, "-TORCH.pfx");
	matches.Insert(15320, 13456, "OC_FIREPLACE_V3 cdDyn -> TRUE");
	matches.Insert(15667, 13109, "CHESTBIG_OCCHESTLARGE -> CHESTBIG_OCCHESTLARGELOCKED");
	matches.Insert(15789, 12986, "OC_SHELF_V3 cdDyn -> TRUE");
	matches.Insert(16347, 12425, "OC_LOB_PLANKBROKEN_SMALL cdDyn -> TRUE");
	matches.Insert(16368, 12403, "SMOKE_WATERPIPE.ASC -> SMOKE_WATERPIPE.MDS");
	matches.Insert(16370, 12401, "SMOKE_WATERPIPE.ASC -> SMOKE_WATERPIPE.MDS");
	matches.Insert(16372, 12399, "SMOKE_WATERPIPE.ASC -> SMOKE_WATERPIPE.MDS");
	matches.Insert(16479, 12288, "DIE_GRUFT +itemInstance");
	matches.Insert(16577, 12190, "MIN_LOB_HANDRAIL_3 cdDyn, staticVob -> TRUE");
	matches.Insert(16578, 12189, "MIN_LOB_HANDRAIL_3 cdDyn, staticVob -> TRUE");
	matches.Insert(16832,    -1, "-FIRE.pfx");
	matches.Insert(16836,    -1, "-FIRE.pfx");
	matches.Insert(16838,    -1, "-FIRE.pfx");
	matches.Insert(16845,    -1, "-FIRE.pfx");
	matches.Insert(   -1,     3, "+MIN_LOB_STONE_V3_30");
	matches.Insert(   -1,     5, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,     7, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,     8, "+OW_TROLLPALISSADE");
	matches.Insert(   -1,  8444, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,  8445, "+MIN_LOB_STONE_V2_30");
	matches.Insert(   -1,  8446, "+OW_LOB_WOODPLANKS_V2");
	matches.Insert(   -1,  8447, "+MIN_LOB_STONE_V2_30");
	matches.Insert(   -1,  8448, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,  8449, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,  8450, "+MIN_LOB_STONE_V2_30");
	matches.Insert(   -1,  8451, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,  8452, "+MIN_LOB_STONE_V3_50");
	matches.Insert(   -1,  8453, "+MIN_LOB_COUPLESTONES");
	matches.Insert(   -1,  8454, "+MIN_LOB_COUPLESTONES");
	matches.Insert(   -1,  8455, "+MIN_LOB_STONE_V2_30");
	matches.Insert(   -1,  8456, "+ORC_SKELETON_V4");
	matches.Insert(   -1,  8457, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,  8458, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,  8459, "+MIN_LOB_STONE_V2_30");
	matches.Insert(   -1,  8460, "+MIN_LOB_STONE_V3_50");
	matches.Insert(   -1,  8461, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,  8462, "+MIN_LOB_STONE_V2_30");
	matches.Insert(   -1,  8463, "+MIN_LOB_STONE_V2_30");
	matches.Insert(   -1,  8464, "+MIN_LOB_STONE_V3_30");
	matches.Insert(   -1,  8465, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,  8466, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,  8467, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,  8468, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,  8469, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,  8470, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,  8471, "+MIN_LOB_STONE_V3_50");
	matches.Insert(   -1,  8472, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,  8473, "+MIN_LOB_STONE_V3_30");
	matches.Insert(   -1,  8474, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,  8475, "+MIN_LOB_STONE_V3_50");
	matches.Insert(   -1,  8476, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,  8477, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,  8478, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,  8479, "+MIN_LOB_STONE_V2_30");
	matches.Insert(   -1,  8480, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,  8481, "+MIN_LOB_STONE_V2_30");
	matches.Insert(   -1,  8482, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,  8483, "+MIN_LOB_STONE_V2_30");
	matches.Insert(   -1,  8484, "+MIN_LOB_STONE_V2_30");
	matches.Insert(   -1,  8485, "+MIN_LOB_STONE_V2_30");
	matches.Insert(   -1,  8486, "+MIN_LOB_STONE_V3_40");
	matches.Insert(   -1,  8487, "+MIN_LOB_STONE_V2_30");
	matches.Insert(   -1,  8488, "+MIN_LOB_STONE_V2_30");
	matches.Insert(   -1,  8489, "+MIN_LOB_COUPLESTONES");
	matches.Insert(   -1,  8490, "+MIN_LOB_COUPLESTONES");
	matches.Insert(   -1,  8491, "+MIN_LOB_COUPLESTONES");
	matches.Insert(   -1,  8492, "+MIN_LOB_COUPLESTONES");
	matches.Insert(   -1,  8493, "+OC_LOB_PLANKBROKEN_SMALL");
	matches.Insert(   -1,  8494, "+OC_LOB_PLANKBROKEN_SMALL");
	matches.Insert(   -1,  8495, "+OW_LOB_WOODPLANKS_V2");
	matches.Insert(   -1,  8496, "+OC_LOB_PLANKBROKEN_SMALL");
	matches.Insert(   -1,  8497, "+OC_LOB_PLANKBROKEN_SMALL");
	matches.Insert(   -1,  8498, "+OC_LOB_PLANKBROKEN_SMALL");
	matches.Insert(   -1,  8499, "+OW_LOB_WOODPLANKS_V2");
	matches.Insert(   -1,  8500, "+MIN_LOB_COUPLESTONES");
	matches.Insert(   -1,  8501, "+MIN_LOB_STONE_V2_30");
	matches.Insert(   -1,  8502, "+MIN_LOB_STONE_V2_30");
	matches.Insert(   -1,  8503, "+MIN_LOB_STONE_V2_30");
	matches.Insert(   -1,  8504, "+MIN_LOB_STONE_V2_30");
	matches.Insert(   -1,  8505, "+MIN_LOB_STONE_V2_30");
	matches.Insert(   -1,  9080, "+zCVobLight NC_LIGHTERIN");
	matches.Insert(   -1, 11514, "+zCVobLight INHOUSE_OVEN");
	matches.Insert(   -1, 11919, "+zCVobLight INHOUSE_OVEN");
	matches.Insert(   -1, 11920, "+zCVobLight INHOUSE_OVEN");
	matches.Insert(   -1, 11923, "+WASTEOUTLET_BOTTOM.pfx");
	matches.Insert(   -1, 11925, "+WASTEOUTLET_BOTTOM.pfx");
	matches.Insert(   -1, 13519, "+zCVobLight inhouse_ofen");
	matches.Insert(   -1, 14340, "+zCVobLight inhouse_ofen");
	matches.Insert(   -1, 16913, "+zCVobLight inhouse_ofen");
	matches.Insert(   -1, 16914, "+zCVobLight inhouse_ofen");
	matches.Insert(   -1, 16915, "+zCVobLight inhouse_ofen");
	matches.Insert(   -1, 16916, "+zCVobLight inhouse_ofen");

	printf("loaded %d matches\n", matches.objects1.numInArray);
#endif

#if 0
	zFILE matches_file("C:\\Gothic\\GothicDiffMatches\\switch.txt", TRUE);

	for (int32 i = 0; i < matches.objects1.numInArray; i++)
	{
		zSTRING line;
		char buffer[50];

		sprintf(buffer, "%5d", matches.objects1[i]);
		line += buffer;

		line += "\t";

		sprintf(buffer, "%5d", matches.objects2[i]);
		line += buffer;

		line += "\t";

		line += matches.reasons[i];

		matches_file.WriteLine(line);
	}
#endif

	zCArchiver arc1_diff;
	zCArchiver arc2_diff;

	arc1_diff.mode = zARC_MODE_ASCII_DIFF;
	arc2_diff.mode = zARC_MODE_ASCII_DIFF;

	// TODO trafoOSToWSRot needs IsFlaky

	arc1_diff.OpenFile("C:\\Gothic\\Gothic\\_work\\DATA\\Worlds\\WORLD_VANILLA.ZEN.DIFF", TRUE);

	for (int32 i = 0; i < world1->vobs.numInArray; i++)
	{
		zCVob *vob1 = world1->vobs[i];

		if (!vob1->found && !matches.objects1.IsInList(vob1->chunk.objectIndex))
		{
			arc1_diff.WriteObject(vob1);

			not_found1++;
		}
	}

	arc2_diff.OpenFile("C:\\Gothic\\Gothic Switch\\_work\\DATA\\Worlds\\WORLD.ZEN.DIFF", TRUE);

	for (int32 i = 0; i < world2->vobs.numInArray; i++)
	{
		zCVob *vob2 = world2->vobs[i];

		if (!vob2->found && !matches.objects2.IsInList(vob2->chunk.objectIndex))
		{
			arc2_diff.WriteObject(vob2);

			not_found2++;
		}
	}

	printf("not found: %d, %d\n", not_found1, not_found2);

#if 0
	auto &levels1 = world1->registry->GetList<zCVobLevelCompo>();
	auto &levels2 = world2->registry->GetList<zCVobLevelCompo>();

	for (int32 i = 0; i < levels1.numInArray; i++)
	{
		for (int32 j = 0; j < levels2.numInArray; j++)
		{
			if (levels1[i]->IsHashEqual(levels2[j]))
			{
				levels1[i]->found = TRUE;
				levels2[j]->found = TRUE;

				break;
			}
		}

		if (!levels1[i]->found)
		{
			printf("%s\n", levels1[i]->visualName.ToChar());
		}
	}
#endif

	zDELETE(world1);
	zDELETE(world2);
#endif

	return zOK;
}
