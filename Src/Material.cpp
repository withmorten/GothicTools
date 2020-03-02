#include "GothicTools.h"

zVOID zCMaterial::InitValues()
{
	smoothAngle = 60.0f;
	libFlag = 0;
	default_mapping = zVEC2(2.34375f, 2.34375f);
	texScale = "1 1";
	texAniMap = FALSE;
	texAniMapDelta = "0 0";
	lodDontCollapse = FALSE;
	noCollDet = FALSE;
	rndAlphaBlendFunc = zRND_ALPHA_FUNC_NONE;

	forceOccluder = FALSE;
	detailTextureScale = 1.0f;

	m_bEnvironmentalMapping = FALSE;
	m_bEnvironmentalMappingStrength = 1.0f;

	m_bIgnoreSun = FALSE;

	m_enuWaveMode = zWAVEANI_NONE;
	m_enuWaveSpeed = zTFFT_NORMAL;
	m_fWaveMaxAmplitude = 30.0f;
	m_fWaveGridSize = 100.0f;

	color.SetRGBA(150, 150, 150, 255);
	matGroup = zMAT_GROUP_UNDEF;
	dontUseLightmaps = FALSE;
}

zVOID zCMaterial::Archive(zCFileBIN &file, zDWORD objectIndex)
{
	zLONG startPos = file.BinPos(); // chunk.startPos
	zLONG pos;
	zDWORD size;

	// Write chunk header
	file.BinWriteDWord(0); // chunk.size
	file.BinWriteWord(materialVersionOut); // chunk.version
	file.BinWriteDWord(objectIndex); // chunk.objectIndex
	file.BinWriteString("%"); // chunk.name
	file.BinWriteString("zCMaterial"); // chunk.className

	// Write chunk data
	file.BinWriteString(name); // name
	file.BinWriteEnumByte(matGroup); // matGroup
	file.BinWriteDWord(color.dword); // color
	file.BinWriteFloat(smoothAngle); // smoothAngle
	file.BinWriteString(texture); // texture
	file.BinWriteString(texScale); // texScale
	file.BinWriteFloat(aniFPS); // texAniFPS
	file.BinWriteByte(texAniMap); // texAniMapMode
	file.BinWriteString(texAniMapDelta); // texAniMapDir
	file.BinWriteByte(noCollDet); // noCollDet
	file.BinWriteByte(dontUseLightmaps); // noLightmap
	file.BinWriteByte(lodDontCollapse); // lodDontCollapse
	file.BinWriteString(detailTexture); // detailObject

	if (materialVersionOut == MATERIAL_VERSION_SUM_GOTHIC_1_30)
	{
		file.BinWriteFloat(detailTextureScale); // detailObjectScale
		file.BinWriteByte(forceOccluder); // forceOccluder
		file.BinWriteByte(m_bEnvironmentalMapping); // environmentalMapping
		file.BinWriteFloat(m_bEnvironmentalMappingStrength); // environmentalMappingStrength
		file.BinWriteEnumByte(m_enuWaveMode); // waveMode
		file.BinWriteEnumByte(m_enuWaveSpeed); // waveSpeed
		file.BinWriteFloat(m_fWaveMaxAmplitude); // waveMaxAmplitude
		file.BinWriteFloat(m_fWaveGridSize); // waveGridSize
		file.BinWriteByte(m_bIgnoreSun); // ignoreSunLight
		file.BinWriteEnumByte(rndAlphaBlendFunc); // alphaFunc
	}

	file.BinWrite(&default_mapping, sizeof(default_mapping));

	// Fix chunk size
	pos = file.BinPos();
	size = pos - startPos;
	file.BinSeek(startPos);
	file.BinWriteDWord(size);
	file.BinSeek(pos);
}

zBOOL zCMaterial::Unarchive(zCFileBIN &file)
{
	zDWORD dw;
	zWORD version;
	zSTRING s;

	// Read chunk header
	file.BinReadDWord(dw); // chunk.size
	file.BinReadWord(version); // chunk.version
	file.BinReadDWord(dw); // chunk.objectIndex, 0
	file.BinReadString(s); // chunk.name, %
	file.BinReadString(s); // chunk.className, zCMaterial

	if (version != materialVersionIn
		&& ((materialVersionIn == MATERIAL_VERSION_SUM_GOTHIC_1_30 && version != MATERIAL_VERSION_SUM_GOTHIC_OLD1)
		&& (materialVersionIn == MATERIAL_VERSION_SUM_GOTHIC_1_30 && version != MATERIAL_VERSION_SUM_GOTHIC_OLD2)))
	{

		printf("Wrong Material version\n");

		return FALSE;
	}

	// Read chunk data
	file.BinReadString(s); // name
	file.BinReadEnumByte(matGroup); // matGroup
	file.BinReadDWord(color.dword); // color

	file.BinReadFloat(smoothAngle); // smoothAngle

	file.BinReadString(texture); // texture

	file.BinReadString(texScale); // texScale

	file.BinReadFloat(aniFPS); // texAniFPS
	file.BinReadByte(texAniMap); // texAniMapMode

	file.BinReadString(texAniMapDelta); // texAniMapDir

	file.BinReadByte(noCollDet); // noCollDet
	file.BinReadByte(dontUseLightmaps); // noLightmap
	file.BinReadByte(lodDontCollapse); // lodDontCollapse

	file.BinReadString(detailTexture); // detailObject

	if (materialVersionIn == MATERIAL_VERSION_SUM_GOTHIC_1_01 || materialVersionIn == MATERIAL_VERSION_SUM_GOTHIC_1_04)
	{
		if (color.alpha == 0xFF) rndAlphaBlendFunc = zRND_ALPHA_FUNC_NONE;
		else rndAlphaBlendFunc = zRND_ALPHA_FUNC_BLEND;

		if (matGroup == zMAT_GROUP_WATER && rndAlphaBlendFunc == zRND_ALPHA_FUNC_NONE) rndAlphaBlendFunc = zRND_ALPHA_FUNC_BLEND;
	}

	if (materialVersionIn == MATERIAL_VERSION_SUM_GOTHIC_1_30)
	{
		file.BinReadFloat(detailTextureScale); // detailObjectScale
		if (detailTextureScale < 0.0f) detailTextureScale = 1.0f;

		file.BinReadByte(forceOccluder); // forceOccluder
		file.BinReadByte(m_bEnvironmentalMapping); // environmentalMapping
		file.BinReadFloat(m_bEnvironmentalMappingStrength); // environmentalMappingStrength
		file.BinReadEnumByte(m_enuWaveMode); // waveMode
		file.BinReadEnumByte(m_enuWaveSpeed); // waveSpeed

		if (version == MATERIAL_VERSION_SUM_GOTHIC_OLD1)
		{
			zWORD waveMaxAmplitude;
			zWORD waveGridSize;

			file.BinReadWord(waveMaxAmplitude); // waveMaxAmplitude
			file.BinReadWord(waveGridSize); // waveGridSize

			m_fWaveMaxAmplitude = waveMaxAmplitude;
			m_fWaveGridSize = waveGridSize;
		}
		else
		{
			file.BinReadFloat(m_fWaveMaxAmplitude); // waveMaxAmplitude
			file.BinReadFloat(m_fWaveGridSize); // waveGridSize
		}

		file.BinReadByte(m_bIgnoreSun); // ignoreSunLight

		if (version != MATERIAL_VERSION_SUM_GOTHIC_OLD1 && version != MATERIAL_VERSION_SUM_GOTHIC_OLD2)
		{
			file.BinReadEnumByte(rndAlphaBlendFunc); // alphaFunc
			if (rndAlphaBlendFunc == zRND_ALPHA_FUNC_SUB || (color.alpha < 255 && rndAlphaBlendFunc == zRND_ALPHA_FUNC_NONE)) rndAlphaBlendFunc = zRND_ALPHA_FUNC_BLEND;
		}
	}

	file.BinRead(&default_mapping, sizeof(default_mapping)); // defaultMapping

	return TRUE;
}