#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>
#include <Library/IoLib.h>
#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/LoadedImage.h>

#define DEBUG_PRINT 1

#ifdef DEBUG_PRINT
#define debug_print(expr, ...) Debug_Print("[%05d @ %a] " expr "\r\n", __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define debug_print
#endif

#define sleep(sec) gBS->Stall(1000000 * (sec))

VOID SMDbgPrint (
	IN const CHAR8*	_str,
	...
	);

#define PHY_PCI32_Addr(_M_BUS, _M_DEV, _M_FUN, _M_OFF) \
    (0x80000000 |                                      \
     ((((UINT8)_M_BUS) & 0xff) << 16) |                \
     ((((UINT8)_M_DEV) & 0x1f) << 11) |                \
     ((((UINT8)_M_FUN) & 0x07) << 8) |                 \
     (((UINT8)_M_OFF) & 0xfc))

#define debug_pause()                                                                         \
    if (gDebugPrint)                                                                          \
    {                                                                                         \
        UINTN __Index = 0;                                                                    \
        gST->ConOut->OutputString(gST->ConOut, L"###### Press Enter to continue ######\r\n"); \
        gST->ConIn->Reset(gST->ConIn, FALSE);                                                 \
        gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &__Index);                              \
    }

BOOLEAN gDebugPrint = TRUE;

VOID EFIAPI Debug_Print(IN CONST CHAR8 *Format, ...)
{
    VA_LIST Marker;
    CHAR16 Buffer[512];

    if (!gDebugPrint)
        return;

    VA_START(Marker, Format);
    UnicodeVSPrintAsciiFormat(Buffer, sizeof(Buffer), Format, Marker);
    VA_END(Marker);

    gST->ConOut->OutputString(gST->ConOut, Buffer);
}

BOOLEAN CheckUserContinue(void)
{
    EFI_INPUT_KEY InputKey;
    EFI_STATUS Status;
    UINTN EventIndex;

    Print(L"Are you sure to continue the restore process?\r\n");
    Print(L"Press Y or N for next step.\r\n");

    while (1)
    {
        gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &EventIndex);
        Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &InputKey);
        if (EFI_ERROR(Status))
        {
            debug_print("CheckUserContinue");
            return 0;
        }

        if (InputKey.UnicodeChar == L'Y' || InputKey.UnicodeChar == L'y')
        {
            debug_print("CheckUserContinue");
            return 1;
        }

        if (InputKey.UnicodeChar == L'N' || InputKey.UnicodeChar == L'n')
        {
            debug_print("CheckUserContinue");
            return 0;
        }
    }

    return 0;
}

STATIC EFI_STATUS EFIAPI parse_cmdline(IN EFI_HANDLE ImageHandle)
{
    CHAR16 *pCmdLine = NULL;
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_LOADED_IMAGE_PROTOCOL *pImageInfo = NULL;

    Status = gBS->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **)&pImageInfo);
    if (EFI_ERROR(Status))
    {
        debug_print("Failed to handle load image protocol %r", Status);
        return Status;
    }

    pCmdLine = (CHAR16 *)AllocatePool(pImageInfo->LoadOptionsSize + 4);
    SetMem(pCmdLine, pImageInfo->LoadOptionsSize + 4, 0);
    CopyMem(pCmdLine, pImageInfo->LoadOptions, pImageInfo->LoadOptionsSize);

    if (StrStr(pCmdLine, L"debug"))
    {
        gDebugPrint = TRUE;
        debug_print("parameter = debug");
        debug_pause();
    }

    if (StrStr(pCmdLine, L"garyq"))
    {
        //gLoadIsoEfi = TRUE;
        debug_print("parameter = garyq");
        debug_pause();
    }

    FreePool(pCmdLine);
    return EFI_SUCCESS;
}

EFI_STATUS
RWByteToSmbusV(
    UINT8 SlaveAddress,
    UINT8 Offset,
    UINT8 *Buffer);

EFI_STATUS
WriteByteToSmbus(
    UINT8 SlaveAddress,
    UINT8 Offset,
    UINT8 _Buffer)
{
    return RWByteToSmbusV(SlaveAddress & 0xFE, Offset, (UINT8 *)(UINTN)_Buffer);
}

EFI_STATUS
ReadByteFromSmbus(
    IN UINT8 SlaveAddress,
    IN UINT8 Offset,
    OUT UINT8 *Buffer)
{
    return RWByteToSmbusV(SlaveAddress | 0x01, Offset, Buffer);
}

EFI_STATUS
RWByteToSmbusV(
    UINT8 SlaveAddress,
    UINT8 Offset,
    UINT8 *Buffer)
{
    UINT32 u64PciAddress;
    UINT32 u32SMBUS_OrigBaseAddr;
    UINT16 SMBus_Address;
    UINT8 u8SMBCmd;
    UINT8 RetryCount = 3;
    UINT8 SmbStatus = 0;
    EFI_STATUS Status;

 //   debug_print("Hi, this is new progrm of SMBus.");
 //   debug_pause();

    // SMBus Device SMB_Base
    u64PciAddress = PHY_PCI32_Addr(0, 0x1f, 4, 0x20);
    IoWrite32(0xcf8, u64PciAddress);
    u32SMBUS_OrigBaseAddr = IoRead32(0xcfc);
    u32SMBUS_OrigBaseAddr &= 0xfffffffe;

    SMBus_Address = (UINT16)u32SMBUS_OrigBaseAddr;

 //   debug_print("Smbus base addre = %x", SMBus_Address);
 //   debug_pause();

    // Set "Write Byte"/"Read Byte" protocol and start bit
    u8SMBCmd = 0x48;

    while (RetryCount--)
    {
        // Clear all status bits
        IoWrite8(SMBus_Address + 0, 0xFE);

        // Set slave device command offset (Offset)
        IoWrite8(SMBus_Address + 3, Offset);

        // Set smbus slave address (Device)
        IoWrite8(SMBus_Address + 4, SlaveAddress);

        // Set smbus Write Data
        if (!(SlaveAddress & 1))
            IoWrite8(SMBus_Address + 5, (UINT8)(UINTN)Buffer);

        // Set smbus Command and Issue Started.
        IoWrite8(SMBus_Address + 2, u8SMBCmd);

        // Wait for HSTS.HBSY to be clear
        while (RetryCount | 1)
        {
            SmbStatus = IoRead8(SMBus_Address + 0);
            if (SmbStatus & BIT0)
                continue;
            // poll until any of FAIL, BERR, DERR, INTR
            if (SmbStatus & (BIT4 | BIT3 | BIT2 | BIT1))
                break;
        };

        // Check Status of FAIL, BERR, DERR
        if (SmbStatus & (BIT4 | BIT3 | BIT2))
        {
            // If Read Error, Change other "Byte" command
            //            if( SlaveAddress&1 )
            //                u8SMBCmd = 0x44;

            continue;
        }
        else if (SmbStatus & BIT1)
        {
            if (SlaveAddress & 1)
                *Buffer = IoRead8(SMBus_Address + 5);

            Status = EFI_SUCCESS;
            break;
        }
    }

    return Status;
}

EFI_STATUS
EFIAPI
SMBusEntry(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    //EFI_STATUS Status;
    UINT8 data;
    UINT8 i;

    debug_print("Hi, this is new progrm of SMBus.");
    for (i = 0; i < 0x10; i++)
    {
        ReadByteFromSmbus(0xA0, i, &data);
        Debug_Print("%02x ", data);
        SMDbgPrint("%02x ",data);
    }

    Debug_Print("\n");
    debug_pause();

    SMDbgPrint("Gary SMBus Test\r\n");

    return EFI_SUCCESS;
}
