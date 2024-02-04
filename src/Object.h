#pragma once

#include "xxhash/xxhash.h"

#include "GothicTools.h"

#include "Archiver.h"

#define zOBJECT_DECLARATION(name) \
static const zSTRING className; \
virtual const zSTRING &GetClassName() { return name::className; } \

#define zOBJECT_DEFINITION(name) \
const zSTRING name::className = # name

class zCObject
{
public:
	zOBJECT_DECLARATION(zCObject);

public:
	uint16 classVersion;
	int32 objectIndex; // only for reading and diffing, not for writing
	zSTRING classHierarchy;

	XXH64_hash_t hash; // for easy equality check
	bool32 found; // for checking if this object was previously found ...

public:
	zCObject() { hash = 0; found = FALSE; }
	virtual ~zCObject() { };

	virtual bool32 Unarchive(zCArchiver &arc) { return TRUE; }
	virtual void Archive(zCArchiver &arc) { }
	virtual void Hash();
	virtual bool32 IsEqual(zCObject *obj); // if hash equal has failed - also has special logic for some classes

	bool32 IsHashEqual(zCObject *obj);
};

class zCAIBase : public zCObject
{
public:
	zOBJECT_DECLARATION(zCAIBase);
};

class zCAICamera : public zCAIBase
{
public:
	zOBJECT_DECLARATION(zCAICamera);
};
