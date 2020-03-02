#pragma once

#define zFCHUNK_BSP ((zWORD)0xC000)
#define zFCHUNK_BSP_POLYLIST ((zWORD)0xC010)
#define zFCHUNK_BSP_TREE ((zWORD)0xC040)
#define zFCHUNK_BSP_LEAF_LIGHT ((zWORD)0xC045)
#define zFCHUNK_BSP_OUTDOOR_SECTORS ((zWORD)0xC050)
#define zFCHUNK_BSP_END ((zWORD)0xC0FF)

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

	zDWORD treePolyIndex; // index to bspTree->treePolyIndices 
	zINT numPolys;

public:
	zCBspBase() { parent = NULL; numPolys = 0; treePolyIndex = 0; }

	zBOOL IsLeaf() { return nodeType == zBSP_LEAF; }
	zBOOL IsNode() { return nodeType == zBSP_NODE; }
	zVOID LoadBINRec(zCFileBIN &file);
	zVOID SaveBINRec(zCFileBIN &file);
};

class zCBspNode : public zCBspBase
{
public:
	zCBspBase *front;
	zCBspBase *back;

	zCBspLeaf *leafList;
	zINT numLeafs;

	zTPlane plane;

	zBYTE hasLod;

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

	zINT numSectorNodes;
	zDWORD *leafIndices;

	zINT numSectorPortals;
	zDWORD *sectorIndices; // indices to mesh->polyArray

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

	zINT numPolys;
	zDWORD *treePolyIndices; // indices to mesh->polyArray

	zINT numNodes;
	zCBspNode *nodeList;

	zINT numLeafs;
	zCBspLeaf *leafList;

	zINT numPortals;
	zDWORD *portalIndices; // indices to mesh->polyArray, indoor only

	zINT numSectors;
	zCBspSector *sectorList;

public:
	zCBspTree();
	~zCBspTree();

	zVOID LODDegenerate();

	zBOOL LoadBIN(const zSTRING &fileName) { zCFileBIN f(fileName); return LoadBIN(f); }
	zBOOL LoadBIN(zCFileBIN &file);

	zBOOL SaveBIN(const zSTRING &fileName) { zCFileBIN f(fileName, TRUE); return SaveBIN(f); }
	zBOOL SaveBIN(zCFileBIN &file);
};
