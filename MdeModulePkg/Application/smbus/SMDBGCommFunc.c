//**********************************************************************
// $Header: SMDBGCommFunc.c
//
// $Revision: 1 $
//       Create by HPBirdChen.
//
// $Date: 
//**********************************************************************
#include "SMDBG_BaseType.h"
#include "SMDBG_BaseDef.h"
#include <Uefi.h>
#define SMDBG_DEVICE_SMBUS_ADDR     0x66
#define SMDBG_DEVICE_OUTPUT_ASCII   0x7E

#define NEW_LINE_LF_ADD_CR          1

#define REPLACE_EMPTY_BY_EZRO		1

//typedef struct {
//  UINT32  Data1;
//  UINT16  Data2;
//  UINT16  Data3;
//  UINT8   Data4[8];
//} EFI_GUID;

struct {
    EFI_STATUS      _val;
    const CHAR8*    _str;
} EFI_TypeStr_Table[] = 
{
    EFI_ERROR_TYPE_STR_TABLE,
    EFI_WARN_TYPE_STR_TABLE,
    EFI_TYPE_STR_TFORM(EFI_SUCCESS)
};

EFI_STATUS
WriteByteToSmbus(
	UINT8 SlaveAddress,
	UINT8 Offset,
	UINT8 _Buffer);

EFI_STATUS SMDBG_SendByte(UINT8   _OutByte, ...)
{
    return WriteByteToSmbus(SMDBG_DEVICE_SMBUS_ADDR,SMDBG_DEVICE_OUTPUT_ASCII,(CHAR8)_OutByte);
}

#define FIX_STRING	0x02000000

EFI_STATUS CheckDevPresent(
    VOID
){
	return EFI_SUCCESS;
}

UINT64 CpuShr64(
	IN UINT64 Value,
	IN UINT8 Shift
	)
{
	return Value>>Shift;
}

UINT64 CpuShl64(
	IN UINT64 Value,
	IN UINT8 Shift
	)
{
	return Value<<Shift;
}

//#if SMDBG_WSTRING_SUPPORT
VOID __SMDebugTypePrintW (
  IN  const UINTN ErrorLevel,
  IN  const CHAR16 *Format,
  ...
  )
{
	va_list	Marker;
	UINT16	u16Idx, u16Count;
	va_start(Marker, Format);
	switch(ErrorLevel){
		case FIX_STRING:
			u16Count = (UINT16)va_arg(Marker,UINT32);
			for(u16Idx=0; u16Idx<u16Count; u16Idx++){
#if NEW_LINE_LF_ADD_CR
                // LF: 0x0A, CR: 0x0D
				if(Format[u16Idx] == 0x0A ){
				    SMDBG_SendByte(0x0D);
//				    SMDBG_SendByte(0x00);
                }
#endif
				SMDBG_SendByte( (CHAR8)Format[u16Idx] );
//				SMDBG_SendByte( (CHAR8)(Format[u16Idx]>>8) );
			}
			break;
	}
	va_end(Marker);
}
//#endif

VOID __SMDebugTypePrint (
  IN  const UINTN ErrorLevel,
  IN  const CHAR8 *Format,
  ...
  )
{
	va_list	Marker;
	UINT16	u16Idx, u16Count;
	va_start(Marker, Format);
	switch(ErrorLevel){
		case FIX_STRING:
			u16Count = va_arg(Marker,UINT16);
			for(u16Idx=0; u16Idx<u16Count; u16Idx++){
#if NEW_LINE_LF_ADD_CR
                // LF: 0x0A, CR: 0x0D
				if(Format[u16Idx] == 0x0A ){
				    SMDBG_SendByte(0x0D);
                }
#endif
				SMDBG_SendByte( Format[u16Idx] );
			}
			break;
	}
	va_end(Marker);
}


UINT32 _GetStrLen(const CHAR8* _str)
{
	UINT32	u32StrCount;
	for(u32StrCount = 0; _str[u32StrCount] != '\0'; ++u32StrCount) {};
	return u32StrCount;
}

//#if SMDBG_WSTRING_SUPPORT
UINT32 _GetStrLenW(const CHAR16* _str)
{
	UINT32	u32StrCount;
	for(u32StrCount = 0; _str[u32StrCount] != (CHAR16)'\0'; ++u32StrCount) {};
	return u32StrCount;
}
//#endif

VOID HexToStr(
	IN	UINT64	_Value,
	IN	UINT8*	_pOutBuf,
	IN	UINT16	_u16BufSize,
	OUT	UINT16*	_u16StrLen)
{
	UINT16	u16RemBufIdx;
	UINT8	u8TmpValue;

	_pOutBuf[0] = '0';

    if(_Value == 0){
		*_pOutBuf = '0';
		*_u16StrLen = 1;
		return;
	}

	for( u16RemBufIdx = _u16BufSize-1; \
		_Value>0 && u16RemBufIdx !=-1; _Value = CpuShr64( _Value, 4 ), --u16RemBufIdx){
		u8TmpValue =  (UINT8)(_Value & 0x0f);
		if(u8TmpValue < 0x0A)
			_pOutBuf[u16RemBufIdx] = u8TmpValue + '0';
		else{
			u8TmpValue -= 0x0A;
			_pOutBuf[u16RemBufIdx] = u8TmpValue + 'A';
		}
	}
	if(++u16RemBufIdx == 0){
		(*_u16StrLen) = _u16BufSize;
		 return;
	}

	for( *_u16StrLen=0; u16RemBufIdx<_u16BufSize; ++(*_u16StrLen), ++u16RemBufIdx){
		_pOutBuf[*_u16StrLen] = _pOutBuf[u16RemBufIdx];
	}
}

VOID DecToStr(
		IN UINTN	_Value,
		IN UINT8*	_pOutBuf,
		IN UINT16	_u16BufSize,
		OUT UINT16*	_u16StrLen)
{
	UINT16	u16RemBufIdx;
	if(_Value == 0){
		*_pOutBuf = '0';
		*_u16StrLen = 1;
		return;
	}
	for( u16RemBufIdx = _u16BufSize-1; \
		_Value>0 && u16RemBufIdx !=-1; _Value /= 10, --u16RemBufIdx){
		_pOutBuf[u16RemBufIdx] = (UINT8)(_Value%10) + '0';
	}
	if(++u16RemBufIdx == 0) {
		(*_u16StrLen) = _u16BufSize;
		return;
	}
	for( *_u16StrLen=0; u16RemBufIdx<_u16BufSize; ++(*_u16StrLen), ++u16RemBufIdx){
		_pOutBuf[*_u16StrLen] = _pOutBuf[u16RemBufIdx];
	}
	
}

#define va_argPre(ap,t)    ( *(t *)((ap -= _INTSIZEOF(t)) - _INTSIZEOF(t)) )

VOID SMDbgPrint (
	IN const CHAR8*	_str,
	...
	);

VOID SMDbgPrintV (
	IN const CHAR8*	_str,
	va_list ArgList
	)
{
#define strBufLen 24
	va_list	Marker;
	UINT32	u32Count;
	UINT16	u16TmpVale,u16StrIdx;
	UINT64  ParamValue;
	CHAR8	strBuf[strBufLen];
	UINT8	u8RollBackValue = 0;
	UINT8	u8StrWidth = 0;
	UINT8	u8Idx;
	UINT8	u8TmpVal;
    Marker = ArgList;
	u32Count = 0;

    if(EFI_SUCCESS != CheckDevPresent() ) return;

	for( u16StrIdx=0; _str[u16StrIdx] != '\0'; ++u16StrIdx){

		if( _str[u16StrIdx] == '%'){
			if(u32Count){
				__SMDebugTypePrint( FIX_STRING,&_str[u16StrIdx-u32Count],u32Count );
				u32Count = 0;
			}

			ParamValue = (UINT64)va_arg(Marker,UINTN);
			u8RollBackValue = 0;
			u8StrWidth = 0;
RollBackAgain:
			switch( _str[++u16StrIdx] ){
				case 'c':
					strBuf[0] = (CHAR8)ParamValue;
					u16TmpVale = 1;
					__SMDebugTypePrint( FIX_STRING, strBuf,u16TmpVale );
					break;
				case 'i':
				case 'D':
				case 'd':
					DecToStr((UINTN)ParamValue,(UINT8*)&strBuf[0],strBufLen,&u16TmpVale);
					if(u8StrWidth>u16TmpVale){
						for( u8Idx=0; u8Idx<(u8StrWidth-u16TmpVale); ++u8Idx)
                        {
#if REPLACE_EMPTY_BY_EZRO
                            SMDBG_SendByte('0');
#else
                            SMDBG_SendByte(' ');
#endif
                        }
					}
                    else{
                        if( u8StrWidth ){
                            __SMDebugTypePrint( FIX_STRING, strBuf+(u16TmpVale-u8StrWidth),(UINTN)u8StrWidth );
                            break;
                        }
                    }
					__SMDebugTypePrint( FIX_STRING, strBuf,u16TmpVale );
					break;
				case 'g':		// should pointer to GUID
					SMDbgPrint((const CHAR8*)"{%08x-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x}",
									(UINTN)(*(EFI_GUID*)(UINTN)ParamValue).Data1,
									(UINTN)(*(EFI_GUID*)(UINTN)ParamValue).Data2,
									(UINTN)(*(EFI_GUID*)(UINTN)ParamValue).Data3,
									(UINTN)(*(EFI_GUID*)(UINTN)ParamValue).Data4[0],
									(UINTN)(*(EFI_GUID*)(UINTN)ParamValue).Data4[1],
									(UINTN)(*(EFI_GUID*)(UINTN)ParamValue).Data4[2],
									(UINTN)(*(EFI_GUID*)(UINTN)ParamValue).Data4[3],
									(UINTN)(*(EFI_GUID*)(UINTN)ParamValue).Data4[4],
									(UINTN)(*(EFI_GUID*)(UINTN)ParamValue).Data4[5],
									(UINTN)(*(EFI_GUID*)(UINTN)ParamValue).Data4[6],
									(UINTN)(*(EFI_GUID*)(UINTN)ParamValue).Data4[7] );
					break;
                case 'r':   
                    for(u8Idx=0; u8Idx<sizeof(EFI_TypeStr_Table)/sizeof(EFI_TypeStr_Table[0]); ++u8Idx){
                        if( EFI_TypeStr_Table[u8Idx]._val == (EFI_STATUS)ParamValue)
                            break;
                    }
                    if( u8Idx == sizeof(EFI_TypeStr_Table)/sizeof(EFI_TypeStr_Table[0] ) )
                        SMDbgPrint((const CHAR8*)"EFI_STATUS[%x]", ParamValue);
                    else
                        SMDbgPrint((const CHAR8*)"%s",EFI_TypeStr_Table[u8Idx]._str);
                    break;
				case 'X':
				case 'x':
					HexToStr(ParamValue,(UINT8*)&strBuf[0],strBufLen,&u16TmpVale);
					if(u8StrWidth>u16TmpVale){
						for( u8Idx=0; u8Idx<(u8StrWidth-u16TmpVale); ++u8Idx)
						{
#if REPLACE_EMPTY_BY_EZRO
						    SMDBG_SendByte('0');
#else
						    SMDBG_SendByte(' ');
#endif
						}
					}
                    else{
                        if( u8StrWidth ){
                            __SMDebugTypePrint( FIX_STRING, strBuf+(u16TmpVale-u8StrWidth),(UINTN)u8StrWidth );
                            break;
                        }
                    }
					__SMDebugTypePrint( FIX_STRING, strBuf,u16TmpVale );
					break;
				case 'S':
                    u16TmpVale = (UINT16)_GetStrLenW((CHAR16*)(UINTN)ParamValue);
					if(u8StrWidth>u16TmpVale){
						for( u8Idx=0; u8Idx<(u8StrWidth-u16TmpVale); ++u8Idx)
//							SMDBG_SendByte(' ');
                            __SMDebugTypePrintW( FIX_STRING, L" ", 1 );
					}
                    __SMDebugTypePrintW( FIX_STRING, (CHAR16*)(UINTN)ParamValue,u16TmpVale );
                    break;
				case 's':
					u16TmpVale = (UINT16)_GetStrLen((CHAR8*)(UINTN)ParamValue);
					if(u8StrWidth>u16TmpVale){
						for( u8Idx=0; u8Idx<(u8StrWidth-u16TmpVale); ++u8Idx)
						    SMDBG_SendByte(' ');
					}
                    __SMDebugTypePrint( FIX_STRING, (CHAR8*)(UINTN)ParamValue,u16TmpVale );
					break;
				default:	// others ,can't recognize or '\0', need just to print
					u8TmpVal = _str[u16StrIdx];
					if( 'l' == u8TmpVal){
                        u8TmpVal = sizeof(UINTN);
                        if( u8TmpVal != sizeof(UINT64) ){
                            u8StrWidth = sizeof(UINT64)*2;
                            ParamValue |= CpuShl64( (UINT64)va_arg(Marker,UINTN), 32 );
                        }else{
                            u8StrWidth = sizeof(UINTN)*2;
                        }
						++u8RollBackValue;
						goto RollBackAgain;
					}
					else if( '0'<=u8TmpVal && u8TmpVal <= '9'){	
						u8StrWidth *= 10;
						u8StrWidth = u8StrWidth + (u8TmpVal -= '0');
						++u8RollBackValue;
						goto RollBackAgain;
					}
					--u16StrIdx;
					++u32Count;
					if(u8RollBackValue)
						u32Count += u8RollBackValue;
					// roll back to previus argument address
					va_argPre(Marker,UINTN);
					break;

			}
		} // end if( *_str = '%')
		else
			++u32Count;
	}

	if(u32Count){
		__SMDebugTypePrint( FIX_STRING,&_str[u16StrIdx-u32Count],u32Count );
		u32Count = 0;
	}

	va_end(Marker);
#undef strBufLen
}

VOID SMDbgPrint (
	IN const CHAR8*	_str,
	...
	)
{
    va_list	Marker;
    va_start(Marker, _str);
    SMDbgPrintV( _str, Marker);
}

//#endif

//#if defined(M_SMDBG_PEI_PHASE)

/*
VOID SMDbgBreak(
    VOID
    );

VOID SMDbgStepBreak(
    const CHAR8*  _str,
    ...
    )
{
    va_list	Marker;
    va_start(Marker, _str);
    SMDbgPrintV( _str, Marker);
    SMDbgBreak();
}
*/

//===================Below Are General Function.

#if defined(DXE_PHASE)
UINT64 CpuShr64(
	IN UINT64 Value,
	IN UINT8 Shift
	)
{
	return Value>>Shift;
}

UINT64 CpuShl64(
	IN UINT64 Value,
	IN UINT8 Shift
	)
{
	return Value<<Shift;
}

UINT64 CpuDiv64 (
	IN UINT64	Dividend,
	IN UINTN	Divisor,	//Can only be 31 bits.
  	OUT UINTN	*Remainder
  	)
{
	UINT64 	Result = Dividend/Divisor;
	if (Remainder) *Remainder=Dividend%Divisor;
  	return Result;
}

UINT64 CpuMul64(
    IN UINT64   Value64,
    IN UINTN	Value32
    )
{
	return Value64*Value32;
}

#endif
