#pragma once

// Windows type aliases for cross-reference clarity.
// When compiling on Windows with <windows.h>, these types are already defined.
// This header exists as a reference and for potential cross-platform stubs.

#ifndef _WINDOWS_

#include <cstdint>

typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;
typedef unsigned long long  UINT64;
typedef long                LONG;
typedef long long           LONGLONG;
typedef int                 INT32;
typedef wchar_t             WCHAR;
typedef void*               HANDLE;
typedef void*               LPVOID;
typedef const wchar_t*      LPCWSTR;
typedef wchar_t*            LPWSTR;
typedef int                 BOOL;

#define INVALID_HANDLE_VALUE ((HANDLE)(long long)-1)
#define TRUE  1
#define FALSE 0

union LARGE_INTEGER {
    struct {
        DWORD LowPart;
        LONG  HighPart;
    };
    LONGLONG QuadPart;
};

#endif // _WINDOWS_
