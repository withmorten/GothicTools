#pragma once

#define HANDLE_MSB_CHILD_FILES 0

#define zFCHUNK_MODELSCRIPT ((uint16)0xF000)
#define zFCHUNK_MDS_SOURCE ((uint16)0xF100)
#define zFCHUNK_MDS_MODEL ((uint16)0xF200)
#define zFCHUNK_MDS_MESHANDTREE ((uint16)0xF300)
#define zFCHUNK_MDS_REGISTERMESH ((uint16)0xF400)
#define zFCHUNK_MDS_ANIENUM ((uint16)0xF500)
#define zFCHUNK_MDS_ANIMAXFPS ((uint16)0xF510)
#define zFCHUNK_MDS_ANI ((uint16)0xF520)
#define zFCHUNK_MDS_ANIALIAS ((uint16)0xF530)
#define zFCHUNK_MDS_ANIBLEND ((uint16)0xF540)
#define zFCHUNK_MDS_ANISYNC ((uint16)0xF550)
#define zFCHUNK_MDS_ANIBATCH ((uint16)0xF560)
#define zFCHUNK_MDS_ANICOMB ((uint16)0xF570)
#define zFCHUNK_MDS_ANIDISABLE ((uint16)0xF580)
#define zFCHUNK_MDS_MODELTAG ((uint16)0xF590)
#define zFCHUNK_MDS_ANIEVENTS ((uint16)0xF5A0)
#define zFCHUNK_MDS_EVENTSFX ((uint16)0xF5A1)
#define zFCHUNK_MDS_EVENTSFXGRND ((uint16)0xF5A2)
#define zFCHUNK_MDS_EVENTTAG ((uint16)0xF5A3)
#define zFCHUNK_MDS_EVENTPFX ((uint16)0xF5A4)
#define zFCHUNK_MDS_EVENTPFXSTOP ((uint16)0xF5A5)
#define zFCHUNK_MDS_EVENTPFXGRND ((uint16)0xF5A6)
#define zFCHUNK_MDS_EVENTSETMESH ((uint16)0xF5A7)
#define zFCHUNK_MDS_EVENTSWAPMESH ((uint16)0xF5A8)
#define zFCHUNK_MDS_EVENTMMSTARTANI ((uint16)0xF5A9)
#define zFCHUNK_MDS_EVENTCAMTREMOR ((uint16)0xF5AA)
#define zFCHUNK_MDS_ANIEVENTS_END ((uint16)0xF5AF)
#define zFCHUNK_MDS_ANIENUM_END ((uint16)0xF5FF)
#define zFCHUNK_MDS_MODEL_END ((uint16)0xF2FF)
#define zFCHUNK_MDS_END ((uint16)0xFFFF)

#define zFCHUNK_MODELHIERARCHY ((uint16)0xD100)
#define zFCHUNK_MDH_SOURCE ((uint16)0xD110)
#define zFCHUNK_MDH_END ((uint16)0xD120)

#define zFCHUNK_MODELMESH ((uint16)0xD000)
#define zFCHUNK_MDM_SOURCE ((uint16)0xD010)
#define zFCHUNK_MDM_NODEMESHES ((uint16)0xD020)
#define zFCHUNK_MDM_SOFSKINLIST ((uint16)0xD030)
#define zFCHUNK_MDM_END ((uint16)0xD040)

#define zFCHUNK_MESHSOFTSKIN ((uint16)0xE100)
#define zFCHUNK_MESHSOFTSKIN_END ((uint16)0xE110)

class zCModelAni;
class zCModelAniEvent;

class zCModelNode
{
public:
	zSTRING nodeName;

	uint16 parentIndex;
	zMATRIX4 trafo;
};

class zCModelHierarchy
{
public:
	zSTRING modelHierarchyName;
	zTSrcFileStats srcFileStats;

	uint16 numNodes;
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

	void SetModelHierarchyName(const zSTRING &name) { modelHierarchyName = name; modelHierarchyName.Upper(); }

	bool32 LoadMDH(const zSTRING &filename) { zCFileBIN f(filename); return LoadMDH(f); }
	bool32 LoadMDH(zCFileBIN &file);

	void SaveMDH(const zSTRING &filename) { zCFileBIN f(filename, TRUE); SaveMDH(f); }
	void SaveMDH(zCFileBIN &file);
};

struct zTNodeWedgeNormal
{
	zVEC3 normal;
	int32 nodeIndex;
};

class zCMeshSoftSkin : public zCProgMeshProto
{
public:
	zDWORD size;
	byte *vertWeightStream;

	zDWORD numNodeWedgeNormals;
	zTNodeWedgeNormal *nodeWedgeNormalList;

	uint16 numNodes;
	int32 *nodeIndexList;

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

	bool32 Load(zCFileBIN &file);
	void Save(zCFileBIN &file);
};

class zCModelMesh
{
public:
	zTSrcFileStats srcFileStats;
	zDWORD checkSum;

	uint16 numNodeMeshes;
	zCArray<zSTRING> nodeMeshNames;
	zCProgMeshProto *nodeMeshList;

	uint16 numSoftSkins;
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

	bool32 LoadMDM(const zSTRING &filename) { zCFileBIN f(filename); return LoadMDM(f); }
	bool32 LoadMDM(zCFileBIN &file);

	void SaveMDM(const zSTRING &filename) { zCFileBIN f(filename, TRUE); SaveMDM(f); }
	void SaveMDM(zCFileBIN &file);
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
	bool32 writeMaxFps;

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

	void SetModelProtoName(const zSTRING &name) { modelProtoName = name; modelProtoName.Upper(); }

	bool32 ReadAniEventsMSB(zCFileBIN &file, zCModelAni *aniPtr);
	bool32 ReadAniEnumMSB(zCFileBIN &file);
	bool32 ReadModelMSB(zCFileBIN &file);

	bool32 LoadModelScriptMSB(const zSTRING &filename) { zCFileBIN f(filename); return LoadModelScriptMSB(f); }
	bool32 LoadModelScriptMSB(zCFileBIN &file);

	void ReadAniEvents(zFILE &file, zCModelAni *aniPtr);
	void ReadAniEnum(zFILE &file);
	void ReadModel(zFILE &file);

	bool32 LoadModelScript(const zSTRING &filename) { zFILE f(filename); return LoadModelScript(f); }
	bool32 LoadModelScript(zFILE &file);

	void WriteAniEventsMSB(zCFileBIN &file, zCModelAni *aniPtr);
	void WriteAniEnumMSB(zCFileBIN &file);
	void WriteModelMSB(zCFileBIN &file);

	void SaveModelScriptMSB(const zSTRING &filename) { zCFileBIN f(filename, TRUE); SaveModelScriptMSB(f); }
	void SaveModelScriptMSB(zCFileBIN &file);

	void WriteAniEvents(zFILE &file, zCModelAni *aniPtr);
	void WriteAniEnum(zFILE &file);
	void WriteModel(zFILE &file);

	void SaveModelScript(const zSTRING &filename) { zFILE f(filename, TRUE); SaveModelScript(f); }
	void SaveModelScript(zFILE &file);

	bool32 LoadMDL(const zSTRING &filename) { zCFileBIN f(filename); return LoadMDL(f); }
	bool32 LoadMDL(zCFileBIN &file);

	void SaveMDL(const zSTRING &filename) { zCFileBIN f(filename, TRUE); SaveMDL(f); }
	void SaveMDL(zCFileBIN &file);
};
