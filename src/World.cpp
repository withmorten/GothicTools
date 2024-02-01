#include "World.h"

#include "Archiver.h"
#include "Bsp.h"
#include "Vob.h"
#include "WayNet.h"
#include "ObjectRegistry.h"

zOBJECT_DEFINITION(zCWorld);

zCWorld::zCWorld()
{
	bspTree = NULL;
	vobTree = NULL;
	wayNet = NULL;
	compiled = FALSE;
	vobTreeDepth = 0;
	registry = NULL;
}

zCWorld::~zCWorld()
{
	zDELETE(bspTree);

	for (int32 i = 0; i < vobs.numInArray; i++)
	{
		zDELETE(vobs[i]);
	}

	zDELETE(vobTree);
	zDELETE(wayNet);
	zDELETE(registry);
}

bool32 zCWorld::LoadZEN(zCArchiver &arc)
{
	registry = zNEW(zCObjectRegistry);
	arc.registry = registry;

	bool32 result = arc.ReadObject(this) != NULL;

	return result;
}

void zCWorld::SaveZEN(zCArchiver &arc)
{
	arc.WriteObject(this);
}

bool32 zCWorld::UnarchiveVobTree(zCArchiver &arc, zCVob *parent, int32 &numVobs)
{
	zCVob *vob;

	if (numVobs == 0)
	{
		vob = parent;
	}
	else
	{
		vob = (zCVob *)arc.ReadObject();

		if (!vob) return FALSE;

		parent->childs.Insert(vob);
		vobs.Insert(vob);

		vob->parent = parent;
		vob->depth = vobTreeDepth;

		vobTreeDepth++;
	}

	zSTRING n = numVobs;
	arc.ReadInt(zSTRING("childs" + n).ToChar(), vob->numChilds);

	numVobs++;

	for (int32 i = 0; i < vob->numChilds; i++)
	{
		if (!UnarchiveVobTree(arc, vob, numVobs)) return FALSE;

		vobTreeDepth--;
	}

	return TRUE;
}

bool32 zCWorld::Unarchive(zCArchiver &arc)
{
	if (!zCObject::Unarchive(arc)) return FALSE;

	zTChunkRecord chunk;

	arc.ReadChunkStart(chunk); // MeshAndBsp or VobTree

	if (chunk.name == "MeshAndBsp")
	{
		if (decompile)
		{
			compiled = FALSE;

			zCFileBIN file(arc.file);

			uint32 version;
			uint32 chunkLen;

			file.BinReadDWord(version);
			file.BinReadDWord(chunkLen);

			file.BinSeekRel(chunkLen);

			printf("Skipping BspTree\n");
		}
		else
		{
			compiled = TRUE;

			bspTree = zNEW(zCBspTree);

			if (!bspTree->LoadBIN(zCFileBIN(arc.file)))
			{
				printf("BspTree could not be loaded\n");

				return FALSE;
			}
		}

		if (!arc.ReadChunkEnd()) return FALSE; // MeshAndBsp

		arc.ReadChunkStart(chunk); // VobTree
	}

	vobTree = zNEW(zCVob);
	vobTree->chunk = chunk;

	int32 numVobs = 0;
	UnarchiveVobTree(arc, vobTree, numVobs);
	if (!arc.ReadChunkEnd()) return FALSE; // VobTree

	arc.ReadChunkStart(chunk); // WayNet or EndMarker

	if (chunk.name == "WayNet")
	{
		wayNet = (zCWayNet *)arc.ReadObject();

		if (!arc.ReadChunkEnd()) return FALSE; // WayNet

		arc.ReadChunkStart(chunk); // EndMarker
	}

	if (!arc.ReadChunkEnd()) return FALSE; // EndMarker

	return TRUE;
}

void zCWorld::ArchiveVobTree(zCArchiver &arc, zCVob *parent, int32 &numVobs)
{
	if (numVobs != 0)
	{
		arc.WriteObject(parent);
	}

	zSTRING n = numVobs;
	arc.WriteInt(zSTRING("childs" + n).ToChar(), parent->numChilds);

	numVobs++;

	for (int32 i = 0; i < parent->numChilds; i++)
	{
		ArchiveVobTree(arc, parent->childs[i], numVobs);
	}
}

void zCWorld::Archive(zCArchiver &arc)
{
	zCObject::Archive(arc);

	if (compiled)
	{
		arc.WriteChunkStart("MeshAndBsp");
		bspTree->SaveBIN(zCFileBIN(arc.file));
		arc.WriteChunkEnd();
	}

	arc.WriteChunkStart(vobTree->chunk);

	int32 numVobs = 0;
	ArchiveVobTree(arc, vobTree, numVobs);

	arc.WriteChunkEnd();

	if (wayNet)
	{
		arc.WriteChunkStart("WayNet");
		arc.WriteObject(wayNet);
		arc.WriteChunkEnd();
	}

	arc.WriteChunkStart("EndMarker");
	arc.WriteChunkEnd();
}

zCVob *zCWorld::GetVob(int32 objectIndex)
{
	for (int32 i = 0; i < vobs.numInArray; i++)
	{
		if (vobs[i]->chunk.objectIndex == objectIndex)
		{
			return vobs[i];
		}
	}

	return NULL;
}
