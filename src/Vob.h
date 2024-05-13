#pragma once

#include "GothicTools.h"

#include "Object.h"
#include "3D.h"
#include "Material.h"

const float VOB_IGNORE_MIN_EXTEND = 10.0f;

#pragma pack (push, 1)
struct zSVobArcRawDataG1
{
	zTBBox3D bbox3DWS;
	zPOINT3 positionWS;
	zMAT3 trafoRotWS;

	struct zSBitField // 15 bit in use
	{
		byte showVisual : 1;
		byte visualCamAlign : 2;
		byte cdStatic : 1;
		byte cdDyn : 1;
		byte staticVob : 1;
		byte dynShadow : 2;

		byte hasPresetName : 1;
		byte hasVobName : 1;
		byte hasVisualName : 1;

		byte hasRelevantVisualObject : 1;
		byte hasAIObject : 1;
		byte hasEventManObject : 1; // saveGame only

		byte physicsEnabled : 1; // saveGame only
	} bitfield;
};

struct zSVobArcRawDataG2
{
	zTBBox3D bbox3DWS;
	zPOINT3 positionWS;
	zMAT3 trafoRotWS;

	struct zSBitField // 17 bit in use
	{
		byte showVisual : 1;
		byte visualCamAlign : 2;
		byte cdStatic : 1;
		byte cdDyn : 1;
		byte staticVob : 1;
		byte dynShadow : 2;

		byte hasPresetName : 1;
		byte hasVobName : 1;
		byte hasVisualName : 1;

		byte hasRelevantVisualObject : 1;
		byte hasAIObject : 1;
		byte hasEventManObject : 1; // saveGame only

		byte physicsEnabled : 1; // saveGame only
		byte visualAniMode : 2;
		byte zbias : 5;
		byte bAmbient : 1;
	} bitfield;

	float visualAniStrength;
	float fVobFarClipZ;
};
#pragma pack (pop)

enum zTVisualCamAlign
{
	zVISUAL_CAMALIGN_NONE,
	zVISUAL_CAMALIGN_YAW,
	zVISUAL_CAMALIGN_FULL,
	zVISUAL_CAMALIGN_COUNT,
};

enum zTAnimationMode
{
	zVISUAL_ANIMODE_NONE,
	zVISUAL_ANIMODE_WIND,
	zVISUAL_ANIMODE_WIND2,
};

enum zTDynShadowType
{
	zDYN_SHADOW_TYPE_NONE,
	zDYN_SHADOW_TYPE_BLOB,
	zDYN_SHADOW_TYPE_COUNT,
};

enum { DEFAULT_VOB_ZBIAS = 1 };

class zCVob : public zCObject
{
public:
	zOBJECT_DECLARATION(zCVob);

public:
	zCVob *parent;
	int32 depth; // TODO remove?

	int32 numChilds;
	zCArray<zCVob *> childs;

	zSTRING presetName;
	zTBBox3D bbox3DWS;
	zMAT3 trafoOSToWSRot;
	zPOINT3 trafoOSToWSPos;

	zSTRING vobName;
	zSTRING visualName;
	bool32 showVisual;
	zTVisualCamAlign visualCamAlign;
	zTAnimationMode visualAniMode;
	float visualAniModeStrength;
	float vobFarClipZScale;
	bool32 cdStatic;
	bool32 cdDyn;
	bool32 staticVob;
	zTDynShadowType dynShadow;
	int32 zbias;
	bool32 isAmbient;

	zCObject *visual;
	zCObject *ai;

public:
	zCVob() { visual = NULL; ai = NULL; parent = NULL; depth = 0; }
	~zCVob() { }

	bool32 UnarchiveVerbose(zCArchiver &arc);
	bool32 UnarchivePacked(zCArchiver &arc);
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

enum zTCamTrj_KeyMotionType
{
	zCAMTRJ_KEY_MOTION_UNDEF,
	zCAMTRJ_KEY_MOTION_SMOOTH,
	zCAMTRJ_KEY_MOTION_LINEAR,
	zCAMTRJ_KEY_MOTION_STEP,
	zCAMTRJ_KEY_MOTION_SLOW,
	zCAMTRJ_KEY_MOTION_FAST,
	zCAMTRJ_KEY_MOTION_CUSTOM,
};

class zCCamTrj_KeyFrame : public zCVob
{
public:
	zOBJECT_DECLARATION(zCCamTrj_KeyFrame);

public:
	float time;
	float angleRollDeg;
	float camFOVScale;
	zTCamTrj_KeyMotionType motionType;
	zTCamTrj_KeyMotionType motionTypeFOV;
	zTCamTrj_KeyMotionType motionTypeRoll;
	zTCamTrj_KeyMotionType motionTypeTimeScale;

	float tension;
	float bias;
	float continuity;
	float timeScale;
	bool32 timeIsFixed;

	zMAT4 originalPose;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

enum zTSplLerpMode
{
	zTSPL_LERP_UNDEF,
	zTSPL_LERP_PATH,
	zTSPL_LERP_PATH_IGNOREROLL,
	zTSPL_LERP_PATH_ROT_SAMPLES,
};

class zCCSCamera : public zCVob
{
public:
	zOBJECT_DECLARATION(zCCSCamera);

public:
	enum zTCamTrj_FOR
	{
		zCAMTRJ_KEY_FOR_WORLD,
		zCAMTRJ_KEY_FOR_OBJECT,
	};

	enum zTCamTrj_LoopMode
	{
		zCAMTRJ_LOOPMODE_NONE,
		zCAMTRJ_LOOPMODE_RESTART,
		zCAMTRJ_LOOPMODE_PINGPONG,
	};

public:
	zTCamTrj_FOR camTrjFOR;
	zTCamTrj_FOR targetTrjFOR;
	zTCamTrj_LoopMode loopMode;
	zTSplLerpMode splLerpMode;

	bool32 ignoreFORVobRotCam;
	bool32 ignoreFORVobRotTarget;
	bool32 adaptToSurroundings;
	bool32 easeToFirstKey;
	bool32 easeFromLastKey;
	float totalTime;

	zSTRING autoCamFocusVobName;
	bool32 autoCamPlayerMovable;
	bool32 autoCamUntriggerOnLastKey;
	float autoCamUntriggerOnLastKeyDelay;

	zCArray<zCCamTrj_KeyFrame *> posKeys;
	zCArray<zCCamTrj_KeyFrame *> targetKeys;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class zCVobLevelCompo : public zCVob
{
public:
	zOBJECT_DECLARATION(zCVobLevelCompo);
};

class zCVobSpot : public zCVob
{
public:
	zOBJECT_DECLARATION(zCVobSpot);
};

class zCVobStair : public zCVob
{
public:
	zOBJECT_DECLARATION(zCVobStair);
};

class zCVobStartpoint : public zCVob
{
public:
	zOBJECT_DECLARATION(zCVobStartpoint);
};

class zCVobWaypoint : public zCVob
{
public:
	zOBJECT_DECLARATION(zCVobWaypoint);
};

enum zTVobLightType
{
	zVOBLIGHT_TYPE_POINT,
	zVOBLIGHT_TYPE_SPOT,
	zVOBLIGHT_TYPE_DIR, // not available
	zVOBLIGHT_TYPE_AMBIENT, // not available
};

enum zTVobLightQuality
{
	zVOBLIGHT_QUAL_HI,
	zVOBLIGHT_QUAL_MID,
	zVOBLIGHT_QUAL_FASTEST,
};

class zCVobLightData
{
public:
	zTVobLightType lightType;
	float range;
	zCOLOR color;
	float spotConeAngle;
	bool32 lightStatic;
	zTVobLightQuality lightQuality;
	zSTRING lensflareFX;

	bool32 turnedOn;

	zCArray<float> rangeAniScale;
	float rangeAniFPS;
	bool32 rangeAniSmooth;

	zCArray<zCOLOR> colorAniList;
	float colorAniFPS;
	bool32 colorAniSmooth;

	bool32 canMove;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	XXH64_hash_t CalcHash(XXH64_hash_t hash);
	bool32 IsEqual(zCVobLightData &lightData);
};

class zCVobLight : public zCVob
{
public:
	zOBJECT_DECLARATION(zCVobLight);

public:
	zSTRING lightPresetInUse;
	zCVobLightData lightData;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class zCTriggerBase : public zCVob
{
public:
	zOBJECT_DECLARATION(zCTriggerBase);

public:
	zSTRING triggerTarget;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class zCCodeMaster : public zCTriggerBase
{
public:
	zOBJECT_DECLARATION(zCCodeMaster);

public:
	enum { MAX_SLAVES = 6 };

public:
	struct
	{
		bool32 orderRelevant;
		bool32 firstFalseIsFailure;
		bool32 untriggerCancels;
	} flags;

	zSTRING triggerTargetFailure;

	byte numSlaves;
	zSTRING slaveVobNameList[MAX_SLAVES];
	
public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class zCMessageFilter : public zCTriggerBase
{
public:
	zOBJECT_DECLARATION(zCMessageFilter);

public:
	enum zTMessageType
	{
		MT_NONE,
		MT_TRIGGER,
		MT_UNTRIGGER,
		MT_ENABLE,
		MT_DISABLE,
		MT_TOGGLE_ENABLED,
		MT_RESET
	};

public:
	zTMessageType onTrigger;
	zTMessageType onUntrigger;
	
public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class zCEventMover
{
public:
	enum zTEventMoverSubType
	{
		zEVENT_GOTO_KEY_FIXED_DIRECTLY,
		zEVENT_GOTO_KEY_FIXED_ORDER, // _DISABLED_
		zEVENT_GOTO_KEY_NEXT,
		zEVENT_GOTO_KEY_PREV,
		zEVENT_MISC_NUM_SUBTYPES,
	};
};

class zCMoverControler : public zCTriggerBase
{
public:
	zOBJECT_DECLARATION(zCMoverControler);

public:
	zCEventMover::zTEventMoverSubType moverMessage;
	int32 gotoFixedKey; // frame
	
public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class zCTriggerWorldStart : public zCTriggerBase
{
public:
	zOBJECT_DECLARATION(zCTriggerWorldStart);

public:
	bool32 fireOnlyFirstTime;
	
public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class zCTriggerUntouch : public zCTriggerBase
{
public:
	zOBJECT_DECLARATION(zCTriggerUntouch);
};

#pragma pack (push, 1)
struct zSTriggerFlagsRawData
{
	byte startEnabled : 1;
	byte isEnabled : 1;
	byte sendUntrigger : 1;
};

struct zSTriggerFilterFlagsRawData
{
	byte reactToOnTrigger : 1;
	byte reactToOnTouch : 1;
	byte reactToOnDamage : 1;
	byte respondToObject : 1;
	byte respondToPC : 1;
	byte respondToNPC : 1;
};
#pragma pack (pop)

class zCTrigger : public zCTriggerBase
{
public:
	zOBJECT_DECLARATION(zCTrigger);

public:
	struct
	{
		bool32 startEnabled;
		bool32 isEnabled;
		bool32 sendUntrigger;
	} flags;

	struct
	{
		bool32 reactToOnTrigger;
		bool32 reactToOnTouch;
		bool32 reactToOnDamage;
		bool32 respondToObject;
		bool32 respondToPC;
		bool32 respondToNPC;
	} filterFlags;

	zSTRING respondToVobName;
	int32 numCanBeActivated;
	float retriggerWaitSec;
	float damageThreshold;
	float fireDelaySec;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

enum zTMoverBehavior
{
	MB_2STATE_TOGGLE,
	MB_2STATE_TRIGGER_CONTROL,
	MB_2STATE_OPEN_TIMED,
	MB_NSTATE_LOOP,
	MB_NSTATE_SINGLE_KEYS,
};

enum zTPosLerpType
{
	PL_LINEAR,
	PL_CURVE,
};

enum zTSpeedType
{
	ST_CONST,
	ST_SLOW_START_END,
	ST_SLOW_START,
	ST_SLOW_END,
	ST_SEG_SLOW_START_END,
	ST_SEG_SLOW_START,
	ST_SEG_SLOW_END,
};

class zCMover : public zCTrigger
{
public:
	zOBJECT_DECLARATION(zCMover);

public:
	struct zTMov_Keyframe
	{
		zPOINT3 pos;
		zCQuat quat;
	};

public:
	zTMoverBehavior moverBehavior;
	float touchBlockerDamage;
	float stayOpenTimeSec;
	bool32 moverLocked;
	bool32 autoLinkEnabled;
	bool32 autoRotate;

	uint16 numKeyframes;
	float moveSpeed;
	zTPosLerpType posLerpType;
	zTSpeedType speedType;
	zTMov_Keyframe *keyframes;

	zSTRING sfxOpenStart;
	zSTRING sfxOpenEnd;
	zSTRING sfxMoving;
	zSTRING sfxCloseStart;
	zSTRING sfxCloseEnd;
	zSTRING sfxLock;
	zSTRING sfxUnlock;
	zSTRING sfxUseLocked;

public:
	zCMover() { keyframes = NULL; }
	~zCMover() { zDELETE_ARRAY(keyframes); }

	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class zCTriggerTeleport : public zCTrigger
{
public:
	zOBJECT_DECLARATION(zCTriggerTeleport);

public:
	zSTRING sfxTeleport;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class zCTriggerList : public zCTrigger
{
public:
	zOBJECT_DECLARATION(zCTriggerList);

public:
	enum { MAX_TARGETS = 6 };

	enum zTListProcess
	{
		LP_ALL,
		LP_NEXT_ONE,
		LP_RAND_ONE,
	};

public:
	zTListProcess listProcess;
	byte numTarget;
	zSTRING triggerTargetList[MAX_TARGETS];
	float fireDelayList[MAX_TARGETS];

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class oCCSTrigger : public zCTrigger
{
public:
	zOBJECT_DECLARATION(oCCSTrigger);
};

class oCTriggerChangeLevel : public zCTrigger
{
public:
	zOBJECT_DECLARATION(oCTriggerChangeLevel);

public:
	zSTRING levelName;
	zSTRING startVobName;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class oCTriggerScript : public zCTrigger
{
public:
	zOBJECT_DECLARATION(oCTriggerScript);

public:
	zSTRING scriptFunc;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class zCEffect : public zCVob
{
public:
	zOBJECT_DECLARATION(zCEffect);
};

class zCEarthquake : public zCEffect
{
public:
	zOBJECT_DECLARATION(zCEarthquake);

public:
	float radius;
	float timeSec;
	zVEC3 amplitudeCM;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class zCMusicControler : public zCEffect
{
public:
	zOBJECT_DECLARATION(zCMusicControler);
};

class zCPFXControler : public zCEffect
{
public:
	zOBJECT_DECLARATION(zCPFXControler);

public:
	zSTRING pfxName;
	bool32 killVobWhenDone;
	bool32 pfxStartOn;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class zCTouchDamage : public zCEffect
{
public:
	zOBJECT_DECLARATION(zCTouchDamage);

public:
	enum zTDamageCollType
	{
		CT_NONE,
		CT_BOX,
		CT_POINT,
	};

public:
	float damage;

	struct
	{
		bool32 Barrier;
		bool32 Blunt;
		bool32 Edge;
		bool32 Fire;
		bool32 Fly;
		bool32 Magic;
		bool32 Point;
		bool32 Fall;
	} damageType;

	float damageRepeatDelaySec;
	float damageVolDownScale;
	zTDamageCollType damageCollType;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class oCTouchDamage : public zCTouchDamage
{
public:
	zOBJECT_DECLARATION(oCTouchDamage);
};

class zCTouchAnimate : public zCEffect
{
public:
	zOBJECT_DECLARATION(zCTouchAnimate);
};

class zCTouchAnimateSound : public zCTouchAnimate
{
public:
	zOBJECT_DECLARATION(zCTouchAnimateSound);

public:
	zSTRING sfxTouch;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class zCVobAnimate : public zCEffect
{
public:
	zOBJECT_DECLARATION(zCVobAnimate);

public:
	bool32 startOn;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class zCVobLensFlare : public zCEffect
{
public:
	zOBJECT_DECLARATION(zCVobLensFlare);

public:
	zSTRING lensflareFX;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class zCVobScreenFX : public zCEffect
{
public:
	zOBJECT_DECLARATION(zCVobScreenFX);
};

class oCVisualFX : public zCEffect
{
public:
	zOBJECT_DECLARATION(oCVisualFX);
};

class oCVisFX_Lightning : public oCVisualFX
{
public:
	zOBJECT_DECLARATION(oCVisFX_Lightning);
};

class oCVisFX_MultiTarget : public oCVisualFX
{
public:
	zOBJECT_DECLARATION(oCVisFX_MultiTarget);
};

class zCZone : public zCVob
{
public:
	zOBJECT_DECLARATION(zCZone);

public:
	void CalcHash(); // required for bbox3DWS
};

class zCZoneZFog : public zCZone
{
public:
	zOBJECT_DECLARATION(zCZoneZFog);

public:
	float fogRangeCenter;
	float innerRangePerc;
	zCOLOR fogColor;
	bool32 fadeOutSky;
	bool32 overrideColor;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class zCZoneZFogDefault : public zCZoneZFog
{
public:
	zOBJECT_DECLARATION(zCZoneZFogDefault);
};

class zCZoneVobFarPlane : public zCZone
{
public:
	zOBJECT_DECLARATION(zCZoneVobFarPlane);

public:
	float vobFarPlaneZ;
	float innerRangePerc;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class zCZoneVobFarPlaneDefault : public zCZoneVobFarPlane
{
public:
	zOBJECT_DECLARATION(zCZoneVobFarPlaneDefault);
};

class zCVobSound : public zCZone
{
public:
	zOBJECT_DECLARATION(zCVobSound);

public:
	enum zTSoundMode
	{
		SM_LOOPING,
		SM_ONCE,
		SM_RANDOM,
	};

	enum zTSoundVolType
	{
		SV_SPHERE,
		SV_ELLIPSOID,
	};

public:
	float sndVolume;
	zTSoundMode sndMode;
	float sndRandDelay;
	float sndRandDelayVar;
	bool32 sndStartOn;
	bool32 sndAmbient3D;
	bool32 sndObstruction;
	float sndConeAngle;
	zTSoundVolType sndVolType;
	float sndRadius;
	zSTRING sndName;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class zCVobSoundDaytime : public zCVobSound
{
public:
	zOBJECT_DECLARATION(zCVobSoundDaytime);

public:
	float sndStartTime;
	float sndEndTime;
	zSTRING sndName2;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

enum
{
	zREVERB_PRESET_GENERIC,
	zREVERB_PRESET_PADDEDCELL,
	zREVERB_PRESET_ROOM,
	zREVERB_PRESET_BATHROOM,
	zREVERB_PRESET_LIVINGROOM,
	zREVERB_PRESET_STONEROOM,
	zREVERB_PRESET_AUDITORIUM,
	zREVERB_PRESET_CONCERTHALL,
	zREVERB_PRESET_CAVE,
	zREVERB_PRESET_ARENA,
	zREVERB_PRESET_HANGAR,
	zREVERB_PRESET_CARPETEDHALLWAY,
	zREVERB_PRESET_HALLWAY,
	zREVERB_PRESET_STONECORRIDOR,
	zREVERB_PRESET_ALLEY,
	zREVERB_PRESET_FOREST,
	zREVERB_PRESET_CITY,
	zREVERB_PRESET_MOUNTAINS,
	zREVERB_PRESET_QUARRY,
	zREVERB_PRESET_PLAIN,
	zREVERB_PRESET_PARKINGLOT,
	zREVERB_PRESET_SEWERPIPE,
	zREVERB_PRESET_UNDERWATER,
	zREVERB_PRESET_DRUGGED,
	zREVERB_PRESET_DIZZY,
	zREVERB_PRESET_PSYCHOTIC,
	zNUM_REVERB_PRESETS,
};

class zCZoneReverb : public zCZone
{
public:
	zOBJECT_DECLARATION(zCZoneReverb);

public:
	int32 reverbPreset;
	float reverbWeight;
	float innerRangePerc;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class zCZoneReverbDefault : public zCZoneReverb
{
public:
	zOBJECT_DECLARATION(zCZoneReverbDefault);
};

class zCZoneMusic : public zCZone
{
public:
	zOBJECT_DECLARATION(zCZoneMusic);
};

class oCZoneMusic : public zCZoneMusic
{
public:
	zOBJECT_DECLARATION(oCZoneMusic);

public:
	bool32 enabled;
	int32 priority;
	bool32 ellipsoid;
	float reverbLevel;
	float volumeLevel;
	bool32 loop;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class oCZoneMusicDefault : public oCZoneMusic
{
public:
	zOBJECT_DECLARATION(oCZoneMusicDefault);
};

class oCDummyVobGenerator : public zCVob
{
public:
	zOBJECT_DECLARATION(oCDummyVobGenerator);
};

class oCObjectGenerator : public zCVob
{
public:
	zOBJECT_DECLARATION(oCObjectGenerator);

public:
	zSTRING objectName;
	float objectSpeed;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class oCVob : public zCVob
{
public:
	zOBJECT_DECLARATION(oCVob);
};

class oCItem : public oCVob
{
public:
	zOBJECT_DECLARATION(oCItem);

public:
	static zSTRING itemsZenVisual[2]; // item instances with a visual that contains ".ZEN"

public:
	zSTRING itemInstance;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

enum oTSndMaterial
{
	SND_MAT_WOOD,
	SND_MAT_STONE,
	SND_MAT_METAL,
	SND_MAT_LEATHER,
	SND_MAT_CLAY,
	SND_MAT_GLAS,
};

class oCMOB : public oCVob
{
public:
	zOBJECT_DECLARATION(oCMOB);

public:
	zSTRING focusName;
	int32 hitpoints;
	int32 damage;
	bool32 moveable;
	bool32 takeable;
	bool32 focusOverride;
	oTSndMaterial soundMaterial;
	zSTRING visualDestroyed;
	zSTRING owner;
	zSTRING ownerGuild;
	bool32 isDestroyed;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class oCMobInter : public oCMOB
{
public:
	zOBJECT_DECLARATION(oCMobInter);

public:
	int32 state;
	int32 stateTarget;
	int32 stateNum;
	zSTRING triggerTarget;
	zSTRING useWithItem;
	zSTRING conditionFunc;
	zSTRING onStateFunc;
	bool32 rewind;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class oCMobBed : public oCMobInter
{
public:
	zOBJECT_DECLARATION(oCMobBed);
};

class oCMobFire : public oCMobInter
{
public:
	zOBJECT_DECLARATION(oCMobFire);

public:
	zSTRING fireSlot;
	zSTRING fireVobtreeName;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class oCMobItemSlot : public oCMobInter
{
public:
	zOBJECT_DECLARATION(oCMobItemSlot);

public:
	bool32 itemRemoveable;
	
public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class oCMobLadder : public oCMobInter
{
public:
	zOBJECT_DECLARATION(oCMobLadder);
};

class oCMobLockable : public oCMobInter
{
public:
	zOBJECT_DECLARATION(oCMobLockable);

public:
	bool32 locked;
	zSTRING keyInstance;
	zSTRING pickLockStr;
	
public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class oCMobContainer : public oCMobLockable
{
public:
	zOBJECT_DECLARATION(oCMobContainer);

public:
	zSTRING contains;
	
public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};

class oCMobDoor : public oCMobLockable
{
public:
	zOBJECT_DECLARATION(oCMobDoor);
};

class oCMobSwitch : public oCMobInter
{
public:
	zOBJECT_DECLARATION(oCMobSwitch);
};

class oCMobWheel : public oCMobInter
{
public:
	zOBJECT_DECLARATION(oCMobWheel);
};

class oCNpc : public oCVob
{
public:
	zOBJECT_DECLARATION(oCNpc);

public:
	zSTRING npcInstance;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
	void CalcHash();
	bool32 IsEqual(zCObject *obj);
};
