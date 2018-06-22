#pragma once


typedef struct _KERNEL_MEM_INFO
{
	ULONG		 PeakVirtualSize;
	ULONG		 VirtualSize;
	ULONG        PageFaultCount;
	ULONG        PeakWorkingSetSize;
	ULONG        WorkingSetSize;
	ULONG        QuotaPeakPagedPoolUsage;
	ULONG        QuotaPagedPoolUsage;
	ULONG        QuotaPeakNonPagedPoolUsage;
	ULONG        QuotaNonPagedPoolUsage;
	ULONG        PagefileUsage;
	ULONG        PeakPagefileUsage;
	ULONGLONG	 ReadOperation;
	ULONGLONG    ReadTransferOperation;
	ULONGLONG    WriteOperation;
	ULONGLONG    WriteTransferOperation;
}KERNEL_MEM_INFO, *PKERNEL_MEM_INFO;


NTSTATUS GetProcessMemoryInformationByProcessId(IN HANDLE ProcessId, OUT PVOID OutputBuffer, IN ULONG Outlengthsize);