#pragma once

class zCVob
{
public:
	zCArray<zSTRING *> lines;
};

class oCWorld
{
public:
	zCBspTree *bspTree;
	zBOOL compiled;

	zSTRING dateStr;
	zSTRING userStr;
	zSTRING objectsStr;
	zSTRING worldStr;
	zSTRING meshAndBspStr;
	zSTRING vobTreeStr;
	zSTRING vobTreeChildsStr;
	zSTRING wayNetStr;

	zCArray<zCVob *> vobTree;
	zCArray<zSTRING *> wayNet;

public:
	oCWorld() { bspTree = NULL; compiled = FALSE; }
	~oCWorld() { zDELETE(bspTree); }

	zVOID WriteHeader(zCFileBIN &file);
	zBOOL ReadHeader(zCFileBIN &file);

	zVOID ReadVobTree(zCFileBIN &file);
	zVOID ReadWayNet(zCFileBIN &file);

	zVOID WriteVobTree(zCFileBIN &file);
	zVOID WriteWayNet(zCFileBIN &file);

	zBOOL LoadZEN(const zSTRING &fileName) { zCFileBIN f(fileName); return LoadZEN(f); }
	zBOOL LoadZEN(zCFileBIN &file);

	zVOID SaveZEN(const zSTRING &fileName) { zCFileBIN f(fileName, TRUE); SaveZEN(f); }
	zVOID SaveZEN(zCFileBIN &file);
};
