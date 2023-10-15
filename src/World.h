#pragma once

#include "GothicTools.h"

#include "Object.h"

class zCBspTree;
class zCVob;
class zCWayNet;

class zCWorld : public zCObject
{
public:
	static const zSTRING GetClassName() { return "zCWorld"; }

public:
	zCBspTree *bspTree;
	bool32 compiled;

	zCVob *vobTree;
	zCWayNet *wayNet;

public:
	zCWorld();
	~zCWorld();

	bool32 UnarchiveVobTree(zCArchiver &arc, zCVob *parent, int32 &numVobs);
	bool32 Unarchive(zCArchiver &arc);

	void ArchiveVobTree(zCArchiver &arc, zCVob *parent, int32 &numVobs);
	void Archive(zCArchiver &arc);

	bool32 LoadZEN(zCArchiver &arc);
	void SaveZEN(zCArchiver &arc);
};
