#pragma once

#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <sys/stat.h>
#include <intrin.h>

typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;

typedef uint8 byte;

typedef uint8 bool8;
typedef int32 bool32;

#ifndef _WIN32
#define stricmp strcasecmp
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define zOK 0
#define zERROR 1

#define GOTHIC_VERSION_101 101 // 1.01d, 1.01e
#define GOTHIC_VERSION_104 104 // 1.04d
#define GOTHIC_VERSION_108 108 // 1.06j - 1.08k
#define GOTHIC_VERSION_112 112 // 1.12f
#define GOTHIC_VERSION_130 130 // 1.30
#define GOTHIC_VERSION_260 260 // 2.6

#define TEXTURE_FILE_VERSION_GOTHIC_1_01 ((uint16)0x0000)
#define TEXTURE_FILE_VERSION_GOTHIC_1_04 TEXTURE_FILE_VERSION_GOTHIC_1_01
#define TEXTURE_FILE_VERSION_GOTHIC_1_30 ((uint16)0x0001)

#define MESH_FILE_VERSION_GOTHIC_1_01 ((uint16)0x0009)
#define MESH_FILE_VERSION_GOTHIC_1_04 MESH_FILE_VERSION_GOTHIC_1_01
#define MESH_FILE_VERSION_GOTHIC_1_30 MESH_FILE_VERSION_GOTHIC_1_04

#define MESH_VERSION_GOTHIC_1_01 (uint16((TEXTURE_FILE_VERSION_GOTHIC_1_01 << 8) + MESH_FILE_VERSION_GOTHIC_1_01))
#define MESH_VERSION_GOTHIC_1_04 MESH_VERSION_GOTHIC_1_01
#define MESH_VERSION_GOTHIC_1_30 (uint16((TEXTURE_FILE_VERSION_GOTHIC_1_30 << 8) + MESH_FILE_VERSION_GOTHIC_1_30))

#define BSP_VERSION_GOTHIC_1_01 ((uint16)0x0001)
#define BSP_VERSION_GOTHIC_1_04 ((uint16)0x0002)
#define BSP_VERSION_GOTHIC_1_30 ((uint16)0x0003)

#define BSPMESH_VERSION_GOTHIC_1_01 (uint32((MESH_VERSION_GOTHIC_1_01 << 16) + (BSP_VERSION_GOTHIC_1_01 << 24)))
#define BSPMESH_VERSION_GOTHIC_1_04 (uint32((MESH_VERSION_GOTHIC_1_04 << 16) + (BSP_VERSION_GOTHIC_1_04 << 24)))
#define BSPMESH_VERSION_GOTHIC_1_30 (uint32((MESH_VERSION_GOTHIC_1_30 << 16) + (BSP_VERSION_GOTHIC_1_30 << 24)))

#define MATERIAL_CLASS_VERSION_GOTHIC_1_01 ((uint16)0x4400)
#define MATERIAL_CLASS_VERSION_GOTHIC_1_04 MATERIAL_CLASS_VERSION_GOTHIC_1_01
#define MATERIAL_CLASS_VERSION_GOTHIC_OLD1 ((uint16)0x7401)
#define MATERIAL_CLASS_VERSION_GOTHIC_OLD2 ((uint16)0x6002)
#define MATERIAL_CLASS_VERSION_GOTHIC_1_30 ((uint16)0x9C03)

#define MATERIAL_FILE_VERSION_GOTHIC_1_01 ((uint16)0x0003)
#define MATERIAL_FILE_VERSION_GOTHIC_1_04 MATERIAL_FILE_VERSION_GOTHIC_1_01
#define MATERIAL_FILE_VERSION_GOTHIC_OLD1 ((uint16)0x0007)
#define MATERIAL_FILE_VERSION_GOTHIC_OLD2 ((uint16)0x0008)
#define MATERIAL_FILE_VERSION_GOTHIC_1_30 ((uint16)0x0009)

#define MORPHMESH_VERSION_GOTHIC_1_01 ((uint32)0x00000003)
#define MORPHMESH_VERSION_GOTHIC_1_04 MORPHMESH_VERSION_GOTHIC_1_01
#define MORPHMESH_VERSION_GOTHIC_1_30 MORPHMESH_VERSION_GOTHIC_1_04

#define PROGMESH_FILE_VERSION_GOTHIC_1_01 ((uint16)0x0005)
#define PROGMESH_FILE_VERSION_GOTHIC_1_04 PROGMESH_FILE_VERSION_GOTHIC_1_01
#define PROGMESH_FILE_VERSION_GOTHIC_1_30 PROGMESH_FILE_VERSION_GOTHIC_1_04

#define PROGMESH_VERSION_GOTHIC_1_01 ((uint16)((MATERIAL_FILE_VERSION_GOTHIC_1_01 << 8) + PROGMESH_FILE_VERSION_GOTHIC_1_01))
#define PROGMESH_VERSION_GOTHIC_1_04 ((uint16)((MATERIAL_FILE_VERSION_GOTHIC_1_04 << 8) + PROGMESH_FILE_VERSION_GOTHIC_1_04))
#define PROGMESH_VERSION_GOTHIC_OLD1 ((uint16)((MATERIAL_FILE_VERSION_GOTHIC_OLD1 << 8) + PROGMESH_FILE_VERSION_GOTHIC_1_30))
#define PROGMESH_VERSION_GOTHIC_OLD2 ((uint16)((MATERIAL_FILE_VERSION_GOTHIC_OLD2 << 8) + PROGMESH_FILE_VERSION_GOTHIC_1_30))
#define PROGMESH_VERSION_GOTHIC_1_30 ((uint16)((MATERIAL_FILE_VERSION_GOTHIC_1_30 << 8) + PROGMESH_FILE_VERSION_GOTHIC_1_30))

#define MESHSOFTSKIN_FILE_VERSION_GOTHIC_1_01 ((uint32)0x00000004)
#define MESHSOFTSKIN_FILE_VERSION_GOTHIC_1_04 MESHSOFTSKIN_FILE_VERSION_GOTHIC_1_01
#define MESHSOFTSKIN_FILE_VERSION_GOTHIC_1_30 MESHSOFTSKIN_FILE_VERSION_GOTHIC_1_04

#define MODELHIERARCHY_VERSION_GOTHIC_1_01 (uint32(0x00000003))
#define MODELHIERARCHY_VERSION_GOTHIC_1_04 MODELHIERARCHY_VERSION_GOTHIC_1_01
#define MODELHIERARCHY_VERSION_GOTHIC_1_30 MODELHIERARCHY_VERSION_GOTHIC_1_04

#define MAN_FILE_VERSION_GOTHIC_1_01 ((uint16)0x000C)
#define MAN_FILE_VERSION_GOTHIC_1_04 MAN_FILE_VERSION_GOTHIC_1_01
#define MAN_FILE_VERSION_GOTHIC_1_30 MAN_FILE_VERSION_GOTHIC_1_04

#define MODELMESH_FILE_VERSION_GOTHIC_1_01 ((uint32)0x00000006)
#define MODELMESH_FILE_VERSION_GOTHIC_1_04 MODELMESH_FILE_VERSION_GOTHIC_1_01
#define MODELMESH_FILE_VERSION_GOTHIC_1_30 MODELMESH_FILE_VERSION_GOTHIC_1_04

#define MODELMESH_VERSION_GOTHIC_1_01 ((uint32)(MODELMESH_FILE_VERSION_GOTHIC_1_01 + (((uint32)PROGMESH_VERSION_GOTHIC_1_01) << 8) + (MESHSOFTSKIN_FILE_VERSION_GOTHIC_1_01 << 24)))
#define MODELMESH_VERSION_GOTHIC_1_04 ((uint32)(MODELMESH_FILE_VERSION_GOTHIC_1_04 + (((uint32)PROGMESH_VERSION_GOTHIC_1_04) << 8) + (MESHSOFTSKIN_FILE_VERSION_GOTHIC_1_04 << 24)))
#define MODELMESH_VERSION_GOTHIC_OLD1 ((uint32)(MODELMESH_FILE_VERSION_GOTHIC_1_30 + (((uint32)PROGMESH_VERSION_GOTHIC_OLD1) << 8) + (MESHSOFTSKIN_FILE_VERSION_GOTHIC_1_30 << 24)))
#define MODELMESH_VERSION_GOTHIC_OLD2 ((uint32)(MODELMESH_FILE_VERSION_GOTHIC_1_30 + (((uint32)PROGMESH_VERSION_GOTHIC_OLD2) << 8) + (MESHSOFTSKIN_FILE_VERSION_GOTHIC_1_30 << 24)))
#define MODELMESH_VERSION_GOTHIC_1_30 ((uint32)(MODELMESH_FILE_VERSION_GOTHIC_1_30 + (((uint32)PROGMESH_VERSION_GOTHIC_1_30) << 8) + (MESHSOFTSKIN_FILE_VERSION_GOTHIC_1_30 << 24)))

#define MSB_FILE_VERSION_GOTHIC_1_30 ((uint32)0x00000001)

#include "Memory.h"
#include "String.h"
#include "Container.h"
#include "Algebra.h"
#include "Disk.h"

extern uint32 gothicVersionIn;

extern uint32 meshAndBspVersionIn;
extern uint16 meshVersionIn;
extern uint16 bspVersionIn;
extern uint16 materialVersionIn;

extern uint32 morphMeshVersionIn;
extern uint16 progMeshVersionIn;
extern uint32 meshSoftSkinVersionIn;
extern uint32 modelHierarchyVersionIn;
extern uint32 modelMeshVersionIn;
extern uint16 modelAnimationVersionIn;
extern uint32 modelScriptVersionIn;

extern uint32 gothicVersionOut;

extern uint32 meshAndBspVersionOut;
extern uint16 meshVersionOut;
extern uint16 bspVersionOut;
extern uint16 materialVersionOut;

extern uint32 morphMeshVersionOut;
extern uint16 progMeshVersionOut;
extern uint32 meshSoftSkinVersionOut;
extern uint32 modelHierarchyVersionOut;
extern uint32 modelMeshVersionOut;
extern uint16 modelAnimationVersionOut;
extern uint32 modelScriptVersionOut;

extern bool32 xZenIn;
extern bool32 xZenOut;

extern bool32 decompile;
extern bool32 deLOD;

bool32 SetInVersions(int32 version);
bool32 SetOutVersions(int32 version);
