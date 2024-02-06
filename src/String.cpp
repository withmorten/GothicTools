#include "GothicTools.h" // shrug

zSTRING::zSTRING(const float xWert, int32 digits)
{
	if (digits > 9 || digits < 0) digits = 9;

	char format[] = "%.9g";
	format[2] = (char)digits + '0';

	char buffer[100];
	sprintf(buffer, format, xWert);
	assign(buffer);
}

char zSTRING::GetLastChar()
{
	if (size() == 0)
	{
		return ' ';
	}
	else
	{
		return at(size() - 1);
	}
}

zSTRING &zSTRING::Upper()
{
	size_t pos = size();

	while (pos--)
	{
		(*this)[pos] = toupper((*this)[pos]);
	}

	return *this;
}

zSTRING &zSTRING::Lower()
{
	size_t pos = size();

	while (pos--)
	{
		(*this)[pos] = tolower((*this)[pos]);
	}

	return *this;
}

zSTRING zSTRING::PickWord(size_t num, const zSTRING &trenn, const zSTRING &skip) const
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

	int32 pos = 0;

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

const char *zSTRING::PickWordPos(size_t num, const zSTRING &trenn, const zSTRING &skip) const
{
	const size_t strsize = size();

	if (strsize > 0)
	{
		char ch;
		size_t pos = 0;
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

zSTRING zSTRING::PickWord_Old(size_t num, const zSTRING &trenn)
{
	return PickWord(num, trenn, " ");
}

zSTRING zSTRING::PickWordCutMarks(size_t num, const zSTRING &trenn, const zSTRING &skip) const
{
	zSTRING word = PickWord(num, trenn, skip);

	word.Delete("\"", zSTR_TO);
	word.Delete("\"", zSTR_FROM);

	return word;
}

void zSTRING::Delete(const zSTRING &xStr, zTSTR_KIND kind)
{
	size_t pos = find(xStr);

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

int32 zSTRING::Delete(size_t pos, size_t length)
{
	if (pos >= size()) return 0;
	erase(pos, length);
	return 0;
}

zSTRING zSTRING::Deleted(size_t pos, size_t length) const
{
	zSTRING s(this);
	s.Delete(pos, length);
	return s;
}

void zSTRING::DeleteRight(size_t length)
{
	if (length > size())
	{
		length = size();
	}

	erase(size() - length, length);
}

void zSTRING::TrimLeft(char ch)
{
	size_t pos = 0;

	while (pos < size() && (*this)[pos] == ch)
	{
		pos++;
	}

	if (pos > 0)
	{
		Delete(0, pos);
	}
}

void zSTRING::TrimRight(char ch)
{
	size_t pos = size() - 1;

	while (pos >= 0 && (*this)[pos] == ch)
	{
		pos++;
	}

	if (pos < size() - 1)
	{
		Delete(pos + 1);
	}
}

int32 zSTRING::Search(size_t startIndex, const char *substr, uint32 num) const
{
	if (!substr) return -1;

	size_t index = startIndex;
	int32 pos = -1;

	for (uint32 i = 0; i < num; i++)
	{
		pos = (int32)find(substr, index);

		if (pos == (int32)string::npos) return -1;

		index = pos + 1;
	}

	return pos;
}
