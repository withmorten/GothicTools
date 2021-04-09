#include "GothicTools.h"

zCMesh::zCMesh()
{
	numPoly = 0;
	numVert = 0;
	numFeat = 0;

	polyArray = NULL;
	vertArray = NULL;
	featArray = NULL;

	bbox3D.mins = zVEC3(0.0f, 0.0f, 0.0f);
	bbox3D.maxs = zVEC3(0.0f, 0.0f, 0.0f);

	hasLightmaps = FALSE;
	m_bUsesAlphaTesting = FALSE;

	lightMapsLen = 0;
	lightMaps = NULL;
}

zCMesh::~zCMesh()
{
	for (zINT i = 0; i < numPoly; i++)
	{
		zDELETE(polyArray[i]);
	}

	zDELETE_ARRAY(polyArray);
	zDELETE_ARRAY(vertArray);
	zDELETE_ARRAY(featArray);
	zFREE(lightMaps);
}

zBOOL zCMesh::SaveMSH(zCFileBIN &file)
{
	if (meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_01 && numVert > 65535)
	{
		printf("Mesh version 101 only supports 65535 vertices, %d given\n", numVert);

		return FALSE;
	}

	if ((meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_04 && numVert > 65535) && !xZenOut)
	{
		printf("Mesh version 108 only supports 65535 vertices, %d given, try appending \"xzen\" to the command\n", numVert);

		return FALSE;
	}

	file.BinStartChunk(zFCHUNK_MESH);
	{
		file.BinWriteWord(meshVersionOut);
		file.BinWrite(&date, sizeof(date));
		file.BinWriteLine(name);
	}

	file.BinStartChunk(zFCHUNK_BBOX3D);
	{
		bbox3D.SaveBIN(file);
		obbox3D.SaveBIN(file);
	}

	file.BinStartChunk(zFCHUNK_MATLIST);
	{
		ArchiveMatList(file);
	}

	if (hasLightmaps)
	{
		file.BinStartChunk(zFCHUNK_LIGHTMAPLIST_SHARED);
		{
			file.BinWrite(lightMaps, lightMapsLen);
		}
	}

	file.BinStartChunk(zFCHUNK_VERTLIST);
	{
		file.BinWriteInt(numVert);

		for (zINT i = 0; i < numVert; i++)
		{
			zCVertex *vert = &vertArray[i];

			file.BinWrite(&vert->position, sizeof(vert->position));
		}
	}

	file.BinStartChunk(zFCHUNK_FEATLIST);
	{
		file.BinWriteInt(numFeat);

		for (zINT i = 0; i < numFeat; i++)
		{
			zTMSH_FeatureChunk chunk;
			zCVertFeature *feat = &featArray[i];

			chunk.texu = feat->texu;
			chunk.texv = feat->texv;
			chunk.lightStat = feat->lightStat;
			chunk.vertNormal = feat->vertNormal;

			file.BinWrite(&chunk, sizeof(chunk));
		}
	}

	file.BinStartChunk(zFCHUNK_POLYLIST);
	{
		ArchivePolyList(file);
	}

	file.BinStartChunk(zFCHUNK_MESH_END);

	file.BinEndChunk();

	return TRUE;
}

zBOOL zCMesh::LoadMSH(zCFileBIN &file)
{
	uint16 id;
	zLONG len;

	while (!file.BinEof())
	{
		file.BinOpenChunk(id, len);

		switch (id)
		{
		case zFCHUNK_MESH:
		{
			uint16 vers;

			file.BinReadWord(vers);

			if (vers != meshVersionIn)
			{
				printf("Wrong Mesh version\n");

				return FALSE;
			}

			file.BinRead(&date, sizeof(date));
			file.BinReadLine(name);

			break;
		}
		case zFCHUNK_BBOX3D:
		{
			bbox3D.LoadBIN(file);
			obbox3D.LoadBIN(file);

			break;
		}
		case zFCHUNK_MATLIST:
		{
			if (!UnarchiveMatList(file))
			{
				printf("Materials could not be loaded!\n");

				return FALSE;
			}

			break;
		}
		case zFCHUNK_LIGHTMAPLIST_SHARED:
		{
			hasLightmaps = TRUE;

			lightMapsLen = len;
			lightMaps = zMALLOC<byte>(len);

			file.BinRead(lightMaps, lightMapsLen);

			break;
		}
		case zFCHUNK_VERTLIST:
		{
			file.BinReadInt(numVert);

			vertArray = zNEW_ARRAY(zCVertex, numVert);
			zPOINT3 *block = zNEW_ARRAY(zPOINT3, numVert);

			file.BinRead(block, numVert * sizeof(zPOINT3));

			for (zINT i = 0; i < numVert; i++)
			{
				vertArray[i].position = block[i];
			}

			zDELETE_ARRAY(block);

			break;
		}
		case zFCHUNK_FEATLIST:
		{
			file.BinReadInt(numFeat);

			featArray = zNEW_ARRAY(zCVertFeature, numFeat);
			zTMSH_FeatureChunk *block = zMALLOC<zTMSH_FeatureChunk>(numFeat);

			file.BinRead(block, numFeat * sizeof(zTMSH_FeatureChunk));

			for (zINT i = 0; i < numFeat; i++)
			{
				featArray[i].texu = block[i].texu;
				featArray[i].texv = block[i].texv;
				featArray[i].vertNormal = block[i].vertNormal;
				featArray[i].lightStat = block[i].lightStat;
			}

			zDELETE_ARRAY(block);

			break;
		}
		case zFCHUNK_POLYLIST:
		{
			UnarchivePolyList(file, len);

			break;
		}
		case zFCHUNK_MESH_END:
		{
			goto fileEnd;

			break;
		}
		default:
		{
			printf("Unknown chunk encountered while parsing Mesh!\n");

			return FALSE;

			break;
		}
		}
	}

fileEnd:;

	return TRUE;
}

void zCMesh::ArchiveMatList(zCFileBIN &file)
{
	file.BinWriteLine("ZenGin Archive");
	file.BinWriteLine("ver 1");
	file.BinWriteLine("zCArchiverGeneric");
	file.BinWriteLine("BINARY");
	file.BinWriteLine("saveGame 0");
	file.BinWriteLine("END");

	if (meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_01)
	{
		file.BinWriteLine("objects 0        ");
	}
	else
	{
		zSTRING l;

		l += "objects " + zSTRING(matList.numInArray);
		zSIZE_T fill = strlen("objects 0        ") - l.Length();

		for (zSIZE_T i = 0; i < fill; i++)
		{
			l.Append(" ");
		}

		file.BinWriteLine(l);
	}

	file.BinWriteLine("END");
	file.BinWriteLine();

	file.BinWriteInt(matList.numInArray);

	for (zINT i = 0; i < matList.numInArray; i++)
	{
		zCMaterial *mat = matList[i];

		file.BinWriteString(mat->name);

		mat->Archive(file, i);
	}

	if (materialVersionOut == MATERIAL_VERSION_SUM_GOTHIC_1_30)
	{
		file.BinWriteBool(m_bUsesAlphaTesting);
	}
}

zBOOL zCMesh::UnarchiveMatList(zCFileBIN &file)
{
	zSTRING s;

	file.BinReadLine(s); // ZenGin Archive
	file.BinReadLine(s); // ver 1
	file.BinReadLine(s); // zCArchiverGeneric
	file.BinReadLine(s); // BINARY
	file.BinReadLine(s); // saveGame 0
	file.BinReadLine(s); // END
	file.BinReadLine(s); // objects [num]
	file.BinReadLine(s); // END
	file.BinReadLine(s); //

	zINT numMats;
	file.BinReadInt(numMats);

	matList.AllocAbs(numMats);

	for (zINT i = 0; i < numMats; i++)
	{
		file.BinReadString(s);

		s.Upper();

		zCMaterial *mat = zNEW(zCMaterial)(s);

		if (!mat->Unarchive(file))
		{
			return FALSE;
		}

		matList.InsertEnd(mat);
	}

	if (materialVersionIn == MATERIAL_VERSION_SUM_GOTHIC_1_30)
	{
		file.BinReadBool(m_bUsesAlphaTesting);
	}

	return TRUE;
}

void zCMesh::ArchivePolyList(zCFileBIN &file)
{
	file.BinWriteInt(numPoly);

	for (zINT i = 0; i < numPoly; i++)
	{
		zCPolygon *poly = polyArray[i];

		if (meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_01 || meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_04)
		{
			zTMSH_PolyChunkG1 chunk;
			memset(&chunk.flags, 0x00, sizeof(TFlagsG1));

			chunk.matIndex = matList.Search(poly->material);
			chunk.lightmapIndex = poly->lightmapIndex;
			chunk.polyPlane = poly->polyPlane;
			chunk.flags.portalPoly = poly->flags.portalPoly;
			chunk.flags.occluder = poly->flags.occluder;
			chunk.flags.sectorPoly = poly->flags.sectorPoly;
			chunk.flags.lodFlag = poly->flags.mustRelight;
			chunk.flags.portalIndoorOutdoor = poly->flags.portalIndoorOutdoor;
			chunk.flags.ghostOccluder = poly->flags.ghostOccluder;
			chunk.flags.normalMainAxis = poly->flags.normalMainAxis;
			chunk.flags.sectorIndex = poly->flags.sectorIndex;
			chunk.polyNumVert = poly->numVert;

			file.BinWrite(&chunk, sizeof(chunk));

			for (zINT j = 0; j < poly->numVert; j++)
			{
				if (!xZenOut)
				{
					zTVertIndexG1 vi = (zTVertIndexG1)poly->indexList[j].vertIndex;
					file.BinWrite(&vi, sizeof(vi));
				}
				else
				{
					zTVertIndexG2 vi = (zTVertIndexG2)poly->indexList[j].vertIndex;
					file.BinWrite(&vi, sizeof(vi));
				}

				zTFeatIndex fi = (zTFeatIndex)poly->indexList[j].featIndex;
				file.BinWrite(&fi, sizeof(fi));
			}
		}
		else if (meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_30)
		{
			zTMSH_PolyChunkG2 chunk;
			memset(&chunk.flags, 0x00, sizeof(TFlagsG2));

			chunk.matIndex = matList.Search(poly->material);
			chunk.lightmapIndex = poly->lightmapIndex;
			chunk.polyPlane = poly->polyPlane;
			chunk.flags.portalPoly = poly->flags.portalPoly;
			chunk.flags.occluder = poly->flags.occluder;
			chunk.flags.sectorPoly = poly->flags.sectorPoly;
			chunk.flags.mustRelight = poly->flags.mustRelight;
			chunk.flags.portalIndoorOutdoor = poly->flags.portalIndoorOutdoor;
			chunk.flags.ghostOccluder = poly->flags.ghostOccluder;
			chunk.flags.noDynLightNear = poly->flags.noDynLightNear;
			chunk.flags.sectorIndex = poly->flags.sectorIndex;
			chunk.polyNumVert = poly->numVert;

			file.BinWrite(&chunk, sizeof(chunk));

			for (zINT j = 0; j < poly->numVert; j++)
			{
				zTVertIndexG2 vi = (zTVertIndexG2)poly->indexList[j].vertIndex;
				zTFeatIndex fi = (zTFeatIndex)poly->indexList[j].featIndex;

				file.BinWrite(&vi, sizeof(vi));
				file.BinWrite(&fi, sizeof(fi));
			}
		}
	}
}

void zCMesh::UnarchivePolyList(zCFileBIN &file, zLONG len)
{
	file.BinReadInt(numPoly);

	polyArray = zNEW_ARRAY(zCPolygon *, numPoly);

	byte *block = zMALLOC<byte>(len - sizeof(zINT));
	file.BinRead(block, len - sizeof(zINT));

	byte *blockPtr = block;
	zINT inc = 0;

	zTMSH_PolyReadChunk chunk;

	for (zINT i = 0; i < numPoly; i++)
	{
		zCPolygon *poly = zNEW(zCPolygon);
		memset(&chunk.flags, 0x00, sizeof(chunk.flags));

		if (meshAndBspVersionIn == BSPMESH_VERSION_GOTHIC_1_01 || meshAndBspVersionIn == BSPMESH_VERSION_GOTHIC_1_04)
		{
			zTMSH_PolyChunkG1 *chunkg1 = (zTMSH_PolyChunkG1 *)blockPtr;

			chunk.matIndex = chunkg1->matIndex;
			chunk.lightmapIndex = chunkg1->lightmapIndex;
			chunk.polyPlane = chunkg1->polyPlane;

			chunk.flags.portalPoly = chunkg1->flags.portalPoly;
			chunk.flags.occluder = chunkg1->flags.occluder;
			chunk.flags.sectorPoly = chunkg1->flags.sectorPoly;
			chunk.flags.mustRelight = chunkg1->flags.lodFlag;
			chunk.flags.portalIndoorOutdoor = chunkg1->flags.portalIndoorOutdoor;
			chunk.flags.ghostOccluder = chunkg1->flags.ghostOccluder;
			chunk.flags.normalMainAxis = chunkg1->flags.normalMainAxis;
			chunk.flags.sectorIndex = chunkg1->flags.sectorIndex;

			chunk.polyNumVert = chunkg1->polyNumVert;

			blockPtr += sizeof(zTMSH_PolyChunkG1);

			if (!xZenIn)
			{
				zTIndexG1 *indexList = (zTIndexG1 *)blockPtr;

				for (zINT j = 0; j < chunk.polyNumVert; j++)
				{
					chunk.indexList[j].vertIndex = indexList[j].vertIndex;
					chunk.indexList[j].featIndex = indexList[j].featIndex;
				}

				inc = sizeof(zTIndexG1) * chunk.polyNumVert;
			}
			else
			{
				zTIndexG2 *indexList = (zTIndexG2 *)blockPtr;

				for (zINT j = 0; j < chunk.polyNumVert; j++)
				{
					chunk.indexList[j].vertIndex = indexList[j].vertIndex;
					chunk.indexList[j].featIndex = indexList[j].featIndex;
				}

				inc = sizeof(zTIndexG2) * chunk.polyNumVert;
			}
		}
		else if (meshAndBspVersionIn == BSPMESH_VERSION_GOTHIC_1_30)
		{
			zTMSH_PolyReadChunkG2 *chunkg2 = (zTMSH_PolyReadChunkG2 *)blockPtr;

			inc = sizeof(zTMSH_PolyChunkG2) + (sizeof(zTIndexG2) * chunkg2->polyNumVert);

			chunk.matIndex = chunkg2->matIndex;
			chunk.lightmapIndex = chunkg2->lightmapIndex;
			chunk.polyPlane = chunkg2->polyPlane;

			chunk.flags.portalPoly = chunkg2->flags.portalPoly;
			chunk.flags.occluder = chunkg2->flags.occluder;
			chunk.flags.sectorPoly = chunkg2->flags.sectorPoly;
			chunk.flags.mustRelight = chunkg2->flags.mustRelight;
			chunk.flags.portalIndoorOutdoor = chunkg2->flags.portalIndoorOutdoor;
			chunk.flags.ghostOccluder = chunkg2->flags.ghostOccluder;
			chunk.flags.noDynLightNear = chunkg2->flags.noDynLightNear;
			chunk.flags.sectorIndex = chunkg2->flags.sectorIndex;

			chunk.polyNumVert = chunkg2->polyNumVert;

			for (zINT j = 0; j < chunk.polyNumVert; j++)
			{
				chunk.indexList[j].vertIndex = chunkg2->indexList[j].vertIndex;
				chunk.indexList[j].featIndex = chunkg2->indexList[j].featIndex;
			}
		}

		poly->polyPlane = chunk.polyPlane;
		poly->material = matList[chunk.matIndex];
		poly->lightmapIndex = chunk.lightmapIndex;

		poly->flags.portalPoly = chunk.flags.portalPoly;
		poly->flags.occluder = chunk.flags.occluder;
		poly->flags.sectorPoly = chunk.flags.sectorPoly;
		poly->flags.mustRelight = chunk.flags.mustRelight;
		poly->flags.portalIndoorOutdoor = chunk.flags.portalIndoorOutdoor;
		poly->flags.ghostOccluder = chunk.flags.ghostOccluder;
		poly->flags.noDynLightNear = chunk.flags.noDynLightNear;
		poly->flags.normalMainAxis = chunk.flags.normalMainAxis;
		poly->flags.sectorIndex = chunk.flags.sectorIndex;

		poly->numVert = chunk.polyNumVert;

		poly->indexList = zMALLOC<zTIndex>(poly->numVert);

		for (zINT j = 0; j < poly->numVert; j++)
		{
			poly->indexList[j].vertIndex = chunk.indexList[j].vertIndex;
			poly->indexList[j].featIndex = chunk.indexList[j].featIndex;
		}

		blockPtr += inc;

		polyArray[i] = poly;
	}

	zFREE(block);
}

void zCMesh::LODDegenerate(zINT *polyIndexMap, zINT newNumPoly)
{
	zCPolygon **newPolyArray = zNEW_ARRAY(zCPolygon *, newNumPoly);

	zINT newOffset = 0;

	for (zINT i = 0; i < numPoly; i++)
	{
		if (polyIndexMap[i] == -1)
		{
			zDELETE(polyArray[i]); // free unused polys
		}
		else
		{
			polyIndexMap[i] = newOffset;
			newPolyArray[newOffset] = polyArray[i];
			newOffset++;
		}
	}

	zDELETE_ARRAY(polyArray);

	polyArray = newPolyArray;
	numPoly = newNumPoly;

	for (zINT i = 0; i < matList.numInArray; i++)
	{
		if (matList[i]->name == "Z_PORTALMAT")
		{
			matList.RemoveOrderIndex(i, TRUE);

			break;
		}
	}
}
