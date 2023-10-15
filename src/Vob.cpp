#include "Vob.h"

zCVob::zCVob()
{
	visual = NULL;
	ai = NULL;
}

zCVob::~zCVob()
{
	zDELETE(visual);
	zDELETE(ai);
}

bool32 zCVob::UnarchiveVerbose(zCArchiver &arc)
{
	arc.ReadString("presetName", presetName);

	arc.ReadRawFloat("bbox3DWS", &bbox3DWS, sizeof(bbox3DWS));
	arc.ReadRaw("trafoOSToWSRot", &trafoOSToWSRot, sizeof(trafoOSToWSRot));
	arc.ReadVec3("trafoOSToWSPos", trafoOSToWSPos);

	arc.ReadString("vobName", vobName);
	arc.ReadString("visual", visualName);
	arc.ReadBool("showVisual", showVisual);
	arc.ReadEnum("visualCamAlign", visualCamAlign);

	if (meshAndBspVersionIn == BSPMESH_VERSION_GOTHIC_1_30)
	{
		arc.ReadEnum("visualAniMode", visualAniMode);
		arc.ReadFloat("visualAniModeStrength", visualAniModeStrength);
		arc.ReadFloat("vobFarClipZScale", vobFarClipZScale);
	}
	else
	{
		visualAniMode = zVISUAL_ANIMODE_NONE;
		visualAniModeStrength = 0.0f;
		vobFarClipZScale = 1.0f;
	}

	arc.ReadBool("cdStatic", cdStatic);
	arc.ReadBool("cdDyn", cdDyn);
	arc.ReadBool("staticVob", staticVob);
	arc.ReadEnum("dynShadow", dynShadow);

	if (meshAndBspVersionIn == BSPMESH_VERSION_GOTHIC_1_30)
	{
		arc.ReadInt("zbias", zbias);
		arc.ReadBool("isAmbient", isAmbient);
	}
	else
	{
		zbias = DEFAULT_VOB_ZBIAS;
		isAmbient = FALSE;
	}

	visual = arc.ReadObject("visual");
	ai = arc.ReadObject("ai");

	return TRUE;
}

bool32 zCVob::UnarchivePacked(zCArchiver &arc)
{
	if (meshAndBspVersionIn == BSPMESH_VERSION_GOTHIC_1_04)
	{
		zSVobArcRawDataG1 rawData;

		arc.ReadRaw("dataRaw", &rawData, sizeof(rawData));

		bbox3DWS = rawData.bbox3DWS;
		trafoOSToWSRot = rawData.trafoRotWS;
		trafoOSToWSPos = rawData.positionWS;

		if (rawData.bitfield.hasPresetName)
		{
			arc.ReadString("presetName", presetName);
		}

		if (rawData.bitfield.hasVobName)
		{
			arc.ReadString("vobName", vobName);
		}

		if (rawData.bitfield.hasVisualName)
		{
			arc.ReadString("visual", visualName);
		}

		showVisual = rawData.bitfield.showVisual;
		visualCamAlign = (zTVisualCamAlign)rawData.bitfield.visualCamAlign;
		visualAniMode = zVISUAL_ANIMODE_NONE;
		visualAniModeStrength = 0.0f;

		vobFarClipZScale = 1.0f;

		cdStatic = rawData.bitfield.cdStatic;
		cdDyn = rawData.bitfield.cdDyn;
		staticVob = rawData.bitfield.staticVob;
		dynShadow = (zTDynShadowType)rawData.bitfield.dynShadow;
		zbias = DEFAULT_VOB_ZBIAS;
		isAmbient = FALSE;

		if (rawData.bitfield.hasRelevantVisualObject)
		{
			visual = arc.ReadObject("visual");
		}

		if (rawData.bitfield.hasAIObject)
		{
			ai = arc.ReadObject("ai");
		}
	}
	else if (meshAndBspVersionIn == BSPMESH_VERSION_GOTHIC_1_30)
	{
		zSVobArcRawDataG2 rawData;

		arc.ReadRaw("dataRaw", &rawData, sizeof(rawData));

		bbox3DWS = rawData.bbox3DWS;
		trafoOSToWSRot = rawData.trafoRotWS;
		trafoOSToWSPos = rawData.positionWS;

		if (rawData.bitfield.hasPresetName)
		{
			arc.ReadString("presetName", presetName);
		}

		if (rawData.bitfield.hasVobName)
		{
			arc.ReadString("vobName", vobName);
		}

		if (rawData.bitfield.hasVisualName)
		{
			arc.ReadString("visual", visualName);
		}

		showVisual = rawData.bitfield.showVisual;
		visualCamAlign = (zTVisualCamAlign)rawData.bitfield.visualCamAlign;
		visualAniMode = (zTAnimationMode)rawData.bitfield.visualAniMode;
		visualAniModeStrength = rawData.visualAniStrength;

		vobFarClipZScale = rawData.fVobFarClipZ;

		cdStatic = rawData.bitfield.cdStatic;
		cdDyn = rawData.bitfield.cdDyn;
		staticVob = rawData.bitfield.staticVob;
		dynShadow = (zTDynShadowType)rawData.bitfield.dynShadow;
		zbias = rawData.bitfield.zbias;
		isAmbient = rawData.bitfield.bAmbient;

		if (rawData.bitfield.hasRelevantVisualObject)
		{
			visual = arc.ReadObject("visual");
		}

		if (rawData.bitfield.hasAIObject)
		{
			ai = arc.ReadObject("ai");
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

bool32 zCVob::Unarchive(zCArchiver &arc)
{
	if (!zCObject::Unarchive(arc)) return FALSE;

	int32 pack = 0;

	if (meshAndBspVersionIn == BSPMESH_VERSION_GOTHIC_1_04 || meshAndBspVersionIn == BSPMESH_VERSION_GOTHIC_1_30)
	{
		// it is always true for BIN_SAFE, and never true for ASCII
		arc.ReadInt("pack", pack);
	}

	if (pack)
	{
		return UnarchivePacked(arc);
	}
	else
	{
		return UnarchiveVerbose(arc);
	}
}

void zCVob::Archive(zCArchiver &arc)
{
	zCObject::Archive(arc);

	if (meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_04 || meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_30)
	{
		arc.WriteInt("pack", 0);
	}

	arc.WriteString("presetName", presetName);

	arc.WriteRawFloat("bbox3DWS", &bbox3DWS, sizeof(bbox3DWS));
	arc.WriteRaw("trafoOSToWSRot", &trafoOSToWSRot, sizeof(trafoOSToWSRot));
	arc.WriteVec3("trafoOSToWSPos", trafoOSToWSPos);

	arc.WriteString("vobName", vobName);
	arc.WriteString("visual", visualName);
	arc.WriteBool("showVisual", showVisual);
	arc.WriteEnum("visualCamAlign", visualCamAlign);

	if (meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_30)
	{
		arc.WriteEnum("visualAniMode", visualAniMode);
		arc.WriteFloat("visualAniModeStrength", visualAniModeStrength);
		arc.WriteFloat("vobFarClipZScale", vobFarClipZScale);
	}

	arc.WriteBool("cdStatic", cdStatic);
	arc.WriteBool("cdDyn", cdDyn);
	arc.WriteBool("staticVob", staticVob);
	arc.WriteEnum("dynShadow", dynShadow);

	if (meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_30)
	{
		arc.WriteInt("zbias", zbias);
		arc.WriteBool("isAmbient", isAmbient);
	}

	if (visual) arc.WriteObject(visual);
	else arc.WriteChunk("visual");

	if (ai) arc.WriteObject(ai);
	else arc.WriteChunk("ai");
}

bool32 zCCamTrj_KeyFrame::Unarchive(zCArchiver &arc)
{
	if (!zCVob::Unarchive(arc)) return FALSE;

	arc.ReadFloat("time", time);
	arc.ReadFloat("angleRollDeg", angleRollDeg);
	arc.ReadFloat("camFOVScale", camFOVScale);

	arc.ReadEnum("motionType", motionType);
	arc.ReadEnum("motionTypeFOV", motionTypeFOV);
	arc.ReadEnum("motionTypeRoll", motionTypeRoll);
	arc.ReadEnum("motionTypeTimeScale", motionTypeTimeScale);

	arc.ReadFloat("tension", tension);
	arc.ReadFloat("bias", bias);
	arc.ReadFloat("continuity", continuity);
	arc.ReadFloat("timeScale", timeScale);
	arc.ReadBool("timeIsFixed", timeIsFixed);

	arc.ReadRaw("originalPose", &originalPose, sizeof(originalPose));

	return TRUE;
}

void zCCamTrj_KeyFrame::Archive(zCArchiver &arc)
{
	zCVob::Archive(arc);

	arc.WriteFloat("time", time);
	arc.WriteFloat("angleRollDeg", angleRollDeg);
	arc.WriteFloat("camFOVScale", camFOVScale);

	arc.WriteEnum("motionType", motionType);
	arc.WriteEnum("motionTypeFOV", motionTypeFOV);
	arc.WriteEnum("motionTypeRoll", motionTypeRoll);
	arc.WriteEnum("motionTypeTimeScale", motionTypeTimeScale);

	arc.WriteFloat("tension", tension);
	arc.WriteFloat("bias", bias);
	arc.WriteFloat("continuity", continuity);
	arc.WriteFloat("timeScale", timeScale);
	arc.WriteBool("timeIsFixed", timeIsFixed);

	arc.WriteRaw("originalPose", &originalPose, sizeof(originalPose));
}

bool32 zCCSCamera::Unarchive(zCArchiver &arc)
{
	if (!zCVob::Unarchive(arc)) return FALSE;

	arc.ReadEnum("camTrjFOR", camTrjFOR);
	arc.ReadEnum("targetTrjFOR", targetTrjFOR);
	arc.ReadEnum("loopMode", loopMode);
	arc.ReadEnum("splLerpMode", splLerpMode);

	arc.ReadBool("ignoreFORVobRotCam", ignoreFORVobRotCam);
	arc.ReadBool("ignoreFORVobRotTarget", ignoreFORVobRotTarget);
	arc.ReadBool("adaptToSurroundings", adaptToSurroundings);
	arc.ReadBool("easeToFirstKey", easeToFirstKey);
	arc.ReadBool("easeFromLastKey", easeFromLastKey);
	arc.ReadFloat("totalTime", totalTime);

	arc.ReadString("autoCamFocusVobName", autoCamFocusVobName);
	arc.ReadBool("autoCamPlayerMovable", autoCamPlayerMovable);
	arc.ReadBool("autoCamUntriggerOnLastKey", autoCamUntriggerOnLastKey);
	arc.ReadFloat("autoCamUntriggerOnLastKeyDelay", autoCamUntriggerOnLastKeyDelay);

	arc.ReadInt("numPos", numPos);
	arc.ReadInt("numTargets", numTargets);

	for (int32 i = 0; i < numPos; i++)
	{
		posKeys.Insert((zCCamTrj_KeyFrame *)arc.ReadObject());
	}

	for (int32 i = 0; i < numTargets; i++)
	{
		targetKeys.Insert((zCCamTrj_KeyFrame *)arc.ReadObject());
	}

	return TRUE;
}

void zCCSCamera::Archive(zCArchiver &arc)
{
	zCVob::Archive(arc);

	arc.WriteEnum("camTrjFOR", camTrjFOR);
	arc.WriteEnum("targetTrjFOR", targetTrjFOR);
	arc.WriteEnum("loopMode", loopMode);
	arc.WriteEnum("splLerpMode", splLerpMode);

	arc.WriteBool("ignoreFORVobRotCam", ignoreFORVobRotCam);
	arc.WriteBool("ignoreFORVobRotTarget", ignoreFORVobRotTarget);
	arc.WriteBool("adaptToSurroundings", adaptToSurroundings);
	arc.WriteBool("easeToFirstKey", easeToFirstKey);
	arc.WriteBool("easeFromLastKey", easeFromLastKey);
	arc.WriteFloat("totalTime", totalTime);

	arc.WriteString("autoCamFocusVobName", autoCamFocusVobName);
	arc.WriteBool("autoCamPlayerMovable", autoCamPlayerMovable);
	arc.WriteBool("autoCamUntriggerOnLastKey", autoCamUntriggerOnLastKey);
	arc.WriteFloat("autoCamUntriggerOnLastKeyDelay", autoCamUntriggerOnLastKeyDelay);

	arc.WriteInt("numPos", numPos);
	arc.WriteInt("numTargets", numTargets);

	for (int32 i = 0; i < numPos; i++)
	{
		arc.WriteObject(posKeys[i]);
	}

	for (int32 i = 0; i < numTargets; i++)
	{
		arc.WriteObject(targetKeys[i]);
	}
}

void zCVobLightData::Unarchive(zCArchiver &arc)
{
	arc.ReadEnum("lightType", lightType);
	arc.ReadFloat("range", range);
	arc.ReadColor("color", color);
	arc.ReadFloat("spotConeAngle", spotConeAngle);
	arc.ReadBool("lightStatic", lightStatic);
	arc.ReadEnum("lightQuality", lightQuality);
	arc.ReadString("lensflareFX", lensflareFX);

	if (!lightStatic)
	{
		arc.ReadBool("turnedOn", turnedOn);

		arc.ReadString("rangeAniScale", rangeAniScale);
		arc.ReadFloat("rangeAniFPS", rangeAniFPS);
		arc.ReadBool("rangeAniSmooth", rangeAniSmooth);

		arc.ReadString("colorAniList", colorAniList);
		arc.ReadFloat("colorAniFPS", colorAniFPS);
		arc.ReadBool("colorAniSmooth", colorAniSmooth);

		if (meshAndBspVersionIn == BSPMESH_VERSION_GOTHIC_1_30)
		{
			arc.ReadBool("canMove", canMove);
		}
	}
}

void zCVobLightData::Archive(zCArchiver &arc)
{
	arc.WriteEnum("lightType", lightType);
	arc.WriteFloat("range", range);
	arc.WriteColor("color", color);
	arc.WriteFloat("spotConeAngle", spotConeAngle);
	arc.WriteBool("lightStatic", lightStatic);
	arc.WriteEnum("lightQuality", lightQuality);
	arc.WriteString("lensflareFX", lensflareFX);

	if (!lightStatic)
	{
		arc.WriteBool("turnedOn", turnedOn);

		arc.WriteString("rangeAniScale", rangeAniScale);
		arc.WriteFloat("rangeAniFPS", rangeAniFPS);
		arc.WriteBool("rangeAniSmooth", rangeAniSmooth);

		arc.WriteString("colorAniList", colorAniList);
		arc.WriteFloat("colorAniFPS", colorAniFPS);
		arc.WriteBool("colorAniSmooth", colorAniSmooth);

		if (meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_30)
		{
			arc.WriteBool("canMove", canMove);
		}
	}
}

bool32 zCVobLight::Unarchive(zCArchiver &arc)
{
	if (!zCVob::Unarchive(arc)) return FALSE;

	arc.ReadString("lightPresetInUse", lightPresetInUse);

	lightData.Unarchive(arc);

	return TRUE;
}

void zCVobLight::Archive(zCArchiver &arc)
{
	zCVob::Archive(arc);

	arc.WriteString("lightPresetInUse", lightPresetInUse);

	lightData.Archive(arc);
}

bool32 zCTriggerBase::Unarchive(zCArchiver &arc)
{
	if (!zCVob::Unarchive(arc)) return FALSE;

	arc.ReadString("triggerTarget", triggerTarget);

	return TRUE;
}

void zCTriggerBase::Archive(zCArchiver &arc)
{
	zCVob::Archive(arc);

	arc.WriteString("triggerTarget", triggerTarget);
}

bool32 zCCodeMaster::Unarchive(zCArchiver &arc)
{
	if (!zCTriggerBase::Unarchive(arc)) return FALSE;

	arc.ReadBool("orderRelevant", flags.orderRelevant);
	arc.ReadBool("firstFalseIsFailure", flags.firstFalseIsFailure);
	arc.ReadString("triggerTargetFailure", triggerTargetFailure);
	arc.ReadBool("untriggerCancels", flags.untriggerCancels);

	arc.ReadByte("numSlaves", numSlaves);

	if (numSlaves > MAX_SLAVES) return FALSE;

	for (int32 i = 0; i < numSlaves; i++)
	{
		zSTRING n = i;

		arc.ReadString(zSTRING("slaveVobName" + n).ToChar(), slaveVobNameList[i]);
	}

	return TRUE;
}

void zCCodeMaster::Archive(zCArchiver &arc)
{
	zCTriggerBase::Archive(arc);

	arc.WriteBool("orderRelevant", flags.orderRelevant);
	arc.WriteBool("firstFalseIsFailure", flags.firstFalseIsFailure);
	arc.WriteString("triggerTargetFailure", triggerTargetFailure);
	arc.WriteBool("untriggerCancels", flags.untriggerCancels);

	arc.WriteByte("numSlaves", numSlaves);

	for (int32 i = 0; i < numSlaves; i++)
	{
		zSTRING n = i;

		arc.WriteString(zSTRING("slaveVobName" + n).ToChar(), slaveVobNameList[i]);
	}
}

bool32 zCMessageFilter::Unarchive(zCArchiver &arc)
{
	if (!zCTriggerBase::Unarchive(arc)) return FALSE;

	arc.ReadEnum("onTrigger", onTrigger);
	arc.ReadEnum("onUntrigger", onUntrigger);

	return TRUE;
}

void zCMessageFilter::Archive(zCArchiver &arc)
{
	zCTriggerBase::Archive(arc);

	arc.WriteEnum("onTrigger", onTrigger);
	arc.WriteEnum("onUntrigger", onUntrigger);
}

bool32 zCMoverControler::Unarchive(zCArchiver &arc)
{
	if (!zCTriggerBase::Unarchive(arc)) return FALSE;

	arc.ReadEnum("moverMessage", moverMessage);
	arc.ReadInt("gotoFixedKey", gotoFixedKey);

	return TRUE;
}

void zCMoverControler::Archive(zCArchiver &arc)
{
	zCTriggerBase::Archive(arc);

	arc.WriteEnum("moverMessage", moverMessage);
	arc.WriteInt("gotoFixedKey", gotoFixedKey);
}

bool32 zCTriggerWorldStart::Unarchive(zCArchiver &arc)
{
	if (!zCTriggerBase::Unarchive(arc)) return FALSE;

	arc.ReadBool("fireOnlyFirstTime", fireOnlyFirstTime);

	return TRUE;
}

void zCTriggerWorldStart::Archive(zCArchiver &arc)
{
	zCTriggerBase::Archive(arc);

	arc.WriteBool("fireOnlyFirstTime", fireOnlyFirstTime);
}

bool32 zCTrigger::Unarchive(zCArchiver &arc)
{
	if (!zCTriggerBase::Unarchive(arc)) return FALSE;

	zSTriggerFlagsRawData flagsRaw = { 0x00 };
	arc.ReadRaw("flags", &flagsRaw, sizeof(flagsRaw));

	flags.startEnabled = flagsRaw.startEnabled;
	flags.isEnabled = flagsRaw.isEnabled;
	flags.sendUntrigger = flagsRaw.sendUntrigger;

	zSTriggerFilterFlagsRawData filterFlagsRaw = { 0x00 };
	arc.ReadRaw("filterFlags", &filterFlagsRaw, sizeof(filterFlagsRaw));

	filterFlags.reactToOnTrigger = filterFlagsRaw.reactToOnTrigger;
	filterFlags.reactToOnTouch = filterFlagsRaw.reactToOnTouch;
	filterFlags.reactToOnDamage = filterFlagsRaw.reactToOnDamage;
	filterFlags.respondToObject = filterFlagsRaw.respondToObject;
	filterFlags.respondToPC = filterFlagsRaw.respondToPC;
	filterFlags.respondToNPC = filterFlagsRaw.respondToNPC;

	arc.ReadString("respondToVobName", respondToVobName);
	arc.ReadInt("numCanBeActivated", numCanBeActivated);
	arc.ReadFloat("retriggerWaitSec", retriggerWaitSec);
	arc.ReadFloat("damageThreshold", damageThreshold);

	arc.ReadFloat("fireDelaySec", fireDelaySec);

	return TRUE;
}

void zCTrigger::Archive(zCArchiver &arc)
{
	zCTriggerBase::Archive(arc);

	zSTriggerFlagsRawData flagsRaw = { 0x00 };

	flagsRaw.startEnabled = flags.startEnabled;
	flagsRaw.isEnabled = flags.isEnabled;
	flagsRaw.sendUntrigger = flags.sendUntrigger;

	arc.WriteRaw("flags", &flagsRaw, sizeof(flagsRaw));

	zSTriggerFilterFlagsRawData filterFlagsRaw = { 0x00 };

	filterFlagsRaw.reactToOnTrigger = filterFlags.reactToOnTrigger;
	filterFlagsRaw.reactToOnTouch = filterFlags.reactToOnTouch;
	filterFlagsRaw.reactToOnDamage = filterFlags.reactToOnDamage;
	filterFlagsRaw.respondToObject = filterFlags.respondToObject;
	filterFlagsRaw.respondToPC = filterFlags.respondToPC;
	filterFlagsRaw.respondToNPC = filterFlags.respondToNPC;

	arc.WriteRaw("filterFlags", &filterFlagsRaw, sizeof(filterFlagsRaw));

	arc.WriteString("respondToVobName", respondToVobName);
	arc.WriteInt("numCanBeActivated", numCanBeActivated);
	arc.WriteFloat("retriggerWaitSec", retriggerWaitSec);
	arc.WriteFloat("damageThreshold", damageThreshold);

	arc.WriteFloat("fireDelaySec", fireDelaySec);
}

bool32 zCTriggerTeleport::Unarchive(zCArchiver &arc)
{
	if (!zCTrigger::Unarchive(arc)) return FALSE;

	arc.ReadString("sfxTeleport", sfxTeleport);

	return TRUE;
}

void zCTriggerTeleport::Archive(zCArchiver &arc)
{
	zCTrigger::Archive(arc);

	arc.WriteString("sfxTeleport", sfxTeleport);
}

bool32 zCMover::Unarchive(zCArchiver &arc)
{
	if (!zCTrigger::Unarchive(arc)) return FALSE;

	arc.ReadEnum("moverBehavior", moverBehavior);
	arc.ReadFloat("touchBlockerDamage", touchBlockerDamage);
	arc.ReadFloat("stayOpenTimeSec", stayOpenTimeSec);
	arc.ReadBool("moverLocked", moverLocked);
	arc.ReadBool("autoLinkEnabled", autoLinkEnabled);

	if (meshAndBspVersionIn == BSPMESH_VERSION_GOTHIC_1_30)
	{
		arc.ReadBool("autoRotate", autoRotate);
	}

	arc.ReadWord("numKeyframes", numKeyframes);

	if (numKeyframes > 0)
	{
		keyframes = zNEW_ARRAY(zTMov_Keyframe, numKeyframes);

		arc.ReadFloat("moveSpeed", moveSpeed);
		arc.ReadEnum("posLerpType", posLerpType);
		arc.ReadEnum("speedType", speedType);

		arc.ReadRaw("keyframes", keyframes, sizeof(*keyframes) * numKeyframes);
	}

	arc.ReadString("sfxOpenStart", sfxOpenStart);
	arc.ReadString("sfxOpenEnd", sfxOpenEnd);
	arc.ReadString("sfxMoving", sfxMoving);
	arc.ReadString("sfxCloseStart", sfxCloseStart);
	arc.ReadString("sfxCloseEnd", sfxCloseEnd);
	arc.ReadString("sfxLock", sfxLock);
	arc.ReadString("sfxUnlock", sfxUnlock);
	arc.ReadString("sfxUseLocked", sfxUseLocked);

	return TRUE;
}

void zCMover::Archive(zCArchiver &arc)
{
	zCTrigger::Archive(arc);

	arc.WriteEnum("moverBehavior", moverBehavior);
	arc.WriteFloat("touchBlockerDamage", touchBlockerDamage);
	arc.WriteFloat("stayOpenTimeSec", stayOpenTimeSec);
	arc.WriteBool("moverLocked", moverLocked);
	arc.WriteBool("autoLinkEnabled", autoLinkEnabled);

	if (meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_30)
	{
		arc.WriteBool("autoRotate", autoRotate);
	}

	arc.WriteWord("numKeyframes", numKeyframes);

	if (numKeyframes > 0)
	{
		arc.WriteFloat("moveSpeed", moveSpeed);
		arc.WriteEnum("posLerpType", posLerpType);
		arc.WriteEnum("speedType", speedType);

		arc.WriteRaw("keyframes", keyframes, sizeof(*keyframes) * numKeyframes);
	}

	arc.WriteString("sfxOpenStart", sfxOpenStart);
	arc.WriteString("sfxOpenEnd", sfxOpenEnd);
	arc.WriteString("sfxMoving", sfxMoving);
	arc.WriteString("sfxCloseStart", sfxCloseStart);
	arc.WriteString("sfxCloseEnd", sfxCloseEnd);
	arc.WriteString("sfxLock", sfxLock);
	arc.WriteString("sfxUnlock", sfxUnlock);
	arc.WriteString("sfxUseLocked", sfxUseLocked);
}

bool32 zCTriggerList::Unarchive(zCArchiver &arc)
{
	if (!zCTrigger::Unarchive(arc)) return FALSE;

	arc.ReadEnum("listProcess", listProcess);
	arc.ReadByte("numTarget", numTarget);

	if (numTarget > MAX_TARGETS) return FALSE;

	for (int32 i = 0; i < numTarget; i++)
	{
		zSTRING n = i;

		arc.ReadString(zSTRING("triggerTarget" + n).ToChar(), triggerTargetList[i]);
		arc.ReadFloat(zSTRING("fireDelay" + n).ToChar(), fireDelayList[i]);
	}

	return TRUE;
}

void zCTriggerList::Archive(zCArchiver &arc)
{
	zCTrigger::Archive(arc);

	arc.WriteEnum("listProcess", listProcess);
	arc.WriteByte("numTarget", numTarget);

	for (int32 i = 0; i < numTarget; i++)
	{
		zSTRING n = i;

		arc.WriteString(zSTRING("triggerTarget" + n).ToChar(), triggerTargetList[i]);
		arc.WriteFloat(zSTRING("fireDelay" + n).ToChar(), fireDelayList[i]);
	}
}

bool32 oCTriggerChangeLevel::Unarchive(zCArchiver &arc)
{
	if (!zCTrigger::Unarchive(arc)) return FALSE;

	arc.ReadString("levelName", levelName);
	arc.ReadString("startVobName", startVobName);

	return TRUE;
}

void oCTriggerChangeLevel::Archive(zCArchiver &arc)
{
	zCTrigger::Archive(arc);

	arc.WriteString("levelName", levelName);
	arc.WriteString("startVobName", startVobName);
}

bool32 oCTriggerScript::Unarchive(zCArchiver &arc)
{
	if (!zCTrigger::Unarchive(arc)) return FALSE;

	arc.ReadString("scriptFunc", scriptFunc);

	return TRUE;
}

void oCTriggerScript::Archive(zCArchiver &arc)
{
	zCTrigger::Archive(arc);

	arc.WriteString("scriptFunc", scriptFunc);
}

bool32 zCEarthquake::Unarchive(zCArchiver &arc)
{
	if (!zCEffect::Unarchive(arc)) return FALSE;

	arc.ReadFloat("radius", radius);
	arc.ReadFloat("timeSec", timeSec);
	arc.ReadVec3("amplitudeCM", amplitudeCM);

	return TRUE;
}

void zCEarthquake::Archive(zCArchiver &arc)
{
	zCEffect::Archive(arc);

	arc.WriteFloat("radius", radius);
	arc.WriteFloat("timeSec", timeSec);
	arc.WriteVec3("amplitudeCM", amplitudeCM);
}

bool32 zCPFXControler::Unarchive(zCArchiver &arc)
{
	if (!zCEffect::Unarchive(arc)) return FALSE;

	arc.ReadString("pfxName", pfxName);
	arc.ReadBool("killVobWhenDone", killVobWhenDone);
	arc.ReadBool("pfxStartOn", pfxStartOn);

	return TRUE;
}

void zCPFXControler::Archive(zCArchiver &arc)
{
	zCEffect::Archive(arc);

	arc.WriteString("pfxName", pfxName);
	arc.WriteBool("killVobWhenDone", killVobWhenDone);
	arc.WriteBool("pfxStartOn", pfxStartOn);
}

bool32 zCTouchDamage::Unarchive(zCArchiver &arc)
{
	if (!zCEffect::Unarchive(arc)) return FALSE;

	arc.ReadFloat("damage", damage);

	// originally retrieved from oCTouchDamage::GetDamageTypeArcEnum() and done via looping
	// and "fallback" to damageType "1" in case we are actually a zCTouchDamage - but only oCTouchDamage actually exists
	arc.ReadBool("Barrier", damageType.Barrier);
	arc.ReadBool("Blunt", damageType.Blunt);
	arc.ReadBool("Edge", damageType.Edge);
	arc.ReadBool("Fire", damageType.Fire);
	arc.ReadBool("Fly", damageType.Fly);
	arc.ReadBool("Magic", damageType.Magic);
	arc.ReadBool("Point", damageType.Point);
	arc.ReadBool("Fall", damageType.Fall);

	arc.ReadFloat("damageRepeatDelaySec", damageRepeatDelaySec);
	arc.ReadFloat("damageVolDownScale", damageVolDownScale);
	arc.ReadEnum("damageCollType", damageCollType);

	return TRUE;
}

void zCTouchDamage::Archive(zCArchiver &arc)
{
	zCEffect::Archive(arc);

	arc.WriteFloat("damage", damage);

	// originally retrieved from oCTouchDamage::GetDamageTypeArcEnum() and done via looping
	// and "fallback" to damageType "1" in case we are actually a zCTouchDamage - but only oCTouchDamage actually exists
	arc.WriteBool("Barrier", damageType.Barrier);
	arc.WriteBool("Blunt", damageType.Blunt);
	arc.WriteBool("Edge", damageType.Edge);
	arc.WriteBool("Fire", damageType.Fire);
	arc.WriteBool("Fly", damageType.Fly);
	arc.WriteBool("Magic", damageType.Magic);
	arc.WriteBool("Point", damageType.Point);
	arc.WriteBool("Fall", damageType.Fall);

	arc.WriteFloat("damageRepeatDelaySec", damageRepeatDelaySec);
	arc.WriteFloat("damageVolDownScale", damageVolDownScale);
	arc.WriteEnum("damageCollType", damageCollType);
}

bool32 zCTouchAnimateSound::Unarchive(zCArchiver &arc)
{
	if (!zCTouchAnimate::Unarchive(arc)) return FALSE;

	arc.ReadString("sfxTouch", sfxTouch);

	return TRUE;
}

void zCTouchAnimateSound::Archive(zCArchiver &arc)
{
	zCTouchAnimate::Archive(arc);

	arc.WriteString("sfxTouch", sfxTouch);
}

bool32 zCVobAnimate::Unarchive(zCArchiver &arc)
{
	if (!zCEffect::Unarchive(arc)) return FALSE;

	arc.ReadBool("startOn", startOn);

	return TRUE;
}

void zCVobAnimate::Archive(zCArchiver &arc)
{
	zCEffect::Archive(arc);

	arc.WriteBool("startOn", startOn);
}

bool32 zCVobLensFlare::Unarchive(zCArchiver &arc)
{
	if (!zCEffect::Unarchive(arc)) return FALSE;

	arc.ReadString("lensflareFX", lensflareFX);

	return TRUE;
}

void zCVobLensFlare::Archive(zCArchiver &arc)
{
	zCEffect::Archive(arc);

	arc.WriteString("lensflareFX", lensflareFX);
}

bool32 zCZoneZFog::Unarchive(zCArchiver &arc)
{
	if (!zCZone::Unarchive(arc)) return FALSE;

	arc.ReadFloat("fogRangeCenter", fogRangeCenter);
	arc.ReadFloat("innerRangePerc", innerRangePerc);
	arc.ReadColor("fogColor", fogColor);

	if (meshAndBspVersionIn == BSPMESH_VERSION_GOTHIC_1_30)
	{
		arc.ReadBool("fadeOutSky", fadeOutSky);
		arc.ReadBool("overrideColor", overrideColor);
	}

	return TRUE;
}

void zCZoneZFog::Archive(zCArchiver &arc)
{
	zCZone::Archive(arc);

	arc.WriteFloat("fogRangeCenter", fogRangeCenter);
	arc.WriteFloat("innerRangePerc", innerRangePerc);
	arc.WriteColor("fogColor", fogColor);

	if (meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_30)
	{
		arc.WriteBool("fadeOutSky", fadeOutSky);
		arc.WriteBool("overrideColor", overrideColor);
	}
}

bool32 zCZoneVobFarPlane::Unarchive(zCArchiver &arc)
{
	if (!zCZone::Unarchive(arc)) return FALSE;

	arc.ReadFloat("vobFarPlaneZ", vobFarPlaneZ);
	arc.ReadFloat("innerRangePerc", innerRangePerc);

	return TRUE;
}

void zCZoneVobFarPlane::Archive(zCArchiver &arc)
{
	zCZone::Archive(arc);

	arc.WriteFloat("vobFarPlaneZ", vobFarPlaneZ);
	arc.WriteFloat("innerRangePerc", innerRangePerc);
}

bool32 zCVobSound::Unarchive(zCArchiver &arc)
{
	if (!zCZone::Unarchive(arc)) return FALSE;

	arc.ReadFloat("sndVolume", sndVolume);
	arc.ReadEnum("sndMode", sndMode);
	arc.ReadFloat("sndRandDelay", sndRandDelay);
	arc.ReadFloat("sndRandDelayVar", sndRandDelayVar);
	arc.ReadBool("sndStartOn", sndStartOn);
	arc.ReadBool("sndAmbient3D", sndAmbient3D);
	arc.ReadBool("sndObstruction", sndObstruction);
	arc.ReadFloat("sndConeAngle", sndConeAngle);
	arc.ReadEnum("sndVolType", sndVolType);
	arc.ReadFloat("sndRadius", sndRadius);
	arc.ReadString("sndName", sndName);

	return TRUE;
}

void zCVobSound::Archive(zCArchiver &arc)
{
	zCZone::Archive(arc);

	arc.WriteFloat("sndVolume", sndVolume);
	arc.WriteEnum("sndMode", sndMode);
	arc.WriteFloat("sndRandDelay", sndRandDelay);
	arc.WriteFloat("sndRandDelayVar", sndRandDelayVar);
	arc.WriteBool("sndStartOn", sndStartOn);
	arc.WriteBool("sndAmbient3D", sndAmbient3D);
	arc.WriteBool("sndObstruction", sndObstruction);
	arc.WriteFloat("sndConeAngle", sndConeAngle);
	arc.WriteEnum("sndVolType", sndVolType);
	arc.WriteFloat("sndRadius", sndRadius);
	arc.WriteString("sndName", sndName);
}

bool32 zCVobSoundDaytime::Unarchive(zCArchiver &arc)
{
	if (!zCVobSound::Unarchive(arc)) return FALSE;

	arc.ReadFloat("sndStartTime", sndStartTime);
	arc.ReadFloat("sndEndTime", sndEndTime);
	arc.ReadString("sndName2", sndName2);

	return TRUE;
}

void zCVobSoundDaytime::Archive(zCArchiver &arc)
{
	zCVobSound::Archive(arc);

	arc.WriteFloat("sndStartTime", sndStartTime);
	arc.WriteFloat("sndEndTime", sndEndTime);
	arc.WriteString("sndName2", sndName2);
}

bool32 zCZoneReverb::Unarchive(zCArchiver &arc)
{
	if (!zCZone::Unarchive(arc)) return FALSE;

	arc.ReadEnum("reverbPreset", reverbPreset);
	arc.ReadFloat("reverbWeight", reverbWeight);
	arc.ReadFloat("innerRangePerc", innerRangePerc);

	return TRUE;
}

void zCZoneReverb::Archive(zCArchiver &arc)
{
	zCZone::Archive(arc);

	arc.WriteEnum("reverbPreset", reverbPreset);
	arc.WriteFloat("reverbWeight", reverbWeight);
	arc.WriteFloat("innerRangePerc", innerRangePerc);
}

bool32 oCZoneMusic::Unarchive(zCArchiver &arc)
{
	if (!zCZoneMusic::Unarchive(arc)) return FALSE;

	arc.ReadBool("enabled", enabled);
	arc.ReadInt("priority", priority);
	arc.ReadBool("ellipsoid", ellipsoid);
	arc.ReadFloat("reverbLevel", reverbLevel);
	arc.ReadFloat("volumeLevel", volumeLevel);
	arc.ReadBool("loop", loop);

	return TRUE;
}

void oCZoneMusic::Archive(zCArchiver &arc)
{
	zCZoneMusic::Archive(arc);

	arc.WriteBool("enabled", enabled);
	arc.WriteInt("priority", priority);
	arc.WriteBool("ellipsoid", ellipsoid);
	arc.WriteFloat("reverbLevel", reverbLevel);
	arc.WriteFloat("volumeLevel", volumeLevel);
	arc.WriteBool("loop", loop);
}

bool32 oCObjectGenerator::Unarchive(zCArchiver &arc)
{
	if (!zCVob::Unarchive(arc)) return FALSE;

	arc.ReadString("objectName", objectName);
	arc.ReadFloat("objectSpeed", objectSpeed);

	return TRUE;
}

void oCObjectGenerator::Archive(zCArchiver &arc)
{
	zCVob::Archive(arc);

	arc.WriteString("objectName", objectName);
	arc.WriteFloat("objectSpeed", objectSpeed);
}

bool32 oCItem::Unarchive(zCArchiver &arc)
{
	if (!oCVob::Unarchive(arc)) return FALSE;

	arc.ReadString("itemInstance", itemInstance);

	return TRUE;
}

void oCItem::Archive(zCArchiver &arc)
{
	oCVob::Archive(arc);

	arc.WriteString("itemInstance", itemInstance);
}

bool32 oCMOB::Unarchive(zCArchiver &arc)
{
	if (!oCVob::Unarchive(arc)) return FALSE;

	arc.ReadString("focusName", focusName);
	arc.ReadInt("hitpoints", hitpoints);
	arc.ReadInt("damage", damage);
	arc.ReadBool("moveable", moveable);
	arc.ReadBool("takeable", takeable);
	arc.ReadBool("focusOverride", focusOverride);
	arc.ReadEnum("soundMaterial", soundMaterial);
	arc.ReadString("visualDestroyed", visualDestroyed);
	arc.ReadString("owner", owner);
	arc.ReadString("ownerGuild", ownerGuild);
	arc.ReadBool("isDestroyed", isDestroyed);

	return TRUE;
}

void oCMOB::Archive(zCArchiver &arc)
{
	oCVob::Archive(arc);

	arc.WriteString("focusName", focusName);
	arc.WriteInt("hitpoints", hitpoints);
	arc.WriteInt("damage", damage);
	arc.WriteBool("moveable", moveable);
	arc.WriteBool("takeable", takeable);
	arc.WriteBool("focusOverride", focusOverride);
	arc.WriteEnum("soundMaterial", soundMaterial);
	arc.WriteString("visualDestroyed", visualDestroyed);
	arc.WriteString("owner", owner);
	arc.WriteString("ownerGuild", ownerGuild);
	arc.WriteBool("isDestroyed", isDestroyed);
}

bool32 oCMobInter::Unarchive(zCArchiver &arc)
{
	if (!oCMOB::Unarchive(arc)) return FALSE;

	if (meshAndBspVersionIn == BSPMESH_VERSION_GOTHIC_1_01)
	{
		arc.ReadInt("state", state);
		arc.ReadInt("stateTarget", stateTarget);
	}
	else
	{
		state = 0;
		stateTarget = 0;
	}

	arc.ReadInt("stateNum", stateNum);
	arc.ReadString("triggerTarget", triggerTarget);
	arc.ReadString("useWithItem", useWithItem);
	arc.ReadString("conditionFunc", conditionFunc);
	arc.ReadString("onStateFunc", onStateFunc);
	arc.ReadBool("rewind", rewind);

	return TRUE;
}

void oCMobInter::Archive(zCArchiver &arc)
{
	oCMOB::Archive(arc);

	if (meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_01)
	{
		arc.WriteInt("state", state);
		arc.WriteInt("stateTarget", stateTarget);
	}

	arc.WriteInt("stateNum", stateNum);
	arc.WriteString("triggerTarget", triggerTarget);
	arc.WriteString("useWithItem", useWithItem);
	arc.WriteString("conditionFunc", conditionFunc);
	arc.WriteString("onStateFunc", onStateFunc);
	arc.WriteBool("rewind", rewind);
}

bool32 oCMobFire::Unarchive(zCArchiver &arc)
{
	if (!oCMobInter::Unarchive(arc)) return FALSE;

	arc.ReadString("fireSlot", fireSlot);
	arc.ReadString("fireVobtreeName", fireVobtreeName);

	return TRUE;
}

void oCMobFire::Archive(zCArchiver &arc)
{
	oCMobInter::Archive(arc);

	arc.WriteString("fireSlot", fireSlot);
	arc.WriteString("fireVobtreeName", fireVobtreeName);
}

bool32 oCMobItemSlot::Unarchive(zCArchiver &arc)
{
	if (!oCMobInter::Unarchive(arc)) return FALSE;
	
	arc.ReadBool("itemRemoveable", itemRemoveable);

	return TRUE;
}

void oCMobItemSlot::Archive(zCArchiver &arc)
{
	oCMobInter::Archive(arc);
	
	arc.WriteBool("itemRemoveable", itemRemoveable);
}

bool32 oCMobLockable::Unarchive(zCArchiver &arc)
{
	if (!oCMobInter::Unarchive(arc)) return FALSE;
	
	arc.ReadBool("locked", locked);
	arc.ReadString("keyInstance", keyInstance);
	arc.ReadString("pickLockStr", pickLockStr);

	return TRUE;
}

void oCMobLockable::Archive(zCArchiver &arc)
{
	oCMobInter::Archive(arc);
	
	arc.WriteBool("locked", locked);
	arc.WriteString("keyInstance", keyInstance);
	arc.WriteString("pickLockStr", pickLockStr);
}

bool32 oCMobContainer::Unarchive(zCArchiver &arc)
{
	if (!oCMobLockable::Unarchive(arc)) return FALSE;
	
	arc.ReadString("contains", contains);

	return TRUE;
}

void oCMobContainer::Archive(zCArchiver &arc)
{
	oCMobLockable::Archive(arc);
	
	arc.WriteString("contains", contains);
}

bool32 oCNpc::Unarchive(zCArchiver &arc)
{
	if (!oCVob::Unarchive(arc)) return FALSE;
	
	arc.ReadString("npcInstance", npcInstance);

	return TRUE;
}

void oCNpc::Archive(zCArchiver &arc)
{
	oCVob::Archive(arc);
	
	arc.WriteString("npcInstance", npcInstance);
}
