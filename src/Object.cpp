#include "Object.h"

zOBJECT_DEFINITION(zCObject);
zOBJECT_DEFINITION(zCAIBase);
zOBJECT_DEFINITION(zCAICamera);

bool32 zCObject::IsEqual(zCObject *obj)
{
	if (hash == 0) Hash();
	if (obj->hash == 0) obj->Hash();

	return hash == obj->hash;
}
