#pragma once

#include "GothicTools.h"

#include "Object.h"
#include "Material.h"

class zCVisual : public zCObject
{
public:
	zOBJECT_DECLARATION(zCVisual);
};

class zCPolyStrip : public zCObject
{
public:
	zOBJECT_DECLARATION(zCPolyStrip);
};

class zCDecal : public zCVisual
{
public:
	zOBJECT_DECLARATION(zCDecal);

public:
	zSTRING name;
	zVEC2 decalDim;
	zVEC2 decalOffset;
	bool32 decal2Sided;
	zTRnd_AlphaBlendFunc decalAlphaFunc;
	float decalTexAniFPS;
	byte decalAlphaWeight;
	bool32 ignoreDayLight;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	void CalcID();
};

class zCParticleFX : public zCVisual
{
public:
	zOBJECT_DECLARATION(zCParticleFX);
};

class zCVisualAnimate : public zCVisual
{
public:
	zOBJECT_DECLARATION(zCVisualAnimate);
};

class zCModel : public zCVisualAnimate
{
public:
	zOBJECT_DECLARATION(zCModel);
};

class zCMorphMesh : public zCVisualAnimate
{
public:
	zOBJECT_DECLARATION(zCMorphMesh);
};
