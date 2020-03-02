#pragma once

struct zTBBox3D
{
	zPOINT3 mins;
	zPOINT3 maxs;

public:
	zVOID SaveBIN(zCFileBIN &file) { file.BinWrite(this, sizeof(zTBBox3D)); }
	zVOID LoadBIN(zCFileBIN &file) { file.BinRead(this, sizeof(zTBBox3D)); }
};

class zCOBBox3D
{
public:
	zVEC3 center;
	zVEC3 axis[3];
	zVEC3 extent;

	zWORD numChilds;
	zCArray<zCOBBox3D *> childs;

public:
	zVOID SaveBIN(zCFileBIN &file)
	{
		file.BinWrite(&center, sizeof(center));
		file.BinWrite(&axis, sizeof(axis));
		file.BinWrite(&extent, sizeof(extent));

		file.BinWriteWord(numChilds);

		for (zWORD i = 0; i < numChilds; i++)
		{
			zCOBBox3D *obb = childs[i];
			obb->SaveBIN(file);
		}
	}

	zVOID LoadBIN(zCFileBIN &file)
	{
		file.BinRead(&center, sizeof(center));
		file.BinRead(&axis, sizeof(axis));
		file.BinRead(&extent, sizeof(extent));

		file.BinReadWord(numChilds);
		childs.AllocAbs(numChilds);

		for (zWORD i = 0; i < numChilds; i++)
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

	zVOID SaveBIN(zCFileBIN &file) { file.BinWrite(&date, sizeof(date)); file.BinWriteLine(fileName); }
	zVOID LoadBIN(zCFileBIN &file) { file.BinRead(&date, sizeof(date)); file.BinReadLine(fileName); }
};

struct zTPlane
{
	zREAL distance;
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
	zVALUE texu;
	zVALUE texv;
};

typedef zDWORD zTVertIndex;
typedef zWORD zTVertIndexG1;
typedef zDWORD zTVertIndexGRM;
typedef zDWORD zTVertIndexG2;
typedef zDWORD zTFeatIndex;

#define zMAX_INDEX_NUM 255

#pragma pack (push, 1)
struct TFlags // unified flags
{
	zUINT8 portalPoly : 2;
	zUINT8 occluder : 1;
	zUINT8 sectorPoly : 1;
	zUINT8 mustRelight : 1;
	// zUINT8 lodFlag : 1; // G1, possibly exactly the same as mustRelight, just renamed
	zUINT8 portalIndoorOutdoor : 1;
	zUINT8 ghostOccluder : 1;
	zUINT8 noDynLightNear : 1;
	zUINT8 normalMainAxis : 2; // G1
	zUINT16 sectorIndex : 16;
};

struct TFlagsG1
{
	zUINT8 portalPoly : 2; // tType
	zUINT8 occluder : 1; // tOccluder
	zUINT8 sectorPoly : 1; // tIndoor
	zUINT8 lodFlag : 1; // tLODGenerate
	zUINT8 portalIndoorOutdoor : 1; // tPortalWithoutMat
	zUINT8 ghostOccluder : 1; // tGhost
	zUINT8 normalMainAxis : 2; // m_nMainAxis
	zUINT16 sectorIndex : 16; // m_nSectorIndex
};

struct TFlagsG2
{
	zUINT8 portalPoly : 2;
	zUINT8 occluder : 1;
	zUINT8 sectorPoly : 1;
	zUINT8 mustRelight : 1;
	zUINT8 portalIndoorOutdoor : 1;
	zUINT8 ghostOccluder : 1;
	zUINT8 noDynLightNear : 1;
	zUINT16 sectorIndex : 16;
};

struct zTMSH_FeatureChunk
{
	zREAL texu;
	zREAL texv;
	zCOLOR lightStat;
	zVEC3 vertNormal;
};

struct zTMSH_PolyChunk // unified chunk
{
	zSWORD matIndex;
	zSWORD lightmapIndex;
	zTPlane polyPlane;
	TFlags flags;
	zBYTE polyNumVert;
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
	zSWORD matIndex;
	zSWORD lightmapIndex;
	zTPlane polyPlane;
	TFlagsG1 flags;
	zBYTE polyNumVert;
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
	zSWORD matIndex;
	zSWORD lightmapIndex;
	zTPlane polyPlane;
	TFlagsG2 flags;
	zBYTE polyNumVert;
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

	zBYTE numVert;
	zSWORD lightmapIndex;
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
