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

	void Reset() { dword = 0xFF000000; }
	void ResetToMax() { dword = 0xFFFFFFFF; }

	void SetRGB(const zBYTE c) { r = c; g = c; b = c; }
	void SetRGB(const zBYTE rr, const zBYTE gg, const zBYTE bb) { r = rr; g = gg; b = bb; }
	void SetRGBA(const zBYTE rgb, const zBYTE a) { r = rgb; g = rgb; b = rgb; alpha = a; }
	void SetRGBA(const zBYTE rr, const zBYTE gg, const zBYTE bb, const zBYTE aa) { r = rr; g = gg; b = bb; alpha = aa; }
};
