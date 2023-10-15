#pragma once

#include "GothicTools.h"

#include "3D.h"
#include "Visual.h"

#define zFCHUNK_PROGMESH ((uint16)0xB100)
#define zFCHUNK_PROGMESH_END ((uint16)0xB1FF)

typedef uint16 zPMINDEX;

struct zTPMTriangle
{
	zPMINDEX wedge[3];
};

struct zTPMTriangleEdges
{
	zPMINDEX edge[3];
};

struct zTPMWedge
{
	zVEC3 normal;
	zVEC2 texUV;
	zPMINDEX position;
};

struct zTPMEdge
{
	zPMINDEX wedge[2];
};

struct zTPMVertexUpdate
{
	zPMINDEX numNewTri;
	zPMINDEX numNewWedge;
};

struct zTLODParams
{
	float strength;
	float zDisplace2;
	float morphPerc;
	int32 minVerts;
};

class zCProgMeshProto : public zCVisual
{
public:
	static const zSTRING GetClassName() { return "zCProgMeshProto"; }

public:
	class zCSubMesh
	{
	public:
		zCMaterial *material;

		zCArrayAdapt<zTPMTriangle> triList;
		zCArrayAdapt<zTPMWedge> wedgeList;
		zCArrayAdapt<float> colorList;
		zCArrayAdapt<zPMINDEX> triPlaneIndexList;
		zCArrayAdapt<zTPlane> triPlaneList;
		zCArrayAdapt<zTPMTriangleEdges> triEdgeList;
		zCArrayAdapt<zTPMEdge> edgeList;
		zCArrayAdapt<float> edgeScoreList;

		zCArrayAdapt<zPMINDEX> wedgeMap;
		zCArrayAdapt<zTPMVertexUpdate> vertexUpdates;

		int32 vbStartIndex;

	public:
		zCSubMesh();
		~zCSubMesh();
	};
public:
	bool32 m_bUsesAlphaTesting;

	zTBBox3D bbox3D;
	zCOBBox3D obbox3D;

	zTLODParams lodParams;

	byte *dataPool;
	uint32 dataSize;

	zCArrayAdapt<zPOINT3> posList;
	zCArrayAdapt<zVEC3> posNormalList;

	zCSubMesh *subMeshList;
	byte numSubMeshes;

public:
	zCProgMeshProto()
	{
		numSubMeshes = 0;
		subMeshList = NULL;

		dataSize = 0;
		dataPool = NULL;

		m_bUsesAlphaTesting = FALSE;

		bbox3D.mins = zVEC3(0.0f, 0.0f, 0.0f);
		bbox3D.maxs = zVEC3(0.0f, 0.0f, 0.0f);
	}

	~zCProgMeshProto()
	{
		zFREE(dataPool);

		zDELETE_ARRAY(subMeshList);
	}

	bool32 UnarchiveMatList(zCFileBIN &file);
	void ArchiveMatList(zCFileBIN &file);

	bool32 LoadMRM(zCFileBIN &file);
	bool32 LoadMRM(const zSTRING &filename) { zCFileBIN f(filename); return LoadMRM(f); }

	void SaveMRM(zCFileBIN &file);
	void SaveMRM(const zSTRING &fileName) { zCFileBIN f(fileName, TRUE); SaveMRM(f); }
};

struct zTPMProtoDirEntry
{
	uint32 offset;
	uint32 size;

	void Set(uint32 _offset, uint32 _size)
	{
		offset = _offset;
		size = _size;
	}
};

struct zTPMProtoDirectoryMain
{
	zTPMProtoDirEntry posList;
	zTPMProtoDirEntry posNormalList;

	void TransferFromProto(zCProgMeshProto *pmProto)
	{
#define SetDirEntry(FIELD) FIELD.Set((uint32)((byte *)pmProto->FIELD.GetArray() - pmProto->dataPool), pmProto->FIELD.GetNum())
		SetDirEntry(posList);
		SetDirEntry(posNormalList);
#undef SetDirEntry
	}

	void TransferToProto(zCProgMeshProto *pmProto)
	{
#define GetFromEntry(FIELD) pmProto->FIELD.SetArray((byte *)(pmProto->dataPool + FIELD.offset), FIELD.size)
		GetFromEntry(posList);
		GetFromEntry(posNormalList);
#undef GetFromEntry
	}
};

struct zTPMProtoDirectorySubMesh
{
	zTPMProtoDirEntry triList;
	zTPMProtoDirEntry wedgeList;
	zTPMProtoDirEntry colorList;
	zTPMProtoDirEntry triPlaneIndexList;
	zTPMProtoDirEntry triPlaneList;
	zTPMProtoDirEntry wedgeMap;
	zTPMProtoDirEntry vertexUpdates;
	zTPMProtoDirEntry triEdgeList;
	zTPMProtoDirEntry edgeList;
	zTPMProtoDirEntry edgeScoreList;

	void TransferFromProto(zCProgMeshProto *pmProto, zCProgMeshProto::zCSubMesh *subMesh)
	{
#define SetDirEntry(FIELD) FIELD.Set((uint32)((byte *)subMesh->FIELD.GetArray() - pmProto->dataPool), subMesh->FIELD.GetNum())
		SetDirEntry(triList);
		SetDirEntry(wedgeList);
		SetDirEntry(colorList);
		SetDirEntry(triPlaneIndexList);
		SetDirEntry(triPlaneList);
		SetDirEntry(wedgeMap);
		SetDirEntry(vertexUpdates);
		SetDirEntry(triEdgeList);
		SetDirEntry(edgeList);
		SetDirEntry(edgeScoreList);
#undef SetDirEntry
	}

	void TransferToProto(zCProgMeshProto *pmProto, zCProgMeshProto::zCSubMesh *subMesh)
	{
#define GetFromEntry(FIELD) subMesh->FIELD.SetArray((byte *)(pmProto->dataPool + FIELD.offset), FIELD.size)
		GetFromEntry(triList);
		GetFromEntry(wedgeList);
		GetFromEntry(colorList);
		GetFromEntry(triPlaneIndexList);
		GetFromEntry(triPlaneList);
		GetFromEntry(wedgeMap);
		GetFromEntry(vertexUpdates);
		GetFromEntry(triEdgeList);
		GetFromEntry(edgeList);
		GetFromEntry(edgeScoreList);
#undef GetFromEntry
	}
};
