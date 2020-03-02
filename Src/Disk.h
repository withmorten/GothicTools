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

	zVOID Close() { fclose(file_handle); }
	zVOID Reset() { fseek(file_handle, 0, SEEK_SET); }
	zLONG Size() { return size; }
	zLONG Pos() { return ftell(file_handle); }
	zVOID Append() { fseek(file_handle, 0, SEEK_END); }
	zBOOL Eof() { return (feof(file_handle) != FALSE); }
	zVOID Seek(zLONG pos) { fseek(file_handle, pos, SEEK_SET); }
	zVOID SeekRel(zLONG rel) { fseek(file_handle, rel, SEEK_CUR); }

	zVOID WriteLine(const zSTRING &s = zSTR_EMPTY, zBOOL crLf = TRUE);
	zVOID WriteLineIndented(zUINT numTabs, const zSTRING &s = zSTR_EMPTY, zBOOL crLf = TRUE);
	zVOID Write(const zSTRING &s) { fputs(s.ToChar(), file_handle); fputc('\0', file_handle); }
	zSIZE_T Write(const zVOID *scr, zLONG bytes) { return fwrite(scr, 1, bytes, file_handle); }

	zVOID ReadLine(zSTRING &s);
	zVOID Read(zSTRING &s);
	zSIZE_T Read(zVOID *scr, zLONG bytes) { return fread(scr, 1, bytes, file_handle); }
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
	zVOID BinSeek(zLONG pos) { file->Seek(pos); }
	zVOID BinSeekRel(zLONG rel) { file->SeekRel(rel); }

	zVOID BinRead(zVOID *ptr, zLONG num) { file->Read(ptr, num); }
	zVOID BinReadFloat(zREAL &f) { file->Read(&f, sizeof(f)); }
	zVOID BinReadInt(zINT &i) { file->Read(&i, sizeof(i)); }
	zVOID BinReadDWord(zDWORD &d) { file->Read(&d, sizeof(d)); }
	zVOID BinReadWord(zWORD &w) { file->Read(&w, sizeof(w)); }
	zVOID BinReadByte(zBYTE &b) { file->Read(&b, sizeof(b)); }
	zVOID BinReadBool(zBOOL &b) { file->Read(&b, sizeof(zBYTE)); }
	template<typename zENUM> zVOID BinReadEnum(zENUM &e) { file->Read(&e, sizeof(zENUM)); }
	template<typename zENUM> zVOID BinReadEnumByte(zENUM &e) { file->Read(&e, sizeof(zBYTE)); }
	zVOID BinReadString(zSTRING &s) { file->Read(s); }
	zVOID BinReadLine(zSTRING &s) { file->ReadLine(s); }

	zVOID BinOpenChunk(zWORD &id, zLONG &len);
	zVOID BinSkipChunk();

	zVOID BinStartChunk(const zWORD id);
	zVOID BinEndChunk();

	zVOID BinWrite(const zVOID *ptr, zLONG num) { file->Write(ptr, num); }
	zVOID BinWriteFloat(const zREAL f) { file->Write(&f, sizeof(f)); }
	zVOID BinWriteInt(const zINT i) { file->Write(&i, sizeof(i)); }
	zVOID BinWriteDWord(const zDWORD d) { file->Write(&d, sizeof(d)); }
	zVOID BinWriteWord(const zWORD w) { file->Write(&w, sizeof(w)); }
	zVOID BinWriteByte(const zBYTE b) { file->Write(&b, sizeof(b)); }
	zVOID BinWriteBool(const zBOOL b) { file->Write(&b, sizeof(zBYTE)); }
	template<typename zENUM> zVOID BinWriteEnum(const zENUM e) { file->Write(&e, sizeof(zENUM)); }
	template<typename zENUM> zVOID BinWriteEnumByte(const zENUM e) { file->Write(&e, sizeof(zBYTE)); }
	zVOID BinWriteString(const zSTRING &s) { file->Write(s); }
	zVOID BinWriteLine(const zSTRING &s = zSTR_EMPTY, zBOOL crLf = FALSE) { file->WriteLine(s, crLf); }
	zVOID BinWriteLineIndented(zUINT numTabs, const zSTRING &s = zSTR_EMPTY, zBOOL crLf = FALSE) { file->WriteLineIndented(numTabs, s, crLf); }
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

	zVOID SetTime(zCFileBIN &file) { SetTime(*file.file); }
	zVOID SetTime(zFILE &file);

	static zBOOL IsFileNewer(const zSTRING &one, const zSTRING &two); // checks if file two was modified more recently than file one
};
