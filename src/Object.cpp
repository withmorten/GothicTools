#include "Object.h"

zOBJECT_DEFINITION(zCObject);
zOBJECT_DEFINITION(zCAIBase);
zOBJECT_DEFINITION(zCAICamera);

void zCObject::CalcHash()
{
	hash = XXH64(GetClassName().ToChar(), GetClassName().Length(), hash);
}

void zCObject::CalcID()
{
	id = XXH32(GetClassName().ToChar(), GetClassName().Length(), id);
}

bool32 zCObject::IsEqual(zCObject *obj)
{
	return GetClassName() == obj->GetClassName();
}

XXH64_hash_t zCObject::GetHash()
{
	if (hash == 0) CalcHash();
	
	return hash;
}

XXH32_hash_t zCObject::GetID()
{
	if (id == 0) CalcID();
	
	return id;
}

bool32 zCObject::IsHashEqual(zCObject *obj)
{
	return GetHash() == obj->GetHash();
}
