#pragma once

#pragma once

typedef NTKERNELAPI NTSTATUS(*fnZwSetInformationFile)(HANDLE file,
	PIO_STATUS_BLOCK io, PVOID fileinfo, ULONG length, FILE_INFORMATION_CLASS Infoclass);

typedef NTKERNELAPI NTSTATUS (*fnZwCreateFile)(
	OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PLARGE_INTEGER AllocationSize OPTIONAL,
	IN ULONG FileAttributes,
	IN ULONG ShareAccess,
	IN ULONG CreateDisposition,
	IN ULONG CreateOptions,
	IN PVOID EaBuffer OPTIONAL,
	IN ULONG EaLength
	);

typedef NTKERNELAPI NTSTATUS(*fnZwSetSecurityObject)(
	HANDLE               Handle,
	SECURITY_INFORMATION SecurityInformation,
	PSECURITY_DESCRIPTOR SecurityDescriptor
	);

typedef NTKERNELAPI NTSTATUS(*fnZwQuerySecurityObject)(
	HANDLE               Handle,
	SECURITY_INFORMATION SecurityInformation,
	PSECURITY_DESCRIPTOR SecurityDescriptor,
	ULONG                Length,
	PULONG               LengthNeeded
	);


fnZwSetInformationFile ZwSetInformationFileOriginal;
fnZwCreateFile ZwCreateFileOriginal;
fnZwSetSecurityObject ZwSetSecurityObjectOriginal;
fnZwQuerySecurityObject ZwQuerySecurityObjectOriginal;

NTSTATUS HookZwSetInformationFileDriver(
	HANDLE handle,
	PIO_STATUS_BLOCK io,
	PVOID fileinfo,
	ULONG Length,
	FILE_INFORMATION_CLASS infoclass);

NTSTATUS HookZwCreateFile(
	OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PLARGE_INTEGER AllocationSize OPTIONAL,
	IN ULONG FileAttributes,
	IN ULONG ShareAccess,
	IN ULONG CreateDisposition,
	IN ULONG CreateOptions,
	IN PVOID EaBuffer OPTIONAL,
	IN ULONG EaLength
	);

NTSTATUS HookZwSetSecurityObject(
	HANDLE               Handle,
	SECURITY_INFORMATION SecurityInformation,
	PSECURITY_DESCRIPTOR SecurityDescriptor
	);

NTSTATUS HookZwQuerySecurityObject(
	HANDLE               Handle,
	SECURITY_INFORMATION SecurityInformation,
	PSECURITY_DESCRIPTOR SecurityDescriptor,
	ULONG                Length,
	PULONG               LengthNeeded
	);



void DesprotegerDirectorioAcls();