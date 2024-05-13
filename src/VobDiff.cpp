#include "Vob.h"

#define SPECIAL_FLAKY_VOB_EPSILON // allow a more lenient epsilon for some vob types known to be extra flaky

void zCVob::CalcHash()
{
	zCObject::CalcHash();

	hash = XXH64(presetName.ToChar(), presetName.Length(), hash);

	// hash = XXH64(&bbox3DWS, sizeof(bbox3DWS), hash); // not useful, because hash now is a sort of identifier, but is hashed for zCZone
	hash = XXH64(&trafoOSToWSRot, sizeof(trafoOSToWSRot), hash);
	hash = XXH64(&trafoOSToWSPos, sizeof(trafoOSToWSPos), hash);

	hash = XXH64(vobName.ToChar(), vobName.Length(), hash);
	hash = XXH64(visualName.ToChar(), visualName.Length(), hash);
	hash = XXH64(&showVisual, sizeof(showVisual), hash);
	hash = XXH64(&visualCamAlign, sizeof(visualCamAlign), hash);

	hash = XXH64(&visualAniMode, sizeof(visualAniMode), hash);
	hash = XXH64(&visualAniModeStrength, sizeof(visualAniModeStrength), hash);
	hash = XXH64(&vobFarClipZScale, sizeof(vobFarClipZScale), hash);

	hash = XXH64(&cdStatic, sizeof(cdStatic), hash);
	hash = XXH64(&cdDyn, sizeof(cdDyn), hash);
	hash = XXH64(&staticVob, sizeof(staticVob), hash);
	hash = XXH64(&dynShadow, sizeof(dynShadow), hash);

	hash = XXH64(&zbias, sizeof(zbias), hash);
	hash = XXH64(&isAmbient, sizeof(isAmbient), hash);
}

void zCVob::CalcID()
{
	zCObject::CalcID();

	id = XXH32(presetName.ToChar(), presetName.Length(), id);

	// TODO bbox needs to also be checked for triggers, and potentially other vobs?
	// zCZone
	// zCTriggerBase, zCTrigger?
	// zCTouchDamage?
	// zCTouchAnimate?
	// id = XXH32(&bbox3DWS, sizeof(bbox3DWS), id); // not useful, because id now is a sort of identifier, but is hashed for zCZone
	id = XXH32(&trafoOSToWSRot, sizeof(trafoOSToWSRot), id);
	id = XXH32(&trafoOSToWSPos, sizeof(trafoOSToWSPos), id);

	id = XXH32(vobName.ToChar(), vobName.Length(), id);
	id = XXH32(visualName.ToChar(), visualName.Length(), id);
	id = XXH32(&showVisual, sizeof(showVisual), id);
	id = XXH32(&visualCamAlign, sizeof(visualCamAlign), id);

	id = XXH32(&visualAniMode, sizeof(visualAniMode), id);
	id = XXH32(&visualAniModeStrength, sizeof(visualAniModeStrength), id);
	id = XXH32(&vobFarClipZScale, sizeof(vobFarClipZScale), id);

	id = XXH32(&cdStatic, sizeof(cdStatic), id);
	id = XXH32(&cdDyn, sizeof(cdDyn), id);
	id = XXH32(&staticVob, sizeof(staticVob), id);
	id = XXH32(&dynShadow, sizeof(dynShadow), id);

	id = XXH32(&zbias, sizeof(zbias), id);
	id = XXH32(&isAmbient, sizeof(isAmbient), id);
}

bool32 zCVob::IsEqual(zCObject *obj)
{
	if (!zCObject::IsEqual(obj)) return FALSE;

	zCVob *vob = (zCVob *)obj;

	if (presetName != vob->presetName) return FALSE;

	// if (bbox3DWS != vob->bbox3DWS) return FALSE; // this value varies way too much on i.e. animated objects
	// if (trafoOSToWSRot != vob->trafoOSToWSRot) return FALSE;
	// if (trafoOSToWSPos != vob->trafoOSToWSPos) return FALSE;

	if (vobName != vob->vobName) return FALSE;
	if (visualName != vob->visualName) return FALSE;
	if (showVisual != vob->showVisual) return FALSE;

	if (visualCamAlign != vob->visualCamAlign) return FALSE;

	if (visualAniMode != vob->visualAniMode) return FALSE;
	if (visualAniModeStrength != vob->visualAniModeStrength) return FALSE;
	if (vobFarClipZScale != vob->vobFarClipZScale) return FALSE;

	if (cdStatic != vob->cdStatic) return FALSE;
	if (cdDyn != vob->cdDyn) return FALSE;
	if (staticVob != vob->staticVob) return FALSE;
	if (dynShadow != vob->dynShadow) return FALSE;

	int32 save_zbias = zbias;

	if (gothicVersionOut >= GOTHIC_VERSION_130)
	{
		// originally done in Unarchive
		float minExtend = bbox3DWS.GetMinExtent();
		if (minExtend > VOB_IGNORE_MIN_EXTEND) zbias = 0;
	}

	if (zbias != vob->zbias) return FALSE;

	zbias = save_zbias;

	if (isAmbient != vob->isAmbient) return FALSE;

	if (trafoOSToWSRot != vob->trafoOSToWSRot || trafoOSToWSPos != vob->trafoOSToWSPos)
	{
		float epsilon = 0.1f;

#ifdef SPECIAL_FLAKY_VOB_EPSILON
		if (visualName == "FIRE_SMOKE.pfx" && vob->visualName == "FIRE_SMOKE.pfx")
		{
			epsilon = 1.0f;
		}
#endif

		if (trafoOSToWSRot != vob->trafoOSToWSRot)
		{
			zMAT3 diff = trafoOSToWSRot - vob->trafoOSToWSRot;

			if (fabs(diff[0][VX]) >= epsilon || fabs(diff[0][VY]) >= epsilon || fabs(diff[0][VZ]) >= epsilon
			&& fabs(diff[1][VX]) >= epsilon || fabs(diff[1][VY]) >= epsilon || fabs(diff[1][VZ]) >= epsilon
			&& fabs(diff[2][VX]) >= epsilon || fabs(diff[2][VY]) >= epsilon || fabs(diff[2][VZ]) >= epsilon)
			{
				return FALSE;
			}
		}

		if (trafoOSToWSPos != vob->trafoOSToWSPos)
		{
			zPOINT3 diff = trafoOSToWSPos - vob->trafoOSToWSPos;

			if (fabs(diff[VX]) >= epsilon || fabs(diff[VY]) >= epsilon || fabs(diff[VZ]) >= epsilon)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

void zCCamTrj_KeyFrame::CalcHash()
{
	zCVob::CalcHash();

	hash = XXH64(&time, sizeof(time), hash);
	hash = XXH64(&angleRollDeg, sizeof(angleRollDeg), hash);
	hash = XXH64(&camFOVScale, sizeof(camFOVScale), hash);

	hash = XXH64(&motionType, sizeof(motionType), hash);
	hash = XXH64(&motionTypeFOV, sizeof(motionTypeFOV), hash);
	hash = XXH64(&motionTypeRoll, sizeof(motionTypeRoll), hash);
	hash = XXH64(&motionTypeTimeScale, sizeof(motionTypeTimeScale), hash);

	hash = XXH64(&tension, sizeof(tension), hash);
	hash = XXH64(&bias, sizeof(bias), hash);
	hash = XXH64(&continuity, sizeof(continuity), hash);
	hash = XXH64(&timeScale, sizeof(timeScale), hash);
	hash = XXH64(&timeIsFixed, sizeof(timeIsFixed), hash);

	hash = XXH64(&originalPose, sizeof(originalPose), hash);
}

void zCCamTrj_KeyFrame::CalcID()
{
	zCVob::CalcID();

	id = XXH32(&time, sizeof(time), id);
	id = XXH32(&angleRollDeg, sizeof(angleRollDeg), id);
	id = XXH32(&camFOVScale, sizeof(camFOVScale), id);

	id = XXH32(&motionType, sizeof(motionType), id);
	id = XXH32(&motionTypeFOV, sizeof(motionTypeFOV), id);
	id = XXH32(&motionTypeRoll, sizeof(motionTypeRoll), id);
	id = XXH32(&motionTypeTimeScale, sizeof(motionTypeTimeScale), id);

	id = XXH32(&tension, sizeof(tension), id);
	id = XXH32(&bias, sizeof(bias), id);
	id = XXH32(&continuity, sizeof(continuity), id);
	id = XXH32(&timeScale, sizeof(timeScale), id);
	id = XXH32(&timeIsFixed, sizeof(timeIsFixed), id);

	id = XXH32(&originalPose, sizeof(originalPose), id);
}

bool32 zCCamTrj_KeyFrame::IsEqual(zCObject *obj)
{
	if (!zCVob::IsEqual(obj)) return FALSE;

	zCCamTrj_KeyFrame *vob = (zCCamTrj_KeyFrame *)obj;

	if (time != vob->time) return FALSE;
	if (angleRollDeg != vob->angleRollDeg) return FALSE;
	if (camFOVScale != vob->camFOVScale) return FALSE;

	if (motionType != vob->motionType) return FALSE;
	if (motionTypeFOV != vob->motionTypeFOV) return FALSE;
	if (motionTypeRoll != vob->motionTypeRoll) return FALSE;
	if (motionTypeTimeScale != vob->motionTypeTimeScale) return FALSE;

	if (tension != vob->tension) return FALSE;
	if (bias != vob->bias) return FALSE;
	if (continuity != vob->continuity) return FALSE;
	if (timeScale != vob->timeScale) return FALSE;
	if (timeIsFixed != vob->timeIsFixed) return FALSE;

	if (originalPose != vob->originalPose) return FALSE;

	return TRUE;
}

void zCCSCamera::CalcHash()
{
	zCVob::CalcHash();

	hash = XXH64(&camTrjFOR, sizeof(camTrjFOR), hash);
	hash = XXH64(&targetTrjFOR, sizeof(targetTrjFOR), hash);
	hash = XXH64(&loopMode, sizeof(loopMode), hash);
	hash = XXH64(&splLerpMode, sizeof(splLerpMode), hash);

	hash = XXH64(&ignoreFORVobRotCam, sizeof(ignoreFORVobRotCam), hash);
	hash = XXH64(&ignoreFORVobRotTarget, sizeof(ignoreFORVobRotTarget), hash);
	hash = XXH64(&adaptToSurroundings, sizeof(adaptToSurroundings), hash);
	hash = XXH64(&easeToFirstKey, sizeof(easeToFirstKey), hash);
	hash = XXH64(&easeFromLastKey, sizeof(easeFromLastKey), hash);
	hash = XXH64(&totalTime, sizeof(totalTime), hash);

	hash = XXH64(autoCamFocusVobName.ToChar(), autoCamFocusVobName.Length(), hash);
	hash = XXH64(&autoCamPlayerMovable, sizeof(autoCamPlayerMovable), hash);
	hash = XXH64(&autoCamUntriggerOnLastKey, sizeof(autoCamUntriggerOnLastKey), hash);
	hash = XXH64(&autoCamUntriggerOnLastKeyDelay, sizeof(autoCamUntriggerOnLastKeyDelay), hash);

	hash = XXH64(&posKeys.numInArray, sizeof(posKeys.numInArray), hash);
	hash = XXH64(&targetKeys.numInArray, sizeof(targetKeys.numInArray), hash);
}

void zCCSCamera::CalcID()
{
	zCVob::CalcID();

	id = XXH32(&camTrjFOR, sizeof(camTrjFOR), id);
	id = XXH32(&targetTrjFOR, sizeof(targetTrjFOR), id);
	id = XXH32(&loopMode, sizeof(loopMode), id);
	id = XXH32(&splLerpMode, sizeof(splLerpMode), id);

	id = XXH32(&ignoreFORVobRotCam, sizeof(ignoreFORVobRotCam), id);
	id = XXH32(&ignoreFORVobRotTarget, sizeof(ignoreFORVobRotTarget), id);
	id = XXH32(&adaptToSurroundings, sizeof(adaptToSurroundings), id);
	id = XXH32(&easeToFirstKey, sizeof(easeToFirstKey), id);
	id = XXH32(&easeFromLastKey, sizeof(easeFromLastKey), id);
	id = XXH32(&totalTime, sizeof(totalTime), id);

	id = XXH32(autoCamFocusVobName.ToChar(), autoCamFocusVobName.Length(), id);
	id = XXH32(&autoCamPlayerMovable, sizeof(autoCamPlayerMovable), id);
	id = XXH32(&autoCamUntriggerOnLastKey, sizeof(autoCamUntriggerOnLastKey), id);
	id = XXH32(&autoCamUntriggerOnLastKeyDelay, sizeof(autoCamUntriggerOnLastKeyDelay), id);

	id = XXH32(&posKeys.numInArray, sizeof(posKeys.numInArray), id);
	id = XXH32(&targetKeys.numInArray, sizeof(targetKeys.numInArray), id);
}

bool32 zCCSCamera::IsEqual(zCObject *obj)
{
	if (!zCVob::IsEqual(obj)) return FALSE;

	zCCSCamera *vob = (zCCSCamera *)obj;

	if (camTrjFOR != vob->camTrjFOR) return FALSE;
	if (targetTrjFOR != vob->targetTrjFOR) return FALSE;
	if (loopMode != vob->loopMode) return FALSE;
	if (splLerpMode != vob->splLerpMode) return FALSE;

	if (ignoreFORVobRotCam != vob->ignoreFORVobRotCam) return FALSE;
	if (ignoreFORVobRotTarget != vob->ignoreFORVobRotTarget) return FALSE;
	if (adaptToSurroundings != vob->adaptToSurroundings) return FALSE;
	if (easeToFirstKey != vob->easeToFirstKey) return FALSE;
	if (easeFromLastKey != vob->easeFromLastKey) return FALSE;
	if (totalTime != vob->totalTime) return FALSE;

	if (autoCamFocusVobName != vob->autoCamFocusVobName) return FALSE;
	if (autoCamPlayerMovable != vob->autoCamPlayerMovable) return FALSE;
	if (autoCamUntriggerOnLastKey != vob->autoCamUntriggerOnLastKey) return FALSE;
	if (autoCamUntriggerOnLastKeyDelay != vob->autoCamUntriggerOnLastKeyDelay) return FALSE;

	if (posKeys.numInArray != vob->posKeys.numInArray) return FALSE;
	if (targetKeys.numInArray != vob->targetKeys.numInArray) return FALSE;

	return TRUE;
}

XXH64_hash_t zCVobLightData::CalcHash(XXH64_hash_t hash)
{
	hash = XXH64(&lightType, sizeof(lightType), hash);
	hash = XXH64(&range, sizeof(range), hash);
	hash = XXH64(&color, sizeof(color), hash);
	hash = XXH64(&spotConeAngle, sizeof(spotConeAngle), hash);
	hash = XXH64(&lightStatic, sizeof(lightStatic), hash);
	hash = XXH64(&lightQuality, sizeof(lightQuality), hash);
	hash = XXH64(lensflareFX.ToChar(), lensflareFX.Length(), hash);

	if (!lightStatic)
	{
		hash = XXH64(&turnedOn, sizeof(turnedOn), hash);

		hash = XXH64(rangeAniScale.array, sizeof(*rangeAniScale.array) * rangeAniScale.numInArray, hash);
		hash = XXH64(&rangeAniFPS, sizeof(rangeAniFPS), hash);
		hash = XXH64(&rangeAniSmooth, sizeof(rangeAniSmooth), hash);

		hash = XXH64(colorAniList.array, sizeof(*colorAniList.array) * colorAniList.numInArray, hash);
		hash = XXH64(&colorAniFPS, sizeof(colorAniFPS), hash);
		hash = XXH64(&colorAniSmooth, sizeof(colorAniSmooth), hash);

		hash = XXH64(&canMove, sizeof(canMove), hash);
	}

	return hash;
}

XXH32_hash_t zCVobLightData::CalcID(XXH32_hash_t id)
{
	id = XXH32(&lightType, sizeof(lightType), id);
	id = XXH32(&range, sizeof(range), id);
	id = XXH32(&color, sizeof(color), id);
	id = XXH32(&spotConeAngle, sizeof(spotConeAngle), id);
	id = XXH32(&lightStatic, sizeof(lightStatic), id);
	id = XXH32(&lightQuality, sizeof(lightQuality), id);
	id = XXH32(lensflareFX.ToChar(), lensflareFX.Length(), id);

	if (!lightStatic)
	{
		id = XXH32(&turnedOn, sizeof(turnedOn), id);

		id = XXH32(rangeAniScale.array, sizeof(*rangeAniScale.array) * rangeAniScale.numInArray, id);
		id = XXH32(&rangeAniFPS, sizeof(rangeAniFPS), id);
		id = XXH32(&rangeAniSmooth, sizeof(rangeAniSmooth), id);

		id = XXH32(colorAniList.array, sizeof(*colorAniList.array) * colorAniList.numInArray, id);
		id = XXH32(&colorAniFPS, sizeof(colorAniFPS), id);
		id = XXH32(&colorAniSmooth, sizeof(colorAniSmooth), id);

		id = XXH32(&canMove, sizeof(canMove), id);
	}

	return id;
}

bool32 zCVobLightData::IsEqual(zCVobLightData &lightData)
{
	if (lightType != lightData.lightType) return FALSE;
	// if (color != lightData.color) return FALSE;
	if (range != lightData.range) return FALSE;

	if (spotConeAngle != lightData.spotConeAngle) return FALSE;
	if (lightStatic != lightData.lightStatic) return FALSE;
	if (lightQuality != lightData.lightQuality) return FALSE;
	if (lensflareFX != lightData.lensflareFX) return FALSE;

	if (!lightStatic)
	{
		if (turnedOn != lightData.turnedOn) return FALSE;

		if (rangeAniScale != lightData.rangeAniScale) return FALSE;
		if (rangeAniFPS != lightData.rangeAniFPS) return FALSE;
		if (rangeAniSmooth != lightData.rangeAniSmooth) return FALSE;

		if (colorAniList != lightData.colorAniList) return FALSE;
		// if (colorAniFPS != lightData.colorAniFPS) return FALSE;
		if (colorAniSmooth != lightData.colorAniSmooth) return FALSE;

		if (canMove != lightData.canMove) return FALSE;

		// special handling for colorAniFPS, which can be almost equal
		if (colorAniFPS != lightData.colorAniFPS)
		{
			if (fabs(colorAniFPS - lightData.colorAniFPS) >= 0.0001f) return FALSE;
		}
	}

	// special handling for color
	// which can be a an interpolated value of colorAniList at the moment of saving when light is not static
	// so we check if the light is animated, which is true if colorAniFPS > 0.0f and colorAniList not empty
	// all of these values have already been verified to be equal
	if (color != lightData.color)
	{
		if (lightStatic) return FALSE;

		if (colorAniFPS <= 0.0f) return FALSE;

		if (colorAniList.IsEmpty()) return FALSE;
	}

	return TRUE;
}

void zCVobLight::CalcHash()
{
	zCVob::CalcHash();

	hash = XXH64(lightPresetInUse.ToChar(), lightPresetInUse.Length(), hash);

	hash = lightData.CalcHash(hash);
}

void zCVobLight::CalcID()
{
	zCVob::CalcID();

	id = XXH32(lightPresetInUse.ToChar(), lightPresetInUse.Length(), id);

	id = lightData.CalcID(id);
}

bool32 zCVobLight::IsEqual(zCObject *obj)
{
	if (!zCVob::IsEqual(obj)) return FALSE;

	zCVobLight *vob = (zCVobLight *)obj;

	if (lightPresetInUse != vob->lightPresetInUse) return FALSE;

	if (!lightData.IsEqual(vob->lightData)) return FALSE;

	return TRUE;
}

void zCTriggerBase::CalcHash()
{
	zCVob::CalcHash();

	hash = XXH64(triggerTarget.ToChar(), triggerTarget.Length(), hash);
}

void zCTriggerBase::CalcID()
{
	zCVob::CalcID();

	id = XXH32(triggerTarget.ToChar(), triggerTarget.Length(), id);
}

bool32 zCTriggerBase::IsEqual(zCObject *obj)
{
	if (!zCVob::IsEqual(obj)) return FALSE;

	zCTriggerBase *vob = (zCTriggerBase *)obj;

	if (triggerTarget != vob->triggerTarget) return FALSE;

	return TRUE;
}

void zCCodeMaster::CalcHash()
{
	zCTriggerBase::CalcHash();

	hash = XXH64(&flags.orderRelevant, sizeof(flags.orderRelevant), hash);
	hash = XXH64(&flags.firstFalseIsFailure, sizeof(flags.firstFalseIsFailure), hash);
	hash = XXH64(triggerTargetFailure.ToChar(), triggerTargetFailure.Length(), hash);
	hash = XXH64(&flags.untriggerCancels, sizeof(flags.untriggerCancels), hash);

	hash = XXH64(&numSlaves, sizeof(numSlaves), hash);

	for (int32 i = 0; i < numSlaves; i++)
	{
		hash = XXH64(slaveVobNameList[i].ToChar(), slaveVobNameList[i].Length(), hash);
	}
}

void zCCodeMaster::CalcID()
{
	zCTriggerBase::CalcID();

	id = XXH32(&flags.orderRelevant, sizeof(flags.orderRelevant), id);
	id = XXH32(&flags.firstFalseIsFailure, sizeof(flags.firstFalseIsFailure), id);
	id = XXH32(triggerTargetFailure.ToChar(), triggerTargetFailure.Length(), id);
	id = XXH32(&flags.untriggerCancels, sizeof(flags.untriggerCancels), id);

	id = XXH32(&numSlaves, sizeof(numSlaves), id);

	for (int32 i = 0; i < numSlaves; i++)
	{
		id = XXH32(slaveVobNameList[i].ToChar(), slaveVobNameList[i].Length(), id);
	}
}

bool32 zCCodeMaster::IsEqual(zCObject *obj)
{
	if (!zCTriggerBase::IsEqual(obj)) return FALSE;

	zCCodeMaster *vob = (zCCodeMaster *)obj;

	if (flags.orderRelevant != vob->flags.orderRelevant) return FALSE;
	if (flags.firstFalseIsFailure != vob->flags.firstFalseIsFailure) return FALSE;
	if (triggerTargetFailure != triggerTargetFailure) return FALSE;
	if (flags.untriggerCancels != vob->flags.untriggerCancels) return FALSE;

	if (numSlaves != numSlaves) return FALSE;

	for (int32 i = 0; i < numSlaves; i++)
	{
		if (slaveVobNameList[i] != vob->slaveVobNameList[i]) return FALSE;
	}

	return TRUE;
}

void zCMessageFilter::CalcHash()
{
	zCTriggerBase::CalcHash();

	hash = XXH64(&onTrigger, sizeof(onTrigger), hash);
	hash = XXH64(&onUntrigger, sizeof(onUntrigger), hash);
}

void zCMessageFilter::CalcID()
{
	zCTriggerBase::CalcID();

	id = XXH32(&onTrigger, sizeof(onTrigger), id);
	id = XXH32(&onUntrigger, sizeof(onUntrigger), id);
}

bool32 zCMessageFilter::IsEqual(zCObject *obj)
{
	if (!zCTriggerBase::IsEqual(obj)) return FALSE;

	zCMessageFilter *vob = (zCMessageFilter *)obj;

	if (onTrigger != vob->onTrigger) return FALSE;
	if (onUntrigger != vob->onUntrigger) return FALSE;

	return TRUE;
}

void zCMoverControler::CalcHash()
{
	zCTriggerBase::CalcHash();

	hash = XXH64(&moverMessage, sizeof(moverMessage), hash);
	hash = XXH64(&gotoFixedKey, sizeof(gotoFixedKey), hash);
}

void zCMoverControler::CalcID()
{
	zCTriggerBase::CalcID();

	id = XXH32(&moverMessage, sizeof(moverMessage), id);
	id = XXH32(&gotoFixedKey, sizeof(gotoFixedKey), id);
}

bool32 zCMoverControler::IsEqual(zCObject *obj)
{
	if (!zCTriggerBase::IsEqual(obj)) return FALSE;

	zCMoverControler *vob = (zCMoverControler *)obj;

	if (moverMessage != vob->moverMessage) return FALSE;
	if (gotoFixedKey != vob->gotoFixedKey) return FALSE;

	return TRUE;
}

void zCTriggerWorldStart::CalcHash()
{
	zCTriggerBase::CalcHash();

	hash = XXH64(&fireOnlyFirstTime, sizeof(fireOnlyFirstTime), hash);
}

void zCTriggerWorldStart::CalcID()
{
	zCTriggerBase::CalcID();

	id = XXH32(&fireOnlyFirstTime, sizeof(fireOnlyFirstTime), id);
}

bool32 zCTriggerWorldStart::IsEqual(zCObject *obj)
{
	if (!zCTriggerBase::IsEqual(obj)) return FALSE;

	zCTriggerWorldStart *vob = (zCTriggerWorldStart *)obj;

	if (fireOnlyFirstTime != vob->fireOnlyFirstTime) return FALSE;

	return TRUE;
}

void zCTrigger::CalcHash()
{
	zCTriggerBase::CalcHash();

	hash = XXH64(&flags.startEnabled, sizeof(flags.startEnabled), hash);
	hash = XXH64(&flags.isEnabled, sizeof(flags.isEnabled), hash);
	hash = XXH64(&flags.sendUntrigger, sizeof(flags.sendUntrigger), hash);

	hash = XXH64(&filterFlags.reactToOnTrigger, sizeof(filterFlags.reactToOnTrigger), hash);
	hash = XXH64(&filterFlags.reactToOnTouch, sizeof(filterFlags.reactToOnTouch), hash);
	hash = XXH64(&filterFlags.reactToOnDamage, sizeof(filterFlags.reactToOnDamage), hash);
	hash = XXH64(&filterFlags.respondToObject, sizeof(filterFlags.respondToObject), hash);
	hash = XXH64(&filterFlags.respondToPC, sizeof(filterFlags.respondToPC), hash);
	hash = XXH64(&filterFlags.respondToNPC, sizeof(filterFlags.respondToNPC), hash);

	hash = XXH64(respondToVobName.ToChar(), respondToVobName.Length(), hash);
	hash = XXH64(&numCanBeActivated, sizeof(numCanBeActivated), hash);
	hash = XXH64(&retriggerWaitSec, sizeof(retriggerWaitSec), hash);
	hash = XXH64(&damageThreshold, sizeof(damageThreshold), hash);

	hash = XXH64(&fireDelaySec, sizeof(fireDelaySec), hash);
}

void zCTrigger::CalcID()
{
	zCTriggerBase::CalcID();

	id = XXH32(&flags.startEnabled, sizeof(flags.startEnabled), id);
	id = XXH32(&flags.isEnabled, sizeof(flags.isEnabled), id);
	id = XXH32(&flags.sendUntrigger, sizeof(flags.sendUntrigger), id);

	id = XXH32(&filterFlags.reactToOnTrigger, sizeof(filterFlags.reactToOnTrigger), id);
	id = XXH32(&filterFlags.reactToOnTouch, sizeof(filterFlags.reactToOnTouch), id);
	id = XXH32(&filterFlags.reactToOnDamage, sizeof(filterFlags.reactToOnDamage), id);
	id = XXH32(&filterFlags.respondToObject, sizeof(filterFlags.respondToObject), id);
	id = XXH32(&filterFlags.respondToPC, sizeof(filterFlags.respondToPC), id);
	id = XXH32(&filterFlags.respondToNPC, sizeof(filterFlags.respondToNPC), id);

	id = XXH32(respondToVobName.ToChar(), respondToVobName.Length(), id);
	id = XXH32(&numCanBeActivated, sizeof(numCanBeActivated), id);
	id = XXH32(&retriggerWaitSec, sizeof(retriggerWaitSec), id);
	id = XXH32(&damageThreshold, sizeof(damageThreshold), id);

	id = XXH32(&fireDelaySec, sizeof(fireDelaySec), id);
}

bool32 zCTrigger::IsEqual(zCObject *obj)
{
	if (!zCTriggerBase::IsEqual(obj)) return FALSE;

	zCTrigger *vob = (zCTrigger *)obj;

	if (flags.startEnabled != vob->flags.startEnabled) return FALSE;
	if (flags.isEnabled != vob->flags.isEnabled) return FALSE;
	if (flags.sendUntrigger != vob->flags.sendUntrigger) return FALSE;

	if (filterFlags.reactToOnTrigger != vob->filterFlags.reactToOnTrigger) return FALSE;
	if (filterFlags.reactToOnTouch != vob->filterFlags.reactToOnTouch) return FALSE;
	if (filterFlags.reactToOnDamage != vob->filterFlags.reactToOnDamage) return FALSE;
	if (filterFlags.respondToObject != vob->filterFlags.respondToObject) return FALSE;
	if (filterFlags.respondToPC != vob->filterFlags.respondToPC) return FALSE;
	if (filterFlags.respondToNPC != vob->filterFlags.respondToNPC) return FALSE;

	if (respondToVobName != vob->respondToVobName) return FALSE;
	if (numCanBeActivated != vob->numCanBeActivated) return FALSE;
	if (retriggerWaitSec != vob->retriggerWaitSec) return FALSE;
	if (damageThreshold != vob->damageThreshold) return FALSE;

	if (fireDelaySec != vob->fireDelaySec) return FALSE;

	return TRUE;
}

void zCTriggerTeleport::CalcHash()
{
	zCTrigger::CalcHash();

	hash = XXH64(sfxTeleport.ToChar(), sfxTeleport.Length(), hash);
}

void zCTriggerTeleport::CalcID()
{
	zCTrigger::CalcID();

	id = XXH32(sfxTeleport.ToChar(), sfxTeleport.Length(), id);
}

bool32 zCTriggerTeleport::IsEqual(zCObject *obj)
{
	if (!zCTrigger::IsEqual(obj)) return FALSE;

	zCTriggerTeleport *vob = (zCTriggerTeleport *)obj;

	if (sfxTeleport != vob->sfxTeleport) return FALSE;

	return TRUE;
}

void zCMover::CalcHash()
{
	zCTrigger::CalcHash();

	hash = XXH64(&moverBehavior, sizeof(moverBehavior), hash);
	hash = XXH64(&touchBlockerDamage, sizeof(touchBlockerDamage), hash);
	hash = XXH64(&stayOpenTimeSec, sizeof(stayOpenTimeSec), hash);
	hash = XXH64(&moverLocked, sizeof(moverLocked), hash);
	hash = XXH64(&autoLinkEnabled, sizeof(autoLinkEnabled), hash);

	hash = XXH64(&autoRotate, sizeof(autoRotate), hash);

	hash = XXH64(&numKeyframes, sizeof(numKeyframes), hash);

	if (numKeyframes > 0)
	{
		hash = XXH64(&moveSpeed, sizeof(moveSpeed), hash);
		hash = XXH64(&posLerpType, sizeof(posLerpType), hash);
		hash = XXH64(&speedType, sizeof(speedType), hash);

		hash = XXH64(keyframes, sizeof(*keyframes) * numKeyframes, hash);
	}

	hash = XXH64(sfxOpenStart.ToChar(), sfxOpenStart.Length(), hash);
	hash = XXH64(sfxOpenEnd.ToChar(), sfxOpenEnd.Length(), hash);
	hash = XXH64(sfxMoving.ToChar(), sfxMoving.Length(), hash);
	hash = XXH64(sfxCloseStart.ToChar(), sfxCloseStart.Length(), hash);
	hash = XXH64(sfxCloseEnd.ToChar(), sfxCloseEnd.Length(), hash);
	hash = XXH64(sfxLock.ToChar(), sfxLock.Length(), hash);
	hash = XXH64(sfxUnlock.ToChar(), sfxUnlock.Length(), hash);
	hash = XXH64(sfxUseLocked.ToChar(), sfxUseLocked.Length(), hash);
}

void zCMover::CalcID()
{
	zCTrigger::CalcID();

	id = XXH32(&moverBehavior, sizeof(moverBehavior), id);
	id = XXH32(&touchBlockerDamage, sizeof(touchBlockerDamage), id);
	id = XXH32(&stayOpenTimeSec, sizeof(stayOpenTimeSec), id);
	id = XXH32(&moverLocked, sizeof(moverLocked), id);
	id = XXH32(&autoLinkEnabled, sizeof(autoLinkEnabled), id);

	id = XXH32(&autoRotate, sizeof(autoRotate), id);

	id = XXH32(&numKeyframes, sizeof(numKeyframes), id);

	if (numKeyframes > 0)
	{
		id = XXH32(&moveSpeed, sizeof(moveSpeed), id);
		id = XXH32(&posLerpType, sizeof(posLerpType), id);
		id = XXH32(&speedType, sizeof(speedType), id);

		id = XXH32(keyframes, sizeof(*keyframes) * numKeyframes, id);
	}

	id = XXH32(sfxOpenStart.ToChar(), sfxOpenStart.Length(), id);
	id = XXH32(sfxOpenEnd.ToChar(), sfxOpenEnd.Length(), id);
	id = XXH32(sfxMoving.ToChar(), sfxMoving.Length(), id);
	id = XXH32(sfxCloseStart.ToChar(), sfxCloseStart.Length(), id);
	id = XXH32(sfxCloseEnd.ToChar(), sfxCloseEnd.Length(), id);
	id = XXH32(sfxLock.ToChar(), sfxLock.Length(), id);
	id = XXH32(sfxUnlock.ToChar(), sfxUnlock.Length(), id);
	id = XXH32(sfxUseLocked.ToChar(), sfxUseLocked.Length(), id);
}

bool32 zCMover::IsEqual(zCObject *obj)
{
	if (!zCTrigger::IsEqual(obj)) return FALSE;

	zCMover *vob = (zCMover *)obj;

	if (moverBehavior != vob->moverBehavior) return FALSE;
	if (touchBlockerDamage != vob->touchBlockerDamage) return FALSE;
	if (stayOpenTimeSec != vob->stayOpenTimeSec) return FALSE;
	if (moverLocked != vob->moverLocked) return FALSE;
	if (autoLinkEnabled != vob->autoLinkEnabled) return FALSE;

	if (autoRotate != vob->autoRotate) return FALSE;

	if (numKeyframes != vob->numKeyframes) return FALSE;

	if (numKeyframes > 0)
	{
		if (moveSpeed != vob->moveSpeed) return FALSE;
		if (posLerpType != vob->posLerpType) return FALSE;
		if (speedType != vob->speedType) return FALSE;

		if (!memequal(keyframes, vob->keyframes, sizeof(*keyframes) * numKeyframes)) return FALSE;
	}

	if (sfxOpenStart != vob->sfxOpenStart) return FALSE;
	if (sfxOpenEnd != vob->sfxOpenEnd) return FALSE;
	if (sfxMoving != vob->sfxMoving) return FALSE;
	if (sfxCloseStart != vob->sfxCloseStart) return FALSE;
	if (sfxCloseEnd != vob->sfxCloseEnd) return FALSE;
	if (sfxLock != vob->sfxLock) return FALSE;
	if (sfxUnlock != vob->sfxUnlock) return FALSE;
	if (sfxUseLocked != vob->sfxUseLocked) return FALSE;

	return TRUE;
}

void zCTriggerList::CalcHash()
{
	zCTrigger::CalcHash();

	hash = XXH64(&listProcess, sizeof(listProcess), hash);
	hash = XXH64(&numTarget, sizeof(numTarget), hash);

	for (int32 i = 0; i < numTarget; i++)
	{
		hash = XXH64(triggerTargetList[i].ToChar(), triggerTargetList[i].Length(), hash);
		hash = XXH64(&fireDelayList[i], sizeof(fireDelayList[i]), hash);
	}
}

void zCTriggerList::CalcID()
{
	zCTrigger::CalcID();

	id = XXH32(&listProcess, sizeof(listProcess), id);
	id = XXH32(&numTarget, sizeof(numTarget), id);

	for (int32 i = 0; i < numTarget; i++)
	{
		id = XXH32(triggerTargetList[i].ToChar(), triggerTargetList[i].Length(), id);
		id = XXH32(&fireDelayList[i], sizeof(fireDelayList[i]), id);
	}
}

bool32 zCTriggerList::IsEqual(zCObject *obj)
{
	if (!zCTrigger::IsEqual(obj)) return FALSE;

	zCTriggerList *vob = (zCTriggerList *)obj;

	if (listProcess != vob->listProcess) return FALSE;
	if (numTarget != vob->numTarget) return FALSE;

	for (int32 i = 0; i < numTarget; i++)
	{
		if (triggerTargetList[i] != vob->triggerTargetList[i]) return FALSE;
		if (fireDelayList[i] != vob->fireDelayList[i]) return FALSE;
	}

	return TRUE;
}

void oCTriggerChangeLevel::CalcHash()
{
	zCTrigger::CalcHash();

	hash = XXH64(levelName.ToChar(), levelName.Length(), hash);
	hash = XXH64(startVobName.ToChar(), startVobName.Length(), hash);
}

void oCTriggerChangeLevel::CalcID()
{
	zCTrigger::CalcID();

	id = XXH32(levelName.ToChar(), levelName.Length(), id);
	id = XXH32(startVobName.ToChar(), startVobName.Length(), id);
}

bool32 oCTriggerChangeLevel::IsEqual(zCObject *obj)
{
	if (!zCTrigger::IsEqual(obj)) return FALSE;

	oCTriggerChangeLevel *vob = (oCTriggerChangeLevel *)obj;

	if (levelName != vob->levelName) return FALSE;
	if (startVobName != vob->startVobName) return FALSE;

	return TRUE;
}

void oCTriggerScript::CalcHash()
{
	zCTrigger::CalcHash();

	hash = XXH64(scriptFunc.ToChar(), scriptFunc.Length(), hash);
}

void oCTriggerScript::CalcID()
{
	zCTrigger::CalcID();

	id = XXH32(scriptFunc.ToChar(), scriptFunc.Length(), id);
}

bool32 oCTriggerScript::IsEqual(zCObject *obj)
{
	if (!zCTrigger::IsEqual(obj)) return FALSE;

	oCTriggerScript *vob = (oCTriggerScript *)obj;

	if (scriptFunc != vob->scriptFunc) return FALSE;

	return TRUE;
}

void zCEarthquake::CalcHash()
{
	zCEffect::CalcHash();

	hash = XXH64(&radius, sizeof(radius), hash);
	hash = XXH64(&timeSec, sizeof(timeSec), hash);
	hash = XXH64(&amplitudeCM, sizeof(amplitudeCM), hash);
}

void zCEarthquake::CalcID()
{
	zCEffect::CalcID();

	id = XXH32(&radius, sizeof(radius), id);
	id = XXH32(&timeSec, sizeof(timeSec), id);
	id = XXH32(&amplitudeCM, sizeof(amplitudeCM), id);
}

bool32 zCEarthquake::IsEqual(zCObject *obj)
{
	if (!zCEffect::IsEqual(obj)) return FALSE;

	zCEarthquake *vob = (zCEarthquake *)obj;

	if (radius != vob->radius) return FALSE;
	if (timeSec != vob->timeSec) return FALSE;
	if (amplitudeCM != vob->amplitudeCM) return FALSE;

	return TRUE;
}

void zCPFXControler::CalcHash()
{
	zCEffect::CalcHash();

	hash = XXH64(pfxName.ToChar(), pfxName.Length(), hash);
	hash = XXH64(&killVobWhenDone, sizeof(killVobWhenDone), hash);
	hash = XXH64(&pfxStartOn, sizeof(pfxStartOn), hash);
}

void zCPFXControler::CalcID()
{
	zCEffect::CalcID();

	id = XXH32(pfxName.ToChar(), pfxName.Length(), id);
	id = XXH32(&killVobWhenDone, sizeof(killVobWhenDone), id);
	id = XXH32(&pfxStartOn, sizeof(pfxStartOn), id);
}

bool32 zCPFXControler::IsEqual(zCObject *obj)
{
	if (!zCEffect::IsEqual(obj)) return FALSE;

	zCPFXControler *vob = (zCPFXControler *)obj;

	if (pfxName != vob->pfxName) return FALSE;
	if (killVobWhenDone != vob->killVobWhenDone) return FALSE;
	if (pfxStartOn != vob->pfxStartOn) return FALSE;

	return TRUE;
}

void zCTouchDamage::CalcHash()
{
	zCEffect::CalcHash();

	hash = XXH64(&damage, sizeof(damage), hash);

	hash = XXH64(&damageType.Barrier, sizeof(damageType.Barrier), hash);
	hash = XXH64(&damageType.Blunt, sizeof(damageType.Blunt), hash);
	hash = XXH64(&damageType.Edge, sizeof(damageType.Edge), hash);
	hash = XXH64(&damageType.Fire, sizeof(damageType.Fire), hash);
	hash = XXH64(&damageType.Fly, sizeof(damageType.Fly), hash);
	hash = XXH64(&damageType.Magic, sizeof(damageType.Magic), hash);
	hash = XXH64(&damageType.Point, sizeof(damageType.Point), hash);
	hash = XXH64(&damageType.Fall, sizeof(damageType.Fall), hash);

	hash = XXH64(&damageRepeatDelaySec, sizeof(damageRepeatDelaySec), hash);
	hash = XXH64(&damageVolDownScale, sizeof(damageVolDownScale), hash);
	hash = XXH64(&damageCollType, sizeof(damageCollType), hash);
}

void zCTouchDamage::CalcID()
{
	zCEffect::CalcID();

	id = XXH32(&damage, sizeof(damage), id);

	id = XXH32(&damageType.Barrier, sizeof(damageType.Barrier), id);
	id = XXH32(&damageType.Blunt, sizeof(damageType.Blunt), id);
	id = XXH32(&damageType.Edge, sizeof(damageType.Edge), id);
	id = XXH32(&damageType.Fire, sizeof(damageType.Fire), id);
	id = XXH32(&damageType.Fly, sizeof(damageType.Fly), id);
	id = XXH32(&damageType.Magic, sizeof(damageType.Magic), id);
	id = XXH32(&damageType.Point, sizeof(damageType.Point), id);
	id = XXH32(&damageType.Fall, sizeof(damageType.Fall), id);

	id = XXH32(&damageRepeatDelaySec, sizeof(damageRepeatDelaySec), id);
	id = XXH32(&damageVolDownScale, sizeof(damageVolDownScale), id);
	id = XXH32(&damageCollType, sizeof(damageCollType), id);
}

bool32 zCTouchDamage::IsEqual(zCObject *obj)
{
	if (!zCEffect::IsEqual(obj)) return FALSE;

	zCTouchDamage *vob = (zCTouchDamage *)obj;

	if (damage != vob->damage) return FALSE;

	if (damageType.Barrier != vob->damageType.Barrier) return FALSE;
	if (damageType.Blunt != vob->damageType.Blunt) return FALSE;
	if (damageType.Edge != vob->damageType.Edge) return FALSE;
	if (damageType.Fire != vob->damageType.Fire) return FALSE;
	if (damageType.Fly != vob->damageType.Fly) return FALSE;
	if (damageType.Magic != vob->damageType.Magic) return FALSE;
	if (damageType.Point != vob->damageType.Point) return FALSE;
	if (damageType.Fall != vob->damageType.Fall) return FALSE;

	if (damageRepeatDelaySec != vob->damageRepeatDelaySec) return FALSE;
	if (damageVolDownScale != vob->damageVolDownScale) return FALSE;
	if (damageCollType != vob->damageCollType) return FALSE;

	return TRUE;
}

void zCTouchAnimateSound::CalcHash()
{
	zCTouchAnimate::CalcHash();

	hash = XXH64(sfxTouch.ToChar(), sfxTouch.Length(), hash);
}

void zCTouchAnimateSound::CalcID()
{
	zCTouchAnimate::CalcID();

	id = XXH32(sfxTouch.ToChar(), sfxTouch.Length(), id);
}

bool32 zCTouchAnimateSound::IsEqual(zCObject *obj)
{
	if (!zCTouchAnimate::IsEqual(obj)) return FALSE;

	zCTouchAnimateSound *vob = (zCTouchAnimateSound *)obj;

	if (sfxTouch != vob->sfxTouch) return FALSE;

	return TRUE;
}

void zCVobAnimate::CalcHash()
{
	zCEffect::CalcHash();

	hash = XXH64(&startOn, sizeof(startOn), hash);
}

void zCVobAnimate::CalcID()
{
	zCEffect::CalcID();

	id = XXH32(&startOn, sizeof(startOn), id);
}

bool32 zCVobAnimate::IsEqual(zCObject *obj)
{
	if (!zCEffect::IsEqual(obj)) return FALSE;

	zCVobAnimate *vob = (zCVobAnimate *)obj;

	if (startOn != vob->startOn) return FALSE;

	return TRUE;
}

void zCVobLensFlare::CalcHash()
{
	zCEffect::CalcHash();

	hash = XXH64(lensflareFX.ToChar(), lensflareFX.Length(), hash);
}

void zCVobLensFlare::CalcID()
{
	zCEffect::CalcID();

	id = XXH32(lensflareFX.ToChar(), lensflareFX.Length(), id);
}

bool32 zCVobLensFlare::IsEqual(zCObject *obj)
{
	if (!zCObject::IsEqual(obj)) return FALSE;

	zCVobLensFlare *vob = (zCVobLensFlare *)obj;

	bool32 result = TRUE;
	
	bool32 saveShowVisual1 = showVisual;
	bool32 saveShowVisual2 = vob->showVisual;

	if (gothicVersionOut >= GOTHIC_VERSION_108)
	{
		showVisual = TRUE;
		vob->showVisual = TRUE;
	}

	if (result) result = zCEffect::IsEqual(obj);

	if (result) result = lensflareFX == vob->lensflareFX;

	if (gothicVersionOut >= GOTHIC_VERSION_108)
	{
		showVisual = saveShowVisual1;
		vob->showVisual = saveShowVisual2;
	}

	return result;
}

void zCZone::CalcHash()
{
	zCVob::CalcHash();

	hash = XXH64(&bbox3DWS, sizeof(bbox3DWS), hash);
}

void zCZone::CalcID()
{
	zCVob::CalcID();

	id = XXH32(&bbox3DWS, sizeof(bbox3DWS), id);
}

void zCZoneZFog::CalcHash()
{
	zCZone::CalcHash();

	hash = XXH64(&fogRangeCenter, sizeof(fogRangeCenter), hash);
	hash = XXH64(&innerRangePerc, sizeof(innerRangePerc), hash);
	hash = XXH64(&fogColor, sizeof(fogColor), hash);

	hash = XXH64(&fadeOutSky, sizeof(fadeOutSky), hash);
	hash = XXH64(&overrideColor, sizeof(overrideColor), hash);
}

void zCZoneZFog::CalcID()
{
	zCZone::CalcID();

	id = XXH32(&fogRangeCenter, sizeof(fogRangeCenter), id);
	id = XXH32(&innerRangePerc, sizeof(innerRangePerc), id);
	id = XXH32(&fogColor, sizeof(fogColor), id);

	id = XXH32(&fadeOutSky, sizeof(fadeOutSky), id);
	id = XXH32(&overrideColor, sizeof(overrideColor), id);
}

bool32 zCZoneZFog::IsEqual(zCObject *obj)
{
	if (!zCZone::IsEqual(obj)) return FALSE;

	zCZoneZFog *vob = (zCZoneZFog *)obj;

	if (fogRangeCenter != vob->fogRangeCenter) return FALSE;
	if (innerRangePerc != vob->innerRangePerc) return FALSE;
	if (fogColor != vob->fogColor) return FALSE;

	if (fadeOutSky != vob->fadeOutSky) return FALSE;
	if (overrideColor != vob->overrideColor) return FALSE;

	return TRUE;
}

void zCZoneVobFarPlane::CalcHash()
{
	zCZone::CalcHash();

	hash = XXH64(&vobFarPlaneZ, sizeof(vobFarPlaneZ), hash);
	hash = XXH64(&innerRangePerc, sizeof(innerRangePerc), hash);
}

void zCZoneVobFarPlane::CalcID()
{
	zCZone::CalcID();

	id = XXH32(&vobFarPlaneZ, sizeof(vobFarPlaneZ), id);
	id = XXH32(&innerRangePerc, sizeof(innerRangePerc), id);
}

bool32 zCZoneVobFarPlane::IsEqual(zCObject *obj)
{
	if (!zCZone::IsEqual(obj)) return FALSE;

	zCZoneVobFarPlane *vob = (zCZoneVobFarPlane *)obj;

	if (vobFarPlaneZ != vob->vobFarPlaneZ) return FALSE;
	if (innerRangePerc != vob->innerRangePerc) return FALSE;

	return TRUE;
}

void zCVobSound::CalcHash()
{
	zCZone::CalcHash();

	hash = XXH64(&sndVolume, sizeof(sndVolume), hash);
	hash = XXH64(&sndMode, sizeof(sndMode), hash);
	hash = XXH64(&sndRandDelay, sizeof(sndRandDelay), hash);
	hash = XXH64(&sndRandDelayVar, sizeof(sndRandDelayVar), hash);
	hash = XXH64(&sndStartOn, sizeof(sndStartOn), hash);
	hash = XXH64(&sndAmbient3D, sizeof(sndAmbient3D), hash);
	hash = XXH64(&sndObstruction, sizeof(sndObstruction), hash);
	hash = XXH64(&sndConeAngle, sizeof(sndConeAngle), hash);
	hash = XXH64(&sndVolType, sizeof(sndVolType), hash);
	hash = XXH64(&sndRadius, sizeof(sndRadius), hash);
	hash = XXH64(sndName.ToChar(), sndName.Length(), hash);
}

void zCVobSound::CalcID()
{
	zCZone::CalcID();

	id = XXH32(&sndVolume, sizeof(sndVolume), id);
	id = XXH32(&sndMode, sizeof(sndMode), id);
	id = XXH32(&sndRandDelay, sizeof(sndRandDelay), id);
	id = XXH32(&sndRandDelayVar, sizeof(sndRandDelayVar), id);
	id = XXH32(&sndStartOn, sizeof(sndStartOn), id);
	id = XXH32(&sndAmbient3D, sizeof(sndAmbient3D), id);
	id = XXH32(&sndObstruction, sizeof(sndObstruction), id);
	id = XXH32(&sndConeAngle, sizeof(sndConeAngle), id);
	id = XXH32(&sndVolType, sizeof(sndVolType), id);
	id = XXH32(&sndRadius, sizeof(sndRadius), id);
	id = XXH32(sndName.ToChar(), sndName.Length(), id);
}

bool32 zCVobSound::IsEqual(zCObject *obj)
{
	if (!zCZone::IsEqual(obj)) return FALSE;

	zCVobSound *vob = (zCVobSound *)obj;

	if (sndVolume != vob->sndVolume) return FALSE;
	if (sndMode != vob->sndMode) return FALSE;
	if (sndRandDelay != vob->sndRandDelay) return FALSE;
	if (sndRandDelayVar != vob->sndRandDelayVar) return FALSE;
	if (sndStartOn != vob->sndStartOn) return FALSE;
	if (sndAmbient3D != vob->sndAmbient3D) return FALSE;
	if (sndObstruction != vob->sndObstruction) return FALSE;
	if (sndConeAngle != vob->sndConeAngle) return FALSE;
	if (sndVolType != vob->sndVolType) return FALSE;
	if (sndRadius != vob->sndRadius) return FALSE;
	if (sndName != vob->sndName) return FALSE;

	return TRUE;
}

void zCVobSoundDaytime::CalcHash()
{
	zCVobSound::CalcHash();

	hash = XXH64(&sndStartTime, sizeof(sndStartTime), hash);
	hash = XXH64(&sndEndTime, sizeof(sndEndTime), hash);
	hash = XXH64(sndName2.ToChar(), sndName2.Length(), hash);
}

void zCVobSoundDaytime::CalcID()
{
	zCVobSound::CalcID();

	id = XXH32(&sndStartTime, sizeof(sndStartTime), id);
	id = XXH32(&sndEndTime, sizeof(sndEndTime), id);
	id = XXH32(sndName2.ToChar(), sndName2.Length(), id);
}

bool32 zCVobSoundDaytime::IsEqual(zCObject *obj)
{
	if (!zCVobSound::IsEqual(obj)) return FALSE;

	zCVobSoundDaytime *vob = (zCVobSoundDaytime *)obj;

	if (sndStartTime != vob->sndStartTime) return FALSE;
	if (sndEndTime != vob->sndEndTime) return FALSE;
	if (sndName2 != vob->sndName2) return FALSE;

	return TRUE;
}

void zCZoneReverb::CalcHash()
{
	zCZone::CalcHash();

	hash = XXH64(&reverbPreset, sizeof(reverbPreset), hash);
	hash = XXH64(&reverbWeight, sizeof(reverbWeight), hash);
	hash = XXH64(&innerRangePerc, sizeof(innerRangePerc), hash);
}

void zCZoneReverb::CalcID()
{
	zCZone::CalcID();

	id = XXH32(&reverbPreset, sizeof(reverbPreset), id);
	id = XXH32(&reverbWeight, sizeof(reverbWeight), id);
	id = XXH32(&innerRangePerc, sizeof(innerRangePerc), id);
}

bool32 zCZoneReverb::IsEqual(zCObject *obj)
{
	if (!zCZone::IsEqual(obj)) return FALSE;

	zCZoneReverb *vob = (zCZoneReverb *)obj;

	if (reverbPreset != vob->reverbPreset) return FALSE;
	if (reverbWeight != vob->reverbWeight) return FALSE;
	if (innerRangePerc != vob->innerRangePerc) return FALSE;

	return TRUE;
}

void oCZoneMusic::CalcHash()
{
	zCZoneMusic::CalcHash();

	hash = XXH64(&enabled, sizeof(enabled), hash);
	hash = XXH64(&priority, sizeof(priority), hash);
	hash = XXH64(&ellipsoid, sizeof(ellipsoid), hash);
	hash = XXH64(&reverbLevel, sizeof(reverbLevel), hash);
	hash = XXH64(&volumeLevel, sizeof(volumeLevel), hash);
	hash = XXH64(&loop, sizeof(loop), hash);
}

void oCZoneMusic::CalcID()
{
	zCZoneMusic::CalcID();

	id = XXH32(&enabled, sizeof(enabled), id);
	id = XXH32(&priority, sizeof(priority), id);
	id = XXH32(&ellipsoid, sizeof(ellipsoid), id);
	id = XXH32(&reverbLevel, sizeof(reverbLevel), id);
	id = XXH32(&volumeLevel, sizeof(volumeLevel), id);
	id = XXH32(&loop, sizeof(loop), id);
}

bool32 oCZoneMusic::IsEqual(zCObject *obj)
{
	if (!zCZoneMusic::IsEqual(obj)) return FALSE;

	oCZoneMusic *vob = (oCZoneMusic *)obj;

	if (enabled != vob->enabled) return FALSE;
	if (priority != vob->priority) return FALSE;
	if (ellipsoid != vob->ellipsoid) return FALSE;
	if (reverbLevel != vob->reverbLevel) return FALSE;
	if (volumeLevel != vob->volumeLevel) return FALSE;
	if (loop != vob->loop) return FALSE;

	return TRUE;
}

void oCObjectGenerator::CalcHash()
{
	zCVob::CalcHash();

	hash = XXH64(objectName.ToChar(), objectName.Length(), hash);
	hash = XXH64(&objectSpeed, sizeof(objectSpeed), hash);
}

void oCObjectGenerator::CalcID()
{
	zCVob::CalcID();

	id = XXH32(objectName.ToChar(), objectName.Length(), id);
	id = XXH32(&objectSpeed, sizeof(objectSpeed), id);
}

bool32 oCObjectGenerator::IsEqual(zCObject *obj)
{
	if (!zCVob::IsEqual(obj)) return FALSE;

	oCObjectGenerator *vob = (oCObjectGenerator *)obj;

	if (objectName != vob->objectName) return FALSE;
	if (objectSpeed != vob->objectSpeed) return FALSE;

	return TRUE;
}

void oCItem::CalcHash()
{
	oCVob::CalcHash();

	hash = XXH64(itemInstance.ToChar(), itemInstance.Length(), hash);
}

void oCItem::CalcID()
{
	oCVob::CalcID();

	id = XXH32(itemInstance.ToChar(), itemInstance.Length(), id);
}

bool32 oCItem::IsEqual(zCObject *obj)
{
	if (!zCObject::IsEqual(obj)) return FALSE;

	oCItem *vob = (oCItem *)obj;

	bool32 result = TRUE;

	zSTRING saveVisualName1 = visualName;
	zCObject *saveVisual1 = visual;
	bool32 saveShowVisual1 = showVisual;

	zSTRING saveVisualName2 = vob->visualName;
	zCObject *saveVisual2 = vob->visual;
	bool32 saveShowVisual2 = vob->showVisual;

	if (gothicVersionOut >= GOTHIC_VERSION_108)
	{
		bool32 ignoreVisual1 = TRUE;
		bool32 ignoreVisual2 = TRUE;

		for (int32 i = 0; i < array_size(itemsZenVisual); i++)
		{
			if (itemInstance == itemsZenVisual[i])
			{
				ignoreVisual1 = FALSE;
			}

			if (vob->itemInstance == itemsZenVisual[i])
			{
				ignoreVisual2 = FALSE;
			}

			if (!ignoreVisual1 && !ignoreVisual2) break;
		}

		if (ignoreVisual1)
		{
			visualName = "";
			visual = NULL;
			showVisual = FALSE;
		}

		if (ignoreVisual2)
		{
			vob->visualName = "";
			vob->visual = NULL;
			vob->showVisual = FALSE;
		}
	}

	if (result) result = oCVob::IsEqual(obj);

	if (result) result = itemInstance == vob->itemInstance;

	if (gothicVersionOut >= GOTHIC_VERSION_108)
	{
		visualName = saveVisualName1;
		visual = saveVisual1;
		showVisual = saveShowVisual1;

		vob->visualName = saveVisualName2;
		vob->visual = saveVisual2;
		vob->showVisual = saveShowVisual2;
	}

	return result;
}

void oCMOB::CalcHash()
{
	oCVob::CalcHash();

	hash = XXH64(focusName.ToChar(), focusName.Length(), hash);
	hash = XXH64(&hitpoints, sizeof(hitpoints), hash);
	hash = XXH64(&damage, sizeof(damage), hash);
	hash = XXH64(&moveable, sizeof(moveable), hash);
	hash = XXH64(&takeable, sizeof(takeable), hash);
	hash = XXH64(&focusOverride, sizeof(focusOverride), hash);
	hash = XXH64(&soundMaterial, sizeof(soundMaterial), hash);
	hash = XXH64(visualDestroyed.ToChar(), visualDestroyed.Length(), hash);
	hash = XXH64(owner.ToChar(), owner.Length(), hash);
	hash = XXH64(ownerGuild.ToChar(), ownerGuild.Length(), hash);
	hash = XXH64(&isDestroyed, sizeof(isDestroyed), hash);
}

void oCMOB::CalcID()
{
	oCVob::CalcID();

	id = XXH32(focusName.ToChar(), focusName.Length(), id);
	id = XXH32(&hitpoints, sizeof(hitpoints), id);
	id = XXH32(&damage, sizeof(damage), id);
	id = XXH32(&moveable, sizeof(moveable), id);
	id = XXH32(&takeable, sizeof(takeable), id);
	id = XXH32(&focusOverride, sizeof(focusOverride), id);
	id = XXH32(&soundMaterial, sizeof(soundMaterial), id);
	id = XXH32(visualDestroyed.ToChar(), visualDestroyed.Length(), id);
	id = XXH32(owner.ToChar(), owner.Length(), id);
	id = XXH32(ownerGuild.ToChar(), ownerGuild.Length(), id);
	id = XXH32(&isDestroyed, sizeof(isDestroyed), id);
}

bool32 oCMOB::IsEqual(zCObject *obj)
{
	if (!oCVob::IsEqual(obj)) return FALSE;

	oCMOB *vob = (oCMOB *)obj;

	if (focusName != vob->focusName) return FALSE;
	if (hitpoints != vob->hitpoints) return FALSE;
	if (damage != vob->damage) return FALSE;
	if (moveable != vob->moveable) return FALSE;
	if (takeable != vob->takeable) return FALSE;
	if (focusOverride != vob->focusOverride) return FALSE;
	if (soundMaterial != vob->soundMaterial) return FALSE;
	if (visualDestroyed != vob->visualDestroyed) return FALSE;
	if (owner != vob->owner) return FALSE;
	if (ownerGuild != vob->ownerGuild) return FALSE;
	if (isDestroyed != vob->isDestroyed) return FALSE;

	return TRUE;
}

void oCMobInter::CalcHash()
{
	oCMOB::CalcHash();

	hash = XXH64(&state, sizeof(state), hash);
	hash = XXH64(&stateTarget, sizeof(stateTarget), hash);

	hash = XXH64(&stateNum, sizeof(stateNum), hash);
	hash = XXH64(triggerTarget.ToChar(), triggerTarget.Length(), hash);
	hash = XXH64(useWithItem.ToChar(), useWithItem.Length(), hash);
	hash = XXH64(conditionFunc.ToChar(), conditionFunc.Length(), hash);
	hash = XXH64(onStateFunc.ToChar(), onStateFunc.Length(), hash);
	hash = XXH64(&rewind, sizeof(rewind), hash);
}

void oCMobInter::CalcID()
{
	oCMOB::CalcID();

	id = XXH32(&state, sizeof(state), id);
	id = XXH32(&stateTarget, sizeof(stateTarget), id);

	id = XXH32(&stateNum, sizeof(stateNum), id);
	id = XXH32(triggerTarget.ToChar(), triggerTarget.Length(), id);
	id = XXH32(useWithItem.ToChar(), useWithItem.Length(), id);
	id = XXH32(conditionFunc.ToChar(), conditionFunc.Length(), id);
	id = XXH32(onStateFunc.ToChar(), onStateFunc.Length(), id);
	id = XXH32(&rewind, sizeof(rewind), id);
}

bool32 oCMobInter::IsEqual(zCObject *obj)
{
	if (!oCMOB::IsEqual(obj)) return FALSE;

	oCMobInter *vob = (oCMobInter *)obj;

	if (state != vob->state) return FALSE;
	if (stateTarget != vob->stateTarget) return FALSE;

	if (stateNum != vob->stateNum) return FALSE;
	if (triggerTarget != vob->triggerTarget) return FALSE;
	if (useWithItem != vob->useWithItem) return FALSE;
	if (conditionFunc != vob->conditionFunc) return FALSE;
	if (onStateFunc != vob->onStateFunc) return FALSE;
	if (rewind != vob->rewind) return FALSE;

	return TRUE;
}

void oCMobFire::CalcHash()
{
	oCMobInter::CalcHash();

	hash = XXH64(fireSlot.ToChar(), fireSlot.Length(), hash);
	hash = XXH64(fireVobtreeName.ToChar(), fireVobtreeName.Length(), hash);
}

void oCMobFire::CalcID()
{
	oCMobInter::CalcID();

	id = XXH32(fireSlot.ToChar(), fireSlot.Length(), id);
	id = XXH32(fireVobtreeName.ToChar(), fireVobtreeName.Length(), id);
}

bool32 oCMobFire::IsEqual(zCObject *obj)
{
	if (!oCMobInter::IsEqual(obj)) return FALSE;

	oCMobFire *vob = (oCMobFire *)obj;

	if (fireSlot != vob->fireSlot) return FALSE;
	if (fireVobtreeName != vob->fireVobtreeName) return FALSE;

	return TRUE;
}

void oCMobItemSlot::CalcHash()
{
	oCMobInter::CalcHash();

	hash = XXH64(&itemRemoveable, sizeof(itemRemoveable), hash);
}

void oCMobItemSlot::CalcID()
{
	oCMobInter::CalcID();

	id = XXH32(&itemRemoveable, sizeof(itemRemoveable), id);
}

bool32 oCMobItemSlot::IsEqual(zCObject *obj)
{
	if (!oCMobInter::IsEqual(obj)) return FALSE;

	oCMobItemSlot *vob = (oCMobItemSlot *)obj;

	if (itemRemoveable != vob->itemRemoveable) return FALSE;

	return TRUE;
}

void oCMobLockable::CalcHash()
{
	oCMobInter::CalcHash();

	hash = XXH64(&locked, sizeof(locked), hash);
	hash = XXH64(keyInstance.ToChar(), keyInstance.Length(), hash);
	hash = XXH64(pickLockStr.ToChar(), pickLockStr.Length(), hash);
}

void oCMobLockable::CalcID()
{
	oCMobInter::CalcID();

	id = XXH32(&locked, sizeof(locked), id);
	id = XXH32(keyInstance.ToChar(), keyInstance.Length(), id);
	id = XXH32(pickLockStr.ToChar(), pickLockStr.Length(), id);
}

bool32 oCMobLockable::IsEqual(zCObject *obj)
{
	if (!oCMobInter::IsEqual(obj)) return FALSE;

	oCMobLockable *vob = (oCMobLockable *)obj;

	if (locked != vob->locked) return FALSE;
	if (keyInstance != vob->keyInstance) return FALSE;
	if (pickLockStr != vob->pickLockStr) return FALSE;

	return TRUE;
}

void oCMobContainer::CalcHash()
{
	oCMobLockable::CalcHash();

	hash = XXH64(contains.ToChar(), contains.Length(), hash);
}

void oCMobContainer::CalcID()
{
	oCMobLockable::CalcID();

	id = XXH32(contains.ToChar(), contains.Length(), id);
}

bool32 oCMobContainer::IsEqual(zCObject *obj)
{
	if (!oCMobLockable::IsEqual(obj)) return FALSE;

	oCMobContainer *vob = (oCMobContainer *)obj;

	if (contains != vob->contains) return FALSE;

	return TRUE;
}

void oCNpc::CalcHash()
{
	oCVob::CalcHash();

	hash = XXH64(npcInstance.ToChar(), npcInstance.Length(), hash);
}

void oCNpc::CalcID()
{
	oCVob::CalcID();

	id = XXH32(npcInstance.ToChar(), npcInstance.Length(), id);
}

bool32 oCNpc::IsEqual(zCObject *obj)
{
	if (!oCVob::IsEqual(obj)) return FALSE;

	oCNpc *vob = (oCNpc *)obj;

	if (npcInstance != vob->npcInstance) return FALSE;

	return TRUE;
}
