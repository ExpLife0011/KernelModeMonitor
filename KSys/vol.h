#pragma once

typedef struct _VOL_INFO
{
	WCHAR SymbolicName[MAX_PATH];
	ULONG BytesPerSector;
	LARGE_INTEGER ActualAvailableAllocationUnits;
	LARGE_INTEGER TotalAllocationUnits;
	LARGE_INTEGER CallerAvailableAllocationUnits;
	ULONG SectorsPerAllocationUnit;
	WCHAR		  VolumeCreationTime[MAX_PATH];
	ULONG         VolumeSerialNumber;
	BOOLEAN       SupportsObjects;
}VOL_INFO, *PVOL_INFO;



NTSTATUS IrpQueryVolumeInformation(IN HANDLE handle, IN FS_INFORMATION_CLASS FileInformationClass, OUT PVOID Buffer, IN ULONG Length, OUT PULONG ReturnedLength);
NTSTATUS NTFSLookupVolumeByVolumeId(IN PWCHAR Volume, OUT PVOID OutputBuffer, IN ULONG Outlength);