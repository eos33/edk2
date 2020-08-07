#if !defined(SMDBG_BASE_TYPE)
#define SMDBG_BASE_TYPE

typedef unsigned char BOOLEAN;

#pragma warning ( disable : 4214 )

#if !defined (_MSC_VER) || (_MSC_VER < 1300)
#define __w64
#define __w64__
#endif

#if defined(EFI64) || defined(EFIx64)
typedef __int64             INTN;
typedef unsigned __int64    UINTN;
#else
//typedef int __w64           INTN;
//typedef unsigned int __w64  UINTN;
#endif

typedef signed char         INT8;
typedef unsigned char       UINT8;
typedef short               INT16;
typedef unsigned short      UINT16;
typedef int                 INT32;
typedef unsigned int        UINT32;
typedef __int64             INT64;
typedef unsigned __int64    UINT64;
typedef char                CHAR8;
typedef unsigned short      CHAR16; 

typedef CHAR8*              va_list;

#ifndef VOID
#define __VOID__
#define VOID void
#define TRUE 1
#define FALSE 0
#endif

#if defined(__w64__)
#undef __w64
#endif

#endif  // defined (SMDBG_BASE_TYPE)
