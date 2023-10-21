#include "GothicTools.h"

uint32 meshAndBspVersionIn = 0xFFFFFFFF;
uint16 meshVersionIn = 0xFFFF;
uint16 bspVersionIn = 0xFFFF;
uint16 materialVersionIn = 0xFFFF;

uint32 morphMeshVersionIn = 0xFFFFFFFF;
uint16 progMeshVersionIn = 0xFFFF;
uint32 meshSoftSkinVersionIn = 0xFFFFFFFF;
uint32 modelHierarchyVersionIn = 0xFFFFFFFF;
uint32 modelMeshVersionIn = 0xFFFFFFFF;
uint16 modelAnimationVersionIn = 0xFFFF;
uint32 modelScriptVersionIn = 0xFFFFFFFF;

uint32 meshAndBspVersionOut = 0xFFFFFFFF;
uint16 meshVersionOut = 0xFFFF;
uint16 bspVersionOut = 0xFFFF;
uint16 materialVersionOut = 0xFFFF;

uint32 morphMeshVersionOut = 0xFFFFFFFF;
uint16 progMeshVersionOut = 0xFFFF;
uint32 meshSoftSkinVersionOut = 0xFFFFFFFF;
uint32 modelHierarchyVersionOut = 0xFFFFFFFF;
uint32 modelMeshVersionOut = 0xFFFFFFFF;
uint16 modelAnimationVersionOut = 0xFFFF;
uint32 modelScriptVersionOut = 0xFFFFFFFF;

bool32 xZenIn = FALSE;
bool32 xZenOut = FALSE;

bool32 decompile = FALSE;
bool32 deLOD = FALSE;

bool32 SetInVersions(int32 version)
{
	switch (version)
	{
	case 101:
		meshAndBspVersionIn = BSPMESH_VERSION_GOTHIC_1_01;
		meshVersionIn = MESH_VERSION_GOTHIC_1_01;
		bspVersionIn = BSP_VERSION_GOTHIC_1_01;
		materialVersionIn = MATERIAL_CLASS_VERSION_GOTHIC_1_01;

		morphMeshVersionIn = MORPHMESH_VERSION_GOTHIC_1_01;
		progMeshVersionIn = PROGMESH_VERSION_GOTHIC_1_01;
		meshSoftSkinVersionIn = MESHSOFTSKIN_FILE_VERSION_GOTHIC_1_01;
		modelHierarchyVersionIn = MODELHIERARCHY_VERSION_GOTHIC_1_01;
		modelMeshVersionIn = MODELMESH_VERSION_GOTHIC_1_01;
		modelAnimationVersionIn = MAN_FILE_VERSION_GOTHIC_1_01;

		deLOD = TRUE;

		break;

	case 104:
	case 108:
	case 112:
		meshAndBspVersionIn = BSPMESH_VERSION_GOTHIC_1_04;
		meshVersionIn = MESH_VERSION_GOTHIC_1_04;
		bspVersionIn = BSP_VERSION_GOTHIC_1_04;
		materialVersionIn = MATERIAL_CLASS_VERSION_GOTHIC_1_04;

		morphMeshVersionIn = MORPHMESH_VERSION_GOTHIC_1_04;
		progMeshVersionIn = PROGMESH_VERSION_GOTHIC_1_04;
		meshSoftSkinVersionIn = MESHSOFTSKIN_FILE_VERSION_GOTHIC_1_04;
		modelHierarchyVersionIn = MODELHIERARCHY_VERSION_GOTHIC_1_04;
		modelMeshVersionIn = MODELMESH_VERSION_GOTHIC_1_04;
		modelAnimationVersionIn = MAN_FILE_VERSION_GOTHIC_1_04;

		deLOD = TRUE;

		break;

	case 130:
	case 260:
		meshAndBspVersionIn = BSPMESH_VERSION_GOTHIC_1_30;
		meshVersionIn = MESH_VERSION_GOTHIC_1_30;
		bspVersionIn = BSP_VERSION_GOTHIC_1_30;
		materialVersionIn = MATERIAL_CLASS_VERSION_GOTHIC_1_30;

		morphMeshVersionIn = MORPHMESH_VERSION_GOTHIC_1_30;
		progMeshVersionIn = PROGMESH_VERSION_GOTHIC_1_30;
		meshSoftSkinVersionIn = MESHSOFTSKIN_FILE_VERSION_GOTHIC_1_30;
		modelHierarchyVersionIn = MODELHIERARCHY_VERSION_GOTHIC_1_30;
		modelMeshVersionIn = MODELMESH_VERSION_GOTHIC_1_30;
		modelAnimationVersionIn = MAN_FILE_VERSION_GOTHIC_1_30;
		modelScriptVersionIn = MSB_FILE_VERSION_GOTHIC_1_30;

		break;

	default:
		printf("Unrecognized version in, needs to be 101, 104/108/112 or 130/260\n");

		return FALSE;
	}

	return TRUE;
}

bool32 SetOutVersions(int32 version)
{
	switch (version)
	{
	case 101:
		meshAndBspVersionOut = BSPMESH_VERSION_GOTHIC_1_01;
		meshVersionOut = MESH_VERSION_GOTHIC_1_01;
		bspVersionOut = BSP_VERSION_GOTHIC_1_01;
		materialVersionOut = MATERIAL_CLASS_VERSION_GOTHIC_1_01;

		morphMeshVersionOut = MORPHMESH_VERSION_GOTHIC_1_01;
		progMeshVersionOut = PROGMESH_VERSION_GOTHIC_1_01;
		meshSoftSkinVersionOut = MESHSOFTSKIN_FILE_VERSION_GOTHIC_1_01;
		modelHierarchyVersionOut = MODELHIERARCHY_VERSION_GOTHIC_1_01;
		modelMeshVersionOut = MODELMESH_VERSION_GOTHIC_1_01;
		modelAnimationVersionOut = MAN_FILE_VERSION_GOTHIC_1_01;

		xZenOut = FALSE;

		break;

	case 104:
	case 108:
	case 112:
		meshAndBspVersionOut = BSPMESH_VERSION_GOTHIC_1_04;
		meshVersionOut = MESH_VERSION_GOTHIC_1_04;
		bspVersionOut = BSP_VERSION_GOTHIC_1_04;
		materialVersionOut = MATERIAL_CLASS_VERSION_GOTHIC_1_04;

		morphMeshVersionOut = MORPHMESH_VERSION_GOTHIC_1_04;
		progMeshVersionOut = PROGMESH_VERSION_GOTHIC_1_04;
		meshSoftSkinVersionOut = MESHSOFTSKIN_FILE_VERSION_GOTHIC_1_04;
		modelHierarchyVersionOut = MODELHIERARCHY_VERSION_GOTHIC_1_04;
		modelMeshVersionOut = MODELMESH_VERSION_GOTHIC_1_04;
		modelAnimationVersionOut = MAN_FILE_VERSION_GOTHIC_1_04;

		break;

	case 130:
	case 260:
		meshAndBspVersionOut = BSPMESH_VERSION_GOTHIC_1_30;
		meshVersionOut = MESH_VERSION_GOTHIC_1_30;
		bspVersionOut = BSP_VERSION_GOTHIC_1_30;
		materialVersionOut = MATERIAL_CLASS_VERSION_GOTHIC_1_30;

		morphMeshVersionOut = MORPHMESH_VERSION_GOTHIC_1_30;
		progMeshVersionOut = PROGMESH_VERSION_GOTHIC_1_30;
		meshSoftSkinVersionOut = MESHSOFTSKIN_FILE_VERSION_GOTHIC_1_30;
		modelHierarchyVersionOut = MODELHIERARCHY_VERSION_GOTHIC_1_30;
		modelMeshVersionOut = MODELMESH_VERSION_GOTHIC_1_30;
		modelAnimationVersionOut = MAN_FILE_VERSION_GOTHIC_1_30;
		modelScriptVersionOut = MSB_FILE_VERSION_GOTHIC_1_30;

		break;

	default:
		printf("Unrecognized version out, needs to be 101, 104/108/112 or 130/260\n");

		return FALSE;
	}

	return TRUE;
}
