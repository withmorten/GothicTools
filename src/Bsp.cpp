#include "Bsp.h"

#include "Mesh.h"

zCBspTree *zCBspTree::actBspTree = NULL;
zCBspNode *zCBspTree::actNodePtr = NULL;
zCBspLeaf *zCBspTree::actLeafPtr = NULL;

zCBspTree::zCBspTree()
{
	bspTreeMode = zBSP_MODE_INDOOR;

	bspRoot = NULL;
	mesh = NULL;

	nodeList = NULL;
	leafList = NULL;
	numNodes = 0;
	numLeafs = 0;
	numPolys = 0;

	numSectors = 0;
	numPortals = 0;

	treePolyIndices = NULL;
	portalIndices = NULL;

	sectorList = NULL;
}

zCBspTree::~zCBspTree()
{
	zDELETE_ARRAY(leafList);
	zDELETE_ARRAY(nodeList);
	zDELETE(mesh);
	bspRoot = NULL;
	zFREE(treePolyIndices);
	zFREE(portalIndices);
	zDELETE_ARRAY(sectorList);
}

void zCBspBase::LoadBINRec(zCFileBIN &file)
{
	bbox3D.LoadBIN(file);

	file.BinReadDWord(treePolyIndex); // index to bspTree->treePolyIndices
	file.BinReadInt(numPolys);

	if (IsNode())
	{
		zCBspNode *node = (zCBspNode *)this;

		byte flag;
		file.BinReadByte(flag);

		file.BinRead(&node->plane, sizeof(node->plane));

		if (bspVersionIn == BSP_VERSION_GOTHIC_1_01 || bspVersionIn == BSP_VERSION_GOTHIC_1_04)
		{
			file.BinReadByte(node->hasLod);
		}

		zCBspLeaf *lastLeaf = zCBspTree::actLeafPtr;

		node->front = NULL;
		node->back = NULL;

		if ((flag & zNODE_FRONT))
		{
			if ((flag & zNODE_FRONT_LEAF))
			{
				node->front = zCBspTree::actLeafPtr++;
			}
			else
			{
				node->front = zCBspTree::actNodePtr++;
			}

			node->front->parent = node;
			node->front->LoadBINRec(file);
		}

		if ((flag & zNODE_BACK))
		{
			if ((flag & zNODE_BACK_LEAF))
			{
				node->back = zCBspTree::actLeafPtr++;
			}
			else
			{
				node->back = zCBspTree::actNodePtr++;
			}

			node->back->parent = node;
			node->back->LoadBINRec(file);
		}

		node->leafList = lastLeaf;
		node->numLeafs = (int32)(zCBspTree::actLeafPtr - lastLeaf);
	}
}

bool32 zCBspTree::LoadBIN(zCFileBIN &file)
{
	uint32 version;
	uint32 chunkLen;

	file.BinReadDWord(version);

	if (version != meshAndBspVersionIn)
	{
		printf("Wrong MeshAndBsp version\n");

		return FALSE;
	}

	file.BinReadDWord(chunkLen);

	uint16 id;
	int32 len;

	mesh = zNEW(zCMesh);

	if (!mesh->LoadMSH(file))
	{
		printf("Mesh could not be loaded\n");
		
		return FALSE;
	}

	zCBspTree::actBspTree = this;

	while (!file.BinEof())
	{
		file.BinOpenChunk(id, len);

		switch (id)
		{
		case zFCHUNK_BSP:
		{
			uint16 vers;

			file.BinReadWord(vers);

			if (vers != bspVersionIn)
			{
				printf("Wrong BspTree version\n");

				return FALSE;
			}

			file.BinReadEnum(bspTreeMode);
			if (bspTreeMode == zBSP_MODE_INDOOR) deLOD = FALSE;

			break;
		}
		case zFCHUNK_BSP_POLYLIST:
		{
			file.BinReadInt(numPolys);

			treePolyIndices = zMALLOC<uint32>(numPolys);
			file.BinRead(treePolyIndices, numPolys * sizeof(uint32)); // indices to mesh->polyArray

			break;
		}
		case zFCHUNK_BSP_TREE:
		{
			file.BinReadInt(numNodes);
			file.BinReadInt(numLeafs);

			nodeList = zNEW_ARRAY(zCBspNode, numNodes);
			leafList = zNEW_ARRAY(zCBspLeaf, numLeafs);
			zCBspTree::actNodePtr = nodeList;
			zCBspTree::actLeafPtr = leafList;

			bspRoot = zCBspTree::actNodePtr++;
			bspRoot->parent = NULL;
			bspRoot->LoadBINRec(file);

			break;
		}
		case zFCHUNK_BSP_LEAF_LIGHT:
		{
			if (numLeafs > 0)
			{
				zPOINT3 *lightPositionList = zNEW_ARRAY(zPOINT3, numLeafs);
				file.BinRead(lightPositionList, numLeafs * sizeof(zPOINT3));

				for (int32 i = 0; i < numLeafs; i++)
				{
					leafList[i].lightPosition = lightPositionList[i];
				}

				zDELETE_ARRAY(lightPositionList);
			}

			break;
		}
		case zFCHUNK_BSP_OUTDOOR_SECTORS:
		{
			file.BinReadInt(numSectors);
			sectorList = zNEW_ARRAY(zCBspSector, numSectors);

			for (int32 i = 0; i < numSectors; i++)
			{
				zCBspSector *sector = &sectorList[i];

				file.BinReadLine(sector->sectorName);

				file.BinReadInt(sector->numSectorNodes);
				file.BinReadInt(sector->numSectorPortals);

				sector->leafIndices = zMALLOC<uint32>(sector->numSectorNodes);
				sector->sectorIndices = zMALLOC<uint32>(sector->numSectorPortals);

				file.BinRead(sector->leafIndices, sector->numSectorNodes * sizeof(uint32));
				file.BinRead(sector->sectorIndices, sector->numSectorPortals * sizeof(uint32)); // indices to mesh->polyArray
			}

			file.BinReadInt(numPortals);
			portalIndices = zMALLOC<uint32>(numPortals);
			file.BinRead(portalIndices, numPortals * sizeof(uint32)); // indices to mesh->polyArray

			break;
		}
		case zFCHUNK_BSP_END:
		{
			byte answer;
			file.BinReadByte(answer);

			if (answer != 0x42)
			{
				printf("Wrong answer!\n");

				return FALSE;
			}

			goto fileEnd;

			break;
		}
		default:
		{
			printf("Unknown chunk encountered while parsing BspTree!\n");

			return FALSE;

			break;
		}
		}
	}

fileEnd:;

	if (deLOD)
	{
		bool32 hasLodPolys = FALSE;

		for (int32 i = 0; i < numNodes; i++)
		{
			if (nodeList[i].hasLod)
			{
				hasLodPolys = TRUE;

				break;
			}
		}

		if (hasLodPolys)
		{
			LODDegenerate();
		}
	}

	return TRUE;
}

void zCBspTree::LODDegenerate()
{
	zCArray<uint32> nonLodPolyIndices(numPolys / 8);

	// Get non LOD poly list from leafs, fix index to treePolyIndices
	for (int32 i = 0; i < numLeafs; i++)
	{
		zCBspLeaf *leaf = &leafList[i];

		for (int32 j = 0; j < leaf->numPolys; j++)
		{
			nonLodPolyIndices.Insert(treePolyIndices[leaf->treePolyIndex + j]);
		}

		leaf->treePolyIndex = nonLodPolyIndices.numInArray - leaf->numPolys;
	}

	// Remove all lod poly counts and indexes from lod nodes
	for (int32 i = 0; i < numNodes; i++)
	{
		zCBspNode *node = &nodeList[i];

		if (node->hasLod)
		{
			node->hasLod = FALSE;
			node->treePolyIndex = 0;
			node->numPolys = 0;
		}
	}

	// Switch treePolyIndices
	numPolys = nonLodPolyIndices.numInArray;
	zMEMCPY<uint32>(treePolyIndices, nonLodPolyIndices.array, numPolys);
	zREALLOC<uint32>(treePolyIndices, numPolys);

	int32 *polyIndexMap = zMALLOC<int32>(mesh->numPoly);
	zMEMSET<int32>(polyIndexMap, -1, mesh->numPoly); // -1 = unused
	int32 newMeshNumPoly = 0;

	for (int32 i = 0; i < numPolys; i++)
	{
		if (polyIndexMap[treePolyIndices[i]] == -1) // only inc new numPoly once
		{
			newMeshNumPoly++;
		}

		polyIndexMap[treePolyIndices[i]] = 0; // 0 = non Lod Poly
	}

	// Kill LOD polys in mesh using the index map, add new offsets to the index map, and remove Z_PORTALMAT
	mesh->LODDegenerate(polyIndexMap, newMeshNumPoly);

	// Fix indices to mesh->polyArray
	for (int32 i = 0; i < numPolys; i++)
	{
		treePolyIndices[i] = polyIndexMap[treePolyIndices[i]];
	}

	// Fix indices to mesh->polyArray
	for (int32 i = 0; i < numSectors; i++)
	{
		zCBspSector *sector = &sectorList[i];

		for (int32 j = 0; j < sector->numSectorPortals; j++)
		{
			sector->sectorIndices[j] = polyIndexMap[sector->sectorIndices[j]];
		}
	}

	zFREE(polyIndexMap);
}

void zCBspBase::SaveBINRec(zCFileBIN &file)
{
	bbox3D.SaveBIN(file);

	file.BinWriteDWord(treePolyIndex);
	file.BinWriteInt(numPolys);

	if (IsNode())
	{
		byte flag = zNODE_UNDEFINED;

		zCBspNode *node = (zCBspNode *)this;

		if (node->front)
		{
			flag |= zNODE_FRONT;

			if (node->front->IsLeaf()) flag |= zNODE_FRONT_LEAF;
		}

		if (node->back)
		{
			flag |= zNODE_BACK;

			if (node->back->IsLeaf()) flag |= zNODE_BACK_LEAF;
		}

		file.BinWriteByte(flag);
		file.BinWrite(&node->plane, sizeof(node->plane));

		if (bspVersionOut == BSP_VERSION_GOTHIC_1_01 || bspVersionOut == BSP_VERSION_GOTHIC_1_04)
		{
			file.BinWriteByte(node->hasLod);
		}

		if (node->front) node->front->SaveBINRec(file);
		if (node->back) node->back->SaveBINRec(file);
	}
}

bool32 zCBspTree::SaveBIN(zCFileBIN &file)
{
	zCBspTree::actBspTree = this;

	file.BinWriteDWord(meshAndBspVersionOut);
	int32 chunkLenPos = file.BinPos();
	file.BinWriteDWord(0); // chunkLen

	if (!mesh->SaveMSH(file))
	{
		printf("Mesh could not be saved\n");

		return FALSE;
	}

	file.BinStartChunk(zFCHUNK_BSP);
	{
		file.BinWriteWord(bspVersionOut);
		file.BinWriteEnum(bspTreeMode);
	}

	file.BinStartChunk(zFCHUNK_BSP_POLYLIST);
	{
		file.BinWriteInt(numPolys);
		file.BinWrite(treePolyIndices, numPolys * sizeof(uint32));
	}

	file.BinStartChunk(zFCHUNK_BSP_TREE);
	{
		file.BinWriteInt(numNodes);
		file.BinWriteInt(numLeafs);
		bspRoot->SaveBINRec(file);
	}

	if (bspVersionOut != BSP_VERSION_GOTHIC_1_01)
	{
		file.BinStartChunk(zFCHUNK_BSP_LEAF_LIGHT);
		{
			for (int32 i = 0; i < numLeafs; i++)
			{
				file.BinWrite(&(leafList[i].lightPosition), sizeof(leafList[i].lightPosition));
			}
		}
	}

	file.BinStartChunk(zFCHUNK_BSP_OUTDOOR_SECTORS);
	{
		file.BinWriteInt(numSectors);

		for (int32 i = 0; i < numSectors; i++)
		{
			zCBspSector *sector = &sectorList[i];

			file.BinWriteLine(sector->sectorName);
			file.BinWriteInt(sector->numSectorNodes);
			file.BinWriteInt(sector->numSectorPortals);

			file.BinWrite(sector->leafIndices, sector->numSectorNodes * sizeof(uint32));
			file.BinWrite(sector->sectorIndices, sector->numSectorPortals * sizeof(uint32));
		}

		file.BinWriteInt(numPortals);
		file.BinWrite(portalIndices, numPortals * sizeof(uint32));
	}

	file.BinStartChunk(zFCHUNK_BSP_END);
	{
		file.BinWriteByte(0x42);
	}

	file.BinEndChunk();

	int32 actPos = file.BinPos();
	file.BinSeek(chunkLenPos);
	file.BinWriteDWord(actPos - chunkLenPos - sizeof(uint32));
	file.BinSeek(actPos);

	return TRUE;
}
