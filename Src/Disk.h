#pragma once

#define zFILE_MAXCHARS 1024

#define unixify_path(zStr) zStr.Replace('\\', '/')
#define windowsify_path(zStr) zStr.Replace('/', '\\')

class zFILE
{
public:
	FILE *file_handle;
	zSTRING complete_path;
	zSTRING filename;
	zSTRING extension;
	zSTRING basename;
	zSTRING path;
	bool32 write;
	int32 size;

public:
	zFILE(const zSTRING &filepath, bool32 w = FALSE);
	~zFILE() { fclose(file_handle); }

	void Close() { fclose(file_handle); }
	void Reset() { fseek(file_handle, 0, SEEK_SET); }
	int32 Size() { return size; }
	int32 Pos() { return ftell(file_handle); }
	void Append() { fseek(file_handle, 0, SEEK_END); }
	bool32 Eof() { return (feof(file_handle) != FALSE); }
	void Seek(int32 pos) { fseek(file_handle, pos, SEEK_SET); }
	void SeekRel(int32 rel) { fseek(file_handle, rel, SEEK_CUR); }

	void WriteLine(const zSTRING &s = zSTR_EMPTY, bool32 crLf = TRUE);
	void WriteLineIndented(uint32 numTabs, const zSTRING &s = zSTR_EMPTY, bool32 crLf = TRUE);
	void Write(const zSTRING &s) { fputs(s.ToChar(), file_handle); fputc('\0', file_handle); }
	size_t Write(const void *scr, int32 bytes) { return fwrite(scr, 1, bytes, file_handle); }

	void ReadLine(zSTRING &s);
	void Read(zSTRING &s);
	size_t Read(void *scr, int32 bytes) { return fread(scr, 1, bytes, file_handle); }
};

class zCFileBIN
{
public:
	int32 lastStart;
	int32 nextStart;

	zFILE *file;

public:
	zCFileBIN(const zSTRING &fname, bool32 write = FALSE);
	~zCFileBIN() { zDELETE(file); }

	bool32 BinEof() { return nextStart >= file->Size(); }
	int32 BinSize() { return file->Size(); }
	int32 BinPos() { return file->Pos(); }
	void BinSeek(int32 pos) { file->Seek(pos); }
	void BinSeekRel(int32 rel) { file->SeekRel(rel); }

	void BinRead(void *ptr, int32 num) { file->Read(ptr, num); }
	void BinReadFloat(zREAL &f) { file->Read(&f, sizeof(f)); }
	void BinReadInt(int32 &i) { file->Read(&i, sizeof(i)); }
	void BinReadDWord(uint32 &d) { file->Read(&d, sizeof(d)); }
	void BinReadWord(uint16 &w) { file->Read(&w, sizeof(w)); }
	void BinReadByte(byte &b) { file->Read(&b, sizeof(b)); }
	void BinReadBool(bool32 &b) { file->Read(&b, sizeof(byte)); }
	template<typename zENUM> void BinReadEnum(zENUM &e) { file->Read(&e, sizeof(zENUM)); }
	template<typename zENUM> void BinReadEnumByte(zENUM &e) { file->Read(&e, sizeof(byte)); }
	void BinReadString(zSTRING &s) { file->Read(s); }
	void BinReadLine(zSTRING &s) { file->ReadLine(s); }

	void BinOpenChunk(uint16 &id, int32 &len);
	void BinSkipChunk();

	void BinStartChunk(const uint16 id);
	void BinEndChunk();

	void BinWrite(const void *ptr, int32 num) { file->Write(ptr, num); }
	void BinWriteFloat(const zREAL f) { file->Write(&f, sizeof(f)); }
	void BinWriteInt(const int32 i) { file->Write(&i, sizeof(i)); }
	void BinWriteDWord(const uint32 d) { file->Write(&d, sizeof(d)); }
	void BinWriteWord(const uint16 w) { file->Write(&w, sizeof(w)); }
	void BinWriteByte(const byte b) { file->Write(&b, sizeof(b)); }
	void BinWriteBool(const bool32 b) { file->Write(&b, sizeof(byte)); }
	template<typename zENUM> void BinWriteEnum(const zENUM e) { file->Write(&e, sizeof(zENUM)); }
	template<typename zENUM> void BinWriteEnumByte(const zENUM e) { file->Write(&e, sizeof(byte)); }
	void BinWriteString(const zSTRING &s) { file->Write(s); }
	void BinWriteLine(const zSTRING &s = zSTR_EMPTY, bool32 crLf = FALSE) { file->WriteLine(s, crLf); }
	void BinWriteLineIndented(uint32 numTabs, const zSTRING &s = zSTR_EMPTY, bool32 crLf = FALSE) { file->WriteLineIndented(numTabs, s, crLf); }
};

class zDATE
{
public:
	uint32 year;
	uint16 month;
	uint16 day;
	uint16 hour;
	uint16 minute;
	uint16 second;

public:
	zDATE() { memset(this, 0x00, sizeof(*this)); }

	void SetTime(zCFileBIN &file) { SetTime(*file.file); }
	void SetTime(zFILE &file);

	static bool32 IsFileNewer(const zSTRING &one, const zSTRING &two); // checks if file two was modified more recently than file one
};
