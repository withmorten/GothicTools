#pragma once

#include "xxhash/xxhash.h"

#include "GothicTools.h"

#include "Archiver.h"

#define zOBJECT_DECLARATION(name) \
static const zSTRING className; \
virtual const zSTRING &GetClassName() { return name::className; }

#define zOBJECT_DEFINITION(name) \
const zSTRING name::className = # name

class zCObject
{
public:
	zOBJECT_DECLARATION(zCObject);

public:
	zTChunkRecord chunk;
	zCObject *ref; // i am a reference to this object
	zCArray<zCObject *> refs; // these objects reference me

	XXH64_hash_t hash; // for easy equality check

public:
	zCObject() { ref = NULL; hash = 0; }
	virtual ~zCObject() { };

	virtual bool32 Unarchive(zCArchiver &arc) { return TRUE; }
	virtual void Archive(zCArchiver &arc) { }
	virtual void Hash() { }

	bool32 IsNull() { return chunk.IsNull(); }
	bool32 IsReference() { return chunk.IsReference(); }
	bool32 IsEqual(zCObject *obj);
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
