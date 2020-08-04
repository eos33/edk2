#ifndef _INCLUDED_AILOADER2_H_
#define _INCLUDED_AILOADER2_H_

#define END_DEVICE_PATH                 END_DEVICE_PATH_TYPE
#define END_ENTIRE_SUBTYPE              END_ENTIRE_DEVICE_PATH_SUBTYPE
#if 1
#define NODE_LENGTH(pPath) ((pPath)->Length[0]+((pPath)->Length[1]<<8))
#define SET_NODE_LENGTH(pPath,NodeLength) ((pPath)->Length[0]=(UINT8)(NodeLength),(pPath)->Length[1]=(NodeLength)>>8)
#define NEXT_NODE(pPath) ((EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)(pPath)+NODE_LENGTH(pPath)))
#define isEndNode(pDp) ((pDp)->Type==END_DEVICE_PATH)
#define EfiAppendDevicePathNode             _DPAddNode
#endif
#define GRUB_LOADER_PATH L"\\EFI\\AOPEN\\grubx64.efi"

typedef struct {
    FILEPATH_DEVICE_PATH FileDevicePath;
    CHAR16 PathNameEx[(sizeof(GRUB_LOADER_PATH) / 2) - 1];
    EFI_DEVICE_PATH_PROTOCOL EndDevicePath;
} FILE_DEVICE_PATH;

extern const UINTN rawData_Grucfg_size;
extern CHAR8 rawData_Grucfg[];

#define DEBUG_PRINT 1

#ifdef DEBUG_PRINT
#define debug_print(expr, ...) Debug_Print("[%05d @ %a] "expr"\r\n", __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define debug_print
#endif

#define sleep(sec) gBS->Stall(1000000 * (sec))

#define debug_pause() \
{ \
    UINTN __Index = 0; \
    gST->ConOut->OutputString(gST->ConOut, L"###### Press Enter to continue ######\r\n");\
    gST->ConIn->Reset(gST->ConIn, FALSE); \
    gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &__Index);\
}

#endif
