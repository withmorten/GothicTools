#pragma once

#include "GothicTools.h"

#define zARRAY_START_ALLOC 16

template<class T> class zCArray
{
public:
	T *array;
	int32 numAlloc;
	int32 numInArray;

public:
	zCArray()
	{
		array = NULL;
		numAlloc = 0;
		numInArray = 0;
	}

	zCArray(const int32 startSize)
	{
		array = NULL;

		if (startSize > 0)
		{
			array = zNEW_ARRAY(T, startSize);
		}

		numAlloc = startSize;
		numInArray = 0;
	}

	~zCArray() { DeleteList(); }

	void AllocDelta(const int32 numDelta)
	{
		if (numDelta <= 0)
		{
			return;
		}

		T *newArray = zNEW_ARRAY(T, numAlloc + numDelta);

		if (numInArray > 0)
		{
			for (int i = 0; i < numInArray; i++)
			{
				newArray[i] = array[i];
			}
		}

		zDELETE_ARRAY(array);
		array = newArray;
		numAlloc += numDelta;
	}

	void AllocAbs(const int32 size)
	{
		if (numAlloc >= size)
		{
			return;
		}

		AllocDelta(size - numAlloc);
	}

	bool32 IsEmpty() const { return !numInArray; }

	const T &operator[](const int32 nr) const { return array[nr]; }
	T &operator[](const int32 nr) { return array[nr]; }

	friend bool32 operator ==(const zCArray<T> &array1, const zCArray<T> &array2)
	{
		if (array1.numInArray != array2.numInArray) return FALSE;
		if (memcmp(array1.array, array2.array, sizeof(T) * array1.numInArray)) return FALSE;

		return TRUE;
	}

	friend bool32 operator !=(const zCArray<T> &array1, const zCArray<T> &array2) { return !(array1 == array2); }

	zCArray<T> &operator =(const zCArray<T> &array2)
	{
		EmptyList();
		AllocAbs(array2.numInArray);
		numInArray = array2.numInArray;

		for (int32 i = 0; i < numInArray; i++) array[i] = array2[i];

		return *this;
	}

	void InsertEnd(const T &ins)
	{
		if (numAlloc < numInArray + 1)
		{
			if (numAlloc < zARRAY_START_ALLOC)
			{
				AllocDelta(zARRAY_START_ALLOC);
			}
			else
			{
				AllocDelta(numAlloc * 2);
			}
		}

		array[numInArray++] = ins;
	}

	void Insert(const T &ins) { InsertEnd(ins); }

	void EmptyList()
	{
		numInArray = 0;
	}

	void DeleteList()
	{
		zDELETE_ARRAY(array);
		numAlloc = 0;
		numInArray = 0;
	}

	int32 Search(const T &data) const
	{
		if (numInArray > 0)
		{
			for (int32 i = 0; i < numInArray; i++)
			{
				if (array[i] == data) return i;
			}
		}

		return -1;
	}

	bool32 IsInList(const T &data) const
	{
		if (numInArray > 0)
		{
			for (int32 i = 0; i < numInArray; i++)
			{
				if (array[i] == data) return TRUE;
			}
		}

		return FALSE;
	}

	void ShrinkToFit()
	{
		if (numInArray <= 0)
		{
			DeleteList();

			return;
		}

		if (numAlloc > numInArray)
		{
			T *newArray = zNEW_ARRAY(T, numInArray);

			for (int32 i = 0; i < numInArray; i++)
			{
				newArray[i] = array[i];
			}

			zDELETE_ARRAY(array);

			array = newArray;
			numAlloc = numInArray;
		}
	}

	void RemoveLast(bool32 release = FALSE)
	{
		if (numInArray <= 0) return;

		numInArray--;

		if (release) zDELETE(array[numInArray]);
	}

	void RemoveOrderIndex(const int32 index)
	{
		if (index > numInArray) return;

		if (index != numInArray - 1)
		{
			for (int32 i = index; i < numInArray; i++)
			{
				array[i] = array[i + 1];
			}
		}

		numInArray--;
	}
};

template<class T> class zCArrayAdapt
{
public:
	T *array;
	int32 numInArray;

public:
	uint32 SetArray(void *ptr, const int32 num)
	{
		array = (T *)ptr;
		numInArray = num;

		return sizeof(T) * numInArray;
	}

	T *GetArray()
	{
		return array;
	}

	int32 GetNum()
	{
		return numInArray;
	}
};
