#include <Library/PrintLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>

#include "AOpenSerialPortLib.h"
//#include <Token.h>

//GaryQ0817#01 --->>>
//#include <Setup.h>
//#include <AmiDxeLib.h>

//GaryQ0817#01 ---<<<

/**
  Initialize for BaseSerialLib library instance.

  Initialization including: Cmos runtime flag, Serial Port, and USB.

  @retval RETURN_SUCCESS        The BaseSerialLib was initialized.

**/
RETURN_STATUS
EFIAPI
AOSerialPortInitialize (
  VOID
  )
 {
    UINT16 gCom2Base = AiCU_UART;
    UINTN gBps = AiCU_UART_BAUD_RATE;
    UINT8 gData = 8;
    UINT8 gStop = 1;
    UINT8 gParity = 0;
    UINT8 gBreakSet = 0;

    UINTN Divisor;
    UINT8 OutputData;
    UINT8 Data;

    Data = (UINT8) (gData - (UINT8) 5);

    Divisor = 115200 / gBps;

    OutputData = (UINT8) ((DLAB << 7) | ((gBreakSet << 6) | ((gParity << 3) | ((gStop << 2) | Data))));
    IoWrite8(gCom2Base + LCR_OFFSET, OutputData);

    IoWrite8(gCom2Base + BAUD_HIGH_OFFSET, (UINT8) (Divisor >> 8));
    IoWrite8(gCom2Base + BAUD_LOW_OFFSET, (UINT8) (Divisor & 0xff));

    OutputData = (UINT8) ((~DLAB << 7) | ((gBreakSet << 6) | ((gParity << 3) | ((gStop << 2) | Data))));
    IoWrite8(gCom2Base + LCR_OFFSET, OutputData);
    
    return RETURN_SUCCESS;
}

/**
  Common function to write trace data to a chosen debug interface like
  UART Serial device, USB Serial device or Trace Hub device

  @param  Buffer           Point of data buffer which need to be writed.
  @param  NumberOfBytes    Number of output bytes which are cached in Buffer.

**/
UINTN
EFIAPI
AOSerialPortWrite (
  IN UINT8     *Buffer,
  IN UINTN     NumberOfBytes
)
 {
    UINTN Result;
    UINT8 Data;

    if (NULL == Buffer) {
        return 0;
    }

    Result = NumberOfBytes;

    while (NumberOfBytes--) {
        //
        // Wait for the serial port to be ready.
        //
        do {
            Data = IoRead8(AiCU_UART + LSR_OFFSET);
        } while ((Data & LSR_TXRDY_LINE_IDLE) == 0); // GaryQ0426 >>>
                                                     // LSR : Line Status Register
                                                     // Bit 5 turns high as soon as the transmitter holding register 
                                                     // is empty whereas bit 6 indicates that also the shift register
                                                     // which outputs the bits on the line is empty.
                                                     // GaryQ0426 <<<
        IoWrite8(AiCU_UART, *Buffer++);
    }

    return Result;
}

/**
  Common function to Read data from UART serial device, USB serial device and save the datas in buffer.

  @param  Buffer           Point of data buffer which need to be writed.
  @param  NumberOfBytes    Number of output bytes which are cached in Buffer.

  @retval 0                Read data failed, no data is to be read.
  @retval >0               Actual number of bytes read from debug device.

**/
UINTN
EFIAPI
AOSerialPortRead (
  OUT UINT8     *Buffer,
  IN  UINTN     NumberOfBytes
)
{
    UINTN    MaxDelay = 0x1046A;    
    UINTN Result;
    UINT8 Data;
    
    if (NULL == Buffer) {
      return 0;
    }

    Result = NumberOfBytes;

    while (NumberOfBytes--) {
      MaxDelay = 0x1046A;      
      //
      // Wait for the serial port to be ready.
      //
      do {
        MaxDelay--;         
        Data = IoRead8 (AiCU_UART + LSR_OFFSET);
      } while (((Data & LSR_RXDA) == 0) && (!(MaxDelay <= 0)));
      
      if (!( MaxDelay <= 0 )) {
          *Buffer++ = IoRead8 (AiCU_UART);
      }
      else
      {
          NumberOfBytes = 0;  
      }
    }
    return Result;
}

VOID
EFIAPI
AOPrintMessage (
  IN  CONST CHAR8  *Format,
  ...
  )
{
  CHAR8    Buffer[MAX_DEBUG_MESSAGE_LENGTH];
  VA_LIST  Marker;

  AOSerialPortInitialize();
  //
  // Convert the message to an ASCII String
  //
  VA_START (Marker, Format);
  AsciiVSPrint (Buffer, sizeof (Buffer), Format, Marker);
  VA_END (Marker);

  //
  // Send the print string to a Serial Port
  //
  AOSerialPortWrite ((UINT8 *)Buffer, AsciiStrLen (Buffer));
}
