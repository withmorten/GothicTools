#pragma once

#include "GothicTools.h"

#include "Color.h"

enum zTRnd_TextureFormat
{
	zRND_TEX_FORMAT_ARGB_8888,
	zRND_TEX_FORMAT_ABGR_8888,
	zRND_TEX_FORMAT_RGBA_8888,
	zRND_TEX_FORMAT_BGRA_8888,

	zRND_TEX_FORMAT_RGB_888,
	zRND_TEX_FORMAT_BGR_888,

	zRND_TEX_FORMAT_ARGB_4444,
	zRND_TEX_FORMAT_ARGB_1555,
	zRND_TEX_FORMAT_RGB_565,

	zRND_TEX_FORMAT_PAL_8,

	zRND_TEX_FORMAT_DXT1,
	zRND_TEX_FORMAT_DXT2,
	zRND_TEX_FORMAT_DXT3,
	zRND_TEX_FORMAT_DXT4,
	zRND_TEX_FORMAT_DXT5,

	zRND_TEX_FORMAT_COUNT,
};

struct zTRnd_TexFormatInfo
{
	float bytesPerPixel;

	int32 rPos;
	int32 gPos;
	int32 bPos;
	int32 aPos;

	int32 rSize;
	int32 gSize;
	int32 bSize;
	int32 aSize;
};

class zCTextureInfo
{
public:
	zTRnd_TextureFormat texFormat;
	int32 sizeX;
	int32 sizeY;
	int32 numMipMap;

	int32 refSizeX;
	int32 refSizeY;

	zCOLOR averageColor;
};

struct zTTFFHeader
{
	char cookie[4];
	uint32 vers;
	zCTextureInfo texInfo;
};

struct zTTexPalette
{
	byte r;
	byte g;
	byte b;
};

class zCTexture
{
public:
	zCTexture();
	~zCTexture();

	bool32 Load(zFILE *file);
	void Save(zFILE *file);

public:
	zTTFFHeader header;
	zTTexPalette *palette;
	byte **data;
};

class zCLightMap
{
public:
	zVEC3 lightmapOrigin;
	zVEC3 lightmapUVUp;
	zVEC3 lightmapUVRight;

	int32 texIndex;
	zCTexture *tex;

public:
	bool32 Load(zCFileBIN &file);
	void Save(zCFileBIN &file);
};
