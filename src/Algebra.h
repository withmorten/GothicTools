#pragma once

template<class T> inline void zClamp(T &x, const T min, const T max)
{
	if (x < min)
	{
		x = min;
	}
	else if (x > max)
	{
		x = max;
	}
}

inline float zMin(const float a, const float b)
{
	if (a < b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

inline float zMax(const float a, const float b)
{
	if (a > b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

inline int zMin(const int a, const int b)
{
	if (a < b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

inline int zMax(const int a, const int b)
{
	if (a > b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

#define MAX_FRAC 8

enum { VX, VY, VZ, VW };

class zVEC2
{
public:
	float n[2];

public:
	zVEC2() { n[VX] = 0.0f; n[VY] = 0.0f; }
	zVEC2(const float f) { n[VX] = f; n[VY] = f; }
	zVEC2(const float x, const float y) { n[VX] = x; n[VY] = y; }

	float &operator [](const int32 i) { return n[i]; }
	float operator [](const int32 i) const { return n[i]; }

	void SetByDescription(zSTRING &desc) { n[VX] = desc.PickWorld_Old(1, zSTR_EMPTY).ToFloat(); n[VY] = desc.PickWorld_Old(2, zSTR_EMPTY).ToFloat(); }
	zSTRING GetDescription() const { zSTRING s; zSTRING s1(n[0], MAX_FRAC); zSTRING s2(" "); zSTRING s3(n[1], MAX_FRAC); s += s1 + s2 + s3; return s; }
};

class zVEC3
{
public:
	float n[3];

public:
	zVEC3() { n[VX] = 0.0f; n[VY] = 0.0f; n[VZ] = 0.0f; }
	zVEC3(const float f) { n[VX] = f; n[VY] = f; n[VZ] = f; }
	zVEC3(const float x, const float y, const float z) { n[VX] = x; n[VY] = y; n[VZ] = z; }

	float &operator [](const int32 i) { return n[i]; }
	float operator [](const int32 i) const { return n[i]; }
	friend bool32 operator ==(const zVEC3 &a, const zVEC3 &b) { return (a[VX] == b[VX]) && (a[VY] == b[VY]) && (a[VZ] == b[VZ]); }
	friend bool32 operator !=(const zVEC3 &a, const zVEC3 &b) { return !(a == b); }
};

class zVEC4
{
public:
	float n[4];

public:
	zVEC4() { n[VX] = 0.0f; n[VY] = 0.0f; n[VZ] = 0.0f; n[VW] = 0.0f; }
	zVEC4(const float f) { n[VX] = f; n[VY] = f; n[VZ] = f; n[VW] = f; }
	zVEC4(const float x, const float y, const float z, const float w) { n[VX] = x; n[VY] = y; n[VZ] = z; n[VW] = w; }

	float &operator [](const int32 i) { return n[i]; }
	float operator [](const int32 i) const { return n[i]; }
};

typedef zVEC2 zPOINT2;
typedef zVEC3 zPOINT3;
typedef zVEC4 zPOINT4;

class zMAT3
{
public:
	zVEC3 v[3];

public:
	zMAT3() { v[0] = zVEC3(); v[1] = zVEC3(); v[2] = zVEC3(); v[3] = zVEC3(); }
	zMAT3(const float f) { v[0] = zVEC3(f); v[1] = zVEC3(f); v[2] = zVEC3(f); v[3] = zVEC3(f); }
	zMAT3(const zVEC3 &v0, const zVEC3 &v1, const zVEC3 &v2, const zVEC3 &v3) { v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3; }

	zVEC3 &operator [](const int32 i) { return v[i]; }
	zVEC3 operator [](const int32 i) const { return v[i]; }
};

class zMAT4
{
public:
	zVEC4 v[4];

public:
	zMAT4() { v[0] = zVEC4(); v[1] = zVEC4(); v[2] = zVEC4(); v[3] = zVEC4(); }
	zMAT4(const float f) { v[0] = zVEC4(f); v[1] = zVEC4(f); v[2] = zVEC4(f); v[3] = zVEC4(f); }
	zMAT4(const zVEC4 &v0, const zVEC4 &v1, const zVEC4 &v2, const zVEC4 &v3) { v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3; }

	zVEC4 &operator [](const int32 i) { return v[i]; }
	zVEC4 operator [](const int32 i) const { return v[i]; }
};

typedef zMAT3 zMATRIX3;
typedef zMAT4 zMATRIX4;
