#pragma once

class zCOLOR
{
public:
	union
	{
		struct
		{
			zBYTE b, g, r, alpha;
		};

		zDWORD dword;
	};

public:
	zCOLOR() { Reset(); };
	zCOLOR(zBYTE rr, zBYTE gg, zBYTE bb, zBYTE aa) { r = rr; g = gg; b = bb; alpha = aa; }
	zCOLOR(zDWORD color) { dword = color; }

	zVOID Reset() { dword = 0xFF000000; }
	zVOID ResetToMax() { dword = 0xFFFFFFFF; }

	zVOID SetRGB(const zBYTE c) { r = c; g = c; b = c; }
	zVOID SetRGB(const zBYTE rr, const zBYTE gg, const zBYTE bb) { r = rr; g = gg; b = bb; }
	zVOID SetRGBA(const zBYTE rgb, const zBYTE a) { r = rgb; g = rgb; b = rgb; alpha = a; }
	zVOID SetRGBA(const zBYTE rr, const zBYTE gg, const zBYTE bb, const zBYTE aa) { r = rr; g = gg; b = bb; alpha = aa; }
};
