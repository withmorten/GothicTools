#include "World.h"

#include "Archiver.h"
#include "Bsp.h"
#include "Vob.h"
#include "WayNet.h"

zCWorld::zCWorld()
{
	bspTree = NULL;
	vobTree = NULL;
	compiled = FALSE;
}

zCWorld::~zCWorld()
{
	zDELETE(bspTree);
	zDELETE(vobTree);
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
	}

	zSTRING n = numVobs;
	arc.ReadInt(zSTRING("childs" + n).ToChar(), vob->numChilds);

	numVobs++;

	for (int32 i = 0; i < vob->numChilds; i++)
	{
		if (!UnarchiveVobTree(arc, vob, numVobs)) return FALSE;
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
		compiled = TRUE;

		bspTree = zNEW(zCBspTree);

		if (!bspTree->LoadBIN(zCFileBIN(arc.GetFile())))
		{
			printf("BspTree could not be loaded\n");

			return FALSE;
		}

		if (!arc.ReadChunkEnd()) return FALSE; // MeshAndBsp

		arc.ReadChunkStart(chunk); // VobTree
	}

	vobTree = zNEW(zCVob);
	vobTree->chunk = chunk;

	int32 numVobs = 0;
	UnarchiveVobTree(arc, vobTree, numVobs);
	if (!arc.ReadChunkEnd()) return FALSE; // VobTree

	arc.ReadChunkStart(chunk); // WayNet
	wayNet = (zCWayNet *)arc.ReadObject();
	if (!arc.ReadChunkEnd()) return FALSE; // WayNet

	arc.ReadChunkStart(chunk); // EndMarker
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
		bspTree->SaveBIN(zCFileBIN(arc.GetFile()));
		arc.WriteChunkEnd();
	}

	arc.WriteChunkStart(vobTree->chunk);

	int32 numVobs = 0;
	ArchiveVobTree(arc, vobTree, numVobs);

	arc.WriteChunkEnd();

	arc.WriteChunkStart("WayNet");
	arc.WriteObject(wayNet);
	arc.WriteChunkEnd();

	arc.WriteChunkStart("EndMarker");
	arc.WriteChunkEnd();
}

bool32 zCWorld::LoadZEN(zCArchiver &arc)
{
	return arc.ReadObject(this) != NULL;
}

void zCWorld::SaveZEN(zCArchiver &arc)
{
	arc.WriteObject(this);
}
