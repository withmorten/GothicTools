#include "Archiver.h"

#include "Object.h"
#include "Mesh.h"
#include "Model.h"
#include "ModelAni.h"
#include "Visual.h"
#include "WayNet.h"
#include "Vob.h"
#include "World.h"
#include "ObjectRegistry.h"

zCArchiver::zCArchiver()
{
	file = NULL;

	mode = zARC_MODE_ASCII;
	objCount = 0;
	nCount = 0;
	stringHashMap = NULL;
	chunkDepth = 0;
	lastChunkStart = 0;

	registry = NULL;
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

		if (user == "XZEN" && gothicVersionIn == GOTHIC_VERSION_108) xZenIn = TRUE;

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
	file->WriteLine(mode == zARC_MODE_BINARY ? "BINARY" : "ASCII");
	file->WriteLine("saveGame 0");

	if (!(flags & zARC_FLAG_WRITE_BRIEF_HEADER) && (!user.IsEmpty() && !date.IsEmpty()))
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
#define zNEW_OBJECT(name) \
obj = zNEW(name); \
obj->classVersion = chunk.classVersion; \
obj->objectIndex = chunk.objectIndex; \
obj->arc_className = chunk.className; \
if (registry) registry->Insert(name::className, obj)

	zCObject *obj;

	if (chunk.className.Contains(zCWorld::className))
	{
		zNEW_OBJECT(zCWorld);
	}
	else if (chunk.className.Contains(zCWaypoint::className))
	{
		zNEW_OBJECT(zCWaypoint);
	}
	else if (chunk.className.Contains(zCWayNet::className))
	{
		zNEW_OBJECT(zCWayNet);
	}
	else if (chunk.className.Contains(zCAICamera::className))
	{
		zNEW_OBJECT(zCAICamera);
	}
	else if (chunk.className.Contains(zCMaterial::className))
	{
		zNEW_OBJECT(zCMaterial);
	}
	else if (chunk.className.Contains(zCPolyStrip::className))
	{
		zNEW_OBJECT(zCPolyStrip);
	}
	else if (chunk.className.Contains(zCDecal::className))
	{
		zNEW_OBJECT(zCDecal);
	}
	else if (chunk.className.Contains(zCParticleFX::className))
	{
		zNEW_OBJECT(zCParticleFX);
	}
	else if (chunk.className.Contains(zCMesh::className))
	{
		zNEW_OBJECT(zCMesh);
	}
	else if (chunk.className.Contains(zCModel::className))
	{
		zNEW_OBJECT(zCModel);
	}
	else if (chunk.className.Contains(zCModelAni::className))
	{
		zNEW_OBJECT(zCModelAni);
	}
	else if (chunk.className.Contains(zCModelMeshLib::className))
	{
		zNEW_OBJECT(zCModelMeshLib);
	}
	else if (chunk.className.Contains(zCMorphMesh::className))
	{
		zNEW_OBJECT(zCMorphMesh);
	}
	else if (chunk.className.Contains(zCProgMeshProto::className))
	{
		zNEW_OBJECT(zCProgMeshProto);
	}
	else if (chunk.className.Contains(zCMeshSoftSkin::className))
	{
		zNEW_OBJECT(zCMeshSoftSkin);
	}
	else if (chunk.className.Contains(zCCamTrj_KeyFrame::className))
	{
		zNEW_OBJECT(zCCamTrj_KeyFrame);
	}
	else if (chunk.className.Contains(zCCSCamera::className))
	{
		zNEW_OBJECT(zCCSCamera);
	}
	else if (chunk.className.Contains(zCVobLevelCompo::className))
	{
		zNEW_OBJECT(zCVobLevelCompo);
	}
	else if (chunk.className.Contains(zCVobLight::className))
	{
		zNEW_OBJECT(zCVobLight);
	}
	else if (chunk.className.Contains(zCVobSpot::className))
	{
		zNEW_OBJECT(zCVobSpot);
	}
	else if (chunk.className.Contains(zCVobStair::className))
	{
		zNEW_OBJECT(zCVobStair);
	}
	else if (chunk.className.Contains(zCVobStartpoint::className))
	{
		zNEW_OBJECT(zCVobStartpoint);
	}
	else if (chunk.className.Contains(zCVobWaypoint::className))
	{
		zNEW_OBJECT(zCVobWaypoint);
	}
	else if (chunk.className.Contains(zCCodeMaster::className))
	{
		zNEW_OBJECT(zCCodeMaster);
	}
	else if (chunk.className.Contains(zCMessageFilter::className))
	{
		zNEW_OBJECT(zCMessageFilter);
	}
	else if (chunk.className.Contains(zCMoverControler::className))
	{
		zNEW_OBJECT(zCMoverControler);
	}
	else if (chunk.className.Contains(zCTriggerWorldStart::className))
	{
		zNEW_OBJECT(zCTriggerWorldStart);
	}
	else if (chunk.className.Contains(zCTriggerUntouch::className))
	{
		zNEW_OBJECT(zCTriggerUntouch);
	}
	else if (chunk.className.Contains(zCTrigger::className))
	{
		if (chunk.className.Contains(zCMover::className))
		{
			zNEW_OBJECT(zCMover);
		}
		else if (chunk.className.Contains(zCTriggerTeleport::className))
		{
			zNEW_OBJECT(zCTriggerTeleport);
		}
		else if (chunk.className.Contains(zCTriggerList::className))
		{
			zNEW_OBJECT(zCTriggerList);
		}
		else if (chunk.className.Contains(oCCSTrigger::className))
		{
			zNEW_OBJECT(oCCSTrigger);
		}
		else if (chunk.className.Contains(oCTriggerChangeLevel::className))
		{
			zNEW_OBJECT(oCTriggerChangeLevel);
		}
		else if (chunk.className.Contains(oCTriggerScript::className))
		{
			zNEW_OBJECT(oCTriggerScript);
		}
		else
		{
			zNEW_OBJECT(zCTrigger);
		}
	}
	else if (chunk.className.Contains(zCEarthquake::className))
	{
		zNEW_OBJECT(zCEarthquake);
	}
	else if (chunk.className.Contains(zCMusicControler::className))
	{
		zNEW_OBJECT(zCMusicControler);
	}
	else if (chunk.className.Contains(zCPFXControler::className))
	{
		zNEW_OBJECT(zCPFXControler);
	}
	else if (chunk.className.Contains(zCTouchDamage::className))
	{
		if (chunk.className.Contains(oCTouchDamage::className))
		{
			zNEW_OBJECT(oCTouchDamage);
		}
		else
		{
			printf("Error: zCTouchDamage is not implemented like the game does it, use oCTouchDamage instead!\n");

			obj = NULL;
		}
	}
	else if (chunk.className.Contains(zCTouchAnimate::className))
	{
		if (chunk.className.Contains(zCTouchAnimateSound::className))
		{
			zNEW_OBJECT(zCTouchAnimateSound);
		}
		else
		{
			zNEW_OBJECT(zCTouchAnimate);
		}
	}
	else if (chunk.className.Contains(zCVobAnimate::className))
	{
		zNEW_OBJECT(zCVobAnimate);
	}
	else if (chunk.className.Contains(zCVobLensFlare::className))
	{
		zNEW_OBJECT(zCVobLensFlare);
	}
	else if (chunk.className.Contains(zCVobScreenFX::className))
	{
		zNEW_OBJECT(zCVobScreenFX);
	}
	else if (chunk.className.Contains(oCVisualFX::className))
	{
		if (chunk.className.Contains(oCVisFX_Lightning::className))
		{
			zNEW_OBJECT(oCVisFX_Lightning);
		}
		else if (chunk.className.Contains(oCVisFX_MultiTarget::className))
		{
			zNEW_OBJECT(oCVisFX_MultiTarget);
		}
		else
		{
			zNEW_OBJECT(oCVisualFX);
		}
	}
	else if (chunk.className.Contains(zCZoneZFog::className))
	{
		if (chunk.className.Contains(zCZoneZFogDefault::className))
		{
			zNEW_OBJECT(zCZoneZFogDefault);
		}
		else
		{
			zNEW_OBJECT(zCZoneZFog);
		}
	}
	else if (chunk.className.Contains(zCZoneVobFarPlane::className))
	{
		if (chunk.className.Contains(zCZoneVobFarPlaneDefault::className))
		{
			zNEW_OBJECT(zCZoneVobFarPlaneDefault);
		}
		else
		{
			zNEW_OBJECT(zCZoneVobFarPlane);
		}
	}
	else if (chunk.className.Contains(zCVobSound::className))
	{
		if (chunk.className.Contains(zCVobSoundDaytime::className))
		{
			zNEW_OBJECT(zCVobSoundDaytime);
		}
		else
		{
			zNEW_OBJECT(zCVobSound);
		}
	}
	else if (chunk.className.Contains(oCZoneMusic::className))
	{
		if (chunk.className.Contains(oCZoneMusicDefault::className))
		{
			zNEW_OBJECT(oCZoneMusicDefault);
		}
		else
		{
			zNEW_OBJECT(oCZoneMusic);
		}
	}
	else if (chunk.className.Contains(zCZoneReverb::className))
	{
		if (chunk.className.Contains(zCZoneReverbDefault::className))
		{
			zNEW_OBJECT(zCZoneReverbDefault);
		}
		else
		{
			zNEW_OBJECT(zCZoneReverb);
		}
	}
	else if (chunk.className.Contains(oCDummyVobGenerator::className))
	{
		zNEW_OBJECT(oCDummyVobGenerator);
	}
	else if (chunk.className.Contains(oCObjectGenerator::className))
	{
		zNEW_OBJECT(oCObjectGenerator);
	}
	else if (chunk.className.Contains(oCItem::className))
	{
		zNEW_OBJECT(oCItem);
	}
	else if (chunk.className.Contains(oCMOB::className))
	{
		if (chunk.className.Contains(oCMobInter::className))
		{
			if (chunk.className.Contains(oCMobBed::className))
			{
				zNEW_OBJECT(oCMobBed);
			}
			else if (chunk.className.Contains(oCMobFire::className))
			{
				zNEW_OBJECT(oCMobFire);
			}
			else if (chunk.className.Contains(oCMobItemSlot::className))
			{
				zNEW_OBJECT(oCMobItemSlot);
			}
			else if (chunk.className.Contains(oCMobLadder::className))
			{
				zNEW_OBJECT(oCMobLadder);
			}
			else if (chunk.className.Contains(oCMobContainer::className))
			{
				zNEW_OBJECT(oCMobContainer);
			}
			else if (chunk.className.Contains(oCMobDoor::className))
			{
				zNEW_OBJECT(oCMobDoor);
			}
			else if (chunk.className.Contains(oCMobSwitch::className))
			{
				zNEW_OBJECT(oCMobSwitch);
			}
			else if (chunk.className.Contains(oCMobWheel::className))
			{
				zNEW_OBJECT(oCMobWheel);
			}
			else
			{
				zNEW_OBJECT(oCMobInter);
			}
		}
		else
		{
			zNEW_OBJECT(oCMOB);
		}
	}
	else if (chunk.className.Contains(oCNpc::className))
	{
		zNEW_OBJECT(oCNpc);
	}
	else if (chunk.className.Contains(zCVob::className))
	{
		if (chunk.className != zCVob::className) printf("%s\n", chunk.className.ToChar());

		zNEW_OBJECT(zCVob);
	}
	else
	{
		printf("Error at %d: %s is not implemented!\n", file->Pos(), chunk.className.ToChar());

		obj = NULL;
	}

	return obj;

#undef zNEW_OBJECT
}

bool32 ParseChunkStartLine(zTChunkRecord &chunk, zSTRING &s)
{
	size_t p1 = s.find("[");
	size_t p2 = s.find(" ", p1 + 1);
	size_t p3 = s.find(" ", p2 + 1);
	size_t p4 = s.find(" ", p3 + 1);
	size_t p5 = s.find("]");

	if (p1 == string::npos || p2 == string::npos || p3 == string::npos || p4 == string::npos || p5 == string::npos) return FALSE;

	chunk.name = s.substr(p1 + 1, p2 - p1 - 1);
	chunk.className = s.substr(p2 + 1, p3 - p2 - 1);
	chunk.classVersion = zSTRING(s.substr(p3 + 1, p4 - p3 - 1)).ToInt();
	chunk.objectIndex = zSTRING(s.substr(p4 + 1, p5 - p4 - 1)).ToInt();

	return TRUE;
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

		if (!ParseChunkStartLine(chunk, s)) return FALSE;
	}
	else if (mode == zARC_MODE_BINARY_SAFE)
	{
		zSTRING s;

		ReadBinSafeValue(zARC2_ID_STRING, &s);

		if (!ParseChunkStartLine(chunk, s)) return FALSE;
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
		file->ReadLine(s);

		zSTRING e;
		for (int32 i = 0; i < chunkDepth; i++) e += "\t";
		e += "[]";

		if (s != e) return FALSE;
	}
	else if (mode == zARC_MODE_BINARY_SAFE)
	{
		zSTRING s;
		ReadBinSafeValue(zARC2_ID_STRING, &s);

		if (s != "[]") return FALSE;
	}

	return TRUE;
}

bool32 zCArchiver::PeekChunkStart(zTChunkRecord &chunk)
{
	int32 savePos = file->Pos();

	bool32 result = ReadChunkStart(chunk);
	PopChunk();

	file->Seek(savePos);

	return result;
}

bool32 zCArchiver::PeekChunkEnd()
{
	int32 savePos = file->Pos();

	bool32 result = ReadChunkEnd();
	PushChunk();

	file->Seek(savePos);

	return result;
}

zCObject *zCArchiver::ReadObject(const char *chunkName, zCObject *useThis)
{
	zTChunkRecord chunk;
	PeekChunkStart(chunk);

	if (chunk.name != chunkName) return NULL;

	return ReadObject(useThis);
}

zCObject *zCArchiver::ReadObject(zCObject *useThis)
{
	zCObject *object = NULL;

	zTChunkRecord chunk;
	ReadChunkStart(chunk);

	if (chunk.IsNull())
	{
		object = NULL;
	}
	else if (chunk.IsReference())
	{
		object = objects[chunk.objectIndex];
	}
	else
	{
		if (useThis)
		{
			object = useThis;

			object->classVersion = chunk.classVersion;
			object->objectIndex = chunk.objectIndex;
			object->arc_className = chunk.className;
		}
		else
		{
			object = CreateObject(chunk);
		}

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
	// TODO it seems like this will have to become a loop through all entries in chunk until we have found the right one
	// since MDK surface.zen has values it shouldn't have and is thus unparsable :(
	// but i think we can still just use the current file pos - since order is predictable still, and we will never get need a skipped value ...
	// need to handle skipping remaining values though :(
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
		// if not found, just recurse and try again, oCMobInter from MDK SURFACE.ZEN has state and stateTarget despite being 108
		// TOOD need to check against chunkend here ...

		if (!PeekChunkEnd() && ReadASCIIValue(entryName, typeName, value))
		{
			return TRUE;
		}

		file->Seek(savePos);

		//printf("ReadASCIIValue failed at %d, expected entry '%s' of type '%s', but got '%s' of type '%s'\n", savePos, entryName, typeName, n.ToChar(), t.ToChar());

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
		printf("ReadBinSafeValue failed at %d, expected entry '%s'\n", file->Pos(), entryName);

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
		printf("ReadBinSafeValue failed at %d, expected type '%d', but got type '%d'\n", file->Pos(), entryType, archiveTypeID);

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
	chunk.className = zARC_CHUNK_CLASS_NAME_NULL;

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
	else if (mode == zARC_MODE_ASCII || mode == zARC_MODE_ASCII_DIFF)
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
	else if (mode == zARC_MODE_ASCII || mode == zARC_MODE_ASCII_DIFF)
	{
		file->WriteLineIndented(chunkDepth, "[]");
	}
}

void zCArchiver::WriteChunk(const char *chunkName)
{
	WriteChunkStart(chunkName);
	WriteChunkEnd();
}

int32 zCArchiver::GetWriteObjectListNum()
{
	return (int32)writeObjectList.size();
}

void zCArchiver::AddToWriteObjectList(zCObject *object)
{
	writeObjectList[object] = (int32)writeObjectList.size();
}

int32 zCArchiver::SearchWriteObjectList(zCObject *object)
{
	auto it = writeObjectList.find(object);

	return it != writeObjectList.end() ? it->second : -1;
}

void zCArchiver::WriteObject(zCObject *object)
{
	WriteObject("", object);
}

void zCArchiver::WriteObject(const char *chunkName, zCObject *object)
{
	if (!object)
	{
		WriteChunkStart(chunkName);
	}
	else
	{
		int32 objectIndex = SearchWriteObjectList(object);

		if (objectIndex != -1)
		{
			zTChunkRecord chunk;

			chunk.classVersion = 0;
			chunk.objectIndex = objectIndex;
			chunk.name = *chunkName ? chunkName : zARC_CHUNK_NAME_NAMELESS;
			chunk.className = zARC_CHUNK_CLASS_NAME_REF;

			WriteChunkStart(chunk);
		}
		else
		{
			zTChunkRecord chunk;

			chunk.classVersion = object->classVersion;
			chunk.objectIndex = GetWriteObjectListNum();
			chunk.name = *chunkName ? chunkName : zARC_CHUNK_NAME_NAMELESS;
			chunk.className = object->arc_className;

			AddToWriteObjectList(object);

			WriteChunkStart(chunk);

			object->Archive(*this);
		}
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
	else if (mode == zARC_MODE_ASCII || mode == zARC_MODE_ASCII_DIFF)
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
	else if (mode == zARC_MODE_ASCII_DIFF)
	{
		WriteASCIILine(entryName, "byte", zSTRING(value));
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
	else if (mode == zARC_MODE_ASCII_DIFF)
	{
		WriteASCIILine(entryName, "word", zSTRING(value));
	}
}

void zCArchiver::WriteFloat(const char *entryName, float value)
{
	if (mode == zARC_MODE_BINARY)
	{
		file->Write(&value, sizeof(value));
	}
	else if (mode == zARC_MODE_ASCII || mode == zARC_MODE_ASCII_DIFF)
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
	else if (mode == zARC_MODE_ASCII_DIFF)
	{
		WriteASCIILine(entryName, "bool", value ? zSTRING("TRUE") : zSTRING("FALSE"));
	}
}

void zCArchiver::WriteString(const char *entryName, zSTRING &value)
{
	if (mode == zARC_MODE_BINARY)
	{
		file->Write(value);
	}
	else if (mode == zARC_MODE_ASCII || mode == zARC_MODE_ASCII_DIFF)
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
	else if (mode == zARC_MODE_ASCII || mode == zARC_MODE_ASCII_DIFF)
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
	else if (mode == zARC_MODE_ASCII || mode == zARC_MODE_ASCII_DIFF)
	{
		WriteASCIILine(entryName, "color", zSTRING(value.r) + " " + zSTRING(value.g) + " " + zSTRING(value.b) + " " + zSTRING(value.alpha));
	}
}

void zCArchiver::WriteEnum(const char *entryName, const char *enumChoices, int32 value)
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
	else if (mode == zARC_MODE_ASCII_DIFF)
	{
		zSTRING e = enumChoices;

		WriteASCIILine(entryName, "enum", e.PickWord(value + 1, ";", ";") + ":" + zSTRING(value));
	}
}

void zCArchiver::WriteRaw(const char *entryName, void *buffer, uint32 size, bool32 rawFloat)
{
	if (mode == zARC_MODE_BINARY)
	{
		file->Write(buffer, size);
	}
	else if (mode == zARC_MODE_ASCII)
	{
raw:;
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
	else if (mode == zARC_MODE_ASCII_DIFF)
	{
		if (rawFloat)
		{
			WriteRawFloat(entryName, buffer, size);
		}
		else
		{
			goto raw;
		}
	}
}

void zCArchiver::WriteRawFloat(const char *entryName, void *buffer, uint32 size)
{
	if (mode == zARC_MODE_BINARY)
	{
		file->Write(buffer, size);
	}
	else if (mode == zARC_MODE_ASCII || mode == zARC_MODE_ASCII_DIFF)
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

