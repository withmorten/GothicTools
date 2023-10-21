#pragma once

#include "GothicTools.h"

#define zNEW(className) new className
#define zNEW_ARRAY(className, count) new className[count]

#define zDELETE(obj) do { if (obj) { delete obj; obj = NULL; } } while(0)
#define zDELETE_ARRAY(arr) do { if (arr) { delete[] arr; arr = NULL; } } while(0)

#define zMALLOC(type, count) (type *)malloc(count * sizeof(type))
#define zCALLOC(type, count) (type *)calloc(count, sizeof(type))
#define zREALLOC(type, pblock, newcount) (type *)realloc(pblock, newcount * sizeof(type))

#define zFREE(mem) do { free(mem); mem = NULL; } while(0)

template<typename T> T *zMEMSET(T *_Dst, int32 _Val, size_t _Count) { return (T *)memset(_Dst, _Val, sizeof(T) * _Count); }
template<typename T> T *zMEMCPY(T *_Dst, const T *_Src, size_t _Count) { return (T *)memcpy(_Dst, _Src, sizeof(T) * _Count); }
