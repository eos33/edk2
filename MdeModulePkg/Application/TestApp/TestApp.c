#include <Library/UefiApplicationEntryPoint.h> 
#include <Library/UefiLib.h>

EFI_STATUS
EFIAPI
UefiMain
(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE* SystemTable)
{
	SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Hello World\r\n");  // Body
	return EFI_SUCCESS;
}