#include "GothicTools.h"

zBOOL zCProgMeshProto::UnarchiveMatList(zCFileBIN &file)
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

	for (byte i = 0; i < numSubMeshes; i++)
	{
		file.BinReadString(s);

		s.Upper();

		zCMaterial *mat = zNEW(zCMaterial)(s);

		if (!mat->Unarchive(file))
		{
			return FALSE;
		}

		subMeshList[i].material = mat;
	}

	if (materialVersionIn == MATERIAL_VERSION_SUM_GOTHIC_1_30)
	{
		file.BinReadBool(m_bUsesAlphaTesting);
	}

	return TRUE;
}

zBOOL zCProgMeshProto::LoadMRM(zCFileBIN &file)
{
	uint16 id;
	zLONG len;

	while (!file.BinEof())
	{
		file.BinOpenChunk(id, len);

		switch (id)
		{
		case zFCHUNK_PROGMESH:
		{
			uint16 vers;

			file.BinReadWord(vers);

			if (vers != progMeshVersionIn
				&& ((progMeshVersionIn == PROGMESH_VERSION_GOTHIC_1_30 && vers != PROGMESH_VERSION_GOTHIC_OLD1)
				&& (progMeshVersionIn == PROGMESH_VERSION_GOTHIC_1_30 && vers != PROGMESH_VERSION_GOTHIC_OLD2)))
			{
				printf("Wrong ProgMesh version!\n");

				return FALSE;
			}

			file.BinReadDWord(dataSize);
			dataPool = zMALLOC<byte>(dataSize);
			file.BinRead(dataPool, dataSize);

			file.BinReadByte(numSubMeshes);

			if (numSubMeshes > 32)
			{
				printf("Too many submeshes!\n");

				return FALSE;
			}

			subMeshList = zNEW_ARRAY(zCSubMesh, numSubMeshes);

			zTPMProtoDirectoryMain protoDir;
			file.BinRead(&protoDir, sizeof(protoDir));
			protoDir.TransferToProto(this);

			zTPMProtoDirectorySubMesh subMeshDir[32];
			file.BinRead(&subMeshDir, sizeof(zTPMProtoDirectorySubMesh) * numSubMeshes);

			for (byte i = 0; i < numSubMeshes; i++)
			{
				subMeshDir[i].TransferToProto(this, &(subMeshList[i]));
			}

			if (!UnarchiveMatList(file))
			{
				printf("Materials could not be loaded!\n");

				return FALSE;
			}

			file.BinRead(&bbox3D, sizeof(bbox3D));
			obbox3D.LoadBIN(file);

			file.BinRead(&lodParams, sizeof(lodParams));

			break;
		}
		case zFCHUNK_PROGMESH_END:
		{
			goto fileEnd;

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

fileEnd:;

	return TRUE;
}

void zCProgMeshProto::ArchiveMatList(zCFileBIN &file)
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

		l += "objects " + zSTRING(numSubMeshes);
		zSIZE_T fill = strlen("objects 0        ") - l.Length();

		for (zSIZE_T i = 0; i < fill; i++)
		{
			l.Append(" ");
		}

		file.BinWriteLine(l);
	}

	file.BinWriteLine("END");
	file.BinWriteLine();

	for (byte i = 0; i < numSubMeshes; i++)
	{
		zCMaterial *mat = subMeshList[i].material;
			
		file.BinWriteString(mat->name);

		mat->Archive(file, i);
	}

	if (materialVersionOut == MATERIAL_VERSION_SUM_GOTHIC_1_30)
	{
		file.BinWriteBool(m_bUsesAlphaTesting);
	}
}

void zCProgMeshProto::SaveMRM(zCFileBIN &file)
{
	file.BinStartChunk(zFCHUNK_PROGMESH);
	{
		file.BinWriteWord(progMeshVersionOut);

		file.BinWriteDWord(dataSize);
		file.BinWrite(dataPool, dataSize);

		file.BinWriteByte(numSubMeshes);

		zTPMProtoDirectoryMain protoDir;
		protoDir.TransferFromProto(this);
		file.BinWrite(&protoDir, sizeof(protoDir));

		zTPMProtoDirectorySubMesh subMeshDir[32];

		for (byte i = 0; i < numSubMeshes; i++)
		{
			subMeshDir[i].TransferFromProto(this, &(subMeshList[i]));
		}

		file.BinWrite(&subMeshDir, sizeof(zTPMProtoDirectorySubMesh) * numSubMeshes);

		ArchiveMatList(file);

		file.BinWrite(&bbox3D, sizeof(bbox3D));
		obbox3D.SaveBIN(file);

		file.BinWrite(&lodParams, sizeof(lodParams));
	}

	file.BinStartChunk(zFCHUNK_PROGMESH_END);

	file.BinEndChunk();
}
