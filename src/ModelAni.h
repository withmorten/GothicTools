#pragma once

#include "GothicTools.h"

#include "3D.h"
#include "Object.h"

enum zTMdl_AniEventType
{
	zMDL_EVENT_TAG,
	zMDL_EVENT_SOUND,
	zMDL_EVENT_SOUND_GRND,
	zMDL_EVENT_ANIBATCH,
	zMDL_EVENT_SWAPMESH,
	zMDL_EVENT_HEADING,
	zMDL_EVENT_PFX,
	zMDL_EVENT_PFX_GRND,
	zMDL_EVENT_PFX_STOP,
	zMDL_EVENT_SETMESH,
	zMDL_EVENT_MM_STARTANI,
	zMDL_EVENT_CAM_TREMOR
};

#define zMDL_ANIEVENT_MAXSTRING 4

class zCModelAniEvent
{
public:
	zTMdl_AniEventType aniEventType;

	int32 frameNr;

	float value1;
	float value2;
	float value3;
	float value4;

	zSTRING tagString;
	zSTRING sfxName;
	zSTRING emitterName;
	zSTRING string[zMDL_ANIEVENT_MAXSTRING];

	float prob;

public:
	void Load(zCFileBIN &file);
	void Save(zCFileBIN &file);
};

#define zFCHUNK_MODELANI ((uint16)0xA000)
#define zFCHUNK_MAN_HEADER ((uint16)0xA020)
#define zFCHUNK_MAN_SOURCE ((uint16)0xA010)
#define zFCHUNK_MAN_ANIEVENTS ((uint16)0xA030)
#define zFCHUNK_MAN_RAWDATA ((uint16)0xA090)

#pragma pack (push, 1)
struct zTMdl_AniSample
{
	uint16 rotation[3];
	uint16 position[3];
};
#pragma pack (pop)

enum zTMdl_AniType
{
	zMDL_ANI_TYPE_NORMAL,
	zMDL_ANI_TYPE_BLEND,
	zMDL_ANI_TYPE_SYNC,
	zMDL_ANI_TYPE_ALIAS,
	zMDL_ANI_TYPE_BATCH,
	zMDL_ANI_TYPE_COMB,
	zMDL_ANI_TYPE_DISABLED
};

class zCModelAni : public zCObject
{
public:
	zOBJECT_DECLARATION(zCModelAni);

public:
	zSTRING aniName;
	zSTRING nextAniName;
	zSTRING aliasName;

	zTMdl_AniType aniType;

	int32 layer;
	float fpsRate;
	float fpsRateSource;
	float samplePosRangeMin;
	float samplePosScaler;

	float blendInSpeed;
	float blendOutSpeed;
	zSTRING aniFlagsStr;
	zSTRING ascName;
	zSTRING aniDirStr;
	int32 firstFrame;
	int32 lastFrame;
	float maxFps;
	float speed;
	float collisionVolumeScale;

	zTBBox3D aniBBox3DObjSpace;

	zTSrcFileStats srcFileStats;
	zSTRING scriptDefLine;

	uint32 checksum;

	int32 numNodes;
	int32 *nodeIndexList;

	int32 numFrames;
	int32 numSamples;
	zTMdl_AniSample *aniSampleMatrix;

	int32 numAniEvents;
	zCModelAniEvent *aniEvents;

public:
	zCModelAni() { InitValues(); }
	zCModelAni(zTMdl_AniType type) { InitValues(); aniType = type; }

	~zCModelAni()
	{
		zFREE(nodeIndexList);
		zFREE(aniSampleMatrix);
		zDELETE_ARRAY(aniEvents);
	}

	void InitValues();

	bool32 LoadMAN(const zSTRING &filename) { zCFileBIN f(filename); return LoadMAN(f); }
	bool32 LoadMAN(zCFileBIN &file);

	void SaveMAN(const zSTRING &filename) { zCFileBIN f(filename, TRUE); SaveMAN(f); }
	void SaveMAN(zCFileBIN &file);
};
