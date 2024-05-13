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

	if (arc.mode == zARC_MODE_ASCII_DIFF)
	{
		arc.WriteRaw("hash", &hash, sizeof(hash));
	}

	arc.WriteString("wpName", wpName);
	arc.WriteInt("waterDepth", waterDepth);
	arc.WriteBool("underWater", underWater);
	arc.WriteVec3("position", position);
	arc.WriteVec3("direction", direction);
}

void zCWaypoint::CalcHash()
{
	zCObject::CalcHash();

	hash = XXH64(wpName.ToChar(), wpName.Length(), hash);
	hash = XXH64(&waterDepth, sizeof(waterDepth), hash);
	hash = XXH64(&underWater, sizeof(underWater), hash);
	hash = XXH64(&position, sizeof(position), hash);
	hash = XXH64(&direction, sizeof(direction), hash);
}

void zCWaypoint::CalcID()
{
	zCObject::CalcID();

	id = XXH32(wpName.ToChar(), wpName.Length(), id);
	id = XXH32(&waterDepth, sizeof(waterDepth), id);
	id = XXH32(&underWater, sizeof(underWater), id);
	id = XXH32(&position, sizeof(position), id);
	id = XXH32(&direction, sizeof(direction), id);
}

bool32 zCWaypoint::IsEqual(zCObject *obj)
{
	if (!zCObject::IsEqual(obj)) return FALSE;

	zCWaypoint *wp = (zCWaypoint *)obj;

	if (wpName != wp->wpName) return FALSE;
	if (waterDepth != wp->waterDepth) return FALSE;
	if (underWater != wp->underWater) return FALSE;
	// if (position != wp->position) return FALSE;
	if (direction != wp->direction) return FALSE;

	if (position != wp->position)
	{
		zVEC3 diff = position - wp->position;
		float epsilon = 0.01f;

		if (fabs(diff[VX]) >= epsilon || fabs(diff[VY]) >= epsilon || fabs(diff[VZ]) >= epsilon) return FALSE;
	}

	return TRUE;
}

zCWay::zCWay(zCWaypoint *l, zCWaypoint *r, int32 wI)
{
	left = l;
	right = r;
	wayIndex = wI;
	hash = 0;
	id = 0;
	found = FALSE;
}

bool32 zCWay::IsHashEqual(zCWay *way)
{
	if (!left->IsHashEqual(way->left)) return FALSE;
	if (!right->IsHashEqual(way->right)) return FALSE;

	return TRUE;
}

XXH32_hash_t zCWay::GetID()
{
	if (id == 0)
	{
		id = XXH32(left->wpName.ToChar(), left->wpName.Length(), id);
		id = XXH32(right->wpName.ToChar(), right->wpName.Length(), id);
	}

	return id;
}

XXH64_hash_t zCWay::GetHash()
{
	if (hash == 0)
	{
		hash = XXH64(left->wpName.ToChar(), left->wpName.Length(), hash);
		hash = XXH64(right->wpName.ToChar(), right->wpName.Length(), hash);
	}

	return hash;
}

bool32 zCWay::IsEqual(zCWay *way)
{
	if (left->wpName != way->left->wpName) return FALSE;
	if (right->wpName != way->right->wpName) return FALSE;

	return TRUE;
}

zCWayNet::zCWayNet()
{
	waypoints = NULL;
	ways = NULL;
}

zCWayNet::~zCWayNet()
{
	zDELETE_ARRAY(waypoints);

	for (int32 i = 0; i < numWays; i++)
	{
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
		zSTRING n = i;

		arc.WriteObject(zSTRING("waypoint" + n).ToChar(), waypoints[i]);
	}

	arc.WriteInt("numWays", numWays);

	for (int32 i = 0; i < numWays; i++)
	{
		zSTRING n = i;

		arc.WriteObject(zSTRING("wayl" + n).ToChar(), ways[i]->left);
		arc.WriteObject(zSTRING("wayr" + n).ToChar(), ways[i]->right);
	}
}

zCWay *zCWayNet::GetWay(int32 wayIndex)
{
	for (int32 i = 0; i < numWays; i++)
	{
		if (ways[i]->wayIndex == wayIndex)
		{
			return ways[i];
		}
	}

	return NULL;
}
