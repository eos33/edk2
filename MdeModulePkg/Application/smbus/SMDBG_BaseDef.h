#if !defined(SMDBG_BASE_DEFINITION)
#define SMDBG_BASE_DEFINITION

#pragma warning ( disable : 4214 )

#define EFI_STATUS                UINTN

#define EFI_MAX_BIT               ((UINTN)0x01 << ((sizeof (char *) * 8) - 1))
//#define EFIERR(a)                 (EFI_MAX_BIT | (a))
#define EFIWARN(a)                (a)

#ifndef _INTSIZEOF
#define _INTSIZEOF(n)( (sizeof(n) + sizeof(UINTN) - 1) & ~(sizeof(UINTN) - 1) )
#endif 
// Also support coding convention rules for var arg macros
#ifndef va_start
#define va_start(ap,v)  ( ap = (va_list)&(v) + _INTSIZEOF(v) )
#define va_arg(ap,t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_end(ap)      ( ap = (va_list)0 )
#endif

#if !defined(BIT0)
#define BIT0                    0x0001
#define BIT1                    0x0002
#define BIT2                    0x0004
#define BIT3                    0x0008
#define BIT4                    0x0010
#define BIT5                    0x0020
#define BIT6                    0x0040
#define BIT7                    0x0080
#define BIT8                    0x0100
#define BIT9                    0x0200
#define BIT10                   0x0400
#define BIT11                   0x0800
#define BIT12                   0x1000
#define BIT13                   0x2000
#define BIT14                   0x4000
#define BIT15                   0x8000
#endif
#define BIT16                   0x00010000
#define BIT17                   0x00020000
#define BIT18                   0x00040000
#define BIT19                   0x00080000
#define BIT20                   0x00100000
#define BIT21                   0x00200000
#define BIT22                   0x00400000
#define BIT23                   0x00800000
#define BIT24                   0x01000000
#define BIT25                   0x02000000
#define BIT26                   0x04000000
#define BIT27                   0x08000000
#define BIT28                   0x10000000
#define BIT29                   0x20000000
#define BIT30                   0x40000000
#define BIT31                   0x80000000

#define EFI_TYPE_STR_TFORM( _val ) {_val,(const CHAR8*)#_val}

#define EFI_ERROR_TYPE_STR_TABLE \
        EFI_TYPE_STR_TFORM(EFI_LOAD_ERROR), \
        EFI_TYPE_STR_TFORM(EFI_INVALID_PARAMETER), \
        EFI_TYPE_STR_TFORM(EFI_UNSUPPORTED), \
        EFI_TYPE_STR_TFORM(EFI_BAD_BUFFER_SIZE), \
        EFI_TYPE_STR_TFORM(EFI_BUFFER_TOO_SMALL), \
        EFI_TYPE_STR_TFORM(EFI_NOT_READY), \
        EFI_TYPE_STR_TFORM(EFI_DEVICE_ERROR), \
        EFI_TYPE_STR_TFORM(EFI_WRITE_PROTECTED), \
        EFI_TYPE_STR_TFORM(EFI_OUT_OF_RESOURCES), \
        EFI_TYPE_STR_TFORM(EFI_VOLUME_CORRUPTED), \
        EFI_TYPE_STR_TFORM(EFI_VOLUME_FULL), \
        EFI_TYPE_STR_TFORM(EFI_NO_MEDIA), \
        EFI_TYPE_STR_TFORM(EFI_MEDIA_CHANGED), \
        EFI_TYPE_STR_TFORM(EFI_NOT_FOUND), \
        EFI_TYPE_STR_TFORM(EFI_ACCESS_DENIED), \
        EFI_TYPE_STR_TFORM(EFI_NO_RESPONSE), \
        EFI_TYPE_STR_TFORM(EFI_NO_MAPPING), \
        EFI_TYPE_STR_TFORM(EFI_TIMEOUT), \
        EFI_TYPE_STR_TFORM(EFI_NOT_STARTED), \
        EFI_TYPE_STR_TFORM(EFI_ALREADY_STARTED), \
        EFI_TYPE_STR_TFORM(EFI_ABORTED), \
        EFI_TYPE_STR_TFORM(EFI_ICMP_ERROR), \
        EFI_TYPE_STR_TFORM(EFI_TFTP_ERROR), \
        EFI_TYPE_STR_TFORM(EFI_PROTOCOL_ERROR), \
        EFI_TYPE_STR_TFORM(EFI_INCOMPATIBLE_VERSION), \
        EFI_TYPE_STR_TFORM(EFI_SECURITY_VIOLATION), \
        EFI_TYPE_STR_TFORM(EFI_CRC_ERROR) 

#define EFI_WARN_TYPE_STR_TABLE \
        EFI_TYPE_STR_TFORM(EFI_WARN_UNKNOWN_GLYPH), \
        EFI_TYPE_STR_TFORM(EFI_WARN_DELETE_FAILURE), \
        EFI_TYPE_STR_TFORM(EFI_WARN_WRITE_FAILURE), \
        EFI_TYPE_STR_TFORM(EFI_WARN_BUFFER_TOO_SMALL) \

#endif  // defined(SMDBG_BASE_DEFINITION)
