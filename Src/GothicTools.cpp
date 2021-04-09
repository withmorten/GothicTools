#include "GothicTools.h"

zDWORD meshAndBspVersionIn = 0xFFFFFFFF;
uint16 meshVersionIn = 0xFFFF;
uint16 bspVersionIn = 0xFFFF;
uint16 materialVersionIn = 0xFFFF;

zDWORD morphMeshVersionIn = 0xFFFFFFFF;
uint16 progMeshVersionIn = 0xFFFF;
zDWORD meshSoftSkinVersionIn = 0xFFFFFFFF;
zDWORD modelHierarchyVersionIn = 0xFFFFFFFF;
zDWORD modelMeshVersionIn = 0xFFFFFFFF;
uint16 modelAnimationVersionIn = 0xFFFF;
zDWORD modelScriptVersionIn = 0xFFFFFFFF;

zDWORD meshAndBspVersionOut = 0xFFFFFFFF;
uint16 meshVersionOut = 0xFFFF;
uint16 bspVersionOut = 0xFFFF;
uint16 materialVersionOut = 0xFFFF;

zDWORD morphMeshVersionOut = 0xFFFFFFFF;
uint16 progMeshVersionOut = 0xFFFF;
zDWORD meshSoftSkinVersionOut = 0xFFFFFFFF;
zDWORD modelHierarchyVersionOut = 0xFFFFFFFF;
zDWORD modelMeshVersionOut = 0xFFFFFFFF;
uint16 modelAnimationVersionOut = 0xFFFF;
zDWORD modelScriptVersionOut = 0xFFFFFFFF;

zBOOL xZenIn = FALSE;
zBOOL xZenOut = FALSE;

zBOOL deCompile = FALSE;
zBOOL deLod = FALSE;
