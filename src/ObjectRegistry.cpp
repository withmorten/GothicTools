#include "ObjectRegistry.h"

#include "World.h" // TODO TEMP
#include "Object.h" // TODO TEMP
#include "Vob.h" // TODO TEMP
#include "WayNet.h" // TODO TEMP

bool32 zCObjectMatches::ParseLine(zSTRING &s, zCMatch &match)
{
	size_t t1 = s.find('\t');
	if (t1 == string::npos) return FALSE;

	size_t t2 = s.find('\t', t1 + 1);
	if (t2 == string::npos) return FALSE;

	match.index1 = zSTRING(s.substr(0, t1)).ToInt();
	match.index2 = zSTRING(s.substr(t1 + 1, t2)).ToInt();
	// TODO needs hash parsing code here
	match.reason = s.substr(t2 + 1);

	return TRUE;
}

void zCObjectMatches::Load(const char *fileName)
{
	enum
	{
		INIT,
		OBJECTS,
		WAYPOINTS,
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
			if (s == "waypoints")
			{
				state = WAYPOINTS;
			}
			else
			{
				zCMatch match;

				if (!ParseLine(s, match)) continue;

				objectMatches.Insert(match);
			}

			break;

		case WAYPOINTS:
			if (s == "ways")
			{
				state = WAYS;
			}
			else
			{
				zCMatch match;

				if (!ParseLine(s, match)) continue;

				waypointMatches.Insert(match);
			}

			break;

		case WAYS:
			if (s.IsEmpty())
			{
				state = END;
			}
			else
			{
				zCMatch match;

				if (!ParseLine(s, match)) continue;

				wayMatches.Insert(match);
			}

			break;
		}
	}
	while (state != END && !file.Eof());
}

void zCObjectMatches::FillHashes(zCWorld *world1, zCWorld *world2)
{
	for (int32 i = 0; i < objectMatches.numInArray; i++)
	{
		if (objectMatches[i].index1 == -1)
		{
			objectMatches[i].hash1 = 0;
		}
		else
		{
			objectMatches[i].hash1 = world1->GetVob(objectMatches[i].index1)->GetHash();
		}

		if (objectMatches[i].index2 == -1)
		{
			objectMatches[i].hash2 = 0;
		}
		else
		{
			objectMatches[i].hash2 = world2->GetVob(objectMatches[i].index2)->GetHash();
		}
	}

	for (int32 i = 0; i < waypointMatches.numInArray; i++)
	{
		if (waypointMatches[i].index1 == -1)
		{
			waypointMatches[i].hash1 = 0;
		}
		else
		{
			waypointMatches[i].hash1 = world1->GetWaypoint(waypointMatches[i].index1)->GetHash();
		}

		if (waypointMatches[i].index2 == -1)
		{
			waypointMatches[i].hash2 = 0;
		}
		else
		{
			waypointMatches[i].hash2 = world2->GetWaypoint(waypointMatches[i].index2)->GetHash();
		}
	}

	for (int32 i = 0; i < wayMatches.numInArray; i++)
	{
		if (wayMatches[i].index1 == -1)
		{
			wayMatches[i].id1 = 0;
		}
		else
		{
			wayMatches[i].id1 = world1->wayNet->GetWay(wayMatches[i].index1)->GetID();
		}

		if (wayMatches[i].index2 == -1)
		{
			wayMatches[i].id2 = 0;
		}
		else
		{
			wayMatches[i].id2 = world2->wayNet->GetWay(wayMatches[i].index2)->GetID();
		}
	}
}

void zCObjectMatches::WriteLine(zFILE &file, zCMatch &match)
{
	char line[zFILE_MAXCHARS + 1];

	snprintf(line, sizeof(line), "%5d\t" "%5d\t" "%016llx\t" "%016llx\t" "%s",
		match.index1,
		match.index2,
		match.hash1,
		match.hash2,
		match.reason.ToChar()
	);

	file.WriteLine(line);
}

void zCObjectMatches::WriteHashes(zSTRING &fileName)
{
	zFILE file(fileName, TRUE);

	file.WriteLine("objects");

	for (int32 i = 0; i < objectMatches.numInArray; i++)
	{
		WriteLine(file, objectMatches[i]);
	}

	file.WriteLine("waypoints");

	for (int32 i = 0; i < waypointMatches.numInArray; i++)
	{
		WriteLine(file, waypointMatches[i]);
	}

	file.WriteLine("ways");

	for (int32 i = 0; i < wayMatches.numInArray; i++)
	{
		WriteLine(file, wayMatches[i]);
	}
}

bool32 zCObjectMatches::IsInObjectMatches1(int32 object1)
{
	for (int32 i = 0; i < objectMatches.numInArray; i++)
	{
		if (objectMatches[i].index1 == object1)
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
		if (objectMatches[i].index2 == object2)
		{
			return TRUE;
		}
	}

	return FALSE;
}

bool32 zCObjectMatches::IsInWaypointMatches1(int32 wp1)
{
	for (int32 i = 0; i < waypointMatches.numInArray; i++)
	{
		if (waypointMatches[i].index1 == wp1)
		{
			return TRUE;
		}
	}

	return FALSE;
}

bool32 zCObjectMatches::IsInWaypointMatches2(int32 wp2)
{
	for (int32 i = 0; i < waypointMatches.numInArray; i++)
	{
		if (waypointMatches[i].index2 == wp2)
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
		if (wayMatches[i].index1 == way1)
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
		if (wayMatches[i].index2 == way2)
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
