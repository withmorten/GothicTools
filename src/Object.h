#pragma once

#include "GothicTools.h"

#include "Archiver.h"

class zCObject
{
public:
	static const zSTRING GetClassName() { return "zCObject"; }

public:
	zTChunkRecord chunk;
	zCObject *ref; // TODO actually fill the object with values from reference, but this has to do for now

public:
	zCObject() { ref = NULL; }
	virtual ~zCObject() { };

	virtual bool32 Unarchive(zCArchiver &arc) { return TRUE; }
	virtual void Archive(zCArchiver &arc) { }

	bool32 IsNull() { return chunk.IsNull(); }
	bool32 IsReference() { return chunk.IsReference(); }
};

class zCAIBase : public zCObject
{
public:
	static const zSTRING GetClassName() { return "zCAIBase"; }
};

class zCAICamera : public zCAIBase
{
public:
	static const zSTRING GetClassName() { return "zCAICamera"; }
};
