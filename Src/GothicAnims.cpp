#include "GothicTools.h"

int main(int argc, const char **argv)
{
	if (argc < 5)
	{
		printf("GothicAnims v0.1 by withmorten\n\n");
		printf("GothicAnims can convert an entire anims or meshes directory from 1.08k to 2.6 and vice versa\n\n");
		printf("usage:\n");
		printf("GothicAnims <filetype> <version_in> <version_out> <folder_in> <folder_out>\n\n");
		printf("for <filetype>, use \"anims\" or \"a\" to convert animations, \"meshes\" or \"m\" to convert meshes\n\n");
		printf("version in/out can be 108 (1.08k) or 130 (1.30) / 26 (2.6f)\n\n");
		printf("the animation and mesh formats are the same from 1.01d (101) to 1.08k (108), and probably 1.12f (112)\n");

		return zOK;
	}

	switch (atoi(argv[2])) // version in
	{
	case 101:
	case 104:
	case 108:
	case 112:
		meshAndBspVersionIn = BSPMESH_VERSION_GOTHIC_1_04;
		meshVersionIn = MESH_VERSION_GOTHIC_1_04;
		bspVersionIn = BSP_VERSION_GOTHIC_1_04;
		materialVersionIn = MATERIAL_VERSION_SUM_GOTHIC_1_04;

		morphMeshVersionIn = MORPHMESH_VERSION_GOTHIC_1_04;
		progMeshVersionIn = PROGMESH_VERSION_GOTHIC_1_04;
		meshSoftSkinVersionIn = MESHSOFTSKIN_FILE_VERSION_GOTHIC_1_04;
		modelHierarchyVersionIn = MODELHIERARCHY_VERSION_GOTHIC_1_04;
		modelMeshVersionIn = MODELMESH_VERSION_GOTHIC_1_04;
		modelAnimationVersionIn = MAN_FILE_VERSION_GOTHIC_1_04;

		break;
	case 130:
	case 26:
		meshAndBspVersionIn = BSPMESH_VERSION_GOTHIC_1_30;
		meshVersionIn = MESH_VERSION_GOTHIC_1_30;
		bspVersionIn = BSP_VERSION_GOTHIC_1_30;
		materialVersionIn = MATERIAL_VERSION_SUM_GOTHIC_1_30;

		morphMeshVersionIn = MORPHMESH_VERSION_GOTHIC_1_30;
		progMeshVersionIn = PROGMESH_VERSION_GOTHIC_1_30;
		meshSoftSkinVersionIn = MESHSOFTSKIN_FILE_VERSION_GOTHIC_1_30;
		modelHierarchyVersionIn = MODELHIERARCHY_VERSION_GOTHIC_1_30;
		modelMeshVersionIn = MODELMESH_VERSION_GOTHIC_1_30;
		modelAnimationVersionIn = MAN_FILE_VERSION_GOTHIC_1_30;
		modelScriptVersionIn = MSB_FILE_VERSION_GOTHIC_1_30;

		break;
	default:
		printf("Unrecognized version in, needs to be 101/104/108 or 130/26\n");

		return zERROR;

		break;
	}

	switch (atoi(argv[3])) // version out
	{
	case 101:
	case 104:
	case 108:
	case 112:
		meshAndBspVersionOut = BSPMESH_VERSION_GOTHIC_1_04;
		meshVersionOut = MESH_VERSION_GOTHIC_1_04;
		bspVersionOut = BSP_VERSION_GOTHIC_1_04;
		materialVersionOut = MATERIAL_VERSION_SUM_GOTHIC_1_04;

		morphMeshVersionOut = MORPHMESH_VERSION_GOTHIC_1_04;
		progMeshVersionOut = PROGMESH_VERSION_GOTHIC_1_04;
		meshSoftSkinVersionOut = MESHSOFTSKIN_FILE_VERSION_GOTHIC_1_04;
		modelHierarchyVersionOut = MODELHIERARCHY_VERSION_GOTHIC_1_04;
		modelMeshVersionOut = MODELMESH_VERSION_GOTHIC_1_04;
		modelAnimationVersionOut = MAN_FILE_VERSION_GOTHIC_1_04;

		break;
	case 130:
	case 26:
		meshAndBspVersionOut = BSPMESH_VERSION_GOTHIC_1_30;
		meshVersionOut = MESH_VERSION_GOTHIC_1_30;
		bspVersionOut = BSP_VERSION_GOTHIC_1_30;
		materialVersionOut = MATERIAL_VERSION_SUM_GOTHIC_1_30;

		morphMeshVersionOut = MORPHMESH_VERSION_GOTHIC_1_30;
		progMeshVersionOut = PROGMESH_VERSION_GOTHIC_1_30;
		meshSoftSkinVersionOut = MESHSOFTSKIN_FILE_VERSION_GOTHIC_1_30;
		modelHierarchyVersionOut = MODELHIERARCHY_VERSION_GOTHIC_1_30;
		modelMeshVersionOut = MODELMESH_VERSION_GOTHIC_1_30;
		modelAnimationVersionOut = MAN_FILE_VERSION_GOTHIC_1_30;
		modelScriptVersionOut = MSB_FILE_VERSION_GOTHIC_1_30;

		break;
	default:
		printf("Unrecognized version out, needs to be 101/104/108 or 130/26\n");

		return zERROR;

		break;
	}

	zCScanDir scDir;

	if ((argv[1][1] == '\0' && argv[1][0] == 'a') || !stricmp(argv[1], "anims")) // compiled type
	{
		scDir.ScanDirectory(argv[4]); // folder in

		for (zINT i = 0; i < scDir.files.numInArray; i++)
		{
			zCFileBIN file_in(scDir.files[i]);

			zSTRING extension = file_in.file->extension;
			extension.Upper();

			if (extension == "MDS")
			{
				// If gothic 1, don't do anything
				// If gothic 2, check if msb exists, if not, parse mds and write msb
				// also check if mds is newer than msb, only THEN write

				if (meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_30)
				{
					zSTRING msbName = file_in.file->filename + ".MSB";
					msbName.Upper();

					zSTRING msbPath = scDir.Find(msbName);

					zBOOL writeMsb = TRUE;

					if (msbPath.Length() > 0)
					{
						writeMsb = FALSE;

						if (zDATE::IsFileNewer(scDir.files[i], msbPath))
						{
							writeMsb = TRUE;
						}
					}
					else
					{
						msbPath = argv[5] + zSTRING("/_compiled/") + msbName; // folder out
					}

					if (writeMsb)
					{
						zCModelPrototype *mds = zNEW(zCModelPrototype);

						if (mds->LoadModelScript(*file_in.file))
						{
							zCFileBIN file_out(msbPath, TRUE);

							mds->SaveModelScriptMSB(file_out);

							printf("Saved %s\n", file_out.file->basename.ToChar());
						}
						else
						{
							printf("%s could not be loaded\n", file_in.file->basename.ToChar());

							return zERROR;
						}

						zDELETE(mds);
					}
				}
			}
			else if (extension == "MSB")
			{
				// Won't happen in gothic 1
				// If gothic 2 and converting to gothic 1, check if mds exists, if not, parse msb and write mds
				// also check if msb is older than mds, only THEN write

				// TODO: Check line 196 in G2 Humans_1hST1.mds ... comment at the end of the line, does it get ignored by game or not? yes it does!
				// also problematic: fireplace_ground, fireplace_high

				if (meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_04)
				{
					zSTRING mdsName = file_in.file->filename + ".MDS";
					mdsName.Upper();

					zSTRING mdsPath = scDir.Find(mdsName);

					zBOOL writeMds = TRUE;

					if (mdsPath.Length() > 0)
					{
						writeMds = FALSE;

						if (zDATE::IsFileNewer(scDir.files[i], mdsPath))
						{
							writeMds = TRUE;
						}
					}

					if (writeMds)
					{
						zCModelPrototype *msb = zNEW(zCModelPrototype);

						if (msb->LoadModelScriptMSB(file_in))
						{
							if (!mdsPath.Length())
							{
								mdsPath = argv[5]; // folder out

								if (msb->srcFileStats.fileName.Contains("MDS_MOBSI")) mdsPath.Append("/MDS_MOBSI");
								else if (msb->srcFileStats.fileName.Contains("MDS_OVERLAY")) mdsPath.Append("/MDS_OVERLAY");

								mdsPath.Append("/" + mdsName);
							}

							zCFileBIN file_out(mdsPath, TRUE);

							msb->SaveModelScript(mdsPath);

							printf("Saved %s\n", file_out.file->basename.ToChar());
						}
						else
						{
							printf("%s could not be loaded\n", file_in.file->basename.ToChar());

							return zERROR;
						}
					}
				}
				else if (meshAndBspVersionOut == BSPMESH_VERSION_GOTHIC_1_30)
				{
					zCModelPrototype *msb = zNEW(zCModelPrototype);

					if (msb->LoadModelScriptMSB(file_in))
					{
						zCFileBIN file_out(argv[5] + zSTRING("/_compiled/") + file_in.file->filename + "." + file_in.file->extension, TRUE); // folder out

						msb->SaveModelScriptMSB(file_out);

						printf("Saved %s\n", file_out.file->basename.ToChar());
					}
					else
					{
						printf("%s could not be loaded\n", file_in.file->basename.ToChar());

						return zERROR;
					}

					zDELETE(msb);
				}
			}
			else if (extension == "MMB")
			{
				zCMorphMeshProto *mmb = zNEW(zCMorphMeshProto);

				if (mmb->LoadMMB(file_in))
				{
					if (argc > 5)
					{
						zCFileBIN file_out(argv[5] + zSTRING("/_compiled/") + file_in.file->filename + "." + file_in.file->extension, TRUE); // folder out

						mmb->SaveMMB(file_out);

						printf("Saved %s\n", file_out.file->basename.ToChar());
					}
					else
					{
						printf("%s OK\n", file_in.file->basename.ToChar());
					}
				}
				else
				{
					printf("%s could not be loaded\n", file_in.file->basename.ToChar());

					return zERROR;
				}

				zDELETE(mmb);
			}
			else if (extension == "MDL")
			{
				zCModelPrototype *mdl = zNEW(zCModelPrototype);

				if (mdl->LoadMDL(file_in))
				{
					if (argc > 5)
					{
						zCFileBIN file_out(argv[5] + zSTRING("/_compiled/") + file_in.file->filename + "." + file_in.file->extension, TRUE); // folder out

						mdl->SaveMDL(file_out);

						printf("Saved %s\n", file_out.file->basename.ToChar());
					}
					else
					{
						printf("%s OK\n", file_in.file->basename.ToChar());
					}
				}
				else
				{
					printf("%s could not be loaded\n", file_in.file->basename.ToChar());

					return zERROR;
				}

				zDELETE(mdl);
			}
			else if (extension == "MDH")
			{
				zCModelHierarchy *mdh = zNEW(zCModelHierarchy);

				if (mdh->LoadMDH(file_in))
				{
					if (argc > 5)
					{
						zCFileBIN file_out(argv[5] + zSTRING("/_compiled/") + file_in.file->filename + "." + file_in.file->extension, TRUE); // folder out

						mdh->SaveMDH(file_out);

						printf("Saved %s\n", file_out.file->basename.ToChar());
					}
					else
					{
						printf("%s OK\n", file_in.file->basename.ToChar());
					}
				}
				else
				{
					printf("%s could not be loaded\n", file_in.file->basename.ToChar());

					return zERROR;
				}

				zDELETE(mdh);
			}
			else if (extension == "MDM")
			{
				zCModelMesh *mdm = zNEW(zCModelMesh);

				if (mdm->LoadMDM(file_in))
				{
					if (argc > 5)
					{
						zCFileBIN file_out(argv[5] + zSTRING("/_compiled/") + file_in.file->filename + "." + file_in.file->extension, TRUE); // folder out

						mdm->SaveMDM(file_out);

						printf("Saved %s\n", file_out.file->basename.ToChar());
					}
					else
					{
						printf("%s OK\n", file_in.file->basename.ToChar());
					}
				}
				else
				{
					printf("%s could not be loaded\n", file_in.file->basename.ToChar());

					return zERROR;
				}

				zDELETE(mdm);
			}
			else if (extension == "MAN")
			{
				zCModelAni *man = zNEW(zCModelAni);

				if (man->LoadMAN(file_in))
				{
					if (argc > 5)
					{
						zCFileBIN file_out(argv[5] + zSTRING("/_compiled/") + file_in.file->filename + "." + file_in.file->extension, TRUE); // folder out

						man->SaveMAN(file_out);

						printf("Saved %s\n", file_out.file->basename.ToChar());
					}
					else
					{
						printf("%s OK\n", file_in.file->basename.ToChar());
					}
				}
				else
				{
					printf("%s could not be loaded\n", file_in.file->basename.ToChar());

					return zERROR;
				}

				zDELETE(man);
			}
		}
	}
	else if ((argv[1][1] == '\0' && argv[1][0] == 'm') || !stricmp(argv[1], "meshes")) // compiled type
	{
		scDir.ScanDirectory(argv[4]); // folder in

		for (zINT i = 0; i < scDir.files.numInArray; i++)
		{
			zCFileBIN file_in(scDir.files[i]);

			zSTRING extension = file_in.file->extension;
			extension.Upper();

			if (extension == "MRM")
			{
				zCProgMeshProto *mrm = zNEW(zCProgMeshProto);

				if (mrm->LoadMRM(file_in))
				{
					if (argc > 5)
					{
						zCFileBIN file_out(argv[5] + zSTRING("/_compiled/") + file_in.file->filename + "." + file_in.file->extension, TRUE); // folder out

						mrm->SaveMRM(file_out);

						printf("Saved %s\n", file_out.file->basename.ToChar());
					}
					else
					{
						printf("%s OK\n", file_in.file->basename.ToChar());
					}
				}
				else
				{
					printf("%s could not be loaded\n", file_in.file->basename.ToChar());

					return zERROR;
				}

				zDELETE(mrm);
			}
			else if (extension == "MSH")
			{
				zCMesh *msh = zNEW(zCMesh);

				if (msh->LoadMSH(file_in))
				{
					if (argc > 5)
					{
						zCFileBIN file_out(argv[5] + zSTRING("/_compiled/") + file_in.file->filename + "." + file_in.file->extension, TRUE); // folder out

						msh->SaveMSH(file_out);

						printf("Saved %s\n", file_out.file->basename.ToChar());
					}
					else
					{
						printf("%s OK\n", file_in.file->basename.ToChar());
					}
				}
				else
				{
					printf("%s could not be loaded\n", file_in.file->basename.ToChar());

					return zERROR;
				}

				zDELETE(msh);
			}
		}
	}
	else
	{
		printf("Unrecognized filetype, must be \"anims\" or \"a\" for animations, \"meshes\" or \"m\" for meshes\n");

		return zERROR;
	}

	return zOK;
}
