#pragma once

#include "GothicTools.h"

class zCScanDir
{
public:
	zCArray<zSTRING> files;

public:
	void Reset() { files.DeleteList(); }
	void ScanDirectory(const zSTRING &dir);
	zSTRING Find(const zSTRING &s); // quick & dirty, can have false positives, so only enter a filename with extension
};
