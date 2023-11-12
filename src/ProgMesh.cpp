#include "Progmesh.h"

#include "Material.h"

zOBJECT_DEFINITION(zCProgMeshProto);

zCProgMeshProto::zCSubMesh::zCSubMesh()
{
	material = NULL;
	vbStartIndex = 0;
}

zCProgMeshProto::zCSubMesh::~zCSubMesh()
{
	zDELETE(material);
}

bool32 zCProgMeshProto::LoadMatList(zCFileBIN &file)
{
	zCArchiver arc;
	arc.file = file.file;

	if (!arc.ReadHeader()) return FALSE;

	for (byte i = 0; i < numSubMeshes; i++)
	{
		zSTRING name;
		arc.ReadString("", name);

		zCMaterial *mat = zNEW(zCMaterial);

		if (!arc.ReadObject(mat)) return FALSE;

		subMeshList[i].material = mat;
	}

	if (progMeshVersionIn == PROGMESH_VERSION_GOTHIC_1_30)
	{
		arc.ReadBool("", usesAlphaTesting);
	}

	return TRUE;
}

bool32 zCProgMeshProto::LoadMRM(zCFileBIN &file)
{
	uint16 id;
	int32 len;

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
			dataPool = zMALLOC(byte, dataSize);
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

			if (!LoadMatList(file))
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

void zCProgMeshProto::SaveMatList(zCFileBIN &file)
{
	zCArchiver arc;
	arc.mode = zARC_MODE_BINARY;
	arc.objCount = meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_01 ? 0 : numSubMeshes;
	arc.file = file.file;

	arc.WriteHeader(zARC_FLAG_WRITE_BRIEF_HEADER);

	for (byte i = 0; i < numSubMeshes; i++)
	{
		zCMaterial *mat = subMeshList[i].material;

		mat->chunk.classVersion = materialVersionOut;

		arc.WriteString("", mat->name);

		arc.WriteObject(mat);
	}

	if (progMeshVersionOut == PROGMESH_VERSION_GOTHIC_1_30)
	{
		arc.WriteBool("", usesAlphaTesting);
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

		SaveMatList(file);

		file.BinWrite(&bbox3D, sizeof(bbox3D));
		obbox3D.SaveBIN(file);

		file.BinWrite(&lodParams, sizeof(lodParams));
	}

	file.BinStartChunk(zFCHUNK_PROGMESH_END);

	file.BinEndChunk();
}
