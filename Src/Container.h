#pragma once

#define zARRAY_START_ALLOC 16

template<class T> class zCArray
{
public:
	T *array;
	zINT numAlloc;
	zINT numInArray;

public:
	zCArray()
	{
		array = NULL;
		numAlloc = 0;
		numInArray = 0;
	}

	zCArray(const zINT startSize)
	{
		array = NULL;

		if (startSize > 0)
		{
			array = zNEW_ARRAY(T, startSize);
		}

		numAlloc = startSize;
		numInArray = 0;
	}

	~zCArray() { EmptyList(); }

	void AllocDelta(const zINT numDelta)
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

	void AllocAbs(const zINT size)
	{
		if (numAlloc >= size)
		{
			return;
		}

		AllocDelta(size - numAlloc);
	}

	const T &operator[](const zINT nr) const { return array[nr]; }
	T &operator[](const zINT nr) { return array[nr]; }

	zCArray<T> &operator=(const zCArray<T> &array2)
	{
		EmptyList();
		AllocAbs(array2.numInArray);
		numInArray = array2.numInArray;

		for (zINT i = 0; i < numInArray; i++) array[i] = array2[i];

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

	zINT Search(const T &data) const
	{
		if (numInArray > 0)
		{
			for (zINT i = 0; i < numInArray; i++)
			{
				if (array[i] == data) return i;
			}
		}

		return -1;
	}

	zBOOL IsInList(const T &data) const
	{
		if (numInArray > 0)
		{
			for (zINT i = 0; i < numInArray; i++)
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

			for (zINT i = 0; i < numInArray; i++)
			{
				newArray[i] = array[i];
			}

			zDELETE_ARRAY(array);

			array = newArray;
			numAlloc = numInArray;
		}
	}

	void RemoveLast(zBOOL release = FALSE)
	{
		if (numInArray <= 0) return;

		numInArray--;

		if (release) zDELETE(array[numInArray]);
	}

	void RemoveOrderIndex(const zINT index, zBOOL release = FALSE)
	{
		if (index > numInArray) return;

		if (release) zDELETE(array[index]);

		if (index != numInArray - 1)
		{
			for (zINT i = index; i < numInArray; i++)
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
	zINT numInArray;

public:
	zDWORD SetArray(void *ptr, const zINT num)
	{
		array = (T *)ptr;
		numInArray = num;

		return sizeof(T) * numInArray;
	}

	T *GetArray()
	{
		return array;
	}

	zINT GetNum()
	{
		return numInArray;
	}
};
