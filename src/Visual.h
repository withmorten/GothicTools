#pragma once

#include "GothicTools.h"

#include "Object.h"
#include "Material.h"

class zCVisual : public zCObject
{
public:
	static const zSTRING GetClassName() { return "zCVisual"; }
};

class zCPolyStrip : public zCObject
{
public:
	static const zSTRING GetClassName() { return "zCPolyStrip"; }
};

class zCDecal : public zCVisual
{
public:
	static const zSTRING GetClassName() { return "zCDecal"; }

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
};

class zCParticleFX : public zCVisual
{
public:
	static const zSTRING GetClassName() { return "zCParticleFX"; }
};

class zCVisualAnimate : public zCVisual
{
public:
	static const zSTRING GetClassName() { return "zCVisualAnimate"; }
};

class zCModel : public zCVisualAnimate
{
public:
	static const zSTRING GetClassName() { return "zCModel"; }
};

class zCMorphMesh : public zCVisualAnimate
{
public:
	static const zSTRING GetClassName() { return "zCMorphMesh"; }
};
