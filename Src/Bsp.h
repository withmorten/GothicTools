#pragma once

#define zFCHUNK_BSP ((uint16)0xC000)
#define zFCHUNK_BSP_POLYLIST ((uint16)0xC010)
#define zFCHUNK_BSP_TREE ((uint16)0xC040)
#define zFCHUNK_BSP_LEAF_LIGHT ((uint16)0xC045)
#define zFCHUNK_BSP_OUTDOOR_SECTORS ((uint16)0xC050)
#define zFCHUNK_BSP_END ((uint16)0xC0FF)

#define zNODE_UNDEFINED 0
#define zNODE_FRONT 1
#define zNODE_BACK 2
#define zNODE_FRONT_LEAF 4
#define zNODE_BACK_LEAF 8

class zCBspTree;
class zCBspNode;
class zCBspLeaf;

enum zTBspNodeType
{
	zBSP_NODE,
	zBSP_LEAF,
};

class zCBspBase
{
public:
	zCBspNode *parent;
	zTBspNodeType nodeType;
	zTBBox3D bbox3D;

	uint32 treePolyIndex; // index to bspTree->treePolyIndices 
	int32 numPolys;

public:
	zCBspBase() { parent = NULL; numPolys = 0; treePolyIndex = 0; }

	bool32 IsLeaf() { return nodeType == zBSP_LEAF; }
	bool32 IsNode() { return nodeType == zBSP_NODE; }
	void LoadBINRec(zCFileBIN &file);
	void SaveBINRec(zCFileBIN &file);
};

class zCBspNode : public zCBspBase
{
public:
	zCBspBase *front;
	zCBspBase *back;

	zCBspLeaf *leafList;
	int32 numLeafs;

	zTPlane plane;

	byte hasLod;

public:
	zCBspNode() { nodeType = zBSP_NODE; front = NULL; back = NULL; leafList = NULL; numLeafs = 0; hasLod = FALSE; }
};

#define zBSP_LEAF_LIGHT_UNDEFINED ((zREAL)-99)

class zCBspLeaf : public zCBspBase
{
public:
	zPOINT3 lightPosition;

public:
	zCBspLeaf() { nodeType = zBSP_LEAF; lightPosition = zVEC3(zBSP_LEAF_LIGHT_UNDEFINED); }
};

enum zTBspTreeMode
{
	zBSP_MODE_INDOOR,
	zBSP_MODE_OUTDOOR,
};

class zCBspSector
{
public:
	zSTRING sectorName;

	int32 numSectorNodes;
	uint32 *leafIndices;

	int32 numSectorPortals;
	uint32 *sectorIndices; // indices to mesh->polyArray

public:
	zCBspSector() { leafIndices = NULL; sectorIndices = NULL; numSectorNodes = 0; numSectorPortals = 0; }
	~zCBspSector() { zFREE(leafIndices); zFREE(sectorIndices); }
};

class zCBspTree
{
public:
	static zCBspTree *actBspTree;
	static zCBspNode *actNodePtr;
	static zCBspLeaf *actLeafPtr;

	zTBspTreeMode bspTreeMode;

	zCBspBase *bspRoot;
	zCMesh *mesh;

	int32 numPolys;
	uint32 *treePolyIndices; // indices to mesh->polyArray

	int32 numNodes;
	zCBspNode *nodeList;

	int32 numLeafs;
	zCBspLeaf *leafList;

	int32 numPortals;
	uint32 *portalIndices; // indices to mesh->polyArray, indoor only

	int32 numSectors;
	zCBspSector *sectorList;

public:
	zCBspTree();
	~zCBspTree();

	void LODDegenerate();

	bool32 LoadBIN(const zSTRING &fileName) { zCFileBIN f(fileName); return LoadBIN(f); }
	bool32 LoadBIN(zCFileBIN &file);

	bool32 SaveBIN(const zSTRING &fileName) { zCFileBIN f(fileName, TRUE); return SaveBIN(f); }
	bool32 SaveBIN(zCFileBIN &file);
};
