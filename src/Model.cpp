#include "Model.h"

#include "ModelAni.h"

const zSTRING sep = " \t";
const zSTRING trenn = " \t\r";
const zSTRING skip = " \t\r";

void CutMarks(zSTRING arg)
{
	arg.Delete("\"", zSTR_TO);
	arg.Delete("\"", zSTR_FROM);
}

zSTRING CutMarks2(zSTRING arg)
{
	zSTRING n = arg;
	n.Delete("\"", zSTR_TO);
	n.Delete("\"", zSTR_FROM);
	return n;
}

void GetMDSLineValue(const zSTRING &line, const zSTRING &key, float &result)
{
	int spos = line.Search(key);

	if (spos != -1)
	{
		result = line.Deleted(0, spos + key.Length()).ToFloat();
	}
}

bool32 zCModelPrototype::ReadAniEventsMSB(zCFileBIN &file, zCModelAni *aniPtr)
{
	file.BinReadInt(aniPtr->numAniEvents);
	aniPtr->aniEvents = zNEW_ARRAY(zCModelAniEvent, aniPtr->numAniEvents);

	uint16 id;
	int32 len;

	int32 actEvent = 0;
	zCModelAniEvent *event;

	while (!file.BinEof())
	{
		file.BinOpenChunk(id, len);

		if (actEvent > aniPtr->numAniEvents)
		{
			printf("Too many events, MSB corrupt!\n");

			return FALSE;
		}

		event = &aniPtr->aniEvents[actEvent++];

		switch (id)
		{
		case zFCHUNK_MDS_EVENTSFX:
		{
			event->aniEventType = zMDL_EVENT_SOUND;

			file.BinReadInt(event->frameNr);
			file.BinReadLine(event->sfxName);
			file.BinReadFloat(event->value1);
			file.BinReadFloat(event->value2);

			break;
		}
		case zFCHUNK_MDS_EVENTSFXGRND:
		{
			event->aniEventType = zMDL_EVENT_SOUND_GRND;

			file.BinReadInt(event->frameNr);
			file.BinReadLine(event->sfxName);
			file.BinReadFloat(event->value1);
			file.BinReadFloat(event->value2);

			break;
		}
		case zFCHUNK_MDS_EVENTTAG:
		{
			event->aniEventType = zMDL_EVENT_TAG;

			file.BinReadInt(event->frameNr);
			file.BinReadLine(event->tagString);

			for (int32 i = 0; i < zMDL_ANIEVENT_MAXSTRING; i++)
			{
				file.BinReadLine(event->string[i]);
			}

			break;
		}
		case zFCHUNK_MDS_EVENTPFX:
		{
			event->aniEventType = zMDL_EVENT_PFX;

			file.BinReadInt(event->frameNr);
			file.BinReadFloat(event->value1);
			file.BinReadLine(event->emitterName);
			file.BinReadLine(event->string[0]);
			file.BinReadFloat(event->value2);

			break;
		}
		case zFCHUNK_MDS_EVENTPFXSTOP:
		{
			event->aniEventType = zMDL_EVENT_PFX_STOP;

			file.BinReadInt(event->frameNr);
			file.BinReadFloat(event->value1);

			break;
		}
		case zFCHUNK_MDS_EVENTPFXGRND:
		{
			event->aniEventType = zMDL_EVENT_PFX_GRND;

			file.BinReadInt(event->frameNr);

			break;
		}
		case zFCHUNK_MDS_EVENTSETMESH:
		{
			event->aniEventType = zMDL_EVENT_SETMESH;

			file.BinReadInt(event->frameNr);
			file.BinReadLine(event->string[0]);

			break;
		}
		case zFCHUNK_MDS_EVENTSWAPMESH:
		{
			event->aniEventType = zMDL_EVENT_SWAPMESH;

			file.BinReadInt(event->frameNr);
			file.BinReadLine(event->string[0]);
			file.BinReadLine(event->string[1]);

			break;
		}
		case zFCHUNK_MDS_EVENTMMSTARTANI:
		{
			event->aniEventType = zMDL_EVENT_MM_STARTANI;

			file.BinReadInt(event->frameNr);
			file.BinReadLine(event->string[0]);
			file.BinReadLine(event->string[1]);
			file.BinReadFloat(event->value1);
			file.BinReadFloat(event->value2);

			break;
		}
		case zFCHUNK_MDS_EVENTCAMTREMOR:
		{
			event->aniEventType = zMDL_EVENT_CAM_TREMOR;

			file.BinReadInt(event->frameNr);
			file.BinReadFloat(event->value1);
			file.BinReadFloat(event->value2);
			file.BinReadFloat(event->value3);
			file.BinReadFloat(event->value4);

			break;
		}
		case zFCHUNK_MDS_ANIEVENTS_END:
		{
			goto fileEnd;

			break;
		}
		default:
		{
			printf("Unknown chunk encountered while parsing MSB!\n");

			return FALSE;

			break;
		}
		}
	}

fileEnd:;

	return TRUE;
}

bool32 zCModelPrototype::ReadAniEnumMSB(zCFileBIN &file)
{
	uint16 id;
	int32 len;

	zCModelAni *aniPtr = NULL;

	while (!file.BinEof())
	{
		file.BinOpenChunk(id, len);

		switch (id)
		{
		case zFCHUNK_MDS_ANIMAXFPS:
		{
			file.BinReadFloat(maxFps);

			writeMaxFps = TRUE;

			break;
		}
		case zFCHUNK_MDS_ANI:
		{
			aniPtr = zNEW(zCModelAni)(zMDL_ANI_TYPE_NORMAL);

			file.BinReadLine(aniPtr->aniName);
			file.BinReadInt(aniPtr->layer);
			file.BinReadLine(aniPtr->nextAniName);
			file.BinReadFloat(aniPtr->blendInSpeed);
			file.BinReadFloat(aniPtr->blendOutSpeed);
			file.BinReadLine(aniPtr->aniFlagsStr);
			file.BinReadLine(aniPtr->ascName);
			file.BinReadLine(aniPtr->aniDirStr);
			file.BinReadInt(aniPtr->firstFrame);
			file.BinReadInt(aniPtr->lastFrame);
			file.BinReadFloat(aniPtr->maxFps);
			file.BinReadFloat(aniPtr->speed);
			file.BinReadFloat(aniPtr->collisionVolumeScale);

			protoAnis.Insert(aniPtr);

#if HANDLE_MSB_CHILD_FILES
			zSTRING aniName(modelProtoName + "-" + aniPtr->aniName + ".MAN");

			zCModelAni *modelAni = zNEW(zCModelAni);

			if (!modelAni->LoadMAN(file.file->path + "/" + aniName))
			{
				printf("Error loading %s!\n", aniName.ToChar());
			}

			modelAnis.Insert(modelAni);
#endif

			break;
		}
		case zFCHUNK_MDS_ANIALIAS:
		{
			aniPtr = zNEW(zCModelAni)(zMDL_ANI_TYPE_ALIAS);

			file.BinReadLine(aniPtr->aniName);
			file.BinReadInt(aniPtr->layer);
			file.BinReadLine(aniPtr->nextAniName);
			file.BinReadFloat(aniPtr->blendInSpeed);
			file.BinReadFloat(aniPtr->blendOutSpeed);
			file.BinReadLine(aniPtr->aniFlagsStr);
			file.BinReadLine(aniPtr->aliasName);
			file.BinReadLine(aniPtr->aniDirStr);

			protoAnis.Insert(aniPtr);

			break;
		}
		case zFCHUNK_MDS_ANIBLEND:
		{
			aniPtr = zNEW(zCModelAni)(zMDL_ANI_TYPE_BLEND);

			file.BinReadLine(aniPtr->aniName);
			file.BinReadLine(aniPtr->nextAniName);
			file.BinReadFloat(aniPtr->blendInSpeed);
			file.BinReadFloat(aniPtr->blendOutSpeed);

			protoAnis.Insert(aniPtr);

			break;
		}
		case zFCHUNK_MDS_ANISYNC:
		{
			aniPtr = zNEW(zCModelAni)(zMDL_ANI_TYPE_SYNC);

			file.BinReadLine(aniPtr->aniName);
			file.BinReadLine(aniPtr->nextAniName);

			protoAnis.Insert(aniPtr);

			break;
		}
		case zFCHUNK_MDS_ANICOMB:
		{
			aniPtr = zNEW(zCModelAni)(zMDL_ANI_TYPE_COMB);

			file.BinReadLine(aniPtr->aniName);
			file.BinReadInt(aniPtr->layer);
			file.BinReadLine(aniPtr->nextAniName);
			file.BinReadFloat(aniPtr->blendInSpeed);
			file.BinReadFloat(aniPtr->blendOutSpeed);
			file.BinReadLine(aniPtr->aniFlagsStr);
			file.BinReadLine(aniPtr->ascName);
			file.BinReadInt(aniPtr->lastFrame);

			protoAnis.Insert(aniPtr);

			break;
		}
		case zFCHUNK_MDS_ANIDISABLE:
		{
			aniPtr = zNEW(zCModelAni)(zMDL_ANI_TYPE_DISABLED);

			file.BinReadLine(aniPtr->aniName);

			protoAnis.Insert(aniPtr);

			break;
		}
		case zFCHUNK_MDS_MODELTAG:
		{
			zCModelAniEvent *event = zNEW(zCModelAniEvent);
			event->aniEventType = zMDL_EVENT_TAG;

			file.BinReadLine(event->tagString);

			for (int32 i = 0; i < zMDL_ANIEVENT_MAXSTRING; i++)
			{
				file.BinReadLine(event->string[i]);
			}

			modelEvents.Insert(event);

			break;
		}
		case zFCHUNK_MDS_ANIEVENTS:
		{
			if (!ReadAniEventsMSB(file, aniPtr))
			{
				printf("Error parsing ani events\n");

				return FALSE;
			}

			break;
		}
		case zFCHUNK_MDS_ANIENUM_END:
		{
			goto fileEnd;

			break;
		}
		default:
		{
			printf("Uknown chunk encountered while parsing MSB!\n");

			return FALSE;

			break;
		}
		}
	}

fileEnd:;

	return TRUE;
}

bool32 zCModelPrototype::ReadModelMSB(zCFileBIN &file)
{
	uint16 id;
	int32 len;

	while (!file.BinEof())
	{
		file.BinOpenChunk(id, len);

		switch (id)
		{
		case zFCHUNK_MDS_MESHANDTREE:
		{
			file.BinReadDWord(dontUseMesh);
			file.BinReadLine(ascName);

#if HANDLE_MSB_CHILD_FILES
			hierarchy = zNEW(zCModelHierarchy);
			hierarchy->LoadMDH(file.file->path + "/" + modelProtoName + ".MDH");

			if (!dontUseMesh)
			{
				mesh = zNEW(zCModelMeshLib);

				zSTRING mdmName(ascName);
				if (mdmName.EndsWith(".ASC")) mdmName.DeleteRight(4);

				if (!mesh->LoadMDM(file.file->path + "/" + mdmName + ".MDM"))
				{
					printf("Error loading MDM!\n");
					
					return FALSE;
				}
			}
#endif

			break;
		}
		case zFCHUNK_MDS_REGISTERMESH:
		{
			zSTRING ascName;
			file.BinReadLine(ascName);

			ascNames.Insert(ascName);

#if HANDLE_MSB_CHILD_FILES
			zSTRING mdmName(ascName);
			if (mdmName.EndsWith(".ASC")) mdmName.DeleteRight(4);

			zCModelMeshLib *mesh = zNEW(zCModelMeshLib);

			if (!mesh->LoadMDM(file.file->path + "/" + mdmName + ".MDM"))
			{
				printf("Error loading MDM!\n");

				return FALSE;
			}

			meshList.Insert(mesh);
#endif

			break;
		}
		case zFCHUNK_MDS_ANIENUM:
		{
			if (!ReadAniEnumMSB(file))
			{
				printf("Error parsing AniEnum chunk!\n");

				return FALSE;
			}

			break;
		}
		case zFCHUNK_MDS_MODEL_END:
		{
			goto fileEnd;

			break;
		}
		default:
		{
			printf("Unknown chunk encountered while parsing MSB!\n");

			return FALSE;

			break;
		}
		}
	}

fileEnd:;

	return TRUE;
}

bool32 zCModelPrototype::LoadModelScriptMSB(zCFileBIN &file)
{
	SetModelProtoName(file.file->filename);

	uint16 id;
	int32 len;

	while (!file.BinEof())
	{
		file.BinOpenChunk(id, len);

		switch (id)
		{
		case zFCHUNK_MODELSCRIPT:
		{
			uint32 vers;
			file.BinReadDWord(vers);

			if (vers != modelScriptVersionIn)
			{
				printf("Wrong MSB version!\n");

				return FALSE;
			}

			file.BinReadLine(ascPath);

			break;
		}
		case zFCHUNK_MDS_SOURCE:
		{
			srcFileStats.LoadBIN(file);

			break;
		}
		case zFCHUNK_MDS_MODEL:
		{
			if (!ReadModelMSB(file))
			{
				printf("Error while parsing MSB!\n");

				return FALSE;
			}

			break;
		}
		case zFCHUNK_MDS_END:
		{
			goto fileEnd;

			break;
		}
		default:
		{
			printf("Unknown chunk encountered while parsing MSB!\n");

			return FALSE;

			break;
		}
		}
	}

fileEnd:;

	return TRUE;
}

void zCModelPrototype::ReadAniEvents(zFILE &file, zCModelAni *aniPtr)
{
	zCArray<zCModelAniEvent *> aniEvents;

	zSTRING line;
	zSTRING arg;

	while (!file.Eof())
	{
		file.ReadLine(line, FALSE);
		line.Upper();

		if (line.Contains("//")) continue;

		arg = line;
		arg.Delete("(", zSTR_TO);
		arg.Delete(")", zSTR_FROM);

		if (aniPtr->aniType != zMDL_ANI_TYPE_ALIAS && line.Contains("*EVENT"))
		{
			zCModelAniEvent *event = zNEW(zCModelAniEvent);

			if (line.Contains("*EVENTSFXGRND"))
			{
				event->aniEventType = zMDL_EVENT_SOUND_GRND;

				event->frameNr = arg.PickWord(1, trenn, skip).ToInt();
				event->sfxName = CutMarks2(arg.PickWord(2, trenn, skip));

				float range = zMDL_DEFAULT_RADIUS_EVENTSFXGRND;
				GetMDSLineValue(arg, "R:", range);

				event->value1 = zMin(range, zMDL_MAX_RADIUS_EVENTSFX);
				event->value2 = !arg.Contains("EMPTY_") ? 1.0f : 0.0f;
			}
			else if (line.Contains("*EVENTSFX"))
			{
				event->aniEventType = zMDL_EVENT_SOUND;

				event->frameNr = arg.PickWord(1, trenn, skip).ToInt();
				event->sfxName = CutMarks2(arg.PickWord(2, trenn, skip));

				float range = zMDL_DEFAULT_RADIUS_EVENTSFX;
				GetMDSLineValue(arg, "R:", range);

				event->value1 = zMin(range, zMDL_MAX_RADIUS_EVENTSFX);
				event->value2 = !arg.Contains("EMPTY_") ? 1.0f : 0.0f;
			}
			else if (line.Contains("*EVENTSWAPMESH"))
			{
				event->aniEventType = zMDL_EVENT_SWAPMESH;

				event->frameNr = arg.PickWord(1, trenn, skip).ToInt();
				event->string[0] = CutMarks2(arg.PickWord(2, trenn, skip));
				event->string[1] = CutMarks2(arg.PickWord(3, trenn, skip));
			}
			else if (line.Contains("*EVENTTAG"))
			{
				event->aniEventType = zMDL_EVENT_TAG;

				event->frameNr = arg.PickWord(1, trenn, skip).ToInt();
				event->tagString = CutMarks2(arg.PickWord(2, "\"", "\""));

				for (int32 i = 0; i < zMDL_ANIEVENT_MAXSTRING; i++)
				{
					event->string[i] = CutMarks2(arg.PickWord(4 + (i * 2), "\"", "\""));
				}
			}
			else if (line.Contains("*EVENTPFXSTOP"))
			{
				event->aniEventType = zMDL_EVENT_PFX_STOP;

				event->frameNr = arg.PickWord(1, trenn, skip).ToInt();
				event->value1 = arg.PickWord(2, trenn, skip).ToFloat();
			}
			else if (line.Contains("*EVENTPFXGRND"))
			{
				event->aniEventType = zMDL_EVENT_PFX_GRND;

				event->frameNr = arg.PickWord(1, trenn, skip).ToInt();
			}
			else if (line.Contains("*EVENTPFX"))
			{
				event->aniEventType = zMDL_EVENT_PFX;

				event->frameNr = arg.PickWord(1, trenn, skip).ToInt();
				event->value1 = arg.PickWord(2, trenn, skip).ToFloat();
				event->emitterName = arg.PickWord(2, "\"", "\"");
				event->string[0] = arg.PickWord(4, "\"", "\"");
				event->value2 = (float)arg.Contains("ATTACH");
			}
			else if (line.Contains("*EVENTSETMESH"))
			{
				event->aniEventType = zMDL_EVENT_SETMESH;

				event->frameNr = arg.PickWord(1, trenn, skip).ToInt();
				event->string[0] = CutMarks2(arg.PickWord(2, trenn, skip));
			}
			else if (line.Contains("*EVENTMMSTARTANI"))
			{
				event->aniEventType = zMDL_EVENT_MM_STARTANI;

				event->frameNr = arg.PickWord(1, trenn, skip).ToInt();
				event->string[0] = CutMarks2(arg.PickWord(2, trenn, skip));
				event->string[1] = CutMarks2(arg.PickWord(3, trenn, skip));
				event->value1 = 1.0f;
				event->value2 = -2.0f;
				GetMDSLineValue(arg, "I:", event->value1);
				GetMDSLineValue(arg, "H:", event->value2);
			}
			else if (line.Contains("*EVENTCAMTREMOR"))
			{
				event->aniEventType = zMDL_EVENT_CAM_TREMOR;

				event->frameNr = arg.PickWord(1, trenn, skip).ToInt();
				event->value1 = arg.PickWord(2, trenn, skip).ToFloat();
				event->value1 *= event->value1;
				event->value2 = arg.PickWord(3, trenn, skip).ToFloat();
				event->value3 = arg.PickWord(4, trenn, skip).ToFloat();
				event->value4 = arg.PickWord(5, trenn, skip).ToFloat();
			}
			else
			{
				zDELETE(event);
			}

			if (event) aniEvents.InsertEnd(event);
		}
		else if (line.Contains("}")) // aniEvents end
		{
			break;
		}
	}

	aniPtr->numAniEvents = aniEvents.numInArray;
	aniPtr->aniEvents = zNEW_ARRAY(zCModelAniEvent, aniPtr->numAniEvents);

	for (int32 i = 0; i < aniPtr->numAniEvents; i++)
	{
		memcpy((void *)&aniPtr->aniEvents[i], aniEvents[i], sizeof(zCModelAniEvent));

		aniEvents[i] = NULL; // leaks ahoy
	}
}

void zCModelPrototype::ReadAniEnum(zFILE &file)
{
	zSTRING line;
	zSTRING arg;

	zCModelAni *aniPtr = NULL;

	while (!file.Eof())
	{
		file.ReadLine(line, FALSE);
		line.Upper();

		if (line.Contains("//")) continue;

		arg = line;
		arg.Delete("(", zSTR_TO);
		arg.Delete(")", zSTR_FROM);

		if (line.Contains("ANIMAXFPS"))
		{
			writeMaxFps = TRUE;

			maxFps = arg.PickWord(1, trenn, skip).ToFloat();
		}
		else if (line.Contains("ANI ") || line.Contains("ANI\t") || line.Contains("ANI("))
		{
			aniPtr = zNEW(zCModelAni)(zMDL_ANI_TYPE_NORMAL);

			aniPtr->aniName = arg.PickWordCutMarks(1, trenn, skip);
			aniPtr->layer = arg.PickWord(2, trenn, skip).ToInt();
			aniPtr->nextAniName = arg.PickWordCutMarks(3, trenn, skip);
			aniPtr->blendInSpeed = arg.PickWord(4, trenn, skip).ToFloat();
			aniPtr->blendOutSpeed = arg.PickWord(5, trenn, skip).ToFloat();
			aniPtr->aniFlagsStr = arg.PickWord(6, trenn, skip);
			aniPtr->ascName = arg.PickWordCutMarks(7, trenn, skip);
			aniPtr->aniDirStr = arg.PickWord(8, trenn, skip);
			aniPtr->firstFrame = arg.PickWord(9, trenn, skip).ToInt();
			aniPtr->lastFrame = arg.PickWord(10, trenn, skip).ToInt();
			aniPtr->maxFps = maxFps;
			aniPtr->speed = 0.0f;
			aniPtr->collisionVolumeScale = 1.0f;

			int spos;
			zSTRING tmp;

			spos = arg.Search("FPS:");

			if (spos != -1)
			{
				tmp = arg;
				tmp.Delete("FPS:", zSTR_TO);

				aniPtr->maxFps = tmp.ToFloat();
			}

			spos = arg.Search("SPD:");

			if (spos != -1)
			{
				tmp = arg;
				tmp.Delete("SPD:", zSTR_TO);

				aniPtr->speed = tmp.ToFloat();
			}

			spos = arg.Search("CVS:");

			if (spos != -1)
			{
				tmp = arg;
				tmp.Delete("CVS:", zSTR_TO);

				aniPtr->collisionVolumeScale = tmp.ToFloat();

				zClamp(aniPtr->collisionVolumeScale, 0.0f, 10.0f);
			}

			protoAnis.Insert(aniPtr);
		}
		else if (line.Contains("ANIALIAS"))
		{
			aniPtr = zNEW(zCModelAni)(zMDL_ANI_TYPE_ALIAS);

			aniPtr->aniName = arg.PickWordCutMarks(1, trenn, skip);
			aniPtr->layer = arg.PickWord(2, trenn, skip).ToInt();
			aniPtr->nextAniName = arg.PickWordCutMarks(3, trenn, skip);
			aniPtr->blendInSpeed = arg.PickWord(4, trenn, skip).ToFloat();
			aniPtr->blendOutSpeed = arg.PickWord(5, trenn, skip).ToFloat();
			aniPtr->aniFlagsStr = arg.PickWord(6, trenn, skip);
			aniPtr->aliasName = arg.PickWordCutMarks(7, trenn, skip);
			aniPtr->aniDirStr = arg.PickWord(8, trenn, skip);

			protoAnis.Insert(aniPtr);
		}
		else if (line.Contains("ANIBLEND"))
		{
			aniPtr = zNEW(zCModelAni)(zMDL_ANI_TYPE_BLEND);

			aniPtr->aniName = arg.PickWordCutMarks(1, trenn, skip);
			aniPtr->nextAniName = arg.PickWordCutMarks(2, trenn, skip);
			aniPtr->blendInSpeed = arg.PickWord(3, trenn, skip).ToFloat();
			aniPtr->blendOutSpeed = arg.PickWord(4, trenn, skip).ToFloat();

			protoAnis.Insert(aniPtr);
		}
		else if (line.Contains("ANISYNC"))
		{
			aniPtr = zNEW(zCModelAni)(zMDL_ANI_TYPE_SYNC);

			aniPtr->aniName = arg.PickWordCutMarks(1, trenn, skip);
			aniPtr->nextAniName = arg.PickWordCutMarks(2, trenn, skip);

			protoAnis.Insert(aniPtr);
		}
		else if (line.Contains("ANICOMB"))
		{
			aniPtr = zNEW(zCModelAni)(zMDL_ANI_TYPE_COMB);

			aniPtr->aniName = arg.PickWordCutMarks(1, trenn, skip);
			aniPtr->layer = arg.PickWord(2, trenn, skip).ToInt();
			aniPtr->nextAniName = arg.PickWordCutMarks(3, trenn, skip);
			aniPtr->blendInSpeed = arg.PickWord(4, trenn, skip).ToFloat();
			aniPtr->blendOutSpeed = arg.PickWord(5, trenn, skip).ToFloat();
			aniPtr->aniFlagsStr = arg.PickWord(6, trenn, skip);
			aniPtr->ascName = arg.PickWordCutMarks(7, trenn, skip);
			aniPtr->lastFrame = arg.PickWord(8, trenn, skip).ToInt();

			protoAnis.Insert(aniPtr);
		}
		else if (line.Contains("ANIDISABLE"))
		{
			aniPtr = zNEW(zCModelAni)(zMDL_ANI_TYPE_DISABLED);

			aniPtr->aniName = arg.PickWordCutMarks(1, trenn, skip);

			protoAnis.Insert(aniPtr);
		}
		else if (line.Contains("MODELTAG"))
		{
			zCModelAniEvent *event = zNEW(zCModelAniEvent);
			event->aniEventType = zMDL_EVENT_TAG;

			event->tagString = CutMarks2(arg.PickWord(1, "\"", "\""));

			for (int32 i = 0; i < zMDL_ANIEVENT_MAXSTRING; i++)
			{
				event->string[i] = CutMarks2(arg.PickWord(3 + (i * 2), "\"", "\""));
			}

			modelEvents.Insert(event);
		}
		else if (aniPtr && line.Contains("{")) // aniEvents Start
		{
			ReadAniEvents(file, aniPtr);
		}
		else if (line.Contains("}")) // aniEnum End
		{
			break;
		}
	}
}

void zCModelPrototype::ReadModel(zFILE &file)
{
	zSTRING line;

	while (!file.Eof())
	{
		file.ReadLine(line, FALSE);
		line.Upper();

		if (line.Contains("//")) continue;

		// in case a file has "aniEnum {" in one line, commented out
		// if (line.Contains("{")) continue; // Model Start 

		if (line.Contains("MESHANDTREE"))
		{
			ascName = line;
			CutMarks(ascName);
			ascName.Delete(".", zSTR_FROM);

			dontUseMesh = line.Contains("DONT_USE_MESH");
		}
		else if (line.Contains("REGISTERMESH"))
		{
			zSTRING ascName = line;
			CutMarks(ascName);
			ascName.Delete(".", zSTR_FROM);
			ascName.Append(".ASC");

			ascNames.Insert(ascName);
		}
		else if (line.Contains("ANIENUM"))
		{
			ReadAniEnum(file);
		}

		if (line.Contains("}")) break; // Model End
	}
}

bool32 zCModelPrototype::LoadModelScript(zFILE &file)
{
	SetModelProtoName(file.filename);

	ascPath = file.complete_path;
	ascPath.Upper();

	if (ascPath.Contains("MDS_MOBSI")) ascPath = "\\_WORK\\DATA\\ANIMS\\MDS_MOBSI\\";
	else if (ascPath.Contains("MDS_OVERLAY")) ascPath = "\\_WORK\\DATA\\ANIMS\\MDS_OVERLAY\\";
	else ascPath = "\\_WORK\\DATA\\ANIMS\\";

	srcFileStats.fileName = ascPath + modelProtoName + ".MDS";

	srcFileStats.date.SetTime(file);

	zSTRING line;

	while (!file.Eof())
	{
		file.ReadLine(line, FALSE);
		line.Upper();

		if (line.Contains("//")) continue;

		if (line.Contains("MODEL")) ReadModel(file);
	}

	return TRUE;
}

void zCModelPrototype::WriteAniEventsMSB(zCFileBIN &file, zCModelAni *aniPtr)
{
	if (aniPtr->numAniEvents > 0)
	{
		file.BinStartChunk(zFCHUNK_MDS_ANIEVENTS);
		{
			file.BinWriteInt(aniPtr->numAniEvents);
		}

		for (int32 i = 0; i < aniPtr->numAniEvents; i++)
		{
			zCModelAniEvent *event = &aniPtr->aniEvents[i];

			switch (event->aniEventType)
			{
			case zMDL_EVENT_TAG:
			{
				file.BinStartChunk(zFCHUNK_MDS_EVENTTAG);
				{
					file.BinWriteInt(event->frameNr);
					file.BinWriteLine(event->tagString);

					for (int32 j = 0; j < zMDL_ANIEVENT_MAXSTRING; j++)
					{
						file.BinWriteLine(event->string[j]);
					}
				}

				break;
			}
			case zMDL_EVENT_SOUND:
			{
				file.BinStartChunk(zFCHUNK_MDS_EVENTSFX);
				{
					file.BinWriteInt(event->frameNr);
					file.BinWriteLine(event->sfxName);
					file.BinWriteFloat(event->value1);
					file.BinWriteFloat(event->value2);
				}

				break;
			}
			case zMDL_EVENT_SOUND_GRND:
			{
				file.BinStartChunk(zFCHUNK_MDS_EVENTSFXGRND);
				{
					file.BinWriteInt(event->frameNr);
					file.BinWriteLine(event->sfxName);
					file.BinWriteFloat(event->value1);
					file.BinWriteFloat(event->value2);
				}

				break;
			}
			case zMDL_EVENT_SWAPMESH:
			{
				file.BinStartChunk(zFCHUNK_MDS_EVENTSWAPMESH);
				{
					file.BinWriteInt(event->frameNr);
					file.BinWriteLine(event->string[0]);
					file.BinWriteLine(event->string[1]);
				}

				break;
			}
			case zMDL_EVENT_PFX:
			{
				file.BinStartChunk(zFCHUNK_MDS_EVENTPFX);
				{
					file.BinWriteInt(event->frameNr);
					file.BinWriteFloat(event->value1);
					file.BinWriteLine(event->emitterName);
					file.BinWriteLine(event->string[0]);
					file.BinWriteFloat(event->value2);
				}

				break;
			}
			case zMDL_EVENT_PFX_GRND:
			{
				file.BinStartChunk(zFCHUNK_MDS_EVENTPFXGRND);
				{
					file.BinWriteInt(event->frameNr);
				}

				break;
			}
			case zMDL_EVENT_PFX_STOP:
			{
				file.BinStartChunk(zFCHUNK_MDS_EVENTPFXSTOP);
				{
					file.BinWriteInt(event->frameNr);
					file.BinWriteFloat(event->value1);
				}

				break;
			}
			case zMDL_EVENT_SETMESH:
			{
				file.BinStartChunk(zFCHUNK_MDS_EVENTSETMESH);
				{
					file.BinWriteInt(event->frameNr);
					file.BinWriteLine(event->string[0]);
				}

				break;
			}
			case zMDL_EVENT_MM_STARTANI:
			{
				file.BinStartChunk(zFCHUNK_MDS_EVENTMMSTARTANI);
				{
					file.BinWriteInt(event->frameNr);
					file.BinWriteLine(event->string[0]);
					file.BinWriteLine(event->string[1]);
					file.BinWriteFloat(event->value1);
					file.BinWriteFloat(event->value2);
				}

				break;
			}
			case zMDL_EVENT_CAM_TREMOR:
			{
				file.BinStartChunk(zFCHUNK_MDS_EVENTCAMTREMOR);
				{
					file.BinWriteInt(event->frameNr);
					file.BinWriteFloat(event->value1);
					file.BinWriteFloat(event->value2);
					file.BinWriteFloat(event->value3);
					file.BinWriteFloat(event->value4);
				}

				break;
			}
			default:
			{
				break;
			}
			}
		}

		file.BinStartChunk(zFCHUNK_MDS_ANIEVENTS_END);

		file.BinEndChunk();
	}
}

void zCModelPrototype::WriteAniEnumMSB(zCFileBIN &file)
{
	for (int32 i = 0; i < modelEvents.numInArray; i++)
	{
		zCModelAniEvent *event = modelEvents[i];

		file.BinStartChunk(zFCHUNK_MDS_MODELTAG);
		{

			file.BinWriteLine(event->tagString);

			for (int32 j = 0; j < zMDL_ANIEVENT_MAXSTRING; j++)
			{
				file.BinWriteLine(event->string[j]);
			}
		}
	}

	if (writeMaxFps)
	{
		file.BinStartChunk(zFCHUNK_MDS_ANIMAXFPS);
		{
			file.BinWriteFloat(maxFps);
		}
	}

	for (int32 i = 0; i < protoAnis.numInArray; i++)
	{
		zCModelAni *aniPtr = protoAnis[i];

		switch (aniPtr->aniType)
		{
		case zMDL_ANI_TYPE_NORMAL:
		{
			file.BinStartChunk(zFCHUNK_MDS_ANI);
			{
				file.BinWriteLine(aniPtr->aniName);
				file.BinWriteInt(aniPtr->layer);
				file.BinWriteLine(aniPtr->nextAniName);
				file.BinWriteFloat(aniPtr->blendInSpeed);
				file.BinWriteFloat(aniPtr->blendOutSpeed);
				file.BinWriteLine(aniPtr->aniFlagsStr);
				file.BinWriteLine(aniPtr->ascName);
				file.BinWriteLine(aniPtr->aniDirStr);
				file.BinWriteInt(aniPtr->firstFrame);
				file.BinWriteInt(aniPtr->lastFrame);
				file.BinWriteFloat(aniPtr->maxFps);
				file.BinWriteFloat(aniPtr->speed);
				file.BinWriteFloat(aniPtr->collisionVolumeScale);
			}

			WriteAniEventsMSB(file, aniPtr);

			break;
		}
		case zMDL_ANI_TYPE_ALIAS:
		{
			file.BinStartChunk(zFCHUNK_MDS_ANIALIAS);
			{
				file.BinWriteLine(aniPtr->aniName);
				file.BinWriteInt(aniPtr->layer);
				file.BinWriteLine(aniPtr->nextAniName);
				file.BinWriteFloat(aniPtr->blendInSpeed);
				file.BinWriteFloat(aniPtr->blendOutSpeed);
				file.BinWriteLine(aniPtr->aniFlagsStr);
				file.BinWriteLine(aniPtr->aliasName);
				file.BinWriteLine(aniPtr->aniDirStr);
			}

			break;
		}
		case zMDL_ANI_TYPE_BLEND:
		{
			file.BinStartChunk(zFCHUNK_MDS_ANIBLEND);
			{
				file.BinWriteLine(aniPtr->aniName);
				file.BinWriteLine(aniPtr->nextAniName);
				file.BinWriteFloat(aniPtr->blendInSpeed);
				file.BinWriteFloat(aniPtr->blendOutSpeed);
			}

			WriteAniEventsMSB(file, aniPtr);

			break;
		}
		case zMDL_ANI_TYPE_SYNC:
		{
			file.BinStartChunk(zFCHUNK_MDS_ANISYNC);
			{
				file.BinWriteLine(aniPtr->aniName);
				file.BinWriteLine(aniPtr->nextAniName);
			}

			WriteAniEventsMSB(file, aniPtr);

			break;
		}
		case zMDL_ANI_TYPE_COMB:
		{
			file.BinStartChunk(zFCHUNK_MDS_ANICOMB);
			{
				file.BinWriteLine(aniPtr->aniName);
				file.BinWriteInt(aniPtr->layer);
				file.BinWriteLine(aniPtr->nextAniName);
				file.BinWriteFloat(aniPtr->blendInSpeed);
				file.BinWriteFloat(aniPtr->blendOutSpeed);
				file.BinWriteLine(aniPtr->aniFlagsStr);
				file.BinWriteLine(aniPtr->ascName);
				file.BinWriteInt(aniPtr->lastFrame);
			}

			WriteAniEventsMSB(file, aniPtr);

			break;
		}
		case zMDL_ANI_TYPE_DISABLED:
		{
			file.BinStartChunk(zFCHUNK_MDS_ANIDISABLE);
			{
				file.BinWriteLine(aniPtr->aniName);
			}

			WriteAniEventsMSB(file, aniPtr);

			break;
		}
		default:
		{
			break;
		}
		}
	}

#if HANDLE_MSB_CHILD_FILES
	for (int32 i = 0; i < modelAnis.numInArray; i++)
	{
		zCModelAni *modelAni = modelAnis[i];

		zSTRING aniName(modelProtoName + "-" + modelAni->aniName + ".MAN");

		modelAni->SaveMAN(file.file->path + "/" + aniName);
	}
#endif

	file.BinStartChunk(zFCHUNK_MDS_ANIENUM_END);

	file.BinEndChunk();
}

void zCModelPrototype::WriteModelMSB(zCFileBIN &file)
{
	file.BinStartChunk(zFCHUNK_MDS_MESHANDTREE);
	{
		file.BinWriteDWord(dontUseMesh);
		file.BinWriteLine(ascName);

#if HANDLE_MSB_CHILD_FILES
		hierarchy->SaveMDH(file.file->path + "/" + modelProtoName + ".MDH");

		if (!dontUseMesh)
		{
			zSTRING mdmName(ascName);
			if (mdmName.EndsWith(".ASC")) mdmName.DeleteRight(4);

			mesh->SaveMDM(file.file->path + "/" + ascName + ".MDM");
		}
#endif
	}

	for (int32 i = 0; i < ascNames.numInArray; i++)
	{
		file.BinStartChunk(zFCHUNK_MDS_REGISTERMESH);
		{
			file.BinWriteLine(ascNames[i]);

#if HANDLE_MSB_CHILD_FILES
			zSTRING mdmName(ascNames[i]);
			if (mdmName.EndsWith(".ASC")) mdmName.DeleteRight(4);

			meshList[i]->SaveMDM(file.file->path + "/" + mdmName + ".MDM");
#endif
		}
	}

	if (modelEvents.numInArray > 0 || writeMaxFps || protoAnis.numInArray > 0)
	{
		file.BinStartChunk(zFCHUNK_MDS_ANIENUM);
		{
			WriteAniEnumMSB(file);
		}
	}

	file.BinStartChunk(zFCHUNK_MDS_MODEL_END);

	file.BinEndChunk();
}

void zCModelPrototype::SaveModelScriptMSB(zCFileBIN &file)
{
	file.BinStartChunk(zFCHUNK_MODELSCRIPT);
	{
		file.BinWriteDWord(modelScriptVersionOut);
		file.BinWriteLine(ascPath);
	}

	file.BinStartChunk(zFCHUNK_MDS_SOURCE);
	{
		srcFileStats.SaveBIN(file);
	}

	file.BinStartChunk(zFCHUNK_MDS_MODEL);
	{
		WriteModelMSB(file);
	}

	file.BinStartChunk(zFCHUNK_MDS_END);

	file.BinEndChunk();
}

void zCModelPrototype::WriteAniEvents(zFILE &file, zCModelAni *aniPtr)
{
	if (aniPtr->numAniEvents > 0)
	{
		file.WriteLineIndented(numTabs, "{", TRUE);

		numTabs++;

		zSTRING line;

		for (int32 i = 0; i < aniPtr->numAniEvents; i++)
		{
			zCModelAniEvent *event = &aniPtr->aniEvents[i];

			switch (event->aniEventType)
			{
			case zMDL_EVENT_TAG:
			{
				line = "*EVENTTAG (";

				line.Append(zSTRING(event->frameNr));
				line.Append(" \"" + event->tagString + "\"");

				for (int32 j = 0; j < zMDL_ANIEVENT_MAXSTRING; j++)
				{
					if (event->string[j].Length() > 0)
					{
						line.Append(" \"" + event->string[j] + "\"");
					}
				}

				line.Append(")");

				file.WriteLineIndented(numTabs, line, TRUE);

				break;
			}
			case zMDL_EVENT_SOUND:
			{
				line = "*EVENTSFX (";

				line.Append(zSTRING(event->frameNr));
				line.Append(" \"" + event->sfxName + "\"");
					
				if (event->value1 != zMDL_DEFAULT_RADIUS_EVENTSFX) line.Append(" R:" + zSTRING(event->value1));
				if (event->value2 == 0.0f) line.Append(" EMPTY_SLOT");

				line.Append(")");

				file.WriteLineIndented(numTabs, line, TRUE);

				break;
			}
			case zMDL_EVENT_SOUND_GRND:
			{
				line = "*EVENTSFXGRND (";

				line.Append(zSTRING(event->frameNr));
				line.Append(" \"" + event->sfxName + "\"");

				if (event->value1 != zMDL_DEFAULT_RADIUS_EVENTSFXGRND) line.Append(" R:" + zSTRING(event->value1));
				if (event->value2 == 0.0f) line.Append(" EMPTY_SLOT");

				line.Append(")");

				file.WriteLineIndented(numTabs, line, TRUE);

				break;
			}
			case zMDL_EVENT_SWAPMESH:
			{
				line = "*EVENTSWAPMESH (";

				line.Append(zSTRING(event->frameNr));
				line.Append(" \"" + event->string[0] + "\"");
				line.Append(" \"" + event->string[1] + "\"");

				line.Append(")");

				file.WriteLineIndented(numTabs, line, TRUE);

				break;
			}
			case zMDL_EVENT_PFX:
			{
				line = "*EVENTPFX (";

				line.Append(zSTRING(event->frameNr));
				line.Append(" " + zSTRING(event->value1));
				line.Append(" \"" + event->emitterName + "\"");
				line.Append(" \"" + event->string[0] + "\"");
				if (event->value2 == 1.0f) line.Append(" ATTACH");

				line.Append(")");

				file.WriteLineIndented(numTabs, line, TRUE);

				break;
			}
			case zMDL_EVENT_PFX_GRND:
			{
				line = "*EVENTPFXGRND (";

				line.Append(zSTRING(event->frameNr));

				line.Append(")");

				file.WriteLineIndented(numTabs, line, TRUE);

				break;
			}
			case zMDL_EVENT_PFX_STOP:
			{
				line = "*EVENTPFXSTOP (";

				line.Append(zSTRING(event->frameNr));
				line.Append(" " + zSTRING(event->value1));

				line.Append(")");

				file.WriteLineIndented(numTabs, line, TRUE);

				break;
			}
			case zMDL_EVENT_SETMESH:
			{
				line = "*EVENTSETMESH (";

				line.Append(zSTRING(event->frameNr));
				line.Append(" \"" + event->string[0] + "\"");

				line.Append(")");

				file.WriteLineIndented(numTabs, line, TRUE);

				break;
			}
			case zMDL_EVENT_MM_STARTANI:
			{
				line = "*EVENTMMSTARTANI (";

				line.Append(zSTRING(event->frameNr));
				line.Append(" \"" + event->string[0] + "\"");
				if (event->string[1].Length() > 0) line.Append(" \"" + event->string[1] + "\"");
				if (event->value1 != 1.0f) line.Append(" I:" + zSTRING(event->value1));
				if (event->value2 != -2.0f) line.Append(" H:" + zSTRING(event->value2));

				line.Append(")");

				file.WriteLineIndented(numTabs, line, TRUE);

				break;
			}
			case zMDL_EVENT_CAM_TREMOR:
			{
				line = "*EVENTCAMTREMOR (";

				line.Append(zSTRING(event->frameNr));
				line.Append(" " + zSTRING(sqrtf(event->value1)));
				line.Append(" " + zSTRING(event->value2));
				line.Append(" " + zSTRING(event->value3));
				line.Append(" " + zSTRING(event->value4));

				line.Append(")");

				file.WriteLineIndented(numTabs, line, TRUE);

				break;
			}
			default:
			{
				break;
			}
			}
		}

		numTabs--;

		file.WriteLineIndented(numTabs, "}", TRUE);

		file.WriteLine(TRUE);
	}
}

void zCModelPrototype::WriteAniEnum(zFILE &file)
{
	numTabs++;

	zSTRING line;

	for (int32 i = 0; i < modelEvents.numInArray; i++)
	{
		zCModelAniEvent *event = modelEvents[i];

		line = "MODELTAG (\"" + event->tagString + "\"";

		for (int32 j = 0; j < zMDL_ANIEVENT_MAXSTRING; j++)
		{
			if (event->string[j].Length() > 0)
			{
				line.Append(" \"" + event->string[j] + "\"");
			}
		}

		line.Append(")");

		file.WriteLineIndented(numTabs, line, TRUE);
	}

	if (line.Length() > 0) file.WriteLine(TRUE);

	if (writeMaxFps)
	{
		file.WriteLineIndented(numTabs, "ANIMAXFPS (" + zSTRING(maxFps) + ")", TRUE);

		file.WriteLine(TRUE);
	}

	numTabs++;

	for (int32 i = 0; i < protoAnis.numInArray; i++)
	{
		zCModelAni *aniPtr = protoAnis[i];

		switch (aniPtr->aniType)
		{
		case zMDL_ANI_TYPE_NORMAL:
		{
			line = "ANI (";

			line.Append("\"" + aniPtr->aniName + "\"");
			line.Append(" " + zSTRING(aniPtr->layer));
			line.Append(" \"" + aniPtr->nextAniName + "\"");
			line.Append(" " + zSTRING(aniPtr->blendInSpeed));
			line.Append(" " + zSTRING(aniPtr->blendOutSpeed));
			line.Append(" " + aniPtr->aniFlagsStr);
			line.Append(" \"" + aniPtr->ascName + "\"");
			line.Append(" " + aniPtr->aniDirStr);
			line.Append(" " + zSTRING(aniPtr->firstFrame));
			line.Append(" " + zSTRING(aniPtr->lastFrame));
			if (aniPtr->maxFps != maxFps) line.Append(" FPS:" + zSTRING(aniPtr->maxFps));
			if (aniPtr->speed != 0.0f) line.Append(" SPD:" + zSTRING(aniPtr->speed));
			if (aniPtr->collisionVolumeScale != 1.0f) line.Append(" CVS:" + zSTRING(aniPtr->collisionVolumeScale));

			line.Append(")");

			file.WriteLineIndented(numTabs, line, TRUE);

			WriteAniEvents(file, aniPtr);

			break;
		}
		case zMDL_ANI_TYPE_ALIAS:
		{
			line = "ANIALIAS (";

			line.Append("\"" + aniPtr->aniName + "\"");
			line.Append(" " + zSTRING(aniPtr->layer));
			line.Append(" \"" + aniPtr->nextAniName + "\"");
			line.Append(" " + zSTRING(aniPtr->blendInSpeed));
			line.Append(" " + zSTRING(aniPtr->blendOutSpeed));
			line.Append(" " + aniPtr->aniFlagsStr);
			line.Append(" \"" + aniPtr->aliasName + "\"");
			line.Append(" " + aniPtr->aniDirStr);

			line.Append(")");

			file.WriteLineIndented(numTabs, line, TRUE);

			break;
		}
		case zMDL_ANI_TYPE_BLEND:
		{
			line = "ANIBLEND (";

			line.Append("\"" + aniPtr->aniName + "\"");
			line.Append(" \"" + aniPtr->nextAniName + "\"");
			line.Append(" " + zSTRING(aniPtr->blendInSpeed));
			line.Append(" " + zSTRING(aniPtr->blendOutSpeed));

			line.Append(")");

			file.WriteLineIndented(numTabs, line, TRUE);

			WriteAniEvents(file, aniPtr);

			break;
		}
		case zMDL_ANI_TYPE_SYNC:
		{
			line = "ANISYNC (";

			line.Append("\"" + aniPtr->aniName + "\"");
			line.Append(" \"" + aniPtr->nextAniName + "\"");

			line.Append(")");

			file.WriteLineIndented(numTabs, line, TRUE);

			WriteAniEvents(file, aniPtr);

			break;
		}
		case zMDL_ANI_TYPE_COMB:
		{
			line = "ANICOMB (";

			line.Append("\"" + aniPtr->aniName + "\"");
			line.Append(" " + zSTRING(aniPtr->layer));
			line.Append(" \"" + aniPtr->nextAniName + "\"");
			line.Append(" " + zSTRING(aniPtr->blendInSpeed));
			line.Append(" " + zSTRING(aniPtr->blendOutSpeed));
			line.Append(" " + aniPtr->aniFlagsStr);
			line.Append(" \"" + aniPtr->ascName + "\"");
			line.Append(" " + zSTRING(aniPtr->lastFrame));

			line.Append(")");

			file.WriteLineIndented(numTabs, line, TRUE);

			WriteAniEvents(file, aniPtr);

			break;
		}
		case zMDL_ANI_TYPE_DISABLED:
		{
			line = "ANIDISABLE (";

			line.Append("\"" + aniPtr->aniName + "\"");

			line.Append(")");

			file.WriteLineIndented(numTabs, line, TRUE);

			WriteAniEvents(file, aniPtr);

			break;
		}
		default:
		{
			break;
		}
		}
	}

	numTabs--;

	numTabs--;
}

void zCModelPrototype::WriteModel(zFILE &file)
{
	numTabs++;

	zSTRING line = "MESHANDTREE (\"" + ascName + ".ASC\"";

	if (dontUseMesh) line.Append(" DONT_USE_MESH");

	line.Append(")");

	file.WriteLineIndented(numTabs, line, TRUE);
	file.WriteLine(TRUE);

	for (int32 i = 0; i < ascNames.numInArray; i++)
	{
		file.WriteLineIndented(numTabs, "REGISTERMESH (\"" + ascNames[i] + "\")", TRUE);
	}

	file.WriteLine(TRUE);

	file.WriteLineIndented(numTabs, "ANIENUM", TRUE);
	file.WriteLineIndented(numTabs, "{", TRUE);

	WriteAniEnum(file);

	file.WriteLineIndented(numTabs, "}", TRUE);

	numTabs--;
}

void zCModelPrototype::SaveModelScript(zFILE &file)
{
	file.WriteLineIndented(numTabs, "MODEL (\"" + modelProtoName + "\")", TRUE);
	file.WriteLineIndented(numTabs, "{", TRUE);

	WriteModel(file);

	file.WriteLineIndented(numTabs, "}", TRUE);
}

bool32 zCModelPrototype::LoadMDL(zCFileBIN &file)
{
	hierarchy = zNEW(zCModelHierarchy);

	if (!hierarchy->LoadMDH(file))
	{
		printf("Error while loading MDH!\n");

		return FALSE;
	}

	mesh = zNEW(zCModelMeshLib);

	if (!mesh->LoadMDM(file))
	{
		printf("Error while loading MDM!\n");

		return FALSE;
	}

	return TRUE;
}

void zCModelPrototype::SaveMDL(zCFileBIN &file)
{
	hierarchy->SaveMDH(file);

	mesh->SaveMDM(file);
}

bool32 zCMeshSoftSkin::Load(zCFileBIN &file)
{
	uint16 id;
	int32 len;

	while (!file.BinEof())
	{
		file.BinOpenChunk(id, len);

		switch (id)
		{
		case zFCHUNK_MESHSOFTSKIN:
		{
			uint32 vers;
			file.BinReadDWord(vers);

			if (vers != meshSoftSkinVersionIn)
			{
				printf("Wrong zCMeshSoftSkin version!\n");

				return FALSE;
			}

			if (!LoadMRM(file))
			{
				printf("Error loading MRM!\n");

				return FALSE;
			}

			file.BinReadDWord(size);
			vertWeightStream = zMALLOC<byte>(size);
			file.BinRead(vertWeightStream, size);

			file.BinReadDWord(numNodeWedgeNormals);
			nodeWedgeNormalList = zMALLOC<zTNodeWedgeNormal>(numNodeWedgeNormals);
			file.BinRead(nodeWedgeNormalList, numNodeWedgeNormals * sizeof(zTNodeWedgeNormal));

			file.BinReadWord(numNodes);
			nodeIndexList = zMALLOC<int32>(numNodes);
			file.BinRead(nodeIndexList, numNodes * sizeof(int32));

			nodeOBBList.AllocAbs(numNodes);

			for (uint16 i = 0; i < numNodes; i++)
			{
				zCOBBox3D *obb = zNEW(zCOBBox3D);
				obb->LoadBIN(file);
				nodeOBBList.InsertEnd(obb);
			}

			break;
		}
		case zFCHUNK_MESHSOFTSKIN_END:
		{
			goto fileEnd;

			break;
		}
		default:
		{
			printf("Unknown chunk encountered while parsing zCMeshSoftSkin! %d\n", file.BinPos());

			return FALSE;

			break;
		}
		}
	}

fileEnd:;

	return TRUE;
}

void zCMeshSoftSkin::Save(zCFileBIN &file)
{
	file.BinStartChunk(zFCHUNK_MESHSOFTSKIN);
	{
		file.BinWriteDWord(meshSoftSkinVersionOut);

		SaveMRM(file);

		file.BinWriteDWord(size);
		file.BinWrite(vertWeightStream, size);

		file.BinWriteDWord(numNodeWedgeNormals);
		file.BinWrite(nodeWedgeNormalList, numNodeWedgeNormals * sizeof(zTNodeWedgeNormal));

		file.BinWriteWord(numNodes);
		file.BinWrite(nodeIndexList, numNodes * sizeof(int32));

		for (uint16 i = 0; i < numNodes; i++)
		{
			zCOBBox3D *obb = nodeOBBList[i];
			obb->SaveBIN(file);
		}
	}

	file.BinStartChunk(zFCHUNK_MESHSOFTSKIN_END);

	file.BinEndChunk();
}

bool32 zCModelMeshLib::LoadMDM(zCFileBIN &file)
{
	uint16 id;
	int32 len;

	while (!file.BinEof())
	{
		file.BinOpenChunk(id, len);

		switch (id)
		{
		case zFCHUNK_MODELMESH:
		{
			uint32 vers;
			file.BinReadDWord(vers);

			if (vers != modelMeshVersionIn
				&& ((modelMeshVersionIn == MODELMESH_VERSION_GOTHIC_1_30 && vers != MODELMESH_VERSION_GOTHIC_OLD1)
				&& (modelMeshVersionIn == MODELMESH_VERSION_GOTHIC_1_30 && vers != MODELMESH_VERSION_GOTHIC_OLD2)))
			{
				printf("Wrong MDM version!\n");

				return FALSE;
			}

			if (vers == MODELMESH_VERSION_GOTHIC_OLD1 || vers == MODELMESH_VERSION_GOTHIC_OLD2)
			{
				printf("Loading outdated MDM!\n");
			}

			break;
		}
		case zFCHUNK_MDM_SOURCE:
		{
			srcFileStats.LoadBIN(file);

			break;
		}
		case zFCHUNK_MDM_NODEMESHES:
		{
			file.BinReadWord(numNodeMeshes);

			nodeMeshNames.AllocAbs(numNodeMeshes);

			for (uint16 i = 0; i < numNodeMeshes; i++)
			{
				zSTRING nodeName;
				file.BinReadLine(nodeName);
				nodeMeshNames.InsertEnd(nodeName);
			}

			nodeMeshList = zNEW_ARRAY(zCProgMeshProto, numNodeMeshes);

			for (uint16 i = 0; i < numNodeMeshes; i++)
			{
				zCProgMeshProto *pmesh = &nodeMeshList[i];

				if (!pmesh->LoadMRM(file))
				{
					printf("Error loading MRM!\n");

					return FALSE;
				}
			}

			break;
		}
		case zFCHUNK_MDM_SOFSKINLIST:
		{
			file.BinReadDWord(checkSum);

			file.BinReadWord(numSoftSkins);

			softSkinList = zNEW_ARRAY(zCMeshSoftSkin, numSoftSkins);

			for (uint16 i = 0; i < numSoftSkins; i++)
			{
				zCMeshSoftSkin *mesh = &softSkinList[i];

				if (!mesh->Load(file))
				{
					printf("Error loading zCMeshSoftSkin!\n");

					return FALSE;
				}
			}

			break;
		}
		case zFCHUNK_MDM_END:
		{
			goto fileEnd;

			break;
		}
		default:
		{
			printf("Unknown chunk encountered while parsing MDM! %d\n", file.BinPos());

			return FALSE;

			break;
		}
		}
	}

fileEnd:;

	return TRUE;
}

void zCModelMeshLib::SaveMDM(zCFileBIN &file)
{
	file.BinStartChunk(zFCHUNK_MODELMESH);
	{
		file.BinWriteDWord(modelMeshVersionOut);
	}

	file.BinStartChunk(zFCHUNK_MDM_SOURCE);
	{
		srcFileStats.SaveBIN(file);
	}

	if (numNodeMeshes > 0)
	{
		file.BinStartChunk(zFCHUNK_MDM_NODEMESHES);
		{
			file.BinWriteWord(numNodeMeshes);

			for (uint16 i = 0; i < numNodeMeshes; i++)
			{
				file.BinWriteLine(nodeMeshNames[i]);
			}

			for (uint16 i = 0; i < numNodeMeshes; i++)
			{
				zCProgMeshProto *pmesh = &nodeMeshList[i];

				pmesh->SaveMRM(file);
			}
		}
	}

	if (numSoftSkins > 0)
	{
		file.BinStartChunk(zFCHUNK_MDM_SOFSKINLIST);
		{
			file.BinWriteDWord(checkSum);

			file.BinWriteWord(numSoftSkins);

			for (uint16 i = 0; i < numSoftSkins; i++)
			{
				zCMeshSoftSkin *mesh = &softSkinList[i];

				mesh->Save(file);
			}
		}
	}

	file.BinStartChunk(zFCHUNK_MDM_END);

	file.BinEndChunk();
}

bool32 zCModelHierarchy::LoadMDH(zCFileBIN &file)
{
	SetModelHierarchyName(file.file->filename);

	uint16 id;
	int32 len;

	while (!file.BinEof())
	{
		file.BinOpenChunk(id, len);

		switch (id)
		{
		case zFCHUNK_MODELHIERARCHY:
		{
			uint32 vers;
			file.BinReadDWord(vers);

			if (vers != modelHierarchyVersionIn)
			{
				printf("Wrong MDH version!\n");

				return FALSE;
			}

			file.BinReadWord(numNodes);
			nodeList = zNEW_ARRAY(zCModelNode, numNodes);

			for (uint16 i = 0; i < numNodes; i++)
			{
				zCModelNode *mnode = &nodeList[i];

				file.BinReadLine(mnode->nodeName);
				file.BinReadWord(mnode->parentIndex);
				file.BinRead(&mnode->trafo, sizeof(mnode->trafo));
			}

			bbox3D.LoadBIN(file);
			bbox3DCollDet.LoadBIN(file);

			file.BinRead(&rootNodeTrans, sizeof(rootNodeTrans));

			file.BinReadDWord(nodeListChecksum);

			break;
		}
		case zFCHUNK_MDH_SOURCE:
		{
			srcFileStats.LoadBIN(file);

			break;
		}
		case zFCHUNK_MDH_END:
		{
			goto fileEnd;

			break;
		}
		default:
		{
			printf("Unknown chunk encountered while parsing MDH!\n");

			return FALSE;

			break;
		}
		}
	}
	
fileEnd:;

	return TRUE;
}

void zCModelHierarchy::SaveMDH(zCFileBIN &file)
{
	file.BinStartChunk(zFCHUNK_MODELHIERARCHY);
	{
		file.BinWriteDWord(modelHierarchyVersionOut);

		file.BinWriteWord(numNodes);

		for (uint16 i = 0; i < numNodes; i++)
		{
			zCModelNode *mnode = &nodeList[i];

			file.BinWriteLine(mnode->nodeName);
			file.BinWriteWord(mnode->parentIndex);
			file.BinWrite(&mnode->trafo, sizeof(mnode->trafo));
		}

		bbox3D.SaveBIN(file);
		bbox3DCollDet.SaveBIN(file);

		file.BinWrite(&rootNodeTrans, sizeof(rootNodeTrans));

		file.BinWriteDWord(nodeListChecksum);
	}

	file.BinStartChunk(zFCHUNK_MDH_SOURCE);
	{
		srcFileStats.SaveBIN(file);
	}

	file.BinStartChunk(zFCHUNK_MDH_END);

	file.BinEndChunk();
}
