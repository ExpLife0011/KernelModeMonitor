#include "driver.h"

#include "Dir.h"


/*////////////////////////////////////////////
/
/	File: Dir.c
/	Goal:
/		Hooking functionality for
/		ZwSetInformationFile
/		ZwCreateFile
/		ZwQuerySecurityObject
/		ZwSetSecurityObject
*/////////////////////////////////////////////

#ifndef _WIN64

NTSTATUS HookZwSetInformationFileDriver(
	HANDLE handle,
	PIO_STATUS_BLOCK io,
	void *fileinfo,
	unsigned long Length,
	FILE_INFORMATION_CLASS infoclass)
{
	POBJECT_NAME_INFORMATION ObjName=NULL;
	PFILE_OBJECT fileObj=NULL;
	long st;
	LARGE_INTEGER localtime;
	TIME_FIELDS timef = { 0 };

	ConvertirSysTimeToLocalTime(&localtime, &timef);

	ZwSetInformationFileOriginal = (fnZwSetInformationFile)DireccionServicioOriginal[NUMERO_FUNCION_SYSCALL(ZwSetInformationFile)];

	st = ObReferenceObjectByHandle(handle, FILE_READ_DATA, 0, KernelMode, &fileObj, 0);
	if (NT_SUCCESS(st))
	{
		// convert NT path format to Dos

		if (NT_SUCCESS(IoQueryFileDosDeviceName(fileObj, &ObjName)))
		{
			
			if (wcsstr(ObjName->Name.Buffer,(PCWSTR)UserHelperVariable) != NULL)
			{
				switch (infoclass)
				{
				case FileRenameInformation:
					return STATUS_ACCESS_DENIED;
					break;

				case FileDispositionInformation:
					return STATUS_ACCESS_DENIED;
					break;
				default:
					break;
				}
			
			}
	
		}
	}
	

	return ZwSetInformationFileOriginal(handle, io, fileinfo, Length, infoclass);

}



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
	IN ULONG EaLength)
{

	LARGE_INTEGER localtime;
	TIME_FIELDS timef = {0};
	NTSTATUS st = 0;
	WCHAR buffer[0xff];
	UNICODE_STRING ustr1;
	POBJECT_NAME_INFORMATION objname = NULL;
	PFILE_OBJECT fileobj = NULL;

	/*
		kd> uf nt!ZwCreateFile
			nt!ZwCreateFile:
			82878300 b842000000      mov     eax,42h
			82878305 8d542404        lea     edx,[esp+4]
			82878309 9c              pushfd
			8287830a 6a08            push    8
			8287830c e86d230000      call    nt!KiSystemService (8287a67e)
			82878311 c22c00          ret     2Ch
	*/

	ConvertirSysTimeToLocalTime(&localtime, &timef);

	ZwCreateFileOriginal = (fnZwCreateFile)DireccionServicioOriginal[NUMERO_FUNCION_SYSCALL(ZwCreateFile)];


	wcscpy_s(buffer, 0xFF, L"\\??\\");
	wcscat_s(buffer, 0xFF, UserHelperVariable);

	RtlInitUnicodeString(&ustr1, buffer);

	if (RtlCompareUnicodeString(ObjectAttributes->ObjectName, &ustr1, TRUE) == 0x00)
	{

		return STATUS_ACCESS_DENIED;
	}


	return ZwCreateFileOriginal(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);

}



NTSTATUS HookZwSetSecurityObject(
	HANDLE               Handle,
	SECURITY_INFORMATION SecurityInformation,
	PSECURITY_DESCRIPTOR SecurityDescriptor
	)
{
	PFILE_OBJECT file = NULL;
	POBJECT_NAME_INFORMATION info = { 0 };
	NTSTATUS st;
	TIME_FIELDS timef = { 0 };
	LARGE_INTEGER loctime;

	ConvertirSysTimeToLocalTime(&loctime, &timef);

	/*
		kd> uf nt!ZwQuerySecurityObject
			nt!ZwQuerySecurityObject:
			828791d8 b800010000      mov     eax,100h
			828791dd 8d542404        lea     edx,[esp+4]
			828791e1 9c              pushfd
			828791e2 6a08            push    8
			828791e4 e895140000      call    nt!KiSystemService (8287a67e)
			828791e9 c21400          ret     14h
	*/


	ZwSetSecurityObjectOriginal = (fnZwSetSecurityObject)DireccionServicioOriginal[NUMERO_FUNCION_SYSCALL(ZwSetSecurityObject)];

	st = ObReferenceObjectByHandle(Handle, FILE_GENERIC_READ | FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE, *IoFileObjectType,
		KernelMode, &file, NULL);
	if (NT_SUCCESS(st))
	{
		if (NT_SUCCESS(IoQueryFileDosDeviceName(file, &info)))
		{
			if (wcsstr(info->Name.Buffer,(PCWSTR)UserHelperVariable) != NULL)
			{

				return STATUS_ACCESS_DENIED;
			}
		}
	}

	return ZwSetSecurityObjectOriginal(Handle, SecurityInformation, SecurityDescriptor);
}



NTSTATUS HookZwQuerySecurityObject(
	HANDLE               Handle,
	SECURITY_INFORMATION SecurityInformation,
	PSECURITY_DESCRIPTOR SecurityDescriptor,
	ULONG  Length,
	PULONG LengthNeeded
	)
{
	PFILE_OBJECT file = NULL;
	POBJECT_NAME_INFORMATION info = { 0 };
	NTSTATUS st;
	LARGE_INTEGER loctime;
	TIME_FIELDS timef = { 0 };

	ConvertirSysTimeToLocalTime(&loctime, &timef);

	/*
		kd> uf nt!ZwSetSecurityObject
			nt!ZwSetSecurityObject:
			828798f4 b85b010000      mov     eax,15Bh
			828798f9 8d542404        lea     edx,[esp+4]
			828798fd 9c              pushfd
			828798fe 6a08            push    8
			82879900 e8790d0000      call    nt!KiSystemService (8287a67e)
			82879905 c20c00          ret     0Ch
	
	*/

	ZwQuerySecurityObjectOriginal = (fnZwSetSecurityObject)DireccionServicioOriginal[NUMERO_FUNCION_SYSCALL(ZwQuerySecurityObject)];

	st = ObReferenceObjectByHandle(Handle, 
		FILE_GENERIC_READ | 
		FILE_GENERIC_WRITE | 
		FILE_GENERIC_EXECUTE, 
		*IoFileObjectType,
		KernelMode, 
		&file, 
		NULL);
	if (NT_SUCCESS(st))
	{
		if (NT_SUCCESS(IoQueryFileDosDeviceName((PFILE_OBJECT)file, &info)))
		{
			if (wcsstr(info->Name.Buffer, (PCWSTR)UserHelperVariable) != NULL)
			{

				return STATUS_ACCESS_DENIED;
			}
		}
	}

	return ZwQuerySecurityObjectOriginal(Handle, SecurityInformation, SecurityDescriptor,Length,LengthNeeded);

}


void DesprotegerDirectorioAcls()
{
	Funcion_Unhookeada(ZwSetInformationFile);
	Funcion_Unhookeada(ZwCreateFile);
	Funcion_Unhookeada(ZwQuerySecurityObject);
	Funcion_Unhookeada(ZwSetSecurityObject);
	Funcion_Unhookeada(ZwReadFile);
	Funcion_Unhookeada(ZwWriteFile);
	Funcion_Unhookeada(ZwQueryInformationFile);
	Funcion_Unhookeada(ZwQueryDirectoryFile);
}

#endif _WIN64
