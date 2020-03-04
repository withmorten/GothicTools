#pragma once

class zCScanDir
{
public:
	zCArray<zSTRING> files;

public:
	zVOID Reset() { files.DeleteList(); }
	zVOID ScanDirectory(const zSTRING &dir);
	zSTRING Find(const zSTRING &s); // quick & dirty, can have false positives, so only enter a filename with extension
};
