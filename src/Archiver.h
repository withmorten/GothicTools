#pragma once

#include "GothicTools.h"

class zFILE;
class zCObject;
class zCOLOR;

enum { zARC_VERSION = 2 };

enum zTArchiveMode
{
	zARC_MODE_BINARY,
	zARC_MODE_ASCII,
	zARC_MODE_ASCII_PROPS, // not supported
	zARC_MODE_BINARY_SAFE,
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

struct zTChunkRecord
{
	uint16 classVersion;
	int32 objectIndex;
	zSTRING name;
	zSTRING className;

	bool32 IsNull() { return className[0] == '%'; }
	bool32 IsReference() { return className[0] == '�'; }
};

class zCArchiver
{
public:
	enum { READ_BUFFER_SIZE = 1024 * 8 };

public:
	zFILE *file;
	zTArchiveMode mode;
	int32 chunkDepth;
	int32 lastChunkStart; // for BINARY

	zSTRING user;
	zSTRING date;
	int32 objCount; // from archive
	zCArray<zCObject *> objects; // filled as we go

	uint32 nCount;
	GESStringMapStorage *stringHashMap;

	byte readBuffer[READ_BUFFER_SIZE];

public:
	zCArchiver();
	~zCArchiver();

	void SetObjCount(int32 oC) { objCount = oC; }
	void SetMode(zTArchiveMode m) { mode = m; }

	void OpenFile(const zSTRING &fileName, bool32 write = FALSE);
	void CloseFile();
	void SetFile(zFILE *inFile) { file = inFile; }
	zFILE *GetFile() { return file; }

	bool32 ReadHeader();
	void WriteHeader(int32 flags = 0);

	zCObject *CreateObject(zTChunkRecord &chunk);

	void PushChunk() { chunkDepth++; }
	void PopChunk() { chunkDepth--; }

	void PeekChunk(zTChunkRecord &chunk);
	bool32 ReadChunkStart(zTChunkRecord &chunk);
	bool32 ReadChunkEnd();
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

	void WriteChunkStart(const char *chunkName);
	void WriteChunkStart(zTChunkRecord &chunk);
	void WriteChunkEnd();
	void WriteChunk(const char *chunkName);
	void WriteObject(zCObject *object);

	void WriteASCIILine(const char *entryName, const char *entryType, zSTRING &value);

	void WriteInt(const char *entryName, int32 value);
	void WriteByte(const char *entryName, byte value);
	void WriteWord(const char *entryName, uint16 value);
	void WriteFloat(const char *entryName, float value);
	void WriteBool(const char *entryName, bool32 value);
	void WriteString(const char *entryName, zSTRING &value);
	void WriteVec3(const char *entryName, zVEC3 &value);
	void WriteColor(const char *entryName, zCOLOR &value);
	template<typename E> void WriteEnum(const char *entryName, E value) { WriteEnum(entryName, (int32)value); }
	void WriteEnum(const char *entryName, int32 value);
	void WriteRaw(const char *entryName, void *buffer, uint32 size);
	void WriteRawFloat(const char *entryName, void *buffer, uint32 size);
};