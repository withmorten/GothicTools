#include "Vob.h"

#define SPECIAL_FLAKY_VOB_EPSILON // allow a more lenient epsilon for some vob types known to be extra flaky

void zCVob::Hash()
{
	zCObject::Hash();

	hash = XXH64(presetName.ToChar(), presetName.Length(), hash);

	hash = XXH64(&bbox3DWS, sizeof(bbox3DWS), hash);
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

	if (zbias != vob->zbias) return FALSE;
	if (isAmbient != vob->isAmbient) return FALSE;

	if (trafoOSToWSRot != vob->trafoOSToWSRot || trafoOSToWSPos != vob->trafoOSToWSPos)
	{
		float epsilon = 0.01f;

#ifdef SPECIAL_FLAKY_VOB_EPSILON
		if (visualName == "FIRE_SMOKE.pfx" && vob->visualName == "FIRE_SMOKE.pfx")
		{
			epsilon = 1.0f;
		}
		else if (presetName == "FIREE" && vob->presetName == "FIREE")
		{
			epsilon = 0.1f;
		}
		else if (presetName == "FIRE400" && vob->presetName == "FIRE400")
		{
			epsilon = 0.1f;
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

void zCCamTrj_KeyFrame::Hash()
{
	zCVob::Hash();

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

void zCCSCamera::Hash()
{
	zCVob::Hash();

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

XXH64_hash_t zCVobLightData::Hash(XXH64_hash_t hash)
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

void zCVobLight::Hash()
{
	zCVob::Hash();

	hash = XXH64(lightPresetInUse.ToChar(), lightPresetInUse.Length(), hash);

	hash = lightData.Hash(hash);
}

bool32 zCVobLight::IsEqual(zCObject *obj)
{
	if (!zCVob::IsEqual(obj)) return FALSE;

	zCVobLight *vob = (zCVobLight *)obj;

	if (lightPresetInUse != vob->lightPresetInUse) return FALSE;

	if (!lightData.IsEqual(vob->lightData)) return FALSE;

	return TRUE;
}

void zCTriggerBase::Hash()
{
	zCVob::Hash();

	hash = XXH64(triggerTarget.ToChar(), triggerTarget.Length(), hash);
}

bool32 zCTriggerBase::IsEqual(zCObject *obj)
{
	if (!zCVob::IsEqual(obj)) return FALSE;

	zCTriggerBase *vob = (zCTriggerBase *)obj;

	if (triggerTarget != vob->triggerTarget) return FALSE;

	return TRUE;
}

void zCCodeMaster::Hash()
{
	zCTriggerBase::Hash();

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

void zCMessageFilter::Hash()
{
	zCTriggerBase::Hash();

	hash = XXH64(&onTrigger, sizeof(onTrigger), hash);
	hash = XXH64(&onUntrigger, sizeof(onUntrigger), hash);
}

bool32 zCMessageFilter::IsEqual(zCObject *obj)
{
	if (!zCTriggerBase::IsEqual(obj)) return FALSE;

	zCMessageFilter *vob = (zCMessageFilter *)obj;

	if (onTrigger != vob->onTrigger) return FALSE;
	if (onUntrigger != vob->onUntrigger) return FALSE;

	return TRUE;
}

void zCMoverControler::Hash()
{
	zCTriggerBase::Hash();

	hash = XXH64(&moverMessage, sizeof(moverMessage), hash);
	hash = XXH64(&gotoFixedKey, sizeof(gotoFixedKey), hash);
}

bool32 zCMoverControler::IsEqual(zCObject *obj)
{
	if (!zCTriggerBase::IsEqual(obj)) return FALSE;

	zCMoverControler *vob = (zCMoverControler *)obj;

	if (moverMessage != vob->moverMessage) return FALSE;
	if (gotoFixedKey != vob->gotoFixedKey) return FALSE;

	return TRUE;
}

void zCTriggerWorldStart::Hash()
{
	zCTriggerBase::Hash();

	hash = XXH64(&fireOnlyFirstTime, sizeof(fireOnlyFirstTime), hash);
}

bool32 zCTriggerWorldStart::IsEqual(zCObject *obj)
{
	if (!zCTriggerBase::IsEqual(obj)) return FALSE;

	zCTriggerWorldStart *vob = (zCTriggerWorldStart *)obj;

	if (fireOnlyFirstTime != vob->fireOnlyFirstTime) return FALSE;

	return TRUE;
}

void zCTrigger::Hash()
{
	zCTriggerBase::Hash();

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

void zCTriggerTeleport::Hash()
{
	zCTrigger::Hash();

	hash = XXH64(sfxTeleport.ToChar(), sfxTeleport.Length(), hash);
}

bool32 zCTriggerTeleport::IsEqual(zCObject *obj)
{
	if (!zCTrigger::IsEqual(obj)) return FALSE;

	zCTriggerTeleport *vob = (zCTriggerTeleport *)obj;

	if (sfxTeleport != vob->sfxTeleport) return FALSE;

	return TRUE;
}

void zCMover::Hash()
{
	zCTrigger::Hash();

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

		if (!memcmp(keyframes, vob->keyframes, sizeof(*keyframes) * numKeyframes)) return FALSE;
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

void zCTriggerList::Hash()
{
	zCTrigger::Hash();

	hash = XXH64(&listProcess, sizeof(listProcess), hash);
	hash = XXH64(&numTarget, sizeof(numTarget), hash);

	for (int32 i = 0; i < numTarget; i++)
	{
		hash = XXH64(triggerTargetList[i].ToChar(), triggerTargetList[i].Length(), hash);
		hash = XXH64(&fireDelayList[i], sizeof(fireDelayList[i]), hash);
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

void oCTriggerChangeLevel::Hash()
{
	zCTrigger::Hash();

	hash = XXH64(levelName.ToChar(), levelName.Length(), hash);
	hash = XXH64(startVobName.ToChar(), startVobName.Length(), hash);
}

bool32 oCTriggerChangeLevel::IsEqual(zCObject *obj)
{
	if (!zCTrigger::IsEqual(obj)) return FALSE;

	oCTriggerChangeLevel *vob = (oCTriggerChangeLevel *)obj;

	if (levelName != vob->levelName) return FALSE;
	if (startVobName != vob->startVobName) return FALSE;

	return TRUE;
}

void oCTriggerScript::Hash()
{
	zCTrigger::Hash();

	hash = XXH64(scriptFunc.ToChar(), scriptFunc.Length(), hash);
}

bool32 oCTriggerScript::IsEqual(zCObject *obj)
{
	if (!zCTrigger::IsEqual(obj)) return FALSE;

	oCTriggerScript *vob = (oCTriggerScript *)obj;

	if (scriptFunc != vob->scriptFunc) return FALSE;

	return TRUE;
}

void zCEarthquake::Hash()
{
	zCEffect::Hash();

	hash = XXH64(&radius, sizeof(radius), hash);
	hash = XXH64(&timeSec, sizeof(timeSec), hash);
	hash = XXH64(&amplitudeCM, sizeof(amplitudeCM), hash);
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

void zCPFXControler::Hash()
{
	zCEffect::Hash();

	hash = XXH64(pfxName.ToChar(), pfxName.Length(), hash);
	hash = XXH64(&killVobWhenDone, sizeof(killVobWhenDone), hash);
	hash = XXH64(&pfxStartOn, sizeof(pfxStartOn), hash);
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

void zCTouchDamage::Hash()
{
	zCEffect::Hash();

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

void zCTouchAnimateSound::Hash()
{
	zCTouchAnimate::Hash();

	hash = XXH64(sfxTouch.ToChar(), sfxTouch.Length(), hash);
}

bool32 zCTouchAnimateSound::IsEqual(zCObject *obj)
{
	if (!zCTouchAnimate::IsEqual(obj)) return FALSE;

	zCTouchAnimateSound *vob = (zCTouchAnimateSound *)obj;

	if (sfxTouch != vob->sfxTouch) return FALSE;

	return TRUE;
}

void zCVobAnimate::Hash()
{
	zCEffect::Hash();

	hash = XXH64(&startOn, sizeof(startOn), hash);
}

bool32 zCVobAnimate::IsEqual(zCObject *obj)
{
	if (!zCEffect::IsEqual(obj)) return FALSE;

	zCVobAnimate *vob = (zCVobAnimate *)obj;

	if (startOn != vob->startOn) return FALSE;

	return TRUE;
}

void zCVobLensFlare::Hash()
{
	zCEffect::Hash();

	hash = XXH64(lensflareFX.ToChar(), lensflareFX.Length(), hash);
}

bool32 zCVobLensFlare::IsEqual(zCObject *obj)
{
	if (!zCEffect::IsEqual(obj)) return FALSE;

	zCVobLensFlare *vob = (zCVobLensFlare *)obj;

	if (lensflareFX != vob->lensflareFX) return FALSE;

	return TRUE;
}

void zCZone::Hash()
{
	zCVob::Hash();

	hash = XXH64(&bbox3DWS, sizeof(bbox3DWS), hash);
}

void zCZoneZFog::Hash()
{
	zCZone::Hash();

	hash = XXH64(&fogRangeCenter, sizeof(fogRangeCenter), hash);
	hash = XXH64(&innerRangePerc, sizeof(innerRangePerc), hash);
	hash = XXH64(&fogColor, sizeof(fogColor), hash);

	hash = XXH64(&fadeOutSky, sizeof(fadeOutSky), hash);
	hash = XXH64(&overrideColor, sizeof(overrideColor), hash);
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

void zCZoneVobFarPlane::Hash()
{
	zCZone::Hash();

	hash = XXH64(&vobFarPlaneZ, sizeof(vobFarPlaneZ), hash);
	hash = XXH64(&innerRangePerc, sizeof(innerRangePerc), hash);
}

bool32 zCZoneVobFarPlane::IsEqual(zCObject *obj)
{
	if (!zCZone::IsEqual(obj)) return FALSE;

	zCZoneVobFarPlane *vob = (zCZoneVobFarPlane *)obj;

	if (vobFarPlaneZ != vob->vobFarPlaneZ) return FALSE;
	if (innerRangePerc != vob->innerRangePerc) return FALSE;

	return TRUE;
}

void zCVobSound::Hash()
{
	zCZone::Hash();

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

void zCVobSoundDaytime::Hash()
{
	zCVobSound::Hash();

	hash = XXH64(&sndStartTime, sizeof(sndStartTime), hash);
	hash = XXH64(&sndEndTime, sizeof(sndEndTime), hash);
	hash = XXH64(sndName2.ToChar(), sndName2.Length(), hash);
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

void zCZoneReverb::Hash()
{
	zCZone::Hash();

	hash = XXH64(&reverbPreset, sizeof(reverbPreset), hash);
	hash = XXH64(&reverbWeight, sizeof(reverbWeight), hash);
	hash = XXH64(&innerRangePerc, sizeof(innerRangePerc), hash);
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

void oCZoneMusic::Hash()
{
	zCZoneMusic::Hash();

	hash = XXH64(&enabled, sizeof(enabled), hash);
	hash = XXH64(&priority, sizeof(priority), hash);
	hash = XXH64(&ellipsoid, sizeof(ellipsoid), hash);
	hash = XXH64(&reverbLevel, sizeof(reverbLevel), hash);
	hash = XXH64(&volumeLevel, sizeof(volumeLevel), hash);
	hash = XXH64(&loop, sizeof(loop), hash);
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

void oCObjectGenerator::Hash()
{
	zCVob::Hash();

	hash = XXH64(objectName.ToChar(), objectName.Length(), hash);
	hash = XXH64(&objectSpeed, sizeof(objectSpeed), hash);
}

bool32 oCObjectGenerator::IsEqual(zCObject *obj)
{
	if (!zCVob::IsEqual(obj)) return FALSE;

	oCObjectGenerator *vob = (oCObjectGenerator *)obj;

	if (objectName != vob->objectName) return FALSE;
	if (objectSpeed != vob->objectSpeed) return FALSE;

	return TRUE;
}

void oCItem::Hash()
{
	oCVob::Hash();

	hash = XXH64(itemInstance.ToChar(), itemInstance.Length(), hash);
}

bool32 oCItem::IsEqual(zCObject *obj)
{
	if (!oCVob::IsEqual(obj)) return FALSE;

	oCItem *vob = (oCItem *)obj;

	if (itemInstance != vob->itemInstance) return FALSE;

	return TRUE;
}

void oCMOB::Hash()
{
	oCVob::Hash();

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

void oCMobInter::Hash()
{
	oCMOB::Hash();

	hash = XXH64(&state, sizeof(state), hash);
	hash = XXH64(&stateTarget, sizeof(stateTarget), hash);

	hash = XXH64(&stateNum, sizeof(stateNum), hash);
	hash = XXH64(triggerTarget.ToChar(), triggerTarget.Length(), hash);
	hash = XXH64(useWithItem.ToChar(), useWithItem.Length(), hash);
	hash = XXH64(conditionFunc.ToChar(), conditionFunc.Length(), hash);
	hash = XXH64(onStateFunc.ToChar(), onStateFunc.Length(), hash);
	hash = XXH64(&rewind, sizeof(rewind), hash);
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

void oCMobFire::Hash()
{
	oCMobInter::Hash();

	hash = XXH64(fireSlot.ToChar(), fireSlot.Length(), hash);
	hash = XXH64(fireVobtreeName.ToChar(), fireVobtreeName.Length(), hash);
}

bool32 oCMobFire::IsEqual(zCObject *obj)
{
	if (!oCMobInter::IsEqual(obj)) return FALSE;

	oCMobFire *vob = (oCMobFire *)obj;

	if (fireSlot != vob->fireSlot) return FALSE;
	if (fireVobtreeName != vob->fireVobtreeName) return FALSE;

	return TRUE;
}

void oCMobItemSlot::Hash()
{
	oCMobInter::Hash();

	hash = XXH64(&itemRemoveable, sizeof(itemRemoveable), hash);
}

bool32 oCMobItemSlot::IsEqual(zCObject *obj)
{
	if (!oCMobInter::IsEqual(obj)) return FALSE;

	oCMobItemSlot *vob = (oCMobItemSlot *)obj;

	if (itemRemoveable != vob->itemRemoveable) return FALSE;

	return TRUE;
}

void oCMobLockable::Hash()
{
	oCMobInter::Hash();

	hash = XXH64(&locked, sizeof(locked), hash);
	hash = XXH64(keyInstance.ToChar(), keyInstance.Length(), hash);
	hash = XXH64(pickLockStr.ToChar(), pickLockStr.Length(), hash);
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

void oCMobContainer::Hash()
{
	oCMobLockable::Hash();

	hash = XXH64(contains.ToChar(), contains.Length(), hash);
}

bool32 oCMobContainer::IsEqual(zCObject *obj)
{
	if (!oCMobLockable::IsEqual(obj)) return FALSE;

	oCMobContainer *vob = (oCMobContainer *)obj;

	if (contains != vob->contains) return FALSE;

	return TRUE;
}

void oCNpc::Hash()
{
	oCVob::Hash();

	hash = XXH64(npcInstance.ToChar(), npcInstance.Length(), hash);
}

bool32 oCNpc::IsEqual(zCObject *obj)
{
	if (!oCVob::IsEqual(obj)) return FALSE;

	oCNpc *vob = (oCNpc *)obj;

	if (npcInstance != vob->npcInstance) return FALSE;

	return TRUE;
}
