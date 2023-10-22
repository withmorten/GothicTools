#include "Archiver.h"

#include "Object.h"
#include "Mesh.h"
#include "Model.h"
#include "ModelAni.h"
#include "Visual.h"
#include "WayNet.h"
#include "Vob.h"
#include "World.h"

zCArchiver::zCArchiver()
{
	file = NULL;

	mode = zARC_MODE_ASCII;
	objCount = 0;
	nCount = 0;
	stringHashMap = NULL;
	chunkDepth = 0;
	lastChunkStart = 0;
}

zCArchiver::~zCArchiver()
{
	for (uint32 i = 0; i < nCount; i++)
	{
		GESStringMapStorage *strMapStorage = stringHashMap + i;

		zFREE(strMapStorage->MapValue);
	}

	zFREE(stringHashMap);
}

void zCArchiver::OpenFile(const zSTRING &fileName, bool32 write)
{
	file = zNEW(zFILE(fileName, write));
}

void zCArchiver::CloseFile()
{
	zDELETE(file);
}

zSTRING ParseHeaderLineArg(zSTRING &line)
{
	zSTRING arg;

	size_t pos = line.find(' ');

	if (pos != string::npos)
	{
		arg = line.substr(pos + 1);
	}

	return arg;
}

int __cdecl StringHashMapCompare(const void *value1, const void *value2)
{
	return ((GESStringMapStorage *)value1)->LinearValue - ((GESStringMapStorage *)value2)->LinearValue;
}

bool32 zCArchiver::ReadHeader()
{
	zSTRING s;

	file->ReadLine(s); // ZenGin Archive

	if (s != "ZenGin Archive")
	{
		printf("Not a ZenGin Archive\n");
		return FALSE;
	}

	file->ReadLine(s); // ver 1
	file->ReadLine(s); // zCArchiverGeneric / zCArchiverBinSafe

	file->ReadLine(s); // BINARY / ASCII / BIN_SAFE

	if (s == "BINARY")
	{
		mode = zARC_MODE_BINARY;
	}
	else if (s == "BIN_SAFE")
	{
		mode = zARC_MODE_BINARY_SAFE;
	}
	else if (s == "ASCII")
	{
		mode = zARC_MODE_ASCII;
	}

	file->ReadLine(s); // saveGame 0
	file->ReadLine(s); // END or date

	// !(arcFlags & zARC_FLAG_WRITE_BRIEF_HEADER)
	if (s != "END")
	{
		date = ParseHeaderLineArg(s);

		file->ReadLine(s); // user
		user = ParseHeaderLineArg(s);

		if (user == "XZEN" && meshAndBspVersionIn != BSPMESH_VERSION_GOTHIC_1_01) xZenIn = TRUE;

		file->ReadLine(s); // END
	}

	if (mode == zARC_MODE_BINARY || mode == zARC_MODE_ASCII)
	{
		file->ReadLine(s); // objects
		objCount = ParseHeaderLineArg(s).ToInt();

		file->ReadLine(s); // END
		file->ReadLine(s); //
	}
	else if (mode == zARC_MODE_BINARY_SAFE)
	{
		int32 ver;
		file->Read(&ver, sizeof(ver));

		if (ver != zARC_VERSION)
		{
			printf("Wrong BIN_SAFE archive version, expected %d but got %d!\n", zARC_VERSION, ver);

			return FALSE;
		}

		file->Read(&objCount, sizeof(objCount));

		int32 chunkPos;
		file->Read(&chunkPos, sizeof(chunkPos));

		int32 savePos = file->Pos();

		file->Seek(chunkPos);
		file->Read(&nCount, sizeof(nCount));

		stringHashMap = zMALLOC(GESStringMapStorage, nCount);

		for (uint32 i = 0; i < nCount; i++)
		{
			GESStringMapStorage *strMapStorage = stringHashMap + i;

			file->Read(&strMapStorage->HashMapStringLen, sizeof(strMapStorage->HashMapStringLen));
			file->Read(&strMapStorage->LinearValue, sizeof(strMapStorage->LinearValue));
			file->Read(&strMapStorage->HashValue, sizeof(strMapStorage->HashValue));

			strMapStorage->MapValue = zMALLOC(char, strMapStorage->HashMapStringLen + 1);
			file->Read(strMapStorage->MapValue, strMapStorage->HashMapStringLen);
			strMapStorage->MapValue[strMapStorage->HashMapStringLen] = '\0';
		}

		qsort(stringHashMap, nCount, sizeof(*stringHashMap), StringHashMapCompare);

		file->Seek(savePos);

		return TRUE;
	}
	else
	{
		printf("Mode is neither ASCII, BINARAY or BIN_SAFE!\n");

		return FALSE;
	}

	return TRUE;
}

void zCArchiver::WriteHeader(int32 flags)
{
	file->WriteLine("ZenGin Archive");
	file->WriteLine("ver 1");
	file->WriteLine("zCArchiverGeneric");
	file->WriteLine(mode == zARC_MODE_ASCII ? "ASCII" : "BINARY");
	file->WriteLine("saveGame 0");

	if (!(flags & zARC_FLAG_WRITE_BRIEF_HEADER))
	{
		file->WriteLine("date " + date);

		if (xZenOut)
		{
			file->WriteLine("user XZEN");
		}
		else
		{
			if (user == "XZEN")
			{
				file->WriteLine("user GothicZEN");
			}
			else
			{
				file->WriteLine("user " + user);
			}
		}
	}

	file->WriteLine("END");

	zSTRING s;

	s += "objects " + zSTRING(objCount);
	size_t fill = strlen("objects 0        ") - s.Length();

	for (size_t i = 0; i < fill; i++)
	{
		s.Append(" ");
	}

	file->WriteLine(s);

	file->WriteLine("END");
	file->WriteLine();
}

zCObject *zCArchiver::CreateObject(zTChunkRecord &chunk)
{
	if (chunk.IsNull())
	{
		return zNEW(zCObject);
	}
	else if (chunk.IsReference())
	{
		// TODO for now just add the ref to the obj
		// later actually copy the data as well ...
		zCObject *ref = objects[chunk.objectIndex];
		zCObject *obj = CreateObject(ref->chunk);

		obj->ref = ref;

		return obj;
	}
	else if (chunk.className.Contains(zCWorld::GetClassName()))
	{
		return zNEW(zCWorld);
	}
	else if (chunk.className.Contains(zCWaypoint::GetClassName()))
	{
		return zNEW(zCWaypoint);
	}
	else if (chunk.className.Contains(zCWayNet::GetClassName()))
	{
		return zNEW(zCWayNet);
	}
	else if (chunk.className.Contains(zCAICamera::GetClassName()))
	{
		return zNEW(zCAICamera);
	}
	else if (chunk.className.Contains(zCMaterial::GetClassName()))
	{
		return zNEW(zCMaterial);
	}
	else if (chunk.className.Contains(zCPolyStrip::GetClassName()))
	{
		return zNEW(zCPolyStrip);
	}
	else if (chunk.className.Contains(zCDecal::GetClassName()))
	{
		return zNEW(zCDecal);
	}
	else if (chunk.className.Contains(zCParticleFX::GetClassName()))
	{
		return zNEW(zCParticleFX);
	}
	else if (chunk.className.Contains(zCMesh::GetClassName()))
	{
		return zNEW(zCMesh);
	}
	else if (chunk.className.Contains(zCModel::GetClassName()))
	{
		return zNEW(zCModel);
	}
	else if (chunk.className.Contains(zCModelAni::GetClassName()))
	{
		return zNEW(zCModelAni);
	}
	else if (chunk.className.Contains(zCModelMeshLib::GetClassName()))
	{
		return zNEW(zCModelMeshLib);
	}
	else if (chunk.className.Contains(zCMorphMesh::GetClassName()))
	{
		return zNEW(zCMorphMesh);
	}
	else if (chunk.className.Contains(zCProgMeshProto::GetClassName()))
	{
		return zNEW(zCProgMeshProto);
	}
	else if (chunk.className.Contains(zCMeshSoftSkin::GetClassName()))
	{
		return zNEW(zCMeshSoftSkin);
	}
	else if (chunk.className.Contains(zCCamTrj_KeyFrame::GetClassName()))
	{
		return zNEW(zCCamTrj_KeyFrame);
	}
	else if (chunk.className.Contains(zCCSCamera::GetClassName()))
	{
		return zNEW(zCCSCamera);
	}
	else if (chunk.className.Contains(zCVobLevelCompo::GetClassName()))
	{
		return zNEW(zCVobLevelCompo);
	}
	else if (chunk.className.Contains(zCVobLight::GetClassName()))
	{
		return zNEW(zCVobLight);
	}
	else if (chunk.className.Contains(zCVobSpot::GetClassName()))
	{
		return zNEW(zCVobSpot);
	}
	else if (chunk.className.Contains(zCVobStair::GetClassName()))
	{
		return zNEW(zCVobStair);
	}
	else if (chunk.className.Contains(zCVobStartpoint::GetClassName()))
	{
		return zNEW(zCVobStartpoint);
	}
	else if (chunk.className.Contains(zCVobWaypoint::GetClassName()))
	{
		return zNEW(zCVobWaypoint);
	}
	else if (chunk.className.Contains(zCCodeMaster::GetClassName()))
	{
		return zNEW(zCCodeMaster);
	}
	else if (chunk.className.Contains(zCMessageFilter::GetClassName()))
	{
		return zNEW(zCMessageFilter);
	}
	else if (chunk.className.Contains(zCMoverControler::GetClassName()))
	{
		return zNEW(zCMoverControler);
	}
	else if (chunk.className.Contains(zCTriggerWorldStart::GetClassName()))
	{
		return zNEW(zCTriggerWorldStart);
	}
	else if (chunk.className.Contains(zCTriggerUntouch::GetClassName()))
	{
		return zNEW(zCTriggerUntouch);
	}
	else if (chunk.className.Contains(zCTrigger::GetClassName()))
	{
		if (chunk.className.Contains(zCMover::GetClassName()))
		{
			return zNEW(zCMover);
		}
		else if (chunk.className.Contains(zCTriggerTeleport::GetClassName()))
		{
			return zNEW(zCTriggerTeleport);
		}
		else if (chunk.className.Contains(zCTriggerList::GetClassName()))
		{
			return zNEW(zCTriggerList);
		}
		else if (chunk.className.Contains(oCCSTrigger::GetClassName()))
		{
			return zNEW(oCCSTrigger);
		}
		else if (chunk.className.Contains(oCTriggerChangeLevel::GetClassName()))
		{
			return zNEW(oCTriggerChangeLevel);
		}
		else if (chunk.className.Contains(oCTriggerScript::GetClassName()))
		{
			return zNEW(oCTriggerScript);
		}
		else
		{
			return zNEW(zCTrigger);
		}
	}
	else if (chunk.className.Contains(zCEarthquake::GetClassName()))
	{
		return zNEW(zCEarthquake);
	}
	else if (chunk.className.Contains(zCMusicControler::GetClassName()))
	{
		return zNEW(zCMusicControler);
	}
	else if (chunk.className.Contains(zCPFXControler::GetClassName()))
	{
		return zNEW(zCPFXControler);
	}
	else if (chunk.className.Contains(zCTouchDamage::GetClassName()))
	{
		if (chunk.className.Contains(oCTouchDamage::GetClassName()))
		{
			return zNEW(oCTouchDamage);
		}
		else
		{
			printf("Error: zCTouchDamage is not implemented like the game does it, use oCTouchDamage instead!\n");

			return NULL;
		}
	}
	else if (chunk.className.Contains(zCTouchAnimate::GetClassName()))
	{
		if (chunk.className.Contains(zCTouchAnimateSound::GetClassName()))
		{
			return zNEW(zCTouchAnimateSound);
		}
		else
		{
			return zNEW(zCTouchAnimate);
		}
	}
	else if (chunk.className.Contains(zCVobAnimate::GetClassName()))
	{
		return zNEW(zCVobAnimate);
	}
	else if (chunk.className.Contains(zCVobLensFlare::GetClassName()))
	{
		return zNEW(zCVobLensFlare);
	}
	else if (chunk.className.Contains(zCVobScreenFX::GetClassName()))
	{
		return zNEW(zCVobScreenFX);
	}
	else if (chunk.className.Contains(oCVisualFX::GetClassName()))
	{
		if (chunk.className.Contains(oCVisFX_Lightning::GetClassName()))
		{
			return zNEW(oCVisFX_Lightning);
		}
		else if (chunk.className.Contains(oCVisFX_MultiTarget::GetClassName()))
		{
			return zNEW(oCVisFX_MultiTarget);
		}
		else
		{
			return zNEW(oCVisualFX);
		}
	}
	else if (chunk.className.Contains(zCZoneZFog::GetClassName()))
	{
		if (chunk.className.Contains(zCZoneZFogDefault::GetClassName()))
		{
			return zNEW(zCZoneZFogDefault);
		}
		else
		{
			return zNEW(zCZoneZFog);
		}
	}
	else if (chunk.className.Contains(zCZoneVobFarPlane::GetClassName()))
	{
		if (chunk.className.Contains(zCZoneVobFarPlaneDefault::GetClassName()))
		{
			return zNEW(zCZoneVobFarPlaneDefault);
		}
		else
		{
			return zNEW(zCZoneVobFarPlane);
		}
	}
	else if (chunk.className.Contains(zCVobSound::GetClassName()))
	{
		if (chunk.className.Contains(zCVobSoundDaytime::GetClassName()))
		{
			return zNEW(zCVobSoundDaytime);
		}
		else
		{
			return zNEW(zCVobSound);
		}
	}
	else if (chunk.className.Contains(oCZoneMusic::GetClassName()))
	{
		if (chunk.className.Contains(oCZoneMusicDefault::GetClassName()))
		{
			return zNEW(oCZoneMusicDefault);
		}
		else
		{
			return zNEW(oCZoneMusic);
		}
	}
	else if (chunk.className.Contains(zCZoneReverb::GetClassName()))
	{
		if (chunk.className.Contains(zCZoneReverbDefault::GetClassName()))
		{
			return zNEW(zCZoneReverbDefault);
		}
		else
		{
			return zNEW(zCZoneReverb);
		}
	}
	else if (chunk.className.Contains(oCDummyVobGenerator::GetClassName()))
	{
		return zNEW(oCDummyVobGenerator);
	}
	else if (chunk.className.Contains(oCObjectGenerator::GetClassName()))
	{
		return zNEW(oCObjectGenerator);
	}
	else if (chunk.className.Contains(oCItem::GetClassName()))
	{
		return zNEW(oCItem);
	}
	else if (chunk.className.Contains(oCMOB::GetClassName()))
	{
		if (chunk.className.Contains(oCMobInter::GetClassName()))
		{
			if (chunk.className.Contains(oCMobBed::GetClassName()))
			{
				return zNEW(oCMobBed);
			}
			else if (chunk.className.Contains(oCMobFire::GetClassName()))
			{
				return zNEW(oCMobFire);
			}
			else if (chunk.className.Contains(oCMobItemSlot::GetClassName()))
			{
				return zNEW(oCMobItemSlot);
			}
			else if (chunk.className.Contains(oCMobLadder::GetClassName()))
			{
				return zNEW(oCMobLadder);
			}
			else if (chunk.className.Contains(oCMobContainer::GetClassName()))
			{
				return zNEW(oCMobContainer);
			}
			else if (chunk.className.Contains(oCMobDoor::GetClassName()))
			{
				return zNEW(oCMobDoor);
			}
			else if (chunk.className.Contains(oCMobSwitch::GetClassName()))
			{
				return zNEW(oCMobSwitch);
			}
			else if (chunk.className.Contains(oCMobWheel::GetClassName()))
			{
				return zNEW(oCMobWheel);
			}
			else
			{
				return zNEW(oCMobInter);
			}
		}
		else
		{
			return zNEW(oCMOB);
		}
	}
	else if (chunk.className.Contains(oCNpc::GetClassName()))
	{
		return zNEW(oCNpc);
	}
	else if (chunk.className.Contains(zCVob::GetClassName()))
	{
		if (chunk.className != zCVob::GetClassName()) printf("%s\n", chunk.className.ToChar());

		return zNEW(zCVob);
	}
	else
	{
		printf("Error at %d: %s is not implemented!\n", file->Pos(), chunk.className.ToChar());

		return NULL;
	}
}

void zCArchiver::PeekChunk(zTChunkRecord &chunk)
{
	int32 savePos = file->Pos();

	ReadChunkStart(chunk);
	PopChunk();

	file->Seek(savePos);
}

void ParseChunkStartLine(zTChunkRecord &chunk, zSTRING &s)
{
	size_t p1 = s.find("[");
	size_t p2 = s.find(" ", p1 + 1);
	size_t p3 = s.find(" ", p2 + 1);
	size_t p4 = s.find(" ", p3 + 1);
	size_t p5 = s.find("]");

	chunk.name = s.substr(p1 + 1, p2 - p1 - 1);
	chunk.className = s.substr(p2 + 1, p3 - p2 - 1);
	chunk.classVersion = zSTRING(s.substr(p3 + 1, p4 - p3 - 1)).ToInt();
	chunk.objectIndex = zSTRING(s.substr(p4 + 1, p5 - p4 - 1)).ToInt();
}

bool32 zCArchiver::ReadChunkStart(zTChunkRecord &chunk)
{
	if (mode == zARC_MODE_BINARY)
	{
		uint32 size;
		file->Read(&size, sizeof(size));

		file->Read(&chunk.classVersion, sizeof(chunk.classVersion));
		file->Read(&chunk.objectIndex, sizeof(chunk.objectIndex));
		file->Read(chunk.name);
		file->Read(chunk.className);
	}
	else if (mode == zARC_MODE_ASCII)
	{
		zSTRING s;
		file->ReadLine(s, TRUE);

		ParseChunkStartLine(chunk, s);
	}
	else if (mode == zARC_MODE_BINARY_SAFE)
	{
		zSTRING s;

		ReadBinSafeValue(zARC2_ID_STRING, &s);

		ParseChunkStartLine(chunk, s);
	}

	PushChunk();

	return TRUE;
}

bool32 zCArchiver::ReadChunkEnd()
{
	PopChunk();

	if (mode == zARC_MODE_BINARY)
	{
		// do nothing
	}
	else if (mode == zARC_MODE_ASCII)
	{
		zSTRING s;
		file->ReadLine(s, TRUE);

		if (s != "[]") return FALSE;
	}
	else if (mode == zARC_MODE_BINARY_SAFE)
	{
		zSTRING s;
		ReadBinSafeValue(zARC2_ID_STRING, &s);

		if (s != "[]") return FALSE;
	}

	return TRUE;
}

zCObject *zCArchiver::ReadObject(const char *chunkName, zCObject *useThis)
{
	zTChunkRecord chunk;
	PeekChunk(chunk);

	if (chunk.name != chunkName) return NULL;

	return ReadObject(useThis);
}

zCObject *zCArchiver::ReadObject(zCObject *useThis)
{
	zCObject *object = useThis;

	zTChunkRecord chunk;
	ReadChunkStart(chunk);

	if (!object) object = CreateObject(chunk);

	if (!object) return NULL;

	object->chunk = chunk;

	if (!object->IsNull() && !object->IsReference())
	{
		objects.Insert(object);

		if (!object->Unarchive(*this)) return NULL;
	}

	if (!ReadChunkEnd())
	{
		printf("Error at position %d\n", file->Pos());

		return NULL;
	}

	return object;
}

bool32 zCArchiver::ReadASCIIValue(const char *entryName, const char *typeName, zSTRING &value)
{
	int32 savePos = file->Pos();

	zSTRING l;
	file->ReadLine(l, TRUE);

	size_t eq = l.find('=');
	size_t sc = l.find(':');

	zSTRING n = l.substr(0, eq);
	zSTRING t = l.substr(eq + 1, sc - eq - 1);
	value = l.substr(sc + 1);

	if (n != entryName || t != typeName)
	{
		file->Seek(savePos);

		printf("ReadASCIIValue failed at %d, expected entry %s of type %s, but got %s of type %s\n", savePos, entryName, typeName, n.ToChar(), t.ToChar());

		return FALSE;
	}

	return TRUE;
}

bool32 zCArchiver::ReadBinSafeValue(const char *entryName, zTArchiveTypeID entryType, void *buffer)
{
	zSTRING s;
	ReadBinSafeValue(zARC2_ID_HASH, &s);

	if (s != entryName)
	{
		printf("ReadBinSafeValue failed at %d, expected entry %s\n", file->Pos(), entryName);

		return FALSE;
	}

	return ReadBinSafeValue(entryType, buffer);
}

bool32 zCArchiver::ReadBinSafeValue(zTArchiveTypeID entryType, void *buffer)
{
	byte archiveTypeID;
	file->Read(&archiveTypeID, sizeof(archiveTypeID));

	if (entryType != archiveTypeID)
	{
		printf("ReadBinSafeValue failed at %d, expected type %d, but got type %d\n", file->Pos(), entryType, archiveTypeID);

		return FALSE;
	}

	switch (entryType)
	{
	case zARC2_ID_STRING:
	{
		uint16 size;
		file->Read(&size, sizeof(size));

		if (size > 0) file->Read(readBuffer, size);
		readBuffer[size] = '\0';

		zSTRING *s = (zSTRING *)buffer;
		*s = (char *)readBuffer;

		break;
	}

	case zARC2_ID_INTEGER:
	{
		file->Read(buffer, sizeof(int32));

		break;
	}

	case zARC2_ID_FLOAT:
	{
		file->Read(buffer, sizeof(float));

		break;
	}

	case zARC2_ID_BYTE:
	{
		file->Read(buffer, sizeof(byte));

		break;
	}

	case zARC2_ID_WORD:
	{
		file->Read(buffer, sizeof(uint16));

		break;
	}

	case zARC2_ID_BOOL:
	{
		file->Read(buffer, sizeof(bool32));

		break;
	}

	case zARC2_ID_VEC3:
	{
		file->Read(buffer, sizeof(zVEC3));

		break;
	}

	case zARC2_ID_COLOR:
	{
		file->Read(buffer, sizeof(zCOLOR));

		break;
	}

	case zARC2_ID_RAW:
	{
		uint16 size;
		file->Read(&size, sizeof(size));

		file->Read(buffer, size);

		break;
	}

	case zARC2_ID_RAWFLOAT:
	{
		uint16 size;
		file->Read(&size, sizeof(size));

		file->Read(buffer, size);

		break;
	}

	case zARC2_ID_ENUM:
	{
		file->Read(buffer, sizeof(int32));

		break;
	}

	case zARC2_ID_HASH:
	{
		int32 linearValue;
		file->Read(&linearValue, sizeof(linearValue));

		zSTRING *s = (zSTRING *)buffer;
		*s = stringHashMap[linearValue].MapValue;

		break;
	}
	}

	return TRUE;
}

void zCArchiver::ReadInt(const char *entryName, int32 &value)
{
	if (mode == zARC_MODE_BINARY)
	{
		file->Read(&value, sizeof(value));
	}
	else if (mode == zARC_MODE_ASCII)
	{
		zSTRING v;

		if (ReadASCIIValue(entryName, "int", v))
		{
			value = v.ToInt();
		}
	}
	else if (mode == zARC_MODE_BINARY_SAFE)
	{
		ReadBinSafeValue(entryName, zARC2_ID_INTEGER, &value);
	}
}

void zCArchiver::ReadByte(const char *entryName, byte &value)
{
	if (mode == zARC_MODE_BINARY)
	{
		file->Read(&value, sizeof(value));
	}
	else if (mode == zARC_MODE_ASCII)
	{
		zSTRING v;

		if (ReadASCIIValue(entryName, "int", v))
		{
			value = (byte)v.ToInt();
		}
	}
	else if (mode == zARC_MODE_BINARY_SAFE)
	{
		ReadBinSafeValue(entryName, zARC2_ID_BYTE, &value);
	}
}

void zCArchiver::ReadWord(const char *entryName, uint16 &value)
{
	if (mode == zARC_MODE_BINARY)
	{
		file->Read(&value, sizeof(value));
	}
	else if (mode == zARC_MODE_ASCII)
	{
		zSTRING v;

		if (ReadASCIIValue(entryName, "int", v))
		{
			value = (uint16)v.ToInt();
		}
	}
	else if (mode == zARC_MODE_BINARY_SAFE)
	{
		ReadBinSafeValue(entryName, zARC2_ID_WORD, &value);
	}
}

void zCArchiver::ReadFloat(const char *entryName, float &value)
{
	if (mode == zARC_MODE_BINARY)
	{
		file->Read(&value, sizeof(value));
	}
	else if (mode == zARC_MODE_ASCII)
	{
		zSTRING v;

		if (ReadASCIIValue(entryName, "float", v))
		{
			value = v.ToFloat();
		}
	}
	else if (mode == zARC_MODE_BINARY_SAFE)
	{
		ReadBinSafeValue(entryName, zARC2_ID_FLOAT, &value);
	}
}

void zCArchiver::ReadBool(const char *entryName, bool32 &value)
{
	if (mode == zARC_MODE_BINARY)
	{
		byte b;
		file->Read(&b, sizeof(b));
		value = b;
	}
	else if (mode == zARC_MODE_ASCII)
	{
		zSTRING v;

		if (ReadASCIIValue(entryName, "bool", v))
		{
			value = (bool32)v.ToInt();
		}
	}
	else if (mode == zARC_MODE_BINARY_SAFE)
	{
		ReadBinSafeValue(entryName, zARC2_ID_BOOL, &value);
	}
}

void zCArchiver::ReadString(const char *entryName, zSTRING &value)
{
	if (mode == zARC_MODE_BINARY)
	{
		file->Read(value);
	}
	else if (mode == zARC_MODE_ASCII)
	{
		zSTRING v;

		if (ReadASCIIValue(entryName, "string", v))
		{
			value = v;
		}
	}
	else if (mode == zARC_MODE_BINARY_SAFE)
	{
		ReadBinSafeValue(entryName, zARC2_ID_STRING, &value);
	}
}

void zCArchiver::ReadVec3(const char *entryName, zVEC3 &value)
{
	if (mode == zARC_MODE_BINARY)
	{
		file->Read(&value, sizeof(value));
	}
	else if (mode == zARC_MODE_ASCII)
	{
		zSTRING v;

		if (ReadASCIIValue(entryName, "vec3", v))
		{
			sscanf(v.ToChar(), "%f %f %f", &value[0], &value[1], &value[2]);
		}
	}
	else if (mode == zARC_MODE_BINARY_SAFE)
	{
		ReadBinSafeValue(entryName, zARC2_ID_VEC3, &value);
	}
}

void zCArchiver::ReadColor(const char *entryName, zCOLOR &value)
{
	if (mode == zARC_MODE_BINARY)
	{
		file->Read(&value, sizeof(value));
	}
	else if (mode == zARC_MODE_ASCII)
	{
		zSTRING v;

		if (ReadASCIIValue(entryName, "color", v))
		{
			int32 r, g, b, alpha;
			sscanf(v.ToChar(), "%d %d %d %d", &r, &g, &b, &alpha);

			value.r = (byte)r;
			value.g = (byte)g;
			value.b = (byte)b;
			value.alpha = (byte)alpha;
		}
	}
	else if (mode == zARC_MODE_BINARY_SAFE)
	{
		ReadBinSafeValue(entryName, zARC2_ID_COLOR, &value);
	}
}

void zCArchiver::ReadEnum(const char *entryName, int32 &value)
{
	if (mode == zARC_MODE_BINARY)
	{
		byte b;
		file->Read(&b, sizeof(b));
		value = b;
	}
	else if (mode == zARC_MODE_ASCII)
	{
		zSTRING v;

		if (ReadASCIIValue(entryName, "enum", v))
		{
			value = v.ToInt();
		}
	}
	else if (mode == zARC_MODE_BINARY_SAFE)
	{
		ReadBinSafeValue(entryName, zARC2_ID_ENUM, &value);
	}
}

void zCArchiver::ReadRaw(const char *entryName, void *buffer, uint32 size)
{
	if (mode == zARC_MODE_BINARY)
	{
		file->Read(buffer, size);
	}
	else if (mode == zARC_MODE_ASCII)
	{
		zSTRING v;

		if (ReadASCIIValue(entryName, "raw", v))
		{
			byte *destBuffer = (byte *)buffer;
			char *charBuffer = v.ToChar();
			int32 val;

			for (uint32 i = 0; i < size; i++)
			{
				sscanf(charBuffer + i * 2, "%2x", &val);

				destBuffer[i] = (byte)val;
			}
		}
	}
	else if (mode == zARC_MODE_BINARY_SAFE)
	{
		ReadBinSafeValue(entryName, zARC2_ID_RAW, buffer);
	}
}

void zCArchiver::ReadRawFloat(const char *entryName, void *buffer, uint32 size)
{
	if (mode == zARC_MODE_BINARY)
	{
		file->Read(buffer, size);
	}
	else if (mode == zARC_MODE_ASCII)
	{
		zSTRING v;

		if (ReadASCIIValue(entryName, "rawFloat", v))
		{
			uint32 numFloats = size / sizeof(float);
			float *destBuffer = (float *)buffer;

			for (uint32 i = 0; i < numFloats; i++)
			{
				destBuffer[i] = v.PickWord(i + 1, " ", " ").ToFloat();
			}
		}
	}
	else if (mode == zARC_MODE_BINARY_SAFE)
	{
		ReadBinSafeValue(entryName, zARC2_ID_RAWFLOAT, buffer);
	}
}

void zCArchiver::WriteChunkStart(const char *chunkName)
{
	zTChunkRecord chunk;

	chunk.classVersion = 0;
	chunk.objectIndex = 0;
	chunk.name = chunkName;
	chunk.className = "%";

	WriteChunkStart(chunk);
}

void zCArchiver::WriteChunkStart(zTChunkRecord &chunk)
{
	if (mode == zARC_MODE_BINARY)
	{
		lastChunkStart = file->Pos();

		uint32 size = 0;
		file->Write(&size, sizeof(size));

		file->Write(&chunk.classVersion, sizeof(chunk.classVersion));
		file->Write(&chunk.objectIndex, sizeof(chunk.objectIndex));
		file->Write(chunk.name);
		file->Write(chunk.className);
	}
	else if (mode == zARC_MODE_ASCII)
	{
		zSTRING s = "[" + chunk.name + " " + chunk.className + " " + zSTRING(chunk.classVersion) + " " + zSTRING(chunk.objectIndex) + "]";

		file->WriteLineIndented(chunkDepth, s);
	}

	PushChunk();
}

void zCArchiver::WriteChunkEnd()
{
	PopChunk();

	if (mode == zARC_MODE_BINARY)
	{
		int32 savePos = file->Pos();
		file->Seek(lastChunkStart);

		uint32 size = savePos - lastChunkStart;
		file->Write(&size, sizeof(size));

		file->Seek(savePos);
	}
	else if (mode == zARC_MODE_ASCII)
	{
		file->WriteLineIndented(chunkDepth, "[]");
	}
}

void zCArchiver::WriteChunk(const char *chunkName)
{
	WriteChunkStart(chunkName);
	WriteChunkEnd();
}

void zCArchiver::WriteObject(zCObject *object)
{
	WriteChunkStart(object->chunk);

	if (!object->IsNull() && !object->IsReference())
	{
		object->Archive(*this);
	}

	WriteChunkEnd();
}

void zCArchiver::WriteASCIILine(const char *entryName, const char *entryType, zSTRING &value)
{
	zSTRING n = entryName;
	zSTRING t = entryType;

	zSTRING l = n + "=" + t + ":" + value;

	file->WriteLineIndented(chunkDepth, l);
}

void zCArchiver::WriteInt(const char *entryName, int32 value)
{
	if (mode == zARC_MODE_BINARY)
	{
		file->Write(&value, sizeof(value));
	}
	else if (mode == zARC_MODE_ASCII)
	{
		WriteASCIILine(entryName, "int", zSTRING(value));
	}
}

void zCArchiver::WriteByte(const char *entryName, byte value)
{
	if (mode == zARC_MODE_BINARY)
	{
		file->Write(&value, sizeof(value));
	}
	else if (mode == zARC_MODE_ASCII)
	{
		WriteASCIILine(entryName, "int", zSTRING(value));
	}
}

void zCArchiver::WriteWord(const char *entryName, uint16 value)
{
	if (mode == zARC_MODE_BINARY)
	{
		file->Write(&value, sizeof(value));
	}
	else if (mode == zARC_MODE_ASCII)
	{
		WriteASCIILine(entryName, "int", zSTRING(value));
	}
}

void zCArchiver::WriteFloat(const char *entryName, float value)
{
	if (mode == zARC_MODE_BINARY)
	{
		file->Write(&value, sizeof(value));
	}
	else if (mode == zARC_MODE_ASCII)
	{
		WriteASCIILine(entryName, "float", zSTRING(value));
	}
}

void zCArchiver::WriteBool(const char *entryName, bool32 value)
{
	if (mode == zARC_MODE_BINARY)
	{
		byte b = value;
		file->Write(&b, sizeof(b));
	}
	else if (mode == zARC_MODE_ASCII)
	{
		WriteASCIILine(entryName, "bool", zSTRING(value));
	}
}

void zCArchiver::WriteString(const char *entryName, zSTRING &value)
{
	if (mode == zARC_MODE_BINARY)
	{
		file->Write(value);
	}
	else if (mode == zARC_MODE_ASCII)
	{
		WriteASCIILine(entryName, "string", value);
	}
}

void zCArchiver::WriteVec3(const char *entryName, zVEC3 &value)
{
	if (mode == zARC_MODE_BINARY)
	{
		file->Write(&value, sizeof(value));
	}
	else if (mode == zARC_MODE_ASCII)
	{
		WriteASCIILine(entryName, "vec3", zSTRING(value[VX]) + " " + zSTRING(value[VY]) + " " + zSTRING(value[VZ]));
	}
}

void zCArchiver::WriteColor(const char *entryName, zCOLOR &value)
{
	if (mode == zARC_MODE_BINARY)
	{
		file->Write(&value, sizeof(value));
	}
	else if (mode == zARC_MODE_ASCII)
	{
		WriteASCIILine(entryName, "color", zSTRING(value.r) + " " + zSTRING(value.g) + " " + zSTRING(value.b) + " " + zSTRING(value.alpha));
	}
}

void zCArchiver::WriteEnum(const char *entryName, int32 value)
{
	if (mode == zARC_MODE_BINARY)
	{
		byte b = value;
		file->Write(&b, sizeof(b));
	}
	else if (mode == zARC_MODE_ASCII)
	{
		WriteASCIILine(entryName, "enum", zSTRING(value));
	}
}

void zCArchiver::WriteRaw(const char *entryName, void *buffer, uint32 size)
{
	if (mode == zARC_MODE_BINARY)
	{
		file->Write(buffer, size);
	}
	else if (mode == zARC_MODE_ASCII)
	{
		zSTRING value;
		byte *sourceBuffer = (byte *)buffer;

		for (uint32 i = 0; i < size; i++)
		{
			char charBuffer[32];
			sprintf(charBuffer, "%02x", sourceBuffer[i]);

			value += charBuffer;
		}

		WriteASCIILine(entryName, "raw", value);
	}
}

void zCArchiver::WriteRawFloat(const char *entryName, void *buffer, uint32 size)
{
	if (mode == zARC_MODE_BINARY)
	{
		file->Write(buffer, size);
	}
	else if (mode == zARC_MODE_ASCII)
	{
		zSTRING value;
		uint32 numFloats = size / sizeof(float);
		float *sourceBuffer = (float *)buffer;

		for (uint32 i = 0; i < numFloats; i++)
		{
			value += zSTRING(sourceBuffer[i]) + " ";
		}

		WriteASCIILine(entryName, "rawFloat", value);
	}
}

