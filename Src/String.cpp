#include "GothicTools.h"

zSTRING::zSTRING(const zREAL xWert, zINT digits)
{
	if (digits > 9 || digits < 0) digits = 9;

	char format[] = "%.9g";
	format[2] = (char)digits + 48;

	char buffer[100];
	sprintf(buffer, format, xWert);
	assign(buffer);
}

char zSTRING::GetLastChar()
{
	if (size() == 0)
	{
		return  ' ';
	}
	else
	{
		return at(size() - 1);
	}
}

zSTRING &zSTRING::Upper()
{
	zSIZE_T pos = size();

	while (pos--)
	{
		(*this)[pos] = toupper((*this)[pos]);
	}

	return *this;
}

zSTRING &zSTRING::Lower()
{
	zSIZE_T pos = size();

	while (pos--)
	{
		(*this)[pos] = tolower((*this)[pos]);
	}

	return *this;
}

zSTRING zSTRING::PickWord(zUINT num, const zSTRING &trenn, const zSTRING &skip) const
{
	if (size() <= 0)
	{
		return "";
	}

	zSTRING result;
	const char *str = PickWordPos(num, trenn, skip);

	if (!str)
	{
		return result;
	}

	zINT pos = 0;

	if (trenn.find_first_of(str[pos]) != string::npos)
	{
		result = str[pos];
	}

	while (str[pos] != '\0')
	{
		if ((skip.find_first_of(str[pos]) != string::npos) || (trenn.find_first_of(str[pos]) != string::npos))
		{
			return result;
		}

		result += str[pos];

		pos++;
	}

	return result;
}

const char *zSTRING::PickWordPos(zUINT num, const zSTRING &trenn, const zSTRING &skip) const
{
	const zSIZE_T strsize = size();

	if (strsize > 0)
	{
		char ch;
		zSIZE_T pos = 0;
		byte inWord = FALSE;

		do
		{
			ch = (*this)[pos];

			if (skip.find_first_of(ch) != string::npos)
			{
				inWord = FALSE;
			}
			else if (trenn.find_first_of(ch) != string::npos)
			{
				if ((--num) == 0)
				{
					return c_str() + pos;
				}

				inWord = FALSE;
			}
			else if (!inWord)
			{
				inWord = TRUE;
				if ((--num) == 0)
				{
					return c_str() + pos;
				}
			}
		}
		while ((++pos) < strsize);
	}

	return NULL;
}

zSTRING zSTRING::PickWorld_Old(zINT num, const zSTRING &trenn)
{
	return PickWord(num, trenn, " ");
}

zSTRING zSTRING::PickWordCutMarks(zUINT num, const zSTRING &trenn, const zSTRING &skip) const
{
	zSTRING word = PickWord(num, trenn, skip);

	word.Delete("\"", zSTR_TO);
	word.Delete("\"", zSTR_FROM);

	return word;
}

void zSTRING::Delete(const zSTRING &xStr, zTSTR_KIND kind)
{
	zSIZE_T pos = find(xStr);

	if (pos != npos)
	{
		switch (kind)
		{
			case zSTR_BEFORE:
			{
				erase(0, pos);

				break;
			}
			case zSTR_TO:
			{
				erase(0, pos + xStr.size());

				break;
			}
			case zSTR_AFTER:
			{
				erase(pos + xStr.size());

				break;
			}
			case zSTR_FROM:
			{
				erase(pos);

				break;
			}
			case zSTR_ONLY:
			{
				erase(pos, xStr.size());

				break;
			}
			default:
			{
				erase(pos, xStr.size());

				break;
			}
		}
	}
}

zINT zSTRING::Delete(zSIZE_T pos, zSIZE_T length)
{
	if (pos >= size()) return 0;
	erase(pos, length);
	return 0;
}

zSTRING zSTRING::Deleted(zSIZE_T pos, zSIZE_T length) const
{
	zSTRING s(this);
	s.Delete(pos, length);
	return s;
}

void zSTRING::DeleteRight(zSIZE_T length)
{
	if (length > size())
	{
		length = size();
	}

	erase(size() - length, length);
}

zLONG zSTRING::Search(zSIZE_T startIndex, const char *substr, zUINT num) const
{
	if (!substr) return -1;

	zSIZE_T index = startIndex;
	zLONG pos = -1;

	for (zUINT i = 0; i < num; i++)
	{
		pos = (zLONG)find(substr, index);

		if (pos == (zLONG)string::npos) return -1;

		index = pos + 1;
	}

	return pos;
}
