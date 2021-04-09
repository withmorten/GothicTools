#include "GothicTools.h"

void zCMorphMeshAni::Load(zCFileBIN &file)
{
	file.BinReadLine(aniName);

	file.BinReadFloat(blendInSpeed);
	file.BinReadFloat(blendOutSpeed);
	file.BinReadFloat(holdTime);
	file.BinReadInt(layer);
	file.BinReadFloat(fpsRate);
	file.BinRead(&flags, sizeof(flags));

	file.BinReadInt(numVert);
	file.BinReadInt(numFrames);

	vertIndexList = zMALLOC<zINT>(numVert);
	vertPosMatrix = zNEW_ARRAY(zPOINT3, numVert * numFrames);

	file.BinRead(vertIndexList, sizeof(zINT) * numVert);
	file.BinRead(vertPosMatrix, sizeof(zPOINT3) * numVert * numFrames);
}

void zCMorphMeshAni::Save(zCFileBIN &file)
{
	file.BinWriteLine(aniName);

	file.BinWriteFloat(blendInSpeed);
	file.BinWriteFloat(blendOutSpeed);
	file.BinWriteFloat(holdTime);
	file.BinWriteInt(layer);
	file.BinWriteFloat(fpsRate);
	file.BinWrite(&flags, sizeof(flags));

	file.BinWriteInt(numVert);
	file.BinWriteInt(numFrames);

	file.BinWrite(vertIndexList, sizeof(zINT) * numVert);
	file.BinWrite(vertPosMatrix, sizeof(zPOINT3) * numVert * numFrames);
}

zBOOL zCMorphMeshProto::LoadMMB(zCFileBIN &file)
{
	uint16 id;
	zLONG len;

	while (!file.BinEof())
	{
		file.BinOpenChunk(id, len);

		switch (id)
		{
		case zFCHUNK_MMB_MORPHMESH:
		{
			break;
		}
		case zFCHUNK_MMB_HEADER:
		{
			zDWORD vers;
			file.BinReadDWord(vers);

			if (vers != morphMeshVersionIn)
			{
				printf("Wrong MMB version\n");

				return FALSE;
			}

			file.BinReadLine(morphProtoName);

			morphRefMesh = zNEW(zCProgMeshProto);

			if (!morphRefMesh->LoadMRM(file))
			{
				printf("Could not load RefMesh\n");

				return FALSE;
			}

			morphRefMeshVertPos = zNEW_ARRAY(zPOINT3, morphRefMesh->posList.GetNum());

			file.BinRead(morphRefMeshVertPos, sizeof(zPOINT3) * morphRefMesh->posList.GetNum());

			break;
		}
		case zFCHUNK_MMB_SOURCE:
		{
			file.BinReadWord(numAni);
			srcFileStats = zNEW_ARRAY(zTSrcFileStats, numAni);

			for (uint16 i = 0; i < numAni; i++)
			{
				srcFileStats[i].LoadBIN(file);
			}

			break;
		}
		case zFCHUNK_MMB_ANILIST:
		{
			uint16 numAni2;
			file.BinReadWord(numAni2);

			aniList.AllocAbs(numAni);

			for (uint16 i = 0; i < numAni; i++)
			{
				zCMorphMeshAni *ani = zNEW(zCMorphMeshAni);

				ani->Load(file);

				aniList.InsertEnd(ani);
			}

			break;
		}
		default:
		{
			printf("Unknown chunk encountered while parsing MMB!\n");

			return FALSE;

			break;
		}
		}
	}

	return TRUE;
}

void zCMorphMeshProto::SaveMMB(zCFileBIN &file)
{
	file.BinStartChunk(zFCHUNK_MMB_MORPHMESH); // empty

	file.BinStartChunk(zFCHUNK_MMB_HEADER);
	{
		file.BinWriteDWord(morphMeshVersionOut);

		file.BinWriteLine(morphProtoName);

		morphRefMesh->SaveMRM(file);

		file.BinWrite(morphRefMeshVertPos, sizeof(zPOINT3) * morphRefMesh->posList.GetNum());
	}

	file.BinStartChunk(zFCHUNK_MMB_SOURCE);
	{
		file.BinWriteWord(numAni);

		for (uint16 i = 0; i < numAni; i++)
		{
			srcFileStats[i].SaveBIN(file);
		}
	}

	file.BinStartChunk(zFCHUNK_MMB_ANILIST);
	{
		file.BinWriteWord(numAni);

		for (uint16 i = 0; i < numAni; i++)
		{
			zCMorphMeshAni *ani = aniList[i];

			ani->Save(file);
		}
	}

	file.BinEndChunk();
}
