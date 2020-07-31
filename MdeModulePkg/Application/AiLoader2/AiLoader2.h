#ifndef _INCLUDED_AILOADER2_H_
#define _INCLUDED_AILOADER2_H_

#define EfiAppendDevicePathNode             _DPAddNode

typedef struct {
    FILEPATH_DEVICE_PATH FileDevicePath;
    CHAR16 PathNameEx[(sizeof(GRUB_LOADER_PATH) / 2) - 1];
    EFI_DEVICE_PATH_PROTOCOL EndDevicePath;
} FILE_DEVICE_PATH;

extern const UINTN rawData_Grucfg_size;
extern const CHAR8 rawData_Grucfg[];

//#define DEBUG_PRINT 1
#ifdef DEBUG_PRINT
#define debug_print Print
#else
#define debug_print
#endif

#endif
