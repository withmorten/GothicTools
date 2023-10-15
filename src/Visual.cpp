#include "Visual.h"

bool32 zCDecal::Unarchive(zCArchiver &arc)
{
	if (!zCObject::Unarchive(arc)) return FALSE;

	arc.ReadString("name", name);
	arc.ReadRawFloat("decalDim", &decalDim, sizeof(decalDim));
	arc.ReadRawFloat("decalOffset", &decalOffset, sizeof(decalOffset));
	arc.ReadBool("decal2Sided", decal2Sided);
	arc.ReadEnum("decalAlphaFunc", decalAlphaFunc);
	arc.ReadFloat("decalTexAniFPS", decalTexAniFPS);

	if (meshAndBspVersionIn == BSPMESH_VERSION_GOTHIC_1_30)
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
	arc.WriteEnum("decalAlphaFunc", decalAlphaFunc);
	arc.WriteFloat("decalTexAniFPS", decalTexAniFPS);

	if (meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_30)
	{
		arc.WriteByte("decalAlphaWeight", decalAlphaWeight);
		arc.WriteBool("ignoreDayLight", ignoreDayLight);
	}
}
