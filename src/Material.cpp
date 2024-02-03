#include "Material.h"

zOBJECT_DEFINITION(zCMaterial);

bool32 zCMaterial::Unarchive(zCArchiver &arc)
{
	if (!zCObject::Unarchive(arc)) return FALSE;

	if (classVersion != materialVersionIn
		&& ((materialVersionIn == MATERIAL_CLASS_VERSION_GOTHIC_1_30 && classVersion != MATERIAL_CLASS_VERSION_GOTHIC_OLD1)
		&& (materialVersionIn == MATERIAL_CLASS_VERSION_GOTHIC_1_30 && classVersion != MATERIAL_CLASS_VERSION_GOTHIC_OLD2)))
	{
		printf("Wrong Material version\n");

		return FALSE;
	}

	arc.ReadString("name", name);
	arc.ReadEnum("matGroup", matGroup);
	arc.ReadColor("color", color);
	arc.ReadFloat("smoothAngle", smoothAngle);

	arc.ReadString("texture", texture);
	arc.ReadString("texScale", texScale);
	arc.ReadFloat("texAniFPS", texAniFPS);
	arc.ReadEnum("texAniMapMode", texAniMapMode);
	arc.ReadString("texAniMapDir", texAniMapDir);

	arc.ReadBool("noCollDet", noCollDet);
	arc.ReadBool("noLightmap", noLightmap);
	arc.ReadBool("lodDontCollapse", lodDontCollapse);

	arc.ReadString("detailObject", detailObject);

	if (materialVersionIn == MATERIAL_CLASS_VERSION_GOTHIC_1_01 || materialVersionIn == MATERIAL_CLASS_VERSION_GOTHIC_1_04
		|| classVersion == MATERIAL_CLASS_VERSION_GOTHIC_OLD1 || classVersion == MATERIAL_CLASS_VERSION_GOTHIC_OLD2)
	{
		alphaFunc = color.alpha == 0xFF ? zRND_ALPHA_FUNC_NONE : zRND_ALPHA_FUNC_BLEND;

		if (matGroup == zMAT_GROUP_WATER && alphaFunc == zRND_ALPHA_FUNC_NONE)
		{
			alphaFunc = zRND_ALPHA_FUNC_BLEND;
		}
	}

	if (materialVersionIn == MATERIAL_CLASS_VERSION_GOTHIC_1_30)
	{
		arc.ReadFloat("detailObjectScale", detailObjectScale);

		arc.ReadBool("forceOccluder", forceOccluder);
		arc.ReadBool("environmentalMapping", environmentalMapping);
		arc.ReadFloat("environmentalMappingStrength", environmentalMappingStrength);

		arc.ReadEnum("waveMode", waveMode);
		arc.ReadEnum("waveSpeed", waveSpeed);

		if (classVersion == MATERIAL_CLASS_VERSION_GOTHIC_OLD1)
		{
			uint16 w;

			arc.ReadWord("waveMaxAmplitude", w);
			waveMaxAmplitude = w;

			arc.ReadWord("waveGridSize", w);
			waveGridSize = w;
		}
		else
		{
			arc.ReadFloat("waveMaxAmplitude", waveMaxAmplitude);
			arc.ReadFloat("waveGridSize", waveGridSize);
		}

		arc.ReadBool("ignoreSunLight", ignoreSunLight);

		if (classVersion != MATERIAL_CLASS_VERSION_GOTHIC_OLD1 && classVersion != MATERIAL_CLASS_VERSION_GOTHIC_OLD2)
		{
			arc.ReadEnum("alphaFunc", alphaFunc);
		}
	}
	else
	{
		detailObjectScale = 1.0f;

		forceOccluder = FALSE;
		environmentalMapping = FALSE;
		environmentalMappingStrength = 1.0f;

		waveMode = zWAVEANI_NONE;
		waveSpeed = zTFFT_NORMAL;
		waveMaxAmplitude = 30.0f;
		waveGridSize = 100.0f;

		ignoreSunLight = FALSE;
	}

	arc.ReadRawFloat("defaultMapping", &defaultMapping, sizeof(defaultMapping));

	return TRUE;
}

void zCMaterial::Archive(zCArchiver &arc)
{
	zCObject::Archive(arc);

	arc.WriteString("name", name);
	arc.WriteEnum("matGroup", "UNDEF;METAL;STONE;WOOD;EARTH;WATER;SNOW", matGroup);
	arc.WriteColor("color", color);
	arc.WriteFloat("smoothAngle", smoothAngle);

	arc.WriteString("texture", texture);
	arc.WriteString("texScale", texScale);
	arc.WriteFloat("texAniFPS", texAniFPS);
	arc.WriteEnum("texAniMapMode", "NONE;LINEAR", texAniMapMode);
	arc.WriteString("texAniMapDir", texAniMapDir);

	arc.WriteBool("noCollDet", noCollDet);
	arc.WriteBool("noLightmap", noLightmap);
	arc.WriteBool("lodDontCollapse", lodDontCollapse);

	arc.WriteString("detailObject", detailObject);

	if (materialVersionOut == MATERIAL_CLASS_VERSION_GOTHIC_1_30)
	{
		arc.WriteFloat("detailObjectScale", detailObjectScale);

		arc.WriteBool("forceOccluder", forceOccluder);
		arc.WriteBool("environmentalMapping", environmentalMapping);
		arc.WriteFloat("environmentalMappingStrength", environmentalMappingStrength);

		arc.WriteEnum("waveMode", "NONE;AMBIENT_GROUND;GROUND;AMBIENT_WALL;WALL;ENV;AMBIENT_WIND;WIND", waveMode);
		arc.WriteEnum("waveSpeed", "NONE;SLOW;NORMAL;FAST", waveSpeed);

		arc.WriteFloat("waveMaxAmplitude", waveMaxAmplitude);
		arc.WriteFloat("waveGridSize", waveGridSize);

		arc.WriteBool("ignoreSunLight", ignoreSunLight);

		arc.WriteEnum("alphaFunc", "MAT_DEFAULT;NONE;BLEND;ADD;SUB;MUL;MUL2", alphaFunc);
	}

	arc.WriteRawFloat("defaultMapping", &defaultMapping, sizeof(defaultMapping));
}

void zCMaterial::Hash()
{
	zCObject::Hash();

	hash = XXH64(name.ToChar(), name.Length(), hash);
	hash = XXH64(&matGroup, sizeof(matGroup), hash);
	hash = XXH64(&color, sizeof(color), hash);
	hash = XXH64(&smoothAngle, sizeof(smoothAngle), hash);

	hash = XXH64(texture.ToChar(), texture.Length(), hash);
	hash = XXH64(texScale.ToChar(), texScale.Length(), hash);
	hash = XXH64(&texAniFPS, sizeof(texAniFPS), hash);
	hash = XXH64(&texAniMapMode, sizeof(texAniMapMode), hash);
	hash = XXH64(texAniMapDir.ToChar(), texAniMapDir.Length(), hash);

	hash = XXH64(&noCollDet, sizeof(noCollDet), hash);
	hash = XXH64(&noLightmap, sizeof(noLightmap), hash);
	hash = XXH64(&lodDontCollapse, sizeof(lodDontCollapse), hash);

	hash = XXH64(detailObject.ToChar(), detailObject.Length(), hash);

	hash = XXH64(&detailObjectScale, sizeof(detailObjectScale), hash);

	hash = XXH64(&forceOccluder, sizeof(forceOccluder), hash);
	hash = XXH64(&environmentalMapping, sizeof(environmentalMapping), hash);
	hash = XXH64(&environmentalMappingStrength, sizeof(environmentalMappingStrength), hash);

	hash = XXH64(&waveMode, sizeof(waveMode), hash);
	hash = XXH64(&waveSpeed, sizeof(waveSpeed), hash);

	hash = XXH64(&waveMaxAmplitude, sizeof(waveMaxAmplitude), hash);
	hash = XXH64(&waveGridSize, sizeof(waveGridSize), hash);

	hash = XXH64(&ignoreSunLight, sizeof(ignoreSunLight), hash);

	hash = XXH64(&alphaFunc, sizeof(alphaFunc), hash);

	hash = XXH64(&defaultMapping, sizeof(defaultMapping), hash);
}
