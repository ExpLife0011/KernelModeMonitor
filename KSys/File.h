#pragma once

typedef NTKERNELAPI NTSTATUS(*fnZwReadFile)(
	HANDLE           FileHandle,
	HANDLE           Event,
	PIO_APC_ROUTINE  ApcRoutine,
	PVOID            ApcContext,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID            Buffer,
	ULONG            Length,
	PLARGE_INTEGER   ByteOffset,
	PULONG           Key
	);

typedef NTKERNELAPI NTSTATUS(*fnZwWriteFile)(
	HANDLE           FileHandle,
	HANDLE           Event,
	PIO_APC_ROUTINE  ApcRoutine,
	PVOID            ApcContext,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID            Buffer,
	ULONG            Length,
	PLARGE_INTEGER   ByteOffset,
	PULONG           Key
	);

typedef NTKERNELAPI NTSTATUS (*fnZwQueryInformationFile)(
	HANDLE                 FileHandle,
	PIO_STATUS_BLOCK       IoStatusBlock,
	PVOID                  FileInformation,
	ULONG                  Length,
	FILE_INFORMATION_CLASS FileInformationClass
	);
typedef NTKERNELAPI  NTSTATUS (*fnZwQueryVolumeInformationFile)(
	HANDLE               FileHandle,
	PIO_STATUS_BLOCK     IoStatusBlock,
	PVOID                FsInformation,
	ULONG                Length,
	FS_INFORMATION_CLASS FsInformationClass
	);


typedef NTKERNELAPI NTSTATUS(*fnZwQueryDirectoryFile)(
	HANDLE hFile,
	HANDLE hEvent,
	PIO_APC_ROUTINE IoApcRoutine,
	PVOID IoApcContext,
	PIO_STATUS_BLOCK pIoStatusBlock,
	PVOID FileInformationBuffer,
	ULONG FileInformationBufferLength,
	FILE_INFORMATION_CLASS FileInfoClass,
	BOOLEAN bReturnOnlyOneEntry,
	PUNICODE_STRING PathMask,
	BOOLEAN bRestartQuery
	);




fnZwReadFile ZwReadFileOriginal;
fnZwWriteFile ZwWriteFileOriginal;
fnZwQueryInformationFile ZwQueryInformationFileOriginal;
fnZwQueryVolumeInformationFile ZwQueryVolumeInformationFileOriginal;
fnZwQueryDirectoryFile ZwQueryDirectoryFileOriginal;

NTSTATUS HookZwReadFile(
	HANDLE           FileHandle,
	HANDLE           Event,
	PIO_APC_ROUTINE  ApcRoutine,
	PVOID            ApcContext,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID            Buffer,
	ULONG            Length,
	PLARGE_INTEGER   ByteOffset,
	PULONG           Key
	);

NTSTATUS HookZwWriteFile(
	HANDLE           FileHandle,
	HANDLE           Event,
	PIO_APC_ROUTINE  ApcRoutine,
	PVOID            ApcContext,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID            Buffer,
	ULONG            Length,
	PLARGE_INTEGER   ByteOffset,
	PULONG           Key
	);

NTSTATUS HookZwQueryInformationFile(
	HANDLE                 FileHandle,
	PIO_STATUS_BLOCK       IoStatusBlock,
	PVOID                  FileInformation,
	ULONG                  Length,
	FILE_INFORMATION_CLASS FileInformationClass
	);
