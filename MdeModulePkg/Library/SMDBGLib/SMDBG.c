#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/IoLib.h>
#include <Uefi.h>
#include <Library/BaseMemoryLib.h>

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
