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

	if (argc < ARG_OPTIONAL_MATCHES_FILE)
	{
		printf("GothicDiff v0.1 by withmorten\n");
		printf("usage:\n");
		printf("GothicDiff <filename in 1> <version in 1> <filename in 2> <version in 2> [<matches file>]\n\n");

		return zOK;
	}

	// TODO diff vobtrees instead ...???

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

#if 0
	for (int32 i = 0; i < matches.objectMatches.numInArray; i++)
	{
		zCVob *vob1 = world1->GetVob(matches.objectMatches[i].object1);
		zCVob *vob2 = world2->GetVob(matches.objectMatches[i].object2);

		//if (vob1 && vob1->visualName == "FIRE_SMOKE.pfx" && vob2 && vob2->visualName == "FIRE_SMOKE.pfx")
		if (vob1 && vob1->presetName == "FIREE" && vob2 && vob2->presetName == "FIREE")
		{
			zMAT3 diff_trafoOSToWSRot = vob1->trafoOSToWSRot - vob2->trafoOSToWSRot;
			zPOINT3 diff_trafoOSToWSPos = vob1->trafoOSToWSPos - vob2->trafoOSToWSPos;

#if 0
			printf("diff_trafoOSToWSRot: %d\n", vob1->chunk.objectIndex);
			printf("%f, %f, %f\n", fabs(diff_trafoOSToWSRot[0][VX]), fabs(diff_trafoOSToWSRot[0][VY]), fabs(diff_trafoOSToWSRot[0][VZ]));
			printf("%f, %f, %f\n", fabs(diff_trafoOSToWSRot[1][VX]), fabs(diff_trafoOSToWSRot[1][VY]), fabs(diff_trafoOSToWSRot[1][VZ]));
			printf("%f, %f, %f\n", fabs(diff_trafoOSToWSRot[2][VX]), fabs(diff_trafoOSToWSRot[2][VY]), fabs(diff_trafoOSToWSRot[2][VZ]));
#else
			printf("diff_trafoOSToWSPos: %d\n", vob1->chunk.objectIndex);
			printf("%f, %f, %f\n", fabs(diff_trafoOSToWSPos[VX]), fabs(diff_trafoOSToWSPos[VY]), fabs(diff_trafoOSToWSPos[VZ]));
#endif
			printf("\n");
		}
	}
#endif

#ifdef SPECIAL_VOB_DEBUG
	zCVob *foo1 = (zCVob *)world1->GetVob(59);
	zCVob *foo2 = (zCVob *)world1->GetVob(55);
#endif
	
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

	return zOK;
}
