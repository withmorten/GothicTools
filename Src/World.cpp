#include "GothicTools.h"

bool32 oCWorld::ReadHeader(zCFileBIN &file)
{
	zSTRING s;

	file.BinReadLine(s); // ZenGin Archive
	file.BinReadLine(s); // ver 1
	file.BinReadLine(s); // zCArchiverGeneric / zCArchiverBinSafe

	file.BinReadLine(s); // ASCII / BIN_SAFE

	if (s == "BIN_SAFE")
	{
		printf("Not an ASCII Zen file\n");
		return FALSE;
	}
	else if (s != "ASCII")
	{
		printf("Not a Zen file\n");
		return FALSE;
	}

	file.BinReadLine(s); // saveGame 0
	file.BinReadLine(dateStr); // date

	file.BinReadLine(userStr); // user

	if (userStr == "user XZEN" && meshAndBspVersionIn != BSPMESH_VERSION_GOTHIC_1_01) xZenIn = TRUE;

	file.BinReadLine(s); // END;

	file.BinReadLine(objectsStr); // objects
	file.BinReadLine(s); // END
	file.BinReadLine(s); //

	return TRUE;
}

void oCWorld::ReadVobTree(zCFileBIN &file)
{
	zSTRING *line = NULL;
	bool32 inVob = FALSE;

	do
	{
		zCVob *vob = NULL;
		line = zNEW(zSTRING);
		file.BinReadLine(*line);

		if (!inVob && (line->Contains("zCVob ") || line->Contains("% § 0 ")))
		{
			inVob = TRUE;

			vob = zNEW(zCVob);
			vob->lines.Insert(line);
			vobTree.Insert(vob);
		}
		else if (inVob)
		{
			vob = vobTree[vobTree.numInArray - 1];
			vob->lines.Insert(line);

			if (*line == "\t\t[]") inVob = FALSE;
		}
		else if (!inVob && line->Contains("childs"))
		{
			vob = vobTree[vobTree.numInArray - 1];
			vob->lines.Insert(line);
		}
	}
	while (*line != "\t[]");
}

void oCWorld::ReadWayNet(zCFileBIN &file)
{
	zSTRING *line = NULL;

	do
	{
		line = zNEW(zSTRING);
		file.BinReadLine(*line);

		wayNet.Insert(line);
	}
	while (*line != "\t[]");

	wayNet.RemoveLast();
}

bool32 oCWorld::LoadZEN(zCFileBIN &file)
{
	if (!ReadHeader(file)) return FALSE;

	zSTRING s;

	file.BinReadLine(worldStr); // oCWorld:zCWorld
	file.BinReadLine(s); // MeshAndBsp or VobTree

	if (s.Contains("MeshAndBsp"))
	{
		if (deCompile)
		{
			compiled = FALSE;

			zDWORD version;
			zDWORD chunkLen;

			file.BinReadDWord(version);
			file.BinReadDWord(chunkLen);

			file.BinSeekRel(chunkLen);

			printf("Skipping BspTree\n");
		}
		else
		{
			compiled = TRUE;

			meshAndBspStr = s;

			bspTree = zNEW(zCBspTree);

			if (!bspTree->LoadBIN(file))
			{
				printf("BspTree could not be loaded\n");

				return FALSE;
			}
		}

		file.BinReadLine(s); // []

		file.BinReadLine(vobTreeStr); // VobTree
	}
	else
	{
		compiled = FALSE;

		vobTreeStr = s; // VobTree
	}

	file.BinReadLine(vobTreeChildsStr); // childs
	ReadVobTree(file);

	file.BinReadLine(wayNetStr); // WayNet
	ReadWayNet(file);

	file.BinReadLine(s); // EndMarker
	file.BinReadLine(s); // []

	file.BinReadLine(s); // []

	return TRUE;
}

void oCWorld::WriteHeader(zCFileBIN &file)
{
	file.BinWriteLine("ZenGin Archive");
	file.BinWriteLine("ver 1");
	file.BinWriteLine("zCArchiverGeneric");
	file.BinWriteLine("ASCII");
	file.BinWriteLine("saveGame 0");
	file.BinWriteLine(dateStr);

	if (xZenOut)
	{
		file.BinWriteLine("user XZEN");
	}
	else
	{
		if (userStr == "user XZEN") file.BinWriteLine("user GothicZEN");
		else file.BinWriteLine(userStr);
	}

	file.BinWriteLine("END");
	file.BinWriteLine(objectsStr);
	file.BinWriteLine("END");
	file.BinWriteLine();
}

zUINT CountTabsInLine(const zSTRING &line)
{
	zUINT numTabs = 0;

	for (zSIZE_T i = 0; i < line.Length(); i++)
	{
		if (line[i] == '\t')
		{
			numTabs++;
		}
	}

	return numTabs;
}

void oCWorld::WriteVobTree(zCFileBIN &file)
{
	bool32 sameVersion = meshAndBspVersionIn == meshAndBspVersionOut;

	for (int32 i = 0; i < vobTree.numInArray; i++)
	{
		zCVob *vob = vobTree[i];

		for (int32 j = 0; j < vob->lines.numInArray; j++)
		{
			zSTRING *line = vob->lines[j];

			if (!sameVersion)
			{
				// Remove some data
				if (meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_01)
				{
					// zCVob
					{
						if (line->Contains("pack=int:"))
							continue;
					}
				}

				if (meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_01 || meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_04)
				{
					// zCVob
					{
						if (line->Contains("visualAniMode=enum:") || line->Contains("visualAniModeStrength=float:") || line->Contains("vobFarClipZScale=float:") ||
							line->Contains("zbias=int:") || line->Contains("isAmbient=bool:"))
							continue;
					}

					// zCDecal
					{
						if (line->Contains("decalAlphaWeight=int:") || line->Contains("ignoreDayLight=bool:"))
							continue;
					}

					// zCMover
					{
						if (line->Contains("autoRotate=bool:"))
							continue;
					}

					// zCZoneFog
					{
						if (line->Contains("fadeOutSky=bool:") || line->Contains("overrideColor=bool:"))
							continue;
					}

					// zCVobLightData
					{
						if (line->Contains("canMove=bool:"))
							continue;
					}
				}

				if (meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_04 || meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_30)
				{
					// oCMobInter
					{
						if (line->Contains("state=int:") || line->Contains("stateTarget=int:"))
							continue;
					}
				}
			}

			file.BinWriteLine(line);

			if (!sameVersion)
			{
				zUINT numTabs = CountTabsInLine(*line);

				// Add some data
				if (meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_01)
				{
					// oCMobInter
					{
						if (line->Contains("isDestroyed=bool:"))
						{
							file.BinWriteLineIndented(numTabs, "state=int:0");
							file.BinWriteLineIndented(numTabs, "stateTarget=int:0");
						}
					}
				}

				if (meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_30)
				{
					// zCVob
					{
						if (line->Contains("visualCamAlign=enum:"))
						{
							file.BinWriteLineIndented(numTabs, "visualAniMode=enum:0");
							file.BinWriteLineIndented(numTabs, "visualAniModeStrength=float:0");
							file.BinWriteLineIndented(numTabs, "vobFarClipZScale=float:1");
						}

						if (line->Contains("dynShadow=enum:"))
						{
							file.BinWriteLineIndented(numTabs, "zbias=int:0");
							file.BinWriteLineIndented(numTabs, "isAmbient=bool:0");
						}
					}

					// zCDecal
					{
						if (line->Contains("decalTexAniFPS=float:"))
						{
							file.BinWriteLineIndented(numTabs, "decalAlphaWeight=int:255");
							file.BinWriteLineIndented(numTabs, "ignoreDayLight=bool:0");
						}
					}

					// zCMover
					{
						if (line->Contains("autoLinkEnabled=bool:"))
						{
							file.BinWriteLineIndented(numTabs, "autoRotate=bool:0");
						}
					}

					// zCZoneFog
					{
						if (line->Contains("fogColor=color:"))
						{
							file.BinWriteLineIndented(numTabs, "fadeOutSky=bool:0");
							file.BinWriteLineIndented(numTabs, "overrideColor=bool:0");
						}
					}

					// zCVobLightData
					{
						if (line->Contains("colorAniSmooth=bool:"))
						{
							file.BinWriteLineIndented(numTabs, "canMove=bool:0");
						}
					}
				}
			}
		}
	}
}

void oCWorld::WriteWayNet(zCFileBIN &file)
{
	for (int32 i = 0; i < wayNet.numInArray; i++)
	{
		file.BinWriteLine(wayNet[i]);
	}
}

void oCWorld::SaveZEN(zCFileBIN &file)
{
	WriteHeader(file);

	file.BinWriteLine(worldStr);

	if (compiled)
	{
		file.BinWriteLine(meshAndBspStr);

		bspTree->SaveBIN(file);

		file.BinWriteLine("\t[]");
	}

	file.BinWriteLine(vobTreeStr);
	file.BinWriteLine(vobTreeChildsStr);
	WriteVobTree(file);
	file.BinWriteLine("\t[]");

	file.BinWriteLine(wayNetStr);
	WriteWayNet(file);
	file.BinWriteLine("\t[]");

	file.BinWriteLine("\t[EndMarker % 0 0]");
	file.BinWriteLine("\t[]");

	file.BinWriteLine("[]");
}