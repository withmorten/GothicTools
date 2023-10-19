#pragma once

#include "GothicTools.h"

#include "Color.h"
#include "Object.h"

enum zTMat_Group
{
	zMAT_GROUP_UNDEF,
	zMAT_GROUP_METAL,
	zMAT_GROUP_STONE,
	zMAT_GROUP_WOOD,
	zMAT_GROUP_EARTH,
	zMAT_GROUP_WATER,
	zMAT_GROUP_SNOW, // Gothic 2
	zMAT_NUM_MAT_GROUP,
};

enum zTWaveAniMode
{
	zWAVEANI_NONE,
	zWAVEANI_GROUND_AMBIENT,
	zWAVEANI_GROUND,
	zWAVEANI_WALL_AMBIENT,
	zWAVEANI_WALL,
	zWAVEANI_ENV,
	zWAVEANI_WIND_AMBIENT,
	zWAVEANI_WIND,
};

enum zTFFT
{
	zTFFT_NONE,
	zTFFT_SLOW,
	zTFFT_NORMAL,
	zTFFT_FAST,
};

enum zTRnd_AlphaBlendFunc
{
	zRND_ALPHA_FUNC_MAT_DEFAULT,
	zRND_ALPHA_FUNC_NONE,
	zRND_ALPHA_FUNC_BLEND,
	zRND_ALPHA_FUNC_ADD,
	zRND_ALPHA_FUNC_SUB,
	zRND_ALPHA_FUNC_MUL,
	zRND_ALPHA_FUNC_MUL2,
	zRND_ALPHA_FUNC_TEST,
	zRND_ALPHA_FUNC_BLEND_TEST
};

enum
{
	zMAT_TEX_ANI_MAP_NONE,
	zMAT_TEX_ANI_MAP_LINEAR,
};

class zCMaterial : public zCObject
{
public:
	static const zSTRING GetClassName() { return "zCMaterial"; }

public:
	zSTRING name;
	zTMat_Group matGroup;
	zCOLOR color;
	float smoothAngle;
	zSTRING texture;
	zSTRING texScale;
	float texAniFPS;
	int32 texAniMapMode;
	zSTRING texAniMapDir;

	bool32 noCollDet;
	bool32 noLightmap;
	bool32 lodDontCollapse;

	zSTRING detailObject;
	float detailObjectScale;

	bool32 forceOccluder;
	bool32 environmentalMapping;
	float environmentalMappingStrength;

	zTWaveAniMode waveMode;
	zTFFT waveSpeed;
	float waveMaxAmplitude;
	float waveGridSize;

	bool32 ignoreSunLight;
	zTRnd_AlphaBlendFunc alphaFunc;

	zPOINT2 defaultMapping;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

