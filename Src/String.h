#pragma once

#include <string>

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
	zSTRING(zSIZE_T n, char c) : string(n, c) { }
	zSTRING(const zSTRING &xStr) : string(xStr) { }
	zSTRING(const zSTRING *pStr) : string(*pStr) { }
	zSTRING(const char *pstring) : string(pstring) { }
	zSTRING(char ch) : string(1, ch) { }

	zSTRING(const int32 xWert) { char buffer[50]; snprintf(buffer, sizeof(buffer), "%d", xWert); assign(buffer); }
	zSTRING(const zDWORD xWert) { char buffer[50]; snprintf(buffer, sizeof(buffer), "%u", xWert); assign(buffer); }
	zSTRING(const zREAL xWert, int32 digits = 20);

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

	zSIZE_T Length() const { return size(); }
	char *ToChar() const { return (char *)c_str(); }
	int32 ToInt() const { return atol(c_str()); }
	zREAL ToFloat() const { return (zREAL)atof(c_str()); }

	int32 CompareTo(zSTRING &xStr2) const { return compare(xStr2); }
	int32 CompareTo(const char *cstr) const { return compare(cstr); }

	zSTRING &Upper();
	zSTRING &Lower();

	zSTRING PickWord(zUINT num, const zSTRING &trenn, const zSTRING &skip = zSTR_SKIP) const;
	const char *PickWordPos(zUINT num, const zSTRING &trenn, const zSTRING &skip = zSTR_SKIP) const;
	zSTRING PickWorld_Old(int32 num, const zSTRING &trenn);

	zSTRING PickWordCutMarks(zUINT num, const zSTRING &trenn, const zSTRING &skip = zSTR_SKIP) const;

	void Delete(const zSTRING &xStr, zTSTR_KIND kind);
	int32 Delete(zSIZE_T pos, zSIZE_T = UINT32_MAX);
	zSTRING Deleted(zSIZE_T pos, zSIZE_T length = UINT32_MAX) const;
	void DeleteRight(zSIZE_T length);

	void Append(char c) { append(zSTRING(c)); }
	void Append(const char *c) { append(c); }
	void Append(const zSTRING &s) { append(s); }

	void Prepend(char c, zUINT num) { insert(0, num, c); }
	void Prepend(char c) { insert(0, 1, c); }
	void Prepend(const char *c) { insert(0, c); }

	int32 Search(zSIZE_T startIndex, const char *substr, zUINT num = 1) const;
	int32 Search(zSIZE_T startIndex, const zSTRING &substr, zUINT num = 1) const { return Search(startIndex, substr.ToChar(), num); }
	int32 Search(const char *substr, zUINT num = 1) const { return Search(0, substr, num); }
	int32 Search(const zSTRING &substr, zUINT num = 1) const { return Search(0, substr.ToChar(), num); }

	bool32 Contains(const char *substr) const { return Search(substr) != -1; }
	bool32 Contains(const zSTRING &substr) const { return Search(substr.ToChar()) != -1; }

	char &operator[] (zSIZE_T index) { return at(index); };
	const char &operator[] (zSIZE_T index) const { return at(index); };

	char &First() { return at(0); }
	char &Last() { return at(size() - 1); }

	bool32 EndsWith(const zSTRING &suffix) { return size() >= suffix.size() && !compare(size() - suffix.size(), suffix.size(), suffix); }
	bool32 StartsWith(const zSTRING &prefix) { return size() >= prefix.size() && !compare(0, prefix.size(), prefix); }

	void Replace(char s, char r) { for (zSIZE_T i = 0; i < size(); i++) if (at(i) == s) replace(i, 1, 1, r); }
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
