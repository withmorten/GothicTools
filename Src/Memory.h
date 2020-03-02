#pragma once

#define zNEW(className) new className
#define zNEW_ARRAY(className, count) new className[count]

#define zDELETE(obj) { delete obj; obj = NULL; }
#define zDELETE_ARRAY(arr) { delete[] arr; arr = NULL; }

template<typename T> T *zMALLOC(zSIZE_T _Count) { return (T *)malloc(_Count * sizeof(T)); }
template<typename T> T *zREALLOC(T *_Block, zSIZE_T _Count) { return (T *)realloc(_Block, _Count * sizeof(T)); }
template<typename T> T *zMEMSET(T *_Dst, zINT _Val, zSIZE_T _Count) { return (T *)memset(_Dst, _Val, sizeof(T) * _Count); }
template<typename T> T *zMEMCPY(T *_Dst, const T *_Src, zSIZE_T _Count) { return (T *)memcpy(_Dst, _Src, sizeof(T) * _Count); }

#define zFREE(mem) do { free(mem); mem = NULL; } while(0)
