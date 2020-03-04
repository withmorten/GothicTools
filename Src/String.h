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
	zSTRING(zSIZE_T n, zCHAR c) : string(n, c) { }
	zSTRING(const zSTRING &xStr) : string(xStr) { }
	zSTRING(const zSTRING *pStr) : string(*pStr) { }
	zSTRING(const zCHAR *pstring) : string(pstring) { }
	zSTRING(zCHAR ch) : string(1, ch) { }

	zSTRING(const zINT xWert) { zCHAR buffer[50]; snprintf(buffer, sizeof(buffer), "%d", xWert); assign(buffer); }
	zSTRING(const zDWORD xWert) { zCHAR buffer[50]; snprintf(buffer, sizeof(buffer), "%u", xWert); assign(buffer); }
	zSTRING(const zREAL xWert, zINT digits = 20);

	zSTRING(const zSTRING &xStr1, const zCHAR *pstring) : string(xStr1) { append(pstring); };
	zSTRING(const zSTRING &xStr1, const zSTRING &xStr2) : string(xStr1) { append(xStr2); };
	zSTRING(const zSTRING &xStr1, const zCHAR ch) : string(xStr1) { append(1, ch); };

	inline friend zSTRING operator+ (const zSTRING &xStr1, const zSTRING &xStr2);
	inline friend zSTRING operator+ (const zSTRING &xStr1, const char *pstring);
	inline friend zSTRING operator+ (const zSTRING &xStr2, const char ch);
	inline friend zSTRING operator+ (const zCHAR *pstring, const zSTRING &xStr2);
	inline friend zSTRING operator+ (const zCHAR ch, const zSTRING &xStr2);

	zCHAR GetLastChar();

	zVOID Clear() { erase(); }

	zSIZE_T Length() const { return size(); }
	zCHAR *ToChar() const { return (zCHAR *)c_str(); }
	zLONG ToInt() const { return atol(c_str()); }
	zREAL ToFloat() const { return (zREAL)atof(c_str()); }

	zINT CompareTo(zSTRING &xStr2) const { return compare(xStr2); }
	zINT CompareTo(const zCHAR *cstr) const { return compare(cstr); }

	zSTRING &Upper();
	zSTRING &Lower();

	zSTRING PickWord(zUINT num, const zSTRING &trenn, const zSTRING &skip = zSTR_SKIP) const;
	const zCHAR *PickWordPos(zUINT num, const zSTRING &trenn, const zSTRING &skip = zSTR_SKIP) const;
	zSTRING PickWorld_Old(zINT num, const zSTRING &trenn);

	zSTRING PickWordCutMarks(zUINT num, const zSTRING &trenn, const zSTRING &skip = zSTR_SKIP) const;

	zVOID Delete(const zSTRING &xStr, zTSTR_KIND kind);
	zINT Delete(zSIZE_T pos, zSIZE_T = UINT32_MAX);
	zSTRING Deleted(zSIZE_T pos, zSIZE_T length = UINT32_MAX) const;
	zVOID DeleteRight(zSIZE_T length);

	zVOID Append(zCHAR c) { append(zSTRING(c)); }
	zVOID Append(const zCHAR *c) { append(c); }
	zVOID Append(const zSTRING &s) { append(s); }

	zVOID Prepend(zCHAR c, zUINT num) { insert(0, num, c); }
	zVOID Prepend(zCHAR c) { insert(0, 1, c); }
	zVOID Prepend(const zCHAR *c) { insert(0, c); }

	zLONG Search(zSIZE_T startIndex, const zCHAR *substr, zUINT num = 1) const;
	zLONG Search(zSIZE_T startIndex, const zSTRING &substr, zUINT num = 1) const { return Search(startIndex, substr.ToChar(), num); }
	zLONG Search(const zCHAR *substr, zUINT num = 1) const { return Search(0, substr, num); }
	zLONG Search(const zSTRING &substr, zUINT num = 1) const { return Search(0, substr.ToChar(), num); }

	zBOOL Contains(const zCHAR *substr) const { return Search(substr) != -1; }
	zBOOL Contains(const zSTRING &substr) const { return Search(substr.ToChar()) != -1; }

	zCHAR &operator[] (zSIZE_T index) { return at(index); };
	const zCHAR &operator[] (zSIZE_T index) const { return at(index); };

	zCHAR &First() { return at(0); }
	zCHAR &Last() { return at(size() - 1); }

	zBOOL EndsWith(const zSTRING &suffix) { return size() >= suffix.size() && !compare(size() - suffix.size(), suffix.size(), suffix); }
	zBOOL StartsWith(const zSTRING &prefix) { return size() >= prefix.size() && !compare(0, prefix.size(), prefix); }

	zVOID Replace(zCHAR s, zCHAR r) { for (zSIZE_T i = 0; i < size(); i++) if (at(i) == s) replace(i, 1, 1, r); }
};

zSTRING operator+ (const zSTRING &xStr1, const zSTRING &xStr2)
{
	return zSTRING(xStr1, xStr2);
}

zSTRING operator+ (const zCHAR *str, const zSTRING &xStr2)
{
	return zSTRING(zSTRING(str), xStr2);
};

zSTRING operator+ (const zCHAR ch, const zSTRING &xStr2)
{
	return zSTRING(zSTRING(ch), xStr2);
};

zSTRING operator+ (const zSTRING &xStr1, const zCHAR *str)
{
	return zSTRING(xStr1, str);
};

zSTRING operator+ (const zSTRING &xStr1, const zCHAR ch)
{
	return zSTRING(xStr1, ch);
};
