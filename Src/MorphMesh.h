#pragma once

class zCMorphMeshAni
{
public:
	zSTRING aniName;

	float blendInSpeed;
	float blendOutSpeed;
	float holdTime;
	int32 layer;
	float fpsRate;

	struct
	{
		uint8 discontinuity : 1;
		uint8 looping : 1;
		uint8 shape : 1;
		uint8 refShape : 1;
	} flags;

	int32 numVert;
	int32 *vertIndexList;

	int32 numFrames;
	zPOINT3 *vertPosMatrix;

public:
	zCMorphMeshAni()
	{
		vertIndexList = NULL;
		vertPosMatrix = NULL;

		memset(&flags, 0x00, sizeof(flags));
	}

	~zCMorphMeshAni()
	{
		zFREE(vertIndexList);
		zDELETE_ARRAY(vertPosMatrix);
	}

	void Load(zCFileBIN &file);
	void Save(zCFileBIN &file);
};

#define zFCHUNK_MMB_MORPHMESH ((uint16)0xE000)
#define zFCHUNK_MMB_SOURCE ((uint16)0xE010)
#define zFCHUNK_MMB_HEADER ((uint16)0xE020)
#define zFCHUNK_MMB_ANILIST ((uint16)0xE030)

class zCMorphMeshProto
{
public:
	zSTRING morphProtoName;
	zCProgMeshProto *morphRefMesh;
	zPOINT3 *morphRefMeshVertPos;

	uint16 numAni;
	zTSrcFileStats *srcFileStats;
	zCArray<zCMorphMeshAni *> aniList;

public:
	zCMorphMeshProto()
	{
		morphRefMesh = NULL;
		morphRefMeshVertPos = NULL;
		srcFileStats = NULL;
	}

	~zCMorphMeshProto()
	{
		zDELETE(morphRefMesh);
		zDELETE_ARRAY(morphRefMeshVertPos);
		zDELETE_ARRAY(srcFileStats);
	}

	bool32 LoadMMB(const char *fileName) { zSTRING s(fileName); return LoadMMB(s); }
	bool32 LoadMMB(const zSTRING &fileName) { zCFileBIN f(fileName); return LoadMMB(f); }
	bool32 LoadMMB(zCFileBIN &file);

	void SaveMMB(const char *fileName) { zSTRING s(fileName); SaveMMB(s); }
	void SaveMMB(const zSTRING &fileName) { zCFileBIN f(fileName, TRUE); SaveMMB(f); }
	void SaveMMB(zCFileBIN &file);
};
