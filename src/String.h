#pragma once

#include <string>

#include "GothicTools.h"

using std::string;

#define zSTR_SKIP zSTRING("\r\t ")
#define zSTR_EMPTY zSTRING("")

enum zTSTR_KIND
{
	zSTR_ONLY,
	zSTR_BEFORE,
	zSTR_AFTER,
	zSTR_TO,
	zSTR_FROM
};

class zSTRING : public std::string
{
public:
	zSTRING() : string() { Clear(); }
	zSTRING(string &xStr) : string(xStr) { }
	zSTRING(size_t n, char c) : string(n, c) { }
	zSTRING(const zSTRING &xStr) : string(xStr) { }
	zSTRING(const zSTRING *pStr) : string(*pStr) { }
	zSTRING(const char *pstring) : string(pstring) { }
	zSTRING(char ch) : string(1, ch) { }

	zSTRING(const int32 xWert) { char buffer[50]; snprintf(buffer, sizeof(buffer), "%d", xWert); assign(buffer); }
	zSTRING(const int64 xWert) { char buffer[50]; snprintf(buffer, sizeof(buffer), "%lld", xWert); assign(buffer); }
	zSTRING(const uint32 xWert) { char buffer[100]; snprintf(buffer, sizeof(buffer), "%u", xWert); assign(buffer); }
	zSTRING(const uint64 xWert) { char buffer[100]; snprintf(buffer, sizeof(buffer), "%llu", xWert); assign(buffer); }
	zSTRING(const float xWert, int32 digits = 20);

	zSTRING(const zSTRING &xStr1, const char *pstring) : string(xStr1) { append(pstring); };
	zSTRING(const zSTRING &xStr1, const zSTRING &xStr2) : string(xStr1) { append(xStr2); };
	zSTRING(const zSTRING &xStr1, const char ch) : string(xStr1) { append(1, ch); };

	inline friend zSTRING operator+ (const zSTRING &xStr1, const zSTRING &xStr2);
	inline friend zSTRING operator+ (const zSTRING &xStr1, const char *pstring);
	inline friend zSTRING operator+ (const zSTRING &xStr2, const char ch);
	inline friend zSTRING operator+ (const char *pstring, const zSTRING &xStr2);
	inline friend zSTRING operator+ (const char ch, const zSTRING &xStr2);

	char GetLastChar();

	void Clear() { erase(); }
	bool32 IsEmpty() const { return empty(); }

	size_t Length() const { return size(); }
	char *ToChar() const { return (char *)c_str(); }
	int32 ToInt() const { return atol(c_str()); }
	float ToFloat() const { return (float)atof(c_str()); }

	int32 CompareTo(zSTRING &xStr2) const { return compare(xStr2); }
	int32 CompareTo(const char *cstr) const { return compare(cstr); }

	zSTRING &Upper();
	zSTRING &Lower();

	zSTRING PickWord(size_t num, const zSTRING &trenn, const zSTRING &skip = zSTR_SKIP) const;
	const char *PickWordPos(size_t num, const zSTRING &trenn, const zSTRING &skip = zSTR_SKIP) const;
	zSTRING PickWord_Old(size_t num, const zSTRING &trenn);

	zSTRING PickWordCutMarks(size_t num, const zSTRING &trenn, const zSTRING &skip = zSTR_SKIP) const;

	void Delete(const zSTRING &xStr, zTSTR_KIND kind);
	int32 Delete(size_t pos, size_t = UINT32_MAX);
	zSTRING Deleted(size_t pos, size_t length = UINT32_MAX) const;
	void DeleteRight(size_t length);

	void TrimLeft(char ch = ' ');
	void TrimRight(char ch = ' ');

	void Append(char c) { append(zSTRING(c)); }
	void Append(const char *c) { append(c); }
	void Append(const zSTRING &s) { append(s); }

	void Prepend(char c, uint32 num) { insert(0, num, c); }
	void Prepend(char c) { insert(0, 1, c); }
	void Prepend(const char *c) { insert(0, c); }

	int32 Search(size_t startIndex, const char *substr, uint32 num = 1) const;
	int32 Search(size_t startIndex, const zSTRING &substr, uint32 num = 1) const { return Search(startIndex, substr.ToChar(), num); }
	int32 Search(const char *substr, uint32 num = 1) const { return Search(0, substr, num); }
	int32 Search(const zSTRING &substr, uint32 num = 1) const { return Search(0, substr.ToChar(), num); }

	bool32 Contains(const char *substr) const { return Search(substr) != -1; }
	bool32 Contains(const zSTRING &substr) const { return Search(substr.ToChar()) != -1; }

	char &operator[] (size_t index) { return at(index); };
	const char &operator[] (size_t index) const { return at(index); };

	char &First() { return at(0); }
	char &Last() { return at(size() - 1); }

	bool32 EndsWith(const zSTRING &suffix) { return size() >= suffix.size() && !compare(size() - suffix.size(), suffix.size(), suffix); }
	bool32 StartsWith(const zSTRING &prefix) { return size() >= prefix.size() && !compare(0, prefix.size(), prefix); }

	void Replace(char s, char r) { for (size_t i = 0; i < size(); i++) if (at(i) == s) replace(i, 1, 1, r); }
};

zSTRING operator+ (const zSTRING &xStr1, const zSTRING &xStr2)
{
	return zSTRING(xStr1, xStr2);
}

zSTRING operator+ (const char *str, const zSTRING &xStr2)
{
	return zSTRING(zSTRING(str), xStr2);
};

zSTRING operator+ (const char ch, const zSTRING &xStr2)
{
	return zSTRING(zSTRING(ch), xStr2);
};

zSTRING operator+ (const zSTRING &xStr1, const char *str)
{
	return zSTRING(xStr1, str);
};

zSTRING operator+ (const zSTRING &xStr1, const char ch)
{
	return zSTRING(xStr1, ch);
};
