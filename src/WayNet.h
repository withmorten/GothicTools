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
};

class zCWay
{
public:
	zCWaypoint *left;
	zCWaypoint *right;

public:
	zCWay(zCWaypoint *l, zCWaypoint *r) { left = l; right = r; }
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
	void Hash();
};
