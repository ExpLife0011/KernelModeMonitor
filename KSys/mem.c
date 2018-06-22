#include "driver.h"
#include "mem.h"


/*///////////////////////////////////////////////////
/
/	Function: GetProcessMemoryInformationByProcessId
/
/	Purpose: Get Process memory information by
/	given ID
/
/	Environment:
/		kernel mode only
/
*///////////////////////////////////////////////////

NTSTATUS GetProcessMemoryInformationByProcessId(IN HANDLE ProcessId, OUT PVOID OutputBuffer, IN ULONG Outlengthsize) 
{
	NTSTATUS st;
	UNICODE_STRING func = { 0 };
	ULONG bytes = 0;
	ULONG size = (Outlengthsize - sizeof(KERNEL_MEM_INFO));
	PSYSTEM_PROCESS_INFORMATION psinfo = NULL;
	PKERNEL_MEM_INFO meminfo = (PKERNEL_MEM_INFO)OutputBuffer;

	RtlInitUnicodeString(&func, L"ZwQuerySystemInformation");
	ZwQuerySystemInformation = MmGetSystemRoutineAddress(&func);
	if (ZwQuerySystemInformation && MmIsAddressValid(ZwQuerySystemInformation)) 
	{
		st = ZwQuerySystemInformation(SystemProcessInformation, NULL, 0, &bytes);
		if (st == STATUS_INFO_LENGTH_MISMATCH) 
		{
			PVOID alloc = ExAllocatePoolWithTag(NonPagedPool, bytes, 'KxS');
			if (alloc != NULL)
			{
				st = ZwQuerySystemInformation(SystemProcessInformation, alloc, bytes, &bytes);
				if (NT_SUCCESS(st)) 
				{
					psinfo = (PSYSTEM_PROCESS_INFORMATION)alloc;
					if (psinfo && MmIsAddressValid(psinfo)) 
					{
						while (psinfo->NextEntryOffset)
						{

							psinfo = (PSYSTEM_PROCESS_INFORMATION)(((PUCHAR)psinfo) + psinfo->NextEntryOffset);
							if (psinfo->ProcessId == ProcessId)
							{
								// VM Counters

								meminfo->PagefileUsage = psinfo->VMCounters.PagefileUsage;
								meminfo->PeakVirtualSize = psinfo->VMCounters.PeakVirtualSize;
								meminfo->PeakWorkingSetSize = psinfo->VMCounters.PeakWorkingSetSize;
								meminfo->QuotaNonPagedPoolUsage = psinfo->VMCounters.QuotaNonPagedPoolUsage;
								meminfo->QuotaPagedPoolUsage = psinfo->VMCounters.QuotaPagedPoolUsage;
								meminfo->QuotaPeakNonPagedPoolUsage = psinfo->VMCounters.QuotaPeakNonPagedPoolUsage;
								meminfo->QuotaPeakPagedPoolUsage = psinfo->VMCounters.QuotaPeakPagedPoolUsage;
								meminfo->VirtualSize = psinfo->VMCounters.VirtualSize;
								meminfo->WorkingSetSize = psinfo->VMCounters.WorkingSetSize;

								// IO Counters

								meminfo->ReadOperation = psinfo->IOCounters.ReadOperationCount;
								meminfo->WriteOperation = psinfo->IOCounters.WriteOperationCount;
								meminfo->ReadTransferOperation = psinfo->IOCounters.ReadTransferCount;
								meminfo->WriteTransferOperation = psinfo->IOCounters.WriteTransferCount;

							}
						}
					}
				}
				ExFreePoolWithTag(alloc, 'KxS');
			}
		}
	}
	return st;
}