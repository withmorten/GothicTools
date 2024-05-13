#include "Visual.h"

zOBJECT_DEFINITION(zCVisual);
zOBJECT_DEFINITION(zCPolyStrip);
zOBJECT_DEFINITION(zCDecal);
zOBJECT_DEFINITION(zCParticleFX);
zOBJECT_DEFINITION(zCVisualAnimate);
zOBJECT_DEFINITION(zCModel);
zOBJECT_DEFINITION(zCMorphMesh);

bool32 zCDecal::Unarchive(zCArchiver &arc)
{
	if (!zCObject::Unarchive(arc)) return FALSE;

	arc.ReadString("name", name);
	arc.ReadRawFloat("decalDim", &decalDim, sizeof(decalDim));
	arc.ReadRawFloat("decalOffset", &decalOffset, sizeof(decalOffset));
	arc.ReadBool("decal2Sided", decal2Sided);
	arc.ReadEnum("decalAlphaFunc", decalAlphaFunc);
	arc.ReadFloat("decalTexAniFPS", decalTexAniFPS);

	if (gothicVersionIn >= GOTHIC_VERSION_130)
	{
		arc.ReadByte("decalAlphaWeight", decalAlphaWeight);
		arc.ReadBool("ignoreDayLight", ignoreDayLight);
	}
	else
	{
		decalAlphaWeight = 255;
		ignoreDayLight = FALSE;
	}

	return TRUE;
}

void zCDecal::Archive(zCArchiver &arc)
{
	zCObject::Archive(arc);

	arc.WriteString("name", name);
	arc.WriteRawFloat("decalDim", &decalDim, sizeof(decalDim));
	arc.WriteRawFloat("decalOffset", &decalOffset, sizeof(decalOffset));
	arc.WriteBool("decal2Sided", decal2Sided);
	arc.WriteEnum("decalAlphaFunc", "MAT_DEFAULT;NONE;BLEND;ADD;SUB;MUL;MUL2", decalAlphaFunc);
	arc.WriteFloat("decalTexAniFPS", decalTexAniFPS);

	if (gothicVersionOut >= GOTHIC_VERSION_130)
	{
		arc.WriteByte("decalAlphaWeight", decalAlphaWeight);
		arc.WriteBool("ignoreDayLight", ignoreDayLight);
	}
}

void zCDecal::CalcHash()
{
	zCObject::CalcHash();

	hash = XXH64(name.ToChar(), name.Length(), hash);
	hash = XXH64(&decalDim, sizeof(decalDim), hash);
	hash = XXH64(&decalOffset, sizeof(decalOffset), hash);
	hash = XXH64(&decal2Sided, sizeof(decal2Sided), hash);
	hash = XXH64(&decalAlphaFunc, sizeof(decalAlphaFunc), hash);
	hash = XXH64(&decalTexAniFPS, sizeof(decalTexAniFPS), hash);

	hash = XXH64(&decalAlphaWeight, sizeof(decalAlphaWeight), hash);
	hash = XXH64(&ignoreDayLight, sizeof(ignoreDayLight), hash);
}

void zCDecal::CalcID()
{
	zCObject::CalcID();

	id = XXH32(name.ToChar(), name.Length(), id);
	id = XXH32(&decalDim, sizeof(decalDim), id);
	id = XXH32(&decalOffset, sizeof(decalOffset), id);
	id = XXH32(&decal2Sided, sizeof(decal2Sided), id);
	id = XXH32(&decalAlphaFunc, sizeof(decalAlphaFunc), id);
	id = XXH32(&decalTexAniFPS, sizeof(decalTexAniFPS), id);

	id = XXH32(&decalAlphaWeight, sizeof(decalAlphaWeight), id);
	id = XXH32(&ignoreDayLight, sizeof(ignoreDayLight), id);
}
