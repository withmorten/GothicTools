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

	void WriteHeader(zCFileBIN &file);
	zBOOL ReadHeader(zCFileBIN &file);

	void ReadVobTree(zCFileBIN &file);
	void ReadWayNet(zCFileBIN &file);

	void WriteVobTree(zCFileBIN &file);
	void WriteWayNet(zCFileBIN &file);

	zBOOL LoadZEN(const zSTRING &fileName) { zCFileBIN f(fileName); return LoadZEN(f); }
	zBOOL LoadZEN(zCFileBIN &file);

	void SaveZEN(const zSTRING &fileName) { zCFileBIN f(fileName, TRUE); SaveZEN(f); }
	void SaveZEN(zCFileBIN &file);
};
