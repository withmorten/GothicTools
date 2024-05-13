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
	void CalcHash();
	void CalcID();
	bool32 IsEqual(zCObject *obj);
};

class zCWay
{
public:
	zCWaypoint *left;
	zCWaypoint *right;

	int32 wayIndex;

	XXH64_hash_t hash;
	XXH32_hash_t id;
	bool32 found; // for checking if this way was previously found ...

public:
	zCWay(zCWaypoint *l, zCWaypoint *r, int32 wI);

	bool32 IsHashEqual(zCWay *way);
	XXH64_hash_t GetHash();
	XXH32_hash_t GetID();
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
