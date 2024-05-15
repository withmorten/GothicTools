#pragma once

#include "GothicTools.h"

#include "Object.h"

class zCWorld; // TEMP

class zCObjectMatches
{
public:
	class zCMatch
	{
	public:
		int32 index1;
		int32 index2;
		XXH32_hash_t id1;
		XXH32_hash_t id2;
		XXH64_hash_t hash1;
		XXH64_hash_t hash2;
		zSTRING reason;
	};

public:
	zCArray<zCMatch> objectMatches;
	zCArray<zCMatch> waypointMatches;
	zCArray<zCMatch> wayMatches;

public:
	bool32 ParseLine(zSTRING &s, zCMatch &match);
	void Load(const char *fileName);

	void FillHashes(zCWorld *world1, zCWorld *world2); // TEMP
	void WriteLine(zFILE &file, zCMatch &match); // TEMP
	void WriteHashes(zSTRING &fileName); // TEMP

	bool32 IsInObjectMatches1(int32 object1);
	bool32 IsInObjectMatches2(int32 object2);

	bool32 IsInWaypointMatches1(int32 wp1);
	bool32 IsInWaypointMatches2(int32 wp2);

	bool32 IsInWayMatches1(int32 way1);
	bool32 IsInWayMatches2(int32 way2);
};

class zCObjectList
{
public:
	zSTRING className;
	zCArray<zCObject *> list;

public:
	zCObjectList(const zSTRING &s) { className = s; }
};

class zCObjectRegistry
{
public:
	zCArray<zCObjectList *> lists;

public:
	zCObjectRegistry();
	~zCObjectRegistry();

	void Insert(const zSTRING &className, zCObject *obj);

	zCArray<zCObject *> &GetList(const zSTRING &className);

	template<typename O> zCArray<O *> &GetList()
	{
		return (zCArray<O *> &)GetList(O::className);
	}

	template<typename O> void DeleteList()
	{
		auto &list = GetList<O>();

		for (int32 i = 0; i < list.numInArray; i++)
		{
			zDELETE(list[i]);
		}

		list.EmptyList();
	}
};
