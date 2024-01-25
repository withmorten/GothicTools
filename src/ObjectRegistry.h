#pragma once

#include "GothicTools.h"

#include "Object.h"

class zCObjectMatches
{
public:
	class zCObjectMatch
	{
	public:
		int32 object1;
		int32 object2;
		zSTRING reason;
	};

	class zCWayMatch
	{
	public:
		int32 way1;
		int32 way2;
		zSTRING reason;
	};

public:
	zCArray<zCObjectMatch> objectMatches;
	zCArray<zCWayMatch> wayMatches;

public:
	void Load(const char *fileName);

	bool32 IsInObjectMatches1(int32 object1);
	bool32 IsInObjectMatches2(int32 object2);

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
};
