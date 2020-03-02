#pragma once

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

	zINT frameNr;

	zVALUE value1;
	zVALUE value2;
	zVALUE value3;
	zVALUE value4;

	zSTRING tagString;
	zSTRING sfxName;
	zSTRING emitterName;
	zSTRING string[zMDL_ANIEVENT_MAXSTRING];

	zREAL prob;

public:
	zVOID Load(zCFileBIN &file);
	zVOID Save(zCFileBIN &file);
};

#define zFCHUNK_MODELANI ((zWORD)0xA000)
#define zFCHUNK_MAN_HEADER ((zWORD)0xA020)
#define zFCHUNK_MAN_SOURCE ((zWORD)0xA010)
#define zFCHUNK_MAN_ANIEVENTS ((zWORD)0xA030)
#define zFCHUNK_MAN_RAWDATA ((zWORD)0xA090)

#pragma pack (push, 1)
struct zTMdl_AniSample
{
	zWORD rotation[3];
	zWORD position[3];
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

class zCModelAni
{
public:
	zSTRING aniName;
	zSTRING nextAniName;
	zSTRING aliasName;

	zTMdl_AniType aniType;

	zINT layer;
	zREAL fpsRate;
	zREAL fpsRateSource;
	zREAL samplePosRangeMin;
	zREAL samplePosScaler;

	zREAL blendInSpeed;
	zREAL blendOutSpeed;
	zSTRING aniFlagsStr;
	zSTRING ascName;
	zSTRING aniDirStr;
	zINT firstFrame;
	zINT lastFrame;
	zREAL maxFps;
	zREAL speed;
	zREAL collisionVolumeScale;

	zTBBox3D aniBBox3DObjSpace;

	zTSrcFileStats srcFileStats;
	zSTRING scriptDefLine;

	zDWORD checksum;

	zINT numNodes;
	zINT *nodeIndexList;

	zINT numFrames;
	zINT numSamples;
	zTMdl_AniSample *aniSampleMatrix;

	zINT numAniEvents;
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

	zVOID InitValues();

	zBOOL LoadMAN(const zSTRING &filename) { zCFileBIN f(filename); return LoadMAN(f); }
	zBOOL LoadMAN(zCFileBIN &file);

	zVOID SaveMAN(const zSTRING &filename) { zCFileBIN f(filename, TRUE); SaveMAN(f); }
	zVOID SaveMAN(zCFileBIN &file);
};
