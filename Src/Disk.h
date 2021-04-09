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
	zBOOL write;
	zLONG size;

public:
	zFILE(const zSTRING &filepath, zBOOL w = FALSE);
	~zFILE() { fclose(file_handle); }

	void Close() { fclose(file_handle); }
	void Reset() { fseek(file_handle, 0, SEEK_SET); }
	zLONG Size() { return size; }
	zLONG Pos() { return ftell(file_handle); }
	void Append() { fseek(file_handle, 0, SEEK_END); }
	zBOOL Eof() { return (feof(file_handle) != FALSE); }
	void Seek(zLONG pos) { fseek(file_handle, pos, SEEK_SET); }
	void SeekRel(zLONG rel) { fseek(file_handle, rel, SEEK_CUR); }

	void WriteLine(const zSTRING &s = zSTR_EMPTY, zBOOL crLf = TRUE);
	void WriteLineIndented(zUINT numTabs, const zSTRING &s = zSTR_EMPTY, zBOOL crLf = TRUE);
	void Write(const zSTRING &s) { fputs(s.ToChar(), file_handle); fputc('\0', file_handle); }
	zSIZE_T Write(const void *scr, zLONG bytes) { return fwrite(scr, 1, bytes, file_handle); }

	void ReadLine(zSTRING &s);
	void Read(zSTRING &s);
	zSIZE_T Read(void *scr, zLONG bytes) { return fread(scr, 1, bytes, file_handle); }
};

class zCFileBIN
{
public:
	zINT lastStart;
	zINT nextStart;

	zFILE *file;

public:
	zCFileBIN(const zSTRING &fname, zBOOL write = FALSE);
	~zCFileBIN() { zDELETE(file); }

	zBOOL BinEof() { return nextStart >= file->Size(); }
	zLONG BinSize() { return file->Size(); }
	zLONG BinPos() { return file->Pos(); }
	void BinSeek(zLONG pos) { file->Seek(pos); }
	void BinSeekRel(zLONG rel) { file->SeekRel(rel); }

	void BinRead(void *ptr, zLONG num) { file->Read(ptr, num); }
	void BinReadFloat(zREAL &f) { file->Read(&f, sizeof(f)); }
	void BinReadInt(zINT &i) { file->Read(&i, sizeof(i)); }
	void BinReadDWord(zDWORD &d) { file->Read(&d, sizeof(d)); }
	void BinReadWord(zWORD &w) { file->Read(&w, sizeof(w)); }
	void BinReadByte(byte &b) { file->Read(&b, sizeof(b)); }
	void BinReadBool(zBOOL &b) { file->Read(&b, sizeof(byte)); }
	template<typename zENUM> void BinReadEnum(zENUM &e) { file->Read(&e, sizeof(zENUM)); }
	template<typename zENUM> void BinReadEnumByte(zENUM &e) { file->Read(&e, sizeof(byte)); }
	void BinReadString(zSTRING &s) { file->Read(s); }
	void BinReadLine(zSTRING &s) { file->ReadLine(s); }

	void BinOpenChunk(zWORD &id, zLONG &len);
	void BinSkipChunk();

	void BinStartChunk(const zWORD id);
	void BinEndChunk();

	void BinWrite(const void *ptr, zLONG num) { file->Write(ptr, num); }
	void BinWriteFloat(const zREAL f) { file->Write(&f, sizeof(f)); }
	void BinWriteInt(const zINT i) { file->Write(&i, sizeof(i)); }
	void BinWriteDWord(const zDWORD d) { file->Write(&d, sizeof(d)); }
	void BinWriteWord(const zWORD w) { file->Write(&w, sizeof(w)); }
	void BinWriteByte(const byte b) { file->Write(&b, sizeof(b)); }
	void BinWriteBool(const zBOOL b) { file->Write(&b, sizeof(byte)); }
	template<typename zENUM> void BinWriteEnum(const zENUM e) { file->Write(&e, sizeof(zENUM)); }
	template<typename zENUM> void BinWriteEnumByte(const zENUM e) { file->Write(&e, sizeof(byte)); }
	void BinWriteString(const zSTRING &s) { file->Write(s); }
	void BinWriteLine(const zSTRING &s = zSTR_EMPTY, zBOOL crLf = FALSE) { file->WriteLine(s, crLf); }
	void BinWriteLineIndented(zUINT numTabs, const zSTRING &s = zSTR_EMPTY, zBOOL crLf = FALSE) { file->WriteLineIndented(numTabs, s, crLf); }
};

class zDATE
{
public:
	zUINT year;
	zWORD month;
	zWORD day;
	zWORD hour;
	zWORD minute;
	zWORD second;

public:
	zDATE() { memset(this, 0x00, sizeof(*this)); }

	void SetTime(zCFileBIN &file) { SetTime(*file.file); }
	void SetTime(zFILE &file);

	static zBOOL IsFileNewer(const zSTRING &one, const zSTRING &two); // checks if file two was modified more recently than file one
};
