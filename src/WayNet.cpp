#include "WayNet.h"

#include "Archiver.h"

zOBJECT_DEFINITION(zCWaypoint);
zOBJECT_DEFINITION(zCWayNet);

bool32 zCWaypoint::Unarchive(zCArchiver &arc)
{
	if (!zCObject::Unarchive(arc)) return FALSE;

	arc.ReadString("wpName", wpName);
	arc.ReadInt("waterDepth", waterDepth);
	arc.ReadBool("underWater", underWater);
	arc.ReadVec3("position", position);
	arc.ReadVec3("direction", direction);

	return TRUE;
}

void zCWaypoint::Archive(zCArchiver &arc)
{
	zCObject::Archive(arc);

	arc.WriteString("wpName", wpName);
	arc.WriteInt("waterDepth", waterDepth);
	arc.WriteBool("underWater", underWater);
	arc.WriteVec3("position", position);
	arc.WriteVec3("direction", direction);
}

void zCWaypoint::Hash()
{
	zCObject::Hash();

	hash = XXH64(wpName.ToChar(), wpName.Length(), hash);
	hash = XXH64(&waterDepth, sizeof(waterDepth), hash);
	hash = XXH64(&underWater, sizeof(underWater), hash);
	hash = XXH64(&position, sizeof(position), hash);
	hash = XXH64(&direction, sizeof(direction), hash);
}

bool32 zCWaypoint::IsEqual(zCObject *obj)
{
	if (!zCObject::IsEqual(obj)) return FALSE;

	zCWaypoint *wp = (zCWaypoint *)obj;

#if 1
	if (chunk.objectIndex == 16855 && wp->chunk.objectIndex == 16924
		|| chunk.objectIndex == 16856 && wp->chunk.objectIndex == 16925
		|| chunk.objectIndex == 16857 && wp->chunk.objectIndex == 16926
		|| chunk.objectIndex == 16858 && wp->chunk.objectIndex == 16927
		|| chunk.objectIndex == 16862 && wp->chunk.objectIndex == 16931
		|| chunk.objectIndex == 16863 && wp->chunk.objectIndex == 16932
		|| chunk.objectIndex == 16864 && wp->chunk.objectIndex == 16933
		|| chunk.objectIndex == 16865 && wp->chunk.objectIndex == 16934
		|| chunk.objectIndex == 16866 && wp->chunk.objectIndex == 16935
		)
	{
		//EpsilonTest(wp);
	}
#endif

	if (wpName != wp->wpName) return FALSE;
	if (waterDepth != wp->waterDepth) return FALSE;
	if (underWater != wp->underWater) return FALSE;
#if 0
	if (position != wp->position) return FALSE;
#endif
	if (direction != wp->direction) return FALSE;

#if 1
	if (position != wp->position)
	{
		zVEC3 diff = position - wp->position;
		//float epsilon = 0.001f;
		float epsilon = 0.01f;

		if (fabs(diff[VX]) >= epsilon || fabs(diff[VY]) >= epsilon || fabs(diff[VZ]) >= epsilon) return FALSE;
	}
#endif

	return TRUE;
}

void zCWaypoint::EpsilonTest(zCWaypoint *wp)
{
	zVEC3 diff_position = position - wp->position;
	zVEC3 diff_direction = direction - wp->direction;

	printf("diff_position:\n");
	printf("%f, %f, %f\n", fabs(diff_position[VX]), fabs(diff_position[VY]), fabs(diff_position[VZ]));
	//printf("diff_direction:\n");
	//printf("%f, %f, %f\n", fabs(diff_direction[VX]), fabs(diff_direction[VY]), fabs(diff_direction[VZ]));
	printf("\n");
}

bool32 zCWay::IsHashEqual(zCWay *way)
{
	zCWaypoint *left1 = left->IsReference() ? (zCWaypoint *)left->ref : left;
	zCWaypoint *left2 = way->left->IsReference() ? (zCWaypoint *)way->left->ref : way->left;

	if (!left1->IsHashEqual(left2)) return FALSE;

	zCWaypoint *right1 = right->IsReference() ? (zCWaypoint *)right->ref : right;
	zCWaypoint *right2 = way->right->IsReference() ? (zCWaypoint *)way->right->ref : way->right;

	if (!right1->IsHashEqual(right2)) return FALSE;

	return TRUE;
}

bool32 zCWay::IsEqual(zCWay *way)
{
	zCWaypoint *left1 = left->IsReference() ? (zCWaypoint *)left->ref : left;
	zCWaypoint *left2 = way->left->IsReference() ? (zCWaypoint *)way->left->ref : way->left;

	zCWaypoint *right1 = right->IsReference() ? (zCWaypoint *)right->ref : right;
	zCWaypoint *right2 = way->right->IsReference() ? (zCWaypoint *)way->right->ref : way->right;

	if (wayIndex == 114 && way->wayIndex == 120
		|| wayIndex == 115 && way->wayIndex == 121
		|| wayIndex == 116 && way->wayIndex == 122
		|| wayIndex == 462 && way->wayIndex == 468
		|| wayIndex == 463 && way->wayIndex == 469
	)
	{
		//left1->EpsilonTest(left2);
		//right1->EpsilonTest(right2);
	}

	if (!left1->IsEqual(left2)) return FALSE;
	if (!right1->IsEqual(right2)) return FALSE;

	return TRUE;
}

zCWayNet::zCWayNet()
{
	waypoints = NULL;
	ways = NULL;
}

zCWayNet::~zCWayNet()
{
	for (int32 i = 0; i < numWaypoints; i++)
	{
		zDELETE(waypoints[i]);
	}

	zDELETE_ARRAY(waypoints);

	for (int32 i = 0; i < numWays; i++)
	{
		zDELETE(ways[i]->left);
		zDELETE(ways[i]->right);

		zDELETE(ways[i]);
	}

	zDELETE_ARRAY(ways);
}

bool32 zCWayNet::Unarchive(zCArchiver &arc)
{
	if (!zCObject::Unarchive(arc)) return FALSE;

	int32 waynetVersion = zWAYNET_VERSION_OLD;
	arc.ReadInt("waynetVersion", waynetVersion);

	if (waynetVersion != zWAYNET_VERSION) return FALSE;

	arc.ReadInt("numWaypoints", numWaypoints);
	waypoints = zNEW_ARRAY(zCWaypoint *, numWaypoints);

	for (int32 i = 0; i < numWaypoints; i++)
	{
		zSTRING n = i;
		zCWaypoint *waypoint = (zCWaypoint *)arc.ReadObject(zSTRING("waypoint" + n).ToChar());

		if (!waypoint) return FALSE;

		waypoints[i] = waypoint;
	}

	arc.ReadInt("numWays", numWays);
	ways = zNEW_ARRAY(zCWay *, numWays);

	for (int32 i = 0; i < numWays; i++)
	{
		zSTRING n = i;

		zCWaypoint *left = (zCWaypoint *)arc.ReadObject(zSTRING("wayl" + n).ToChar());
		zCWaypoint *right = (zCWaypoint *)arc.ReadObject(zSTRING("wayr" + n).ToChar());

		if (!left || !right) return FALSE;

		ways[i] = zNEW(zCWay)(left, right, i);
	}

	return TRUE;
}

void zCWayNet::Archive(zCArchiver &arc)
{
	zCObject::Archive(arc);

	arc.WriteInt("waynetVersion", zWAYNET_VERSION);

	arc.WriteInt("numWaypoints", numWaypoints);

	for (int32 i = 0; i < numWaypoints; i++)
	{
		arc.WriteObject(waypoints[i]);
	}

	arc.WriteInt("numWays", numWays);

	for (int32 i = 0; i < numWays; i++)
	{
		arc.WriteObject(ways[i]->left);
		arc.WriteObject(ways[i]->right);
	}
}
