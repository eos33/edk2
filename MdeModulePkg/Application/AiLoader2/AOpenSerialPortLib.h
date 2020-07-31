#ifndef __AOSERIALPORTLIB__H__
#define __AOSERIALPORTLIB__H__

#define AiCU_UART 0x2F8
#define AiCU_UART_BAUD_RATE 115200

#define BAUD_LOW_OFFSET   0x00
#define BAUD_HIGH_OFFSET  0x01
#define IER_OFFSET        0x01
#define LCR_SHADOW_OFFSET 0x01
#define FCR_SHADOW_OFFSET 0x02
#define IR_CONTROL_OFFSET 0x02
#define FCR_OFFSET        0x02
#define EIR_OFFSET        0x02
#define BSR_OFFSET        0x03
#define LCR_OFFSET        0x03
#define MCR_OFFSET        0x04
#define LSR_OFFSET        0x05
#define MSR_OFFSET        0x06

#define LSR_TXRDY_LINE_IDLE 0x40
#define LSR_TXRDY 0x20
#define LSR_RXDA  0x01
#define DLAB      0x01

#define UART_DATA    8
#define UART_STOP    1
#define UART_PARITY  0
#define UART_BREAK_SET  0

#define MAX_DEBUG_MESSAGE_LENGTH  0x100

RETURN_STATUS
EFIAPI
AOSerialPortInitialize (
  VOID
  );

UINTN
EFIAPI
AOSerialPortWrite (
  IN UINT8     *Buffer,
  IN UINTN     NumberOfBytes
  );
  
UINTN
EFIAPI
AOSerialPortRead (
  OUT UINT8   *Buffer,
  IN  UINTN   NumberOfBytes
  );

VOID
EFIAPI
AOPrintMessage (
  IN  CONST CHAR8  *Format,
  ...
  );

//GaryQ0817#01 --->>>
#include <Uefi/UefiBaseType.h>
extern EFI_GUID gSetupGuid;
UINT8 ReadRtcIndex(IN UINT8 Index);
UINT8 GetDayOfTheWeek( IN EFI_TIME *Time);
UINTN
EfiValueToString (
IN  OUT CHAR8  *Buffer,
IN  INT64       Value,
IN  UINTN       Flags,
IN  UINTN       Width
);
void SyncTime_RTC_to_MCU(void);
void Enable_SuperIO_UART2(void);
//GaryQ0817#01 ---<<<
#endif
