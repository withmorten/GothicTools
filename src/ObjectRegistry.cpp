#include "ObjectRegistry.h"

zCObjectRegistry::zCObjectRegistry()
{
}

zCObjectRegistry::~zCObjectRegistry()
{
	for (int32 i = 0; i < lists.numInArray; i++)
	{
		zDELETE(lists[i]);
	}
}

void zCObjectRegistry::Insert(const zSTRING &className, zCObject *obj)
{
	bool32 found = FALSE;

	for (int32 i = 0; i < lists.numInArray; i++)
	{
		if (className == lists[i]->className)
		{
			found = TRUE;

			lists[i]->list.Insert(obj);

			break;
		}
	}

	if (!found)
	{
		zCObjectList *list = zNEW(zCObjectList(className));

		list->list.Insert(obj);

		lists.Insert(list);
	}
}

zCArray<zCObject *> &zCObjectRegistry::GetList(const zSTRING &className)
{
	for (int32 i = 0; i < lists.numInArray; i++)
	{
		if (className == lists[i]->className)
		{
			return lists[i]->list;
		}
	}

	zCObjectList *list = zNEW(zCObjectList(className));

	lists.Insert(list);

	return list->list;
}
