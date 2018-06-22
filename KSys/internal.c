#include "driver.h"
#include "internal.h"

/*//////////////////////////////////////////////////////////////////
/
/	Function: GetInternalOsFunctionAddressByOffsetAndFunctionName:
/
/	Purpose:
/		Scan kernel memory to find internal Functions Address
/
*///////////////////////////////////////////////////////////////////

UINT_PTR GetInternalOsFunctionAddressByOffsetAndFunctionName(
	IN PVOID FunctionAddress,
	IN PCWSTR FunctionName,
	IN UINT8 FirstOffset,
	IN UINT8 SecondOffset,
	IN OPTIONAL UINT8 ThirdOffset)
{
	UNICODE_STRING func;

	FunctionAddress = NULL;
	RtlSecureZeroMemory(&func, sizeof(func));
	RtlInitUnicodeStringEx(&func, FunctionName);

	FunctionAddress = (PVOID)MmGetSystemRoutineAddress(&func);
	if (FunctionAddress && MmIsAddressValid(&func)) 
	{
		
		__try 
		{

			PUINT8 i = NULL;
			PUINT8 StartScannerAddress = (PUINT8)FunctionAddress;
			PUINT8 EndScannerAddress = StartScannerAddress + 0x500;
			UINT8 value1 = 0, value2 = 0, value3 = 0;
#ifndef _WIN64
			INT32 offset = 0;
#else
			INT64 offset = 0;
#endif _WIN64
			for (i = StartScannerAddress; i < EndScannerAddress; ++i) 
			{
				if (ARGUMENT_PRESENT(ThirdOffset))
				{

					if (MmIsAddressValid(i) && MmIsAddressValid(i + 1) && MmIsAddressValid(i + 7))
					{
						
						value1 = *i;
						value2 = *(i + 1);
						value3 = *(i + 7);
						if (value1 == FirstOffset && value2 == SecondOffset && value3 == ThirdOffset)
						{
							RtlCopyMemory(&offset, i + 3, 4);
#ifndef _WIN64
							return(UINT_PTR)(offset + (UINT32)i + 5);
#else
							return(UINT_PTR)(offset + (UINT64)i + 7);
#endif _WIN64
						}
						
					}
				}
				else
				{

					value1 = *i;
					value2 = *(i + 5);
					if (value1 == FirstOffset && value2 == SecondOffset)
					{
						RtlCopyMemory(&offset, i + 1, 4);
#ifndef _WIN64
						return(UINT_PTR)(offset + (UINT32)i + 5);
#else
						return(UINT_PTR)(offset + (UINT64)i + 5);
#endif _WIN64
					}
				}
			}

		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			return GetExceptionCode();
		}
	}

	return 0;
}


/*/////////////////////////////////
/
/	Function: LoadInternalAPI
/
/	purpose:
/		Load APIs
/
*/////////////////////////////////


VOID LoadInternalAPI()
{

	PVOID ntdll = NULL;

#ifndef _WIN64
	IopCreateFile = (IOPCREATEFILE)GetInternalOsFunctionAddressByOffsetAndFunctionName(IoCreateFileOriginal, L"IoCreateFile", 0xe8, 0x5d, NULL);
#else
	IopCreateFile = (IOPCREATEFILE)GetInternalOsFunctionAddressByOffsetAndFunctionName(IoCreateFileOriginal, L"IoCreateFile", 0xe8, 0x48, NULL);
#endif _WIN64
	if (!IopCreateFile || MmIsAddressValid(IopCreateFile) == FALSE) 
	{
		return;
	}


#ifndef _WIN64
	PspTerminateThreadByPointer = (PSPTERMINATETHREADBYPOINTER)GetInternalOsFunctionAddressByOffsetAndFunctionName(PsTerminateSystemThreadOriginal, L"PsTerminateSystemThread", 0xe8, 0x5d, NULL);
#else
	PspTerminateThreadByPointer = (PSPTERMINATETHREADBYPOINTER)GetInternalOsFunctionAddressByOffsetAndFunctionName(PsTerminateSystemThreadOriginal, L"PsTerminateSystemThread", 0xe8, 0x90, NULL);
#endif _WIN64
	if (!PspTerminateThreadByPointer || MmIsAddressValid(PspTerminateThreadByPointer) == FALSE) 
	{
		return;
	}
	

}

