#include "GothicTools.h"

zVOID zCModelAniEvent::Load(zCFileBIN &file)
{
	file.BinReadEnum(aniEventType);
	file.BinReadInt(frameNr);
	file.BinReadLine(tagString);

	for (zINT i = 0; i < zMDL_ANIEVENT_MAXSTRING; i++)
	{
		file.BinReadLine(string[i]);
	}

	file.BinReadFloat(value1);
	file.BinReadFloat(value2);
	file.BinReadFloat(value3);
	file.BinReadFloat(value4);

	file.BinReadFloat(prob);
}

zVOID zCModelAniEvent::Save(zCFileBIN &file)
{
	file.BinWriteEnum(aniEventType);
	file.BinWriteInt(frameNr);
	file.BinWriteLine(tagString);

	for (zINT i = 0; i < zMDL_ANIEVENT_MAXSTRING; i++)
	{
		file.BinWriteLine(string[i]);
	}

	file.BinWriteFloat(value1);
	file.BinWriteFloat(value2);
	file.BinWriteFloat(value3);
	file.BinWriteFloat(value4);

	file.BinWriteFloat(prob);
}

zVOID zCModelAni::InitValues()
{
	nodeIndexList = NULL;
	aniSampleMatrix = NULL;
	aniEvents = NULL;

	numAniEvents = 0;
	numNodes = 0;
	numFrames = 0;
	numSamples = 0;
}

zBOOL zCModelAni::LoadMAN(zCFileBIN &file)
{
	zWORD id;
	zLONG len;

	while (!file.BinEof())
	{
		file.BinOpenChunk(id, len);

		switch (id)
		{
		case zFCHUNK_MODELANI:
		{
			break;
		}
		case zFCHUNK_MAN_HEADER:
		{
			zWORD vers;
			file.BinReadWord(vers);

			if (vers != modelAnimationVersionIn)
			{
				printf("MAN version incorrect!\n");

				return FALSE;
			}

			file.BinReadLine(aniName);

			file.BinReadInt(layer);
			file.BinReadInt(numFrames);
			file.BinReadInt(numNodes);
			file.BinReadFloat(fpsRate);
			file.BinReadFloat(fpsRateSource);
			file.BinReadFloat(samplePosRangeMin);
			file.BinReadFloat(samplePosScaler);

			aniBBox3DObjSpace.LoadBIN(file);

			file.BinReadLine(nextAniName);

			break;
		}
		case zFCHUNK_MAN_SOURCE:
		{
			srcFileStats.LoadBIN(file);

			file.BinReadLine(scriptDefLine);

			break;
		}
		case zFCHUNK_MAN_ANIEVENTS:
		{
			file.BinReadInt(numAniEvents);

			aniEvents = zNEW_ARRAY(zCModelAniEvent, numAniEvents);

			for (zINT i = 0; i < numAniEvents; i++)
			{
				zCModelAniEvent *event = &aniEvents[i];

				event->Load(file);
			}

			break;
		}
		case zFCHUNK_MAN_RAWDATA:
		{
			file.BinReadDWord(checksum);

			nodeIndexList = zMALLOC<zINT>(numNodes);
			file.BinRead(nodeIndexList, sizeof(zINT) * numNodes);

			numSamples = numFrames * numNodes;

			if (numSamples)
			{
				aniSampleMatrix = zMALLOC<zTMdl_AniSample>(numSamples);
				file.BinRead(aniSampleMatrix, sizeof(zTMdl_AniSample) * numSamples);
			}

			break;
		}
		default:
		{
			printf("Unknown chunk encountered while parsing MAN!\n");

			return FALSE;

			break;
		}
		}
	}

	return TRUE;
}

zVOID zCModelAni::SaveMAN(zCFileBIN &file)
{
	file.BinStartChunk(zFCHUNK_MODELANI);

	file.BinStartChunk(zFCHUNK_MAN_HEADER);
	{
		file.BinWriteWord(modelAnimationVersionOut);

		file.BinWriteLine(aniName);

		file.BinWriteInt(layer);
		file.BinWriteInt(numFrames);
		file.BinWriteInt(numNodes);
		file.BinWriteFloat(fpsRate);
		file.BinWriteFloat(fpsRateSource);
		file.BinWriteFloat(samplePosRangeMin);
		file.BinWriteFloat(samplePosScaler);

		aniBBox3DObjSpace.SaveBIN(file);

		file.BinWriteLine(nextAniName);
	}

	file.BinStartChunk(zFCHUNK_MAN_SOURCE);
	{
		srcFileStats.SaveBIN(file);

		file.BinWriteLine(scriptDefLine);
	}

	if (numAniEvents > 0)
	{
		file.BinStartChunk(zFCHUNK_MAN_ANIEVENTS);
		{
			file.BinWriteInt(numAniEvents);

			for (zINT i = 0; i < numAniEvents; i++)
			{
				zCModelAniEvent *event = &aniEvents[i];

				event->Save(file);
			}
		}
	}

	file.BinStartChunk(zFCHUNK_MAN_RAWDATA);
	{
		file.BinWriteDWord(checksum);

		file.BinWrite(nodeIndexList, sizeof(zINT) * numNodes);

		if (numSamples)
		{
			file.BinWrite(aniSampleMatrix, sizeof(zTMdl_AniSample) * numSamples);
		}
	}

	file.BinEndChunk();
}
