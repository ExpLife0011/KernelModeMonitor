#pragma once

NTSTATUS HookZwOpenFile(
	OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN ULONG ShareAccess,
	IN ULONG OpenOptions
	);

NTSTATUS HookZwOpenDirectoryObject(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes
	);

NTSTATUS HookZwOpenSymbolicLinkObject(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes
	);

typedef NTKERNELAPI NTSTATUS(*fnZwOpenDirectoryObject)(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes
	);


typedef NTKERNELAPI NTSTATUS(*fnZwOpenSymbolicLinkObject)(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes
	);

typedef NTKERNELAPI NTSTATUS(*fnZwOpenFile)(
	OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN ULONG ShareAccess,
	IN ULONG OpenOptions
	);


fnZwOpenDirectoryObject ZwOpenDirectoryObjectOriginal;
fnZwOpenSymbolicLinkObject ZwOpenSymbolicLinkObjectOriginal;
fnZwOpenFile ZwOpenFileOriginal;

void DeshacerProteccionesObjetos();


typedef NTKERNELAPI NTSTATUS (*fnZwQueryDirectoryObject)(
	_In_      HANDLE  DirectoryHandle,
	_Out_opt_ PVOID   Buffer,
	_In_      ULONG   Length,
	_In_      BOOLEAN ReturnSingleEntry,
	_In_      BOOLEAN RestartScan,
	_Inout_   PULONG  Context,
	_Out_opt_ PULONG  ReturnLength
	);


extern PVOID *IoDriverObjectType;
NTSTATUS __stdcall
ObReferenceObjectByName(
__in PUNICODE_STRING ObjectName,
__in ULONG Attributes,
__in_opt PACCESS_STATE AccessState,
__in_opt ACCESS_MASK DesiredAccess,
__in POBJECT_TYPE ObjectType,
__in KPROCESSOR_MODE AccessMode,
__inout_opt PVOID ParseContext,
__out PVOID *Object
);

fnZwQueryDirectoryObject ZwQueryDirectoryObject;

typedef struct _OBJECT_DIRECTORY_INFORMATION {
	UNICODE_STRING Name;
	UNICODE_STRING TypeName;
} OBJECT_DIRECTORY_INFORMATION, *POBJECT_DIRECTORY_INFORMATION;

typedef struct _KERNEL_OBJECT_ENTRY
{
	WCHAR ObjectName[MAX_PATH];
	ULONG DriverSize;
	PVOID DriverSection;
	WCHAR DriverName[MAX_PATH];
	PVOID DriverStart;
	ULONG_PTR DriverObject;
}KERNEL_OBJECT_ENTRY, *PKERNEL_OBJECT_ENTRY;

typedef struct _KERNEL_OBJECT_INFORMATION
{
	ULONG_PTR NumberOfEntries;
	KERNEL_OBJECT_ENTRY Entry[1];
}KERNEL_OBJECT_INFORMATION, *PKERNEL_OBJECT_INFORMATION;

typedef struct _DIRECTORY_BASIC_INFORMATION
{
	UNICODE_STRING ObjectName;
	UNICODE_STRING ObjectTypeName;
} DIRECTORY_BASIC_INFORMATION, *PDIRECTORY_BASIC_INFORMATION;



BOOLEAN GetKernelDriverObjectInformation(OUT PVOID OutputBuffer, IN ULONG Outlengthsize);

