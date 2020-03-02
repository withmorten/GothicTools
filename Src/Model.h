#pragma once

#define HANDLE_MSB_CHILD_FILES 0

#define zFCHUNK_MODELSCRIPT ((zWORD)0xF000)
#define zFCHUNK_MDS_SOURCE ((zWORD)0xF100)
#define zFCHUNK_MDS_MODEL ((zWORD)0xF200)
#define zFCHUNK_MDS_MESHANDTREE ((zWORD)0xF300)
#define zFCHUNK_MDS_REGISTERMESH ((zWORD)0xF400)
#define zFCHUNK_MDS_ANIENUM ((zWORD)0xF500)
#define zFCHUNK_MDS_ANIMAXFPS ((zWORD)0xF510)
#define zFCHUNK_MDS_ANI ((zWORD)0xF520)
#define zFCHUNK_MDS_ANIALIAS ((zWORD)0xF530)
#define zFCHUNK_MDS_ANIBLEND ((zWORD)0xF540)
#define zFCHUNK_MDS_ANISYNC ((zWORD)0xF550)
#define zFCHUNK_MDS_ANIBATCH ((zWORD)0xF560)
#define zFCHUNK_MDS_ANICOMB ((zWORD)0xF570)
#define zFCHUNK_MDS_ANIDISABLE ((zWORD)0xF580)
#define zFCHUNK_MDS_MODELTAG ((zWORD)0xF590)
#define zFCHUNK_MDS_ANIEVENTS ((zWORD)0xF5A0)
#define zFCHUNK_MDS_EVENTSFX ((zWORD)0xF5A1)
#define zFCHUNK_MDS_EVENTSFXGRND ((zWORD)0xF5A2)
#define zFCHUNK_MDS_EVENTTAG ((zWORD)0xF5A3)
#define zFCHUNK_MDS_EVENTPFX ((zWORD)0xF5A4)
#define zFCHUNK_MDS_EVENTPFXSTOP ((zWORD)0xF5A5)
#define zFCHUNK_MDS_EVENTPFXGRND ((zWORD)0xF5A6)
#define zFCHUNK_MDS_EVENTSETMESH ((zWORD)0xF5A7)
#define zFCHUNK_MDS_EVENTSWAPMESH ((zWORD)0xF5A8)
#define zFCHUNK_MDS_EVENTMMSTARTANI ((zWORD)0xF5A9)
#define zFCHUNK_MDS_EVENTCAMTREMOR ((zWORD)0xF5AA)
#define zFCHUNK_MDS_ANIEVENTS_END ((zWORD)0xF5AF)
#define zFCHUNK_MDS_ANIENUM_END ((zWORD)0xF5FF)
#define zFCHUNK_MDS_MODEL_END ((zWORD)0xF2FF)
#define zFCHUNK_MDS_END ((zWORD)0xFFFF)

#define zFCHUNK_MODELHIERARCHY ((zWORD)0xD100)
#define zFCHUNK_MDH_SOURCE ((zWORD)0xD110)
#define zFCHUNK_MDH_END ((zWORD)0xD120)

#define zFCHUNK_MODELMESH ((zWORD)0xD000)
#define zFCHUNK_MDM_SOURCE ((zWORD)0xD010)
#define zFCHUNK_MDM_NODEMESHES ((zWORD)0xD020)
#define zFCHUNK_MDM_SOFSKINLIST ((zWORD)0xD030)
#define zFCHUNK_MDM_END ((zWORD)0xD040)

#define zFCHUNK_MESHSOFTSKIN ((zWORD)0xE100)
#define zFCHUNK_MESHSOFTSKIN_END ((zWORD)0xE110)

class zCModelAni;
class zCModelAniEvent;

class zCModelNode
{
public:
	zSTRING nodeName;

	zWORD parentIndex;
	zMATRIX4 trafo;
};

class zCModelHierarchy
{
public:
	zSTRING modelHierarchyName;
	zTSrcFileStats srcFileStats;

	zWORD numNodes;
	zCModelNode *nodeList;

	zTBBox3D bbox3D;
	zTBBox3D bbox3DCollDet;

	zPOINT3 rootNodeTrans;

	zDWORD nodeListChecksum;

public:
	zCModelHierarchy()
	{
		numNodes = 0;
		nodeList = NULL;
	}

	~zCModelHierarchy()
	{
		zDELETE_ARRAY(nodeList);
	}

	zVOID SetModelHierarchyName(const zSTRING &name) { modelHierarchyName = name; modelHierarchyName.Upper(); }

	zBOOL LoadMDH(const zSTRING &filename) { zCFileBIN f(filename); return LoadMDH(f); }
	zBOOL LoadMDH(zCFileBIN &file);

	zVOID SaveMDH(const zSTRING &filename) { zCFileBIN f(filename, TRUE); SaveMDH(f); }
	zVOID SaveMDH(zCFileBIN &file);
};

struct zTNodeWedgeNormal
{
	zVEC3 normal;
	zINT nodeIndex;
};

class zCMeshSoftSkin : public zCProgMeshProto
{
public:
	zDWORD size;
	zBYTE *vertWeightStream;

	zDWORD numNodeWedgeNormals;
	zTNodeWedgeNormal *nodeWedgeNormalList;

	zWORD numNodes;
	zINT *nodeIndexList;

	zCArray<zCOBBox3D *> nodeOBBList;

public:
	zCMeshSoftSkin()
	{
		size = 0;
		numNodeWedgeNormals = 0;
		numNodes = 0;

		vertWeightStream = NULL;
		nodeWedgeNormalList = NULL;
		nodeIndexList = NULL;
	}

	~zCMeshSoftSkin()
	{
		zFREE(vertWeightStream);
		zFREE(nodeWedgeNormalList);
		zFREE(nodeIndexList);
	}

	zBOOL Load(zCFileBIN &file);
	zVOID Save(zCFileBIN &file);
};

class zCModelMesh
{
public:
	zTSrcFileStats srcFileStats;
	zDWORD checkSum;

	zWORD numNodeMeshes;
	zCArray<zSTRING> nodeMeshNames;
	zCProgMeshProto *nodeMeshList;

	zWORD numSoftSkins;
	zCMeshSoftSkin *softSkinList;

public:
	zCModelMesh()
	{
		numNodeMeshes = 0;
		numSoftSkins = 0;
		nodeMeshList = NULL;
		softSkinList = NULL;
	}

	~zCModelMesh()
	{
		zDELETE_ARRAY(nodeMeshList);
		zDELETE_ARRAY(softSkinList);
	}

	zBOOL LoadMDM(const zSTRING &filename) { zCFileBIN f(filename); return LoadMDM(f); }
	zBOOL LoadMDM(zCFileBIN &file);

	zVOID SaveMDM(const zSTRING &filename) { zCFileBIN f(filename, TRUE); SaveMDM(f); }
	zVOID SaveMDM(zCFileBIN &file);
};

const zREAL zMDL_MAX_FPS = 25.0f;

const zREAL zMDL_DEFAULT_RADIUS_EVENTSFXGRND = 1000.0f;
const zREAL zMDL_DEFAULT_RADIUS_EVENTSFX = 2500.0f;
const zREAL zMDL_MAX_RADIUS_EVENTSFX = 3500.0f;

class zCModelPrototype
{
public:
	zSTRING modelProtoName;
	zSTRING ascPath;
	zTSrcFileStats srcFileStats;

	zREAL maxFps;
	zBOOL writeMaxFps;

	zDWORD dontUseMesh;
	zSTRING ascName;

	zCArray<zSTRING> ascNames;

#if HANDLE_MSB_CHILD_FILES
	zCArray<zCModelMesh *> meshList;
	zCArray<zCModelAni *> modelAnis;
#endif

	zCArray<zCModelAni *> protoAnis;
	zCArray<zCModelAniEvent *> modelEvents;

	zCModelHierarchy *hierarchy;
	zCModelMesh *mesh;

	zUINT numTabs;

public:
	zCModelPrototype()
	{
		writeMaxFps = FALSE;
		hierarchy = NULL;
		mesh = NULL;

		maxFps = zMDL_MAX_FPS;

		numTabs = 0;
	}

	~zCModelPrototype()
	{
		zDELETE(hierarchy);
		zDELETE(mesh);
	}

	zVOID SetModelProtoName(const zSTRING &name) { modelProtoName = name; modelProtoName.Upper(); }

	zBOOL ReadAniEventsMSB(zCFileBIN &file, zCModelAni *aniPtr);
	zBOOL ReadAniEnumMSB(zCFileBIN &file);
	zBOOL ReadModelMSB(zCFileBIN &file);

	zBOOL LoadModelScriptMSB(const zSTRING &filename) { zCFileBIN f(filename); return LoadModelScriptMSB(f); }
	zBOOL LoadModelScriptMSB(zCFileBIN &file);

	zVOID ReadAniEvents(zFILE &file, zCModelAni *aniPtr);
	zVOID ReadAniEnum(zFILE &file);
	zVOID ReadModel(zFILE &file);

	zBOOL LoadModelScript(const zSTRING &filename) { zFILE f(filename); return LoadModelScript(f); }
	zBOOL LoadModelScript(zFILE &file);

	zVOID WriteAniEventsMSB(zCFileBIN &file, zCModelAni *aniPtr);
	zVOID WriteAniEnumMSB(zCFileBIN &file);
	zVOID WriteModelMSB(zCFileBIN &file);

	zVOID SaveModelScriptMSB(const zSTRING &filename) { zCFileBIN f(filename, TRUE); SaveModelScriptMSB(f); }
	zVOID SaveModelScriptMSB(zCFileBIN &file);

	zVOID WriteAniEvents(zFILE &file, zCModelAni *aniPtr);
	zVOID WriteAniEnum(zFILE &file);
	zVOID WriteModel(zFILE &file);

	zVOID SaveModelScript(const zSTRING &filename) { zFILE f(filename, TRUE); SaveModelScript(f); }
	zVOID SaveModelScript(zFILE &file);

	zBOOL LoadMDL(const zSTRING &filename) { zCFileBIN f(filename); return LoadMDL(f); }
	zBOOL LoadMDL(zCFileBIN &file);

	zVOID SaveMDL(const zSTRING &filename) { zCFileBIN f(filename, TRUE); SaveMDL(f); }
	zVOID SaveMDL(zCFileBIN &file);
};
