#pragma once

struct zTBBox3D
{
	zPOINT3 mins;
	zPOINT3 maxs;

public:
	void SaveBIN(zCFileBIN &file) { file.BinWrite(this, sizeof(zTBBox3D)); }
	void LoadBIN(zCFileBIN &file) { file.BinRead(this, sizeof(zTBBox3D)); }
};

class zCOBBox3D
{
public:
	zVEC3 center;
	zVEC3 axis[3];
	zVEC3 extent;

	uint16 numChilds;
	zCArray<zCOBBox3D *> childs;

public:
	void SaveBIN(zCFileBIN &file)
	{
		file.BinWrite(&center, sizeof(center));
		file.BinWrite(&axis, sizeof(axis));
		file.BinWrite(&extent, sizeof(extent));

		file.BinWriteWord(numChilds);

		for (uint16 i = 0; i < numChilds; i++)
		{
			zCOBBox3D *obb = childs[i];
			obb->SaveBIN(file);
		}
	}

	void LoadBIN(zCFileBIN &file)
	{
		file.BinRead(&center, sizeof(center));
		file.BinRead(&axis, sizeof(axis));
		file.BinRead(&extent, sizeof(extent));

		file.BinReadWord(numChilds);
		childs.AllocAbs(numChilds);

		for (uint16 i = 0; i < numChilds; i++)
		{
			zCOBBox3D *newOBB = zNEW(zCOBBox3D);
			childs.Insert(newOBB);
			newOBB->LoadBIN(file);
		}
	}
};

struct zTSrcFileStats
{
	zDATE date;
	zSTRING fileName;

	void SaveBIN(zCFileBIN &file) { file.BinWrite(&date, sizeof(date)); file.BinWriteLine(fileName); }
	void LoadBIN(zCFileBIN &file) { file.BinRead(&date, sizeof(date)); file.BinReadLine(fileName); }
};

struct zTPlane
{
	float distance;
	zPOINT3 normal;
};

class zCVertex
{
public:
	zPOINT3 position;
};

class zCVertFeature
{
public:
	zPOINT3 vertNormal;
	zCOLOR lightStat;
	float texu;
	float texv;
};

typedef uint32 zTVertIndex;
typedef uint16 zTVertIndexG1;
typedef uint32 zTVertIndexGRM;
typedef uint32 zTVertIndexG2;
typedef uint32 zTFeatIndex;

#define zMAX_INDEX_NUM 255

#pragma pack (push, 1)
struct TFlags // unified flags
{
	uint8 portalPoly : 2;
	uint8 occluder : 1;
	uint8 sectorPoly : 1;
	uint8 mustRelight : 1;
	// uint8 lodFlag : 1; // G1, possibly exactly the same as mustRelight, just renamed
	uint8 portalIndoorOutdoor : 1;
	uint8 ghostOccluder : 1;
	uint8 noDynLightNear : 1;
	uint8 normalMainAxis : 2; // G1
	uint16 sectorIndex : 16;
};

struct TFlagsG1
{
	uint8 portalPoly : 2; // tType
	uint8 occluder : 1; // tOccluder
	uint8 sectorPoly : 1; // tIndoor
	uint8 lodFlag : 1; // tLODGenerate
	uint8 portalIndoorOutdoor : 1; // tPortalWithoutMat
	uint8 ghostOccluder : 1; // tGhost
	uint8 normalMainAxis : 2; // m_nMainAxis
	uint16 sectorIndex : 16; // m_nSectorIndex
};

struct TFlagsG2
{
	uint8 portalPoly : 2;
	uint8 occluder : 1;
	uint8 sectorPoly : 1;
	uint8 mustRelight : 1;
	uint8 portalIndoorOutdoor : 1;
	uint8 ghostOccluder : 1;
	uint8 noDynLightNear : 1;
	uint16 sectorIndex : 16;
};

struct zTMSH_FeatureChunk
{
	float texu;
	float texv;
	zCOLOR lightStat;
	zVEC3 vertNormal;
};

struct zTMSH_PolyChunk // unified chunk
{
	int16 matIndex;
	int16 lightmapIndex;
	zTPlane polyPlane;
	TFlags flags;
	byte polyNumVert;
};

struct zTIndex // unified chunk
{
	zTVertIndex vertIndex;
	zTFeatIndex featIndex;
};

struct zTMSH_PolyReadChunk : zTMSH_PolyChunk // unified chunk
{
	zTIndex indexList[zMAX_INDEX_NUM];
};

struct zTMSH_PolyChunkG1
{
	int16 matIndex;
	int16 lightmapIndex;
	zTPlane polyPlane;
	TFlagsG1 flags;
	byte polyNumVert;
};

struct zTIndexG1
{
	zTVertIndexG1 vertIndex;
	zTFeatIndex featIndex;
};

struct zTMSH_PolyReadChunkG1 : zTMSH_PolyChunkG1
{
	zTIndexG1 indexList[zMAX_INDEX_NUM];
};

struct zTIndexGRM
{
	zTVertIndexGRM vertIndex;
	zTFeatIndex featIndex;
};

struct zTMSH_PolyReadChunkGRM : zTMSH_PolyChunkG1
{
	zTIndexGRM indexList[zMAX_INDEX_NUM];
};

struct zTMSH_PolyChunkG2
{
	int16 matIndex;
	int16 lightmapIndex;
	zTPlane polyPlane;
	TFlagsG2 flags;
	byte polyNumVert;
};

struct zTIndexG2
{
	zTVertIndexG2 vertIndex;
	zTFeatIndex featIndex;
};

struct zTMSH_PolyReadChunkG2 : zTMSH_PolyChunkG2
{
	zTIndexG2 indexList[255];
};
#pragma pack (pop)

class zCPolygon
{
public:
	zTPlane polyPlane;
	zCMaterial *material;

	byte numVert;
	int16 lightmapIndex;
	TFlags flags;

	zTIndex *indexList;

public:
	zCPolygon()
	{
		material = NULL;
		numVert = 0;
		indexList = NULL;
		lightmapIndex = -1;
		memset(&flags, 0x00, sizeof(flags));
	}

	~zCPolygon()
	{
		// material gets freed from the matList in the mesh
		zFREE(indexList);
	}
};
