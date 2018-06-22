#pragma once


NTKERNELAPI NTSTATUS ZwQueryDirectoryFile
(
HANDLE FileHandle,
HANDLE Event,
PIO_APC_ROUTINE ApcRoutine,
PVOID ApcContext,
PIO_STATUS_BLOCK IoStatusBlock,
PVOID FileInformation,
ULONG Length,
FILE_INFORMATION_CLASS FileInformationClass,
BOOLEAN ReturnSingleEntry,
PUNICODE_STRING FileName,
BOOLEAN RestartScan
);


#define MAX_PATH 260
#define MEM_TAG 'mEm'

typedef struct _FILE_INFORMATION_ENTRY
{
	WCHAR DirectoryName[MAX_PATH];
	WCHAR CreationTime[MAX_PATH];
	WCHAR WriteTime[MAX_PATH];
	WCHAR ChangeTime[MAX_PATH];
	LARGE_INTEGER AllocationSize;
	LARGE_INTEGER EndOfFile;
}FILE_INFORMATION_ENTRY, *PFILE_INFORMATION_ENTRY;

typedef struct _FILE_INFORMATION
{
	ULONG_PTR NumberOfEntries;
	FILE_INFORMATION_ENTRY Entry[1];

}FILE_INFORMATION, *PFILE_INFORMATION;

NTSTATUS QueryCompletion(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp,IN PVOID Context);

NTSTATUS NTFSLookupDirectoryByDirectoryId(IN PWCHAR pszDirectory, OUT PVOID OutputBuffer, IN ULONG Outlengthsize);