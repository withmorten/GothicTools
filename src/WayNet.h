#pragma once

#include "GothicTools.h"

#include "Object.h"

class zCWaypoint : public zCObject
{
public:
	static const zSTRING GetClassName() { return "zCWaypoint"; }

public:
	zSTRING wpName;
	int32 waterDepth;
	bool32 underWater;
	zVEC3 position;
	zVEC3 direction;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
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
	static const zSTRING GetClassName() { return "zCWayNet"; }

public:
	enum
	{
		zWAYNET_VERSION_OLD = 0,
		zWAYNET_VERSION = 1,
	};

public:
	int32 numWaypoints;
	zCArray<zCWaypoint *> waypoints; // TODO make ptr array?

	int32 numWays;
	zCArray<zCWay *> ways; // TODO make ptr array?

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};
