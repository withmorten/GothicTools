#pragma once

#include "GothicTools.h"

#include "Object.h"

class zCBspTree;
class zCVob;
class zCWayNet;
class zCObjectRegistry;

class zCWorld : public zCObject
{
public:
	zOBJECT_DECLARATION(zCWorld);

public:
	zCBspTree *bspTree;
	bool32 compiled;

	zCVob *vobTree;
	zCWayNet *wayNet;

	int32 vobTreeDepth; // for testing if traversal was done correctly TODO remove?
	zCArray<zCVob *> vobs; // for easy access that doesn't require recursion

	zCObjectRegistry *registry;

public:
	zCWorld();
	~zCWorld();

	bool32 LoadZEN(zCArchiver &arc);
	void SaveZEN(zCArchiver &arc);

	bool32 UnarchiveVobTree(zCArchiver &arc, zCVob *parent, int32 &numVobs);
	bool32 Unarchive(zCArchiver &arc);

	void ArchiveVobTree(zCArchiver &arc, zCVob *parent, int32 &numVobs);
	void Archive(zCArchiver &arc);

	zCVob *GetVob(int32 objectIndex);
};
