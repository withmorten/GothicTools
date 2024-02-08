#pragma once

#include <unordered_map>

#include "GothicTools.h"

class zFILE;
class zCObject;
class zCObjectRegistry;
class zCOLOR;

enum { zARC_VERSION = 2 };

enum zTArchiveMode
{
	zARC_MODE_BINARY, // read / write
	zARC_MODE_ASCII, // read / write
	zARC_MODE_ASCII_PROPS, // not supported
	zARC_MODE_BINARY_SAFE, // read
	zARC_MODE_ASCII_DIFF, // write, slightly more human readable output, doesn't exist originally
};

enum zTArchiveFlags
{
	zARC_FLAG_WRITE_BRIEF_HEADER = 1,
};

enum zTArchiveTypeID
{
	zARC2_ID_STRING = 0x1,
	zARC2_ID_INTEGER = 0x2,
	zARC2_ID_FLOAT = 0x3,
	zARC2_ID_BYTE = 0x4,
	zARC2_ID_WORD = 0x5,
	zARC2_ID_BOOL = 0x6,
	zARC2_ID_VEC3 = 0x7,
	zARC2_ID_COLOR = 0x8,
	zARC2_ID_RAW = 0x9,
	zARC2_ID_RAWFLOAT = 0x10,
	zARC2_ID_ENUM = 0x11,
	zARC2_ID_HASH = 0x12,
};

struct GESStringMapStorage
{
	uint16 HashMapStringLen;
	uint16 LinearValue;
	uint32 HashValue;
	char *MapValue;
};

#define zARC_CHUNK_NAME_NAMELESS "%"
#define zARC_CHUNK_CLASS_NAME_NULL "%"
#define zARC_CHUNK_CLASS_NAME_REF "§"

struct zTChunkRecord
{
	uint32 startPos; // BINARY
	uint32 size; // BINARY

	uint16 classVersion;
	int32 objectIndex;
	zSTRING name;
	zSTRING className;

	zTChunkRecord() { startPos = 0; size = 0; }

	bool32 IsNull() { return className == zARC_CHUNK_CLASS_NAME_NULL; }
	bool32 IsReference() { return className == zARC_CHUNK_CLASS_NAME_REF; }
};

class zCArchiver
{
public:
	enum { READ_BUFFER_SIZE = 1024 * 8 };

public:
	zFILE *file;
	zTArchiveMode mode;

	zCArray<zTChunkRecord> chunkStack;

	zSTRING user;
	zSTRING date;

	int32 objCount; // from archive
	zCArray<zCObject *> objects; // filled as we go
	zCObjectRegistry *registry; // owned by the world, can be NULL

	std::unordered_map<zCObject *, int32> writeObjectList; // for detecting references

	uint32 nCount;
	GESStringMapStorage *stringHashMap;

	byte readBuffer[READ_BUFFER_SIZE];

public:
	zCArchiver();
	~zCArchiver();

	void OpenFile(const zSTRING &fileName, bool32 write = FALSE);
	void CloseFile();

	bool32 ReadHeader();
	void WriteHeader(int32 flags = 0);

	zCObject *CreateObject(zTChunkRecord &chunk);

	void PushChunk(zTChunkRecord &chunk);
	zTChunkRecord PopChunk();
	zTChunkRecord PeekChunk();
	int32 ChunkDepth();

	bool32 ReadChunkStart(zTChunkRecord &chunk);
	bool32 ReadChunkEnd();
	bool32 PeekChunkStart(zTChunkRecord &chunk);
	bool32 PeekChunkEnd();

	zCObject *ReadObject(zCObject *useThis = NULL);
	zCObject *ReadObject(const char *chunkName, zCObject *useThis = NULL);

	bool32 ReadASCIIValue(const char *entryName, const char *entryType, zSTRING &value);
	bool32 ReadBinSafeValue(const char *entryName, zTArchiveTypeID entryType, void *buffer);
	bool32 ReadBinSafeValue(zTArchiveTypeID entryType, void *buffer);

	void ReadInt(const char *entryName, int32 &value);
	void ReadByte(const char *entryName, byte &value);
	void ReadWord(const char *entryName, uint16 &value);
	void ReadFloat(const char *entryName, float &value);
	void ReadBool(const char *entryName, bool32 &value);
	void ReadString(const char *entryName, zSTRING &value);
	void ReadVec3(const char *entryName, zVEC3 &value);
	void ReadColor(const char *entryName, zCOLOR &value);
	template<typename E> void ReadEnum(const char *entryName, E &value) { ReadEnum(entryName, (int32 &)value); }
	void ReadEnum(const char *entryName, int32 &value);
	void ReadRaw(const char *entryName, void *buffer, uint32 size);
	void ReadRawFloat(const char *entryName, void *buffer, uint32 size);

	void WriteChunkStart(const char *chunkName, const char *className = zARC_CHUNK_CLASS_NAME_NULL, int32 objectIndex = 0, uint16 classVersion = 0);
	void WriteChunkStart(zTChunkRecord &chunk);
	void WriteChunkEnd();

	int32 GetWriteObjectListNum();
	void AddToWriteObjectList(zCObject *object);
	int32 SearchWriteObjectList(zCObject *object);

	void WriteObject(zCObject *object);
	void WriteObject(const char *chunkName, zCObject *object);

	void WriteASCIILine(const char *entryName, const char *entryType, zSTRING &value);

	void WriteInt(const char *entryName, int32 value);
	void WriteByte(const char *entryName, byte value);
	void WriteWord(const char *entryName, uint16 value);
	void WriteFloat(const char *entryName, float value);
	void WriteBool(const char *entryName, bool32 value);
	void WriteString(const char *entryName, zSTRING &value);
	void WriteVec3(const char *entryName, zVEC3 &value);
	void WriteColor(const char *entryName, zCOLOR &value);
	template<typename E> void WriteEnum(const char *entryName, const char *enumChoices, E value) { WriteEnum(entryName, enumChoices, (int32)value); }
	void WriteEnum(const char *entryName, const char *enumChoices, int32 value);
	void WriteRaw(const char *entryName, void *buffer, uint32 size);
	void WriteRawFloat(const char *entryName, void *buffer, uint32 size);
};
