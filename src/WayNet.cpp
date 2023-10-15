#include "WayNet.h"

#include "Archiver.h"

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

bool32 zCWayNet::Unarchive(zCArchiver &arc)
{
	if (!zCObject::Unarchive(arc)) return FALSE;

	int32 waynetVersion = zWAYNET_VERSION_OLD;
	arc.ReadInt("waynetVersion", waynetVersion);

	if (waynetVersion != zWAYNET_VERSION) return FALSE;

	arc.ReadInt("numWaypoints", numWaypoints);

	for (int32 i = 0; i < numWaypoints; i++)
	{
		zSTRING n = i;
		zCWaypoint *waypoint = (zCWaypoint *)arc.ReadObject(zSTRING("waypoint" + n).ToChar());

		if (!waypoint) return FALSE;

		waypoints.Insert(waypoint);
	}

	arc.ReadInt("numWays", numWays);

	for (int32 i = 0; i < numWays; i++)
	{
		zSTRING n = i;

		zCWaypoint *left = (zCWaypoint *)arc.ReadObject(zSTRING("wayl" + n).ToChar());
		zCWaypoint *right = (zCWaypoint *)arc.ReadObject(zSTRING("wayr" + n).ToChar());

		if (!left || !right) return FALSE;

		ways.Insert(zNEW(zCWay)(left, right));
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

