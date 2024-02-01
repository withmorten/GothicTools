#include "Object.h"

zOBJECT_DEFINITION(zCObject);
zOBJECT_DEFINITION(zCAIBase);
zOBJECT_DEFINITION(zCAICamera);

void zCObject::Hash()
{
	hash = XXH64(GetClassName().ToChar(), GetClassName().Length(), hash);
}

bool32 zCObject::IsEqual(zCObject *obj)
{
	return GetClassName() == obj->GetClassName();
}

bool32 zCObject::IsHashEqual(zCObject *obj)
{
	if (hash == 0) Hash();

	//obj = obj->IsReference() ? obj->ref : obj;

	if (obj->hash == 0) obj->Hash();

	return hash == obj->hash;
}
