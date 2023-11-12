#pragma once

#include "GothicTools.h"

#include "Object.h"
//#include "Vob.h"
//#include "Visual.h"
//#include "Model.h"
//#include "ModelAni.h"
//#include "MorphMesh.h"
//#include "ProgMesh.h"

class zCObjectList
{
public:
	zSTRING className;
	zCArray<zCObject *> list;

public:
	zCObjectList(const zSTRING &s) { className = s; }
};

// TODO does this need to be sorted by name?
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
