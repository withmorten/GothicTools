#pragma once

#include "GothicTools.h"

#include "Object.h"

class zCWaypoint : public zCObject
{
public:
	zOBJECT_DECLARATION(zCWaypoint);

public:
	zSTRING wpName;
	int32 waterDepth;
	bool32 underWater;
	zVEC3 position;
	zVEC3 direction;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void Hash();
	bool32 IsEqual(zCObject *obj);

	void EpsilonTest(zCWaypoint *wp);
};

class zCWay
{
public:
	zCWaypoint *left;
	zCWaypoint *right;

	int32 wayIndex;
	bool32 found;

public:
	zCWay(zCWaypoint *l, zCWaypoint *r, int32 wI) { left = l; right = r; wayIndex = wI; found = FALSE; }

	bool32 IsHashEqual(zCWay *way);
	bool32 IsEqual(zCWay *way);
};

class zCWayNet : public zCObject
{
public:
	zOBJECT_DECLARATION(zCWayNet);

public:
	enum
	{
		zWAYNET_VERSION_OLD = 0,
		zWAYNET_VERSION = 1,
	};

public:
	int32 numWaypoints;
	zCWaypoint **waypoints;

	int32 numWays;
	zCWay **ways;

public:
	zCWayNet();
	~zCWayNet();

	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};
