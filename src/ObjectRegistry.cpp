#include "ObjectRegistry.h"

void zCObjectMatches::Load(const char *fileName)
{
	enum
	{
		INIT,
		OBJECTS,
		WAYS,
		END,
	}
	state = INIT;

	zFILE file(fileName);

	zSTRING s;

	do
	{
		file.ReadLine(s);

		if (!s.IsEmpty() && s[0] == ';') continue;

		switch (state)
		{
		case INIT:
			if (s == "objects")
			{
				state = OBJECTS;
			}

			break;

		case OBJECTS:
			if (s == "ways")
			{
				state = WAYS;
			}
			else
			{
				size_t t1 = s.find('\t');
				if (t1 == string::npos) continue;

				size_t t2 = s.find('\t', t1 + 1);
				if (t2 == string::npos) continue;

				zCObjectMatch match;

				match.object1 = zSTRING(s.substr(0, t1)).ToInt();
				match.object2 = zSTRING(s.substr(t1 + 1, t2)).ToInt();
				match.reason = s.substr(t2 + 1);

				objectMatches.Insert(match);
			}

			break;

		case WAYS:
			if (s.IsEmpty())
			{
				state = END;
			}
			else
			{
				size_t t1 = s.find('\t');
				if (t1 == string::npos) continue;

				size_t t2 = s.find('\t', t1 + 1);
				if (t2 == string::npos) continue;

				zCWayMatch match;

				match.way1 = zSTRING(s.substr(0, t1)).ToInt();
				match.way2 = zSTRING(s.substr(t1 + 1, t2)).ToInt();
				match.reason = s.substr(t2 + 1);

				wayMatches.Insert(match);
			}

			break;
		}

		if (file.Eof()) break;
	}
	while (state != END);
}

bool32 zCObjectMatches::IsInObjectMatches1(int32 object1)
{
	for (int32 i = 0; i < objectMatches.numInArray; i++)
	{
		if (objectMatches[i].object1 == object1)
		{
			return TRUE;
		}
	}

	return FALSE;
}

bool32 zCObjectMatches::IsInObjectMatches2(int32 object2)
{
	for (int32 i = 0; i < objectMatches.numInArray; i++)
	{
		if (objectMatches[i].object2 == object2)
		{
			return TRUE;
		}
	}

	return FALSE;
}

bool32 zCObjectMatches::IsInWayMatches1(int32 way1)
{
	for (int32 i = 0; i < wayMatches.numInArray; i++)
	{
		if (wayMatches[i].way1 == way1)
		{
			return TRUE;
		}
	}

	return FALSE;
}

bool32 zCObjectMatches::IsInWayMatches2(int32 way2)
{
	for (int32 i = 0; i < wayMatches.numInArray; i++)
	{
		if (wayMatches[i].way2 == way2)
		{
			return TRUE;
		}
	}

	return FALSE;
}

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
