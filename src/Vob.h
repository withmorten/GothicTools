#pragma once

#include "GothicTools.h"

#include "Object.h"
#include "3D.h"
#include "Material.h"

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
	static const zSTRING GetClassName() { return "zCVob"; }

public:
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
	zCVob();
	~zCVob();

	bool32 UnarchiveVerbose(zCArchiver &arc);
	bool32 UnarchivePacked(zCArchiver &arc);
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
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
	static const zSTRING GetClassName() { return "zCCamTrj_KeyFrame"; }

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
	static const zSTRING GetClassName() { return "zCCSCamera"; }

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

	int32 numPos;
	int32 numTargets;
	zCArray<zCCamTrj_KeyFrame *> posKeys;
	zCArray<zCCamTrj_KeyFrame *> targetKeys;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

class zCVobLevelCompo : public zCVob
{
public:
	static const zSTRING GetClassName() { return "zCVobLevelCompo"; }
};

class zCVobSpot : public zCVob
{
public:
	static const zSTRING GetClassName() { return "zCVobSpot"; }
};

class zCVobStair : public zCVob
{
public:
	static const zSTRING GetClassName() { return "zCVobStair"; }
};

class zCVobStartpoint : public zCVob
{
public:
	static const zSTRING GetClassName() { return "zCVobStartpoint"; }
};

class zCVobWaypoint : public zCVob
{
public:
	static const zSTRING GetClassName() { return "zCVobWaypoint"; }
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

	zSTRING rangeAniScale;
	float rangeAniFPS;
	bool32 rangeAniSmooth;

	zSTRING colorAniList;
	float colorAniFPS;
	bool32 colorAniSmooth;

	bool32 canMove;

public:
	void Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

class zCVobLight : public zCVob
{
public:
	static const zSTRING GetClassName() { return "zCVobLight"; }

public:
	zSTRING lightPresetInUse;
	zCVobLightData lightData;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

class zCTriggerBase : public zCVob
{
public:
	static const zSTRING GetClassName() { return "zCTriggerBase"; }

public:
	zSTRING triggerTarget;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

class zCCodeMaster : public zCTriggerBase
{
public:
	static const zSTRING GetClassName() { return "zCCodeMaster"; }

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
};

class zCMessageFilter : public zCTriggerBase
{
public:
	static const zSTRING GetClassName() { return "zCMessageFilter"; }

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
	static const zSTRING GetClassName() { return "zCMoverControler"; }

public:
	zCEventMover::zTEventMoverSubType moverMessage;
	int32 gotoFixedKey; // frame
	
public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

class zCTriggerWorldStart : public zCTriggerBase
{
public:
	static const zSTRING GetClassName() { return "zCTriggerWorldStart"; }

public:
	bool32 fireOnlyFirstTime;
	
public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

class zCTriggerUntouch : public zCTriggerBase
{
public:
	static const zSTRING GetClassName() { return "zCTriggerUntouch"; }
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
	static const zSTRING GetClassName() { return "zCTrigger"; }

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
	static const zSTRING GetClassName() { return "zCMover"; }

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
};

class zCTriggerTeleport : public zCTrigger
{
public:
	static const zSTRING GetClassName() { return "zCTriggerTeleport"; }

public:
	zSTRING sfxTeleport;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

class zCTriggerList : public zCTrigger
{
public:
	static const zSTRING GetClassName() { return "zCTriggerList"; }

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
};

class oCCSTrigger : public zCTrigger
{
public:
	static const zSTRING GetClassName() { return "oCCSTrigger"; }
};

class oCTriggerChangeLevel : public zCTrigger
{
public:
	static const zSTRING GetClassName() { return "oCTriggerChangeLevel"; }

public:
	zSTRING levelName;
	zSTRING startVobName;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

class oCTriggerScript : public zCTrigger
{
public:
	static const zSTRING GetClassName() { return "oCTriggerScript"; }

public:
	zSTRING scriptFunc;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

class zCEffect : public zCVob
{
public:
	static const zSTRING GetClassName() { return "zCEffect"; }
};

class zCEarthquake : public zCEffect
{
public:
	static const zSTRING GetClassName() { return "zCEarthquake"; }

public:
	float radius;
	float timeSec;
	zVEC3 amplitudeCM;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

class zCMusicControler : public zCEffect
{
public:
	static const zSTRING GetClassName() { return "zCMusicControler"; }
};

class zCPFXControler : public zCEffect
{
public:
	static const zSTRING GetClassName() { return "zCPFXControler"; }

public:
	zSTRING pfxName;
	bool32 killVobWhenDone;
	bool32 pfxStartOn;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

class zCTouchDamage : public zCEffect
{
public:
	static const zSTRING GetClassName() { return "zCTouchDamage"; }

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
};

class oCTouchDamage : public zCTouchDamage
{
public:
	static const zSTRING GetClassName() { return "oCTouchDamage"; }
};

class zCTouchAnimate : public zCEffect
{
public:
	static const zSTRING GetClassName() { return "zCTouchAnimate"; }
};

class zCTouchAnimateSound : public zCTouchAnimate
{
public:
	static const zSTRING GetClassName() { return "zCTouchAnimateSound"; }

public:
	zSTRING sfxTouch;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

class zCVobAnimate : public zCEffect
{
public:
	static const zSTRING GetClassName() { return "zCVobAnimate"; }

public:
	bool32 startOn;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

class zCVobLensFlare : public zCEffect
{
public:
	static const zSTRING GetClassName() { return "zCVobLensFlare"; }

public:
	zSTRING lensflareFX;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

class zCVobScreenFX : public zCEffect
{
public:
	static const zSTRING GetClassName() { return "zCVobScreenFX"; }
};

class oCVisualFX : public zCEffect
{
public:
	static const zSTRING GetClassName() { return "oCVisualFX"; }
};

class oCVisFX_Lightning : public oCVisualFX
{
public:
	static const zSTRING GetClassName() { return "oCVisFX_Lightning"; }
};

class oCVisFX_MultiTarget : public oCVisualFX
{
public:
	static const zSTRING GetClassName() { return "oCVisFX_MultiTarget"; }
};

class zCZone : public zCVob
{
public:
	static const zSTRING GetClassName() { return "zCZone"; }
};

class zCZoneZFog : public zCZone
{
public:
	static const zSTRING GetClassName() { return "zCZoneZFog"; }

public:
	float fogRangeCenter;
	float innerRangePerc;
	zCOLOR fogColor;
	bool32 fadeOutSky;
	bool32 overrideColor;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

class zCZoneZFogDefault : public zCZoneZFog
{
public:
	static const zSTRING GetClassName() { return "zCZoneZFogDefault"; }
};

class zCZoneVobFarPlane : public zCZone
{
public:
	static const zSTRING GetClassName() { return "zCZoneVobFarPlane"; }

public:
	float vobFarPlaneZ;
	float innerRangePerc;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

class zCZoneVobFarPlaneDefault : public zCZoneVobFarPlane
{
public:
	static const zSTRING GetClassName() { return "zCZoneVobFarPlaneDefault"; }
};

class zCVobSound : public zCZone
{
public:
	static const zSTRING GetClassName() { return "zCVobSound"; }

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
};

class zCVobSoundDaytime : public zCVobSound
{
public:
	static const zSTRING GetClassName() { return "zCVobSoundDaytime"; }

public:
	float sndStartTime;
	float sndEndTime;
	zSTRING sndName2;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
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
	static const zSTRING GetClassName() { return "zCZoneReverb"; }

public:
	int32 reverbPreset;
	float reverbWeight;
	float innerRangePerc;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

class zCZoneReverbDefault : public zCZoneReverb
{
public:
	static const zSTRING GetClassName() { return "zCZoneReverbDefault"; }
};

class zCZoneMusic : public zCZone
{
public:
	static const zSTRING GetClassName() { return "zCZoneMusic"; }
};

class oCZoneMusic : public zCZoneMusic
{
public:
	static const zSTRING GetClassName() { return "oCZoneMusic"; }

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
};

class oCZoneMusicDefault : public oCZoneMusic
{
public:
	static const zSTRING GetClassName() { return "oCZoneMusicDefault"; }
};

class oCDummyVobGenerator : public zCVob
{
public:
	static const zSTRING GetClassName() { return "oCDummyVobGenerator"; }
};

class oCObjectGenerator : public zCVob
{
public:
	static const zSTRING GetClassName() { return "oCObjectGenerator"; }

public:
	zSTRING objectName;
	float objectSpeed;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

class oCVob : public zCVob
{
public:
	static const zSTRING GetClassName() { return "oCVob"; }
};

class oCItem : public oCVob
{
public:
	static const zSTRING GetClassName() { return "oCItem"; }

public:
	zSTRING itemInstance;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
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
	static const zSTRING GetClassName() { return "oCMOB"; }

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
};

class oCMobInter : public oCMOB
{
public:
	static const zSTRING GetClassName() { return "oCMobInter"; }

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
};

class oCMobBed : public oCMobInter
{
public:
	static const zSTRING GetClassName() { return "oCMobBed"; }
};

class oCMobFire : public oCMobInter
{
public:
	static const zSTRING GetClassName() { return "oCMobFire"; }

public:
	zSTRING fireSlot;
	zSTRING fireVobtreeName;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

class oCMobItemSlot : public oCMobInter
{
public:
	static const zSTRING GetClassName() { return "oCMobItemSlot"; }

public:
	bool32 itemRemoveable;
	
public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

class oCMobLadder : public oCMobInter
{
public:
	static const zSTRING GetClassName() { return "oCMobLadder"; }
};

class oCMobLockable : public oCMobInter
{
public:
	static const zSTRING GetClassName() { return "oCMobLockable"; }

public:
	bool32 locked;
	zSTRING keyInstance;
	zSTRING pickLockStr;
	
public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

class oCMobContainer : public oCMobLockable
{
public:
	static const zSTRING GetClassName() { return "oCMobContainer"; }

public:
	zSTRING contains;
	
public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};

class oCMobDoor : public oCMobLockable
{
public:
	static const zSTRING GetClassName() { return "oCMobDoor"; }
};

class oCMobSwitch : public oCMobInter
{
public:
	static const zSTRING GetClassName() { return "oCMobSwitch"; }
};

class oCMobWheel : public oCMobInter
{
public:
	static const zSTRING GetClassName() { return "oCMobWheel"; }
};

class oCNpc : public oCVob
{
public:
	static const zSTRING GetClassName() { return "oCNpc"; }

public:
	zSTRING npcInstance;

public:
	bool32 Unarchive(zCArchiver &arc);
	void Archive(zCArchiver &arc);
};
