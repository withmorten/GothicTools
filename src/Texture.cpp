#include "Texture.h"

zTRnd_TexFormatInfo texFormatInfoList[zRND_TEX_FORMAT_COUNT] =
{
	//	bpp,	rpos,	gpos,	bpos,	apos,	rsize,	gsize,	bsize,	aSize
	{	4.0F,		8,		16,		24,		0,		8,		8,		8,		8},		// argb_8888
	{	4.0F,		24,		16,		8,		0,		8,		8,		8,		8},		// abgr_8888
	{	4.0F,		0,		8,		16,		24,		8,		8,		8,		8},		// rgba_8888
	{	4.0F,		16,		8,		0,		24,		8,		8,		8,		8},		// bgra_8888

	{	3.0F,		0,		8,		16,		0,		8,		8,		8,		0},		// rgb_888
	{	3.0F,		16,		8,		0,		0,		8,		8,		8,		0},		// bgr_888

	{	2.0F,		8,		4,		0,		12,		4,		4,		4,		4},		// argb_4444
	{	2.0F,		10,		5,		0,		15,		5,		5,		5,		1},		// argb_1555
	{	2.0F,		11,		5,		0,		0,		5,		6,		5,		0},		// rgb_565

	{	1.0F,		0,		0,		0,		0,		0,		0,		0,		0},		// PAL_8

	{	0.5F,		0,		0,		0,		0,		0,		0,		0,		0},		// DXT1
	{	1.0F,		0,		0,		0,		0,		0,		0,		0,		0},		// DXT2
	{	1.0F,		0,		0,		0,		0,		0,		0,		0,		1},		// DXT3
	{	1.0F,		0,		0,		0,		0,		0,		0,		0,		0},		// DXT4
	{	1.0F,		0,		0,		0,		0,		0,		0,		0,		0}		// DXT5
};

char zTEX_FILE_INTERNAL_COOKIE[] = "ZTEX";
const uint32 zTEX_FILE_INTERNAL_VERS = 0;

zCTexture::zCTexture()
{
	zMEMSET<zTTFFHeader>(&header, 0x00, 1);

	palette = NULL;
	data = NULL;
}

zCTexture::~zCTexture()
{
	zFREE(palette);

	for (int32 i = header.texInfo.numMipMap - 1; i >= 0; i--)
	{
		zFREE(data[i]);
	}

	zFREE(data);
}

bool32 zCTexture::Load(zFILE *file)
{
	file->Read(&header, sizeof(header));

	if (memcmp(header.cookie, zTEX_FILE_INTERNAL_COOKIE, sizeof(header.cookie))) return FALSE;

	if (header.vers != zTEX_FILE_INTERNAL_VERS) return FALSE;

	if (header.texInfo.texFormat == zRND_TEX_FORMAT_PAL_8)
	{
		palette = zMALLOC(zTTexPalette, 256);

		file->Read(palette, sizeof(*palette) * 256);
	}

	data = zMALLOC(byte *, header.texInfo.numMipMap);

	int32 sizeX;
	int32 sizeY;

	for (int32 i = header.texInfo.numMipMap - 1; i >= 0; i--)
	{
		sizeX = header.texInfo.sizeX >> i;
		sizeY = header.texInfo.sizeY >> i;

		if (sizeX < 1) sizeX = 1;
		if (sizeY < 1) sizeY = 1;

		int32 dataSize = (int32)((sizeX * sizeY) * texFormatInfoList[header.texInfo.texFormat].bytesPerPixel);

		data[i] = zMALLOC(byte, dataSize);

		file->Read(data[i], dataSize);
	}

	return TRUE;
}

void zCTexture::Save(zFILE *file)
{
	file->Write(&header, sizeof(header));

	if (header.texInfo.texFormat == zRND_TEX_FORMAT_PAL_8)
	{
		file->Write(palette, sizeof(*palette) * 256);
	}

	int32 sizeX;
	int32 sizeY;

	for (int32 i = header.texInfo.numMipMap - 1; i >= 0; i--)
	{
		sizeX = header.texInfo.sizeX >> i;
		sizeY = header.texInfo.sizeY >> i;

		if (sizeX < 1) sizeX = 1;
		if (sizeY < 1) sizeY = 1;

		int32 dataSize = (int32)((sizeX * sizeY) * texFormatInfoList[header.texInfo.texFormat].bytesPerPixel);

		file->Write(data[i], dataSize);
	}
}

bool32 zCLightMap::Load(zCFileBIN &file)
{
	file.BinRead(&lightmapOrigin, sizeof(lightmapOrigin));
	file.BinRead(&lightmapUVUp, sizeof(lightmapUVUp));
	file.BinRead(&lightmapUVRight, sizeof(lightmapUVRight));
	file.BinReadInt(texIndex);

	return TRUE;
}

void zCLightMap::Save(zCFileBIN &file)
{
	file.BinWrite(&lightmapOrigin, sizeof(lightmapOrigin));
	file.BinWrite(&lightmapUVUp, sizeof(lightmapUVUp));
	file.BinWrite(&lightmapUVRight, sizeof(lightmapUVRight));
	file.BinWriteInt(texIndex);
}
