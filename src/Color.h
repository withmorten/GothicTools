#pragma once

#include "GothicTools.h"

class zCOLOR
{
public:
	union
	{
		struct
		{
			byte b, g, r, alpha;
		};

		uint32 dword;
	};

public:
	zCOLOR() { Reset(); };
	zCOLOR(byte rr, byte gg, byte bb, byte aa) { r = rr; g = gg; b = bb; alpha = aa; }
	zCOLOR(uint32 color) { dword = color; }

	void Reset() { dword = 0xFF000000; }
	void ResetToMax() { dword = 0xFFFFFFFF; }

	void SetRGB(const byte c) { r = c; g = c; b = c; }
	void SetRGB(const byte rr, const byte gg, const byte bb) { r = rr; g = gg; b = bb; }
	void SetRGBA(const byte rgb, const byte a) { r = rgb; g = rgb; b = rgb; alpha = a; }
	void SetRGBA(const byte rr, const byte gg, const byte bb, const byte aa) { r = rr; g = gg; b = bb; alpha = aa; }

	zSTRING GetDescriptionRGB() const { return zSTRING(r) + " " + zSTRING(g) + " " + zSTRING(b); }

	friend bool32 operator ==(const zCOLOR &col1, const zCOLOR &col2) { return col1.dword == col2.dword; }
	friend bool32 operator !=(const zCOLOR &col1, const zCOLOR &col2) { return col1.dword != col2.dword; }
};
