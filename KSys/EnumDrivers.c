#include "driver.h"
#include "EnumDrivers.h"


/*///////////////////////////////////////////////////
/
/	Function: EnumDriversByQueryInformation
/
/	Purpose: Get list of loaded drivers to
/	send to usermode
/
/	Environment:
/		kernel mode only
/
*///////////////////////////////////////////////////

NTSTATUS EnumDriversByQueryInformation(OUT PVOID OutputBuffer, IN ULONG Outlengthsize) 
{
	NTSTATUS st = STATUS_UNSUCCESSFUL;
	UNICODE_STRING func;
	ULONG size = (Outlengthsize - sizeof(KERNEL_MODULES_INFORMATION)) / sizeof(KERNEL_MODULES_ENTRY);
	ULONG bytes = 0;
	PVOID buffer = NULL;
	PRTL_PROCESS_MODULES allocate = NULL;
	CHAR ModuleName[256];
	CHAR PathName[256];
	PKERNEL_MODULES_INFORMATION modinfo = (PKERNEL_MODULES_INFORMATION)OutputBuffer;

	RtlInitUnicodeStringEx(&func, L"ZwQuerySystemInformation");
	ZwQuerySystemInformation = MmGetSystemRoutineAddress(&func);
	if (ZwQuerySystemInformation && MmIsAddressValid(ZwQuerySystemInformation)) 
	{
		st = ZwQuerySystemInformation(SystemModuleInformation, NULL, 0, &bytes);
		if (st == STATUS_INFO_LENGTH_MISMATCH) 
		{
			buffer = ExAllocatePool(NonPagedPool, bytes);
			if (buffer != NULL)
			{
				st = ZwQuerySystemInformation(SystemModuleInformation, buffer, bytes, &bytes);
				if (NT_SUCCESS(st)) 
				{
					allocate = (PRTL_PROCESS_MODULES)buffer;
					if (allocate && MmIsAddressValid(allocate))
					{
						for (ULONG i = 0; i < allocate->NumberOfModules; ++i) 
						{
							ULONG contador = modinfo->NumberOfModules;					

							RtlStringCchCopyA(ModuleName, 256, allocate->Modules[i].FullPathName + allocate->Modules[i].OffsetToFileName);
							RtlStringCchCopyA(PathName, 256, allocate->Modules[i].FullPathName);

							ULONG ModuleNameSize = strlen(ModuleName);
							ULONG PathNameSize = strlen(PathName);

							modinfo->Modules[contador].ImageBase = allocate->Modules[i].ImageBase;
							modinfo->Modules[contador].ImageSize = allocate->Modules[i].ImageSize;
							modinfo->Modules[contador].LoadOrderIndex = allocate->Modules[i].LoadOrderIndex;

							RtlCopyMemory(modinfo->Modules[contador].FullPathName, PathName, PathNameSize);
							RtlCopyMemory(modinfo->Modules[contador].ModuleName, ModuleName, ModuleNameSize);

							DbgPrint("\n%s", ModuleName);
													
							modinfo->NumberOfModules++;
						}

					}

				}
				ExFreePool(buffer);
			}
		}
	}


	return st;

}