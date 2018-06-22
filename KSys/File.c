#include "driver.h"

#include "File.h"


/*///////////////////////////////////////
/
/	File: File.c
/	Goal:
/		Hooking functionality
/		ZwReadFile
/		ZwWriteFile
/		ZwQueryInformationFile
/		
*///////////////////////////////////////

#ifndef _WIN64

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
	)
{
	NTSTATUS st;
	PFILE_OBJECT fobject = NULL;
	POBJECT_NAME_INFORMATION info = { 0 };


	ZwReadFileOriginal = (fnZwReadFile)DireccionServicioOriginal[NUMERO_FUNCION_SYSCALL(ZwReadFile)];

	st = ObReferenceObjectByHandle(FileHandle, FILE_READ_DATA, *IoFileObjectType,
		KernelMode, &fobject, NULL);

	if (NT_SUCCESS(st))
	{
		st = IoQueryFileDosDeviceName(fobject, &info);
		if (NT_SUCCESS(st))
		{
			
			if (wcsstr(info->Name.Buffer,(PCWSTR)UserHelperVariable) != NULL)
			{
	
				return STATUS_ACCESS_DENIED;
			}
		}
	}

	return ZwReadFileOriginal(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key);
}


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
	)
{

	PFILE_OBJECT file = NULL;
	POBJECT_NAME_INFORMATION objectname = { 0 };
	NTSTATUS st = 0;


	ZwWriteFileOriginal = (fnZwWriteFile)DireccionServicioOriginal[NUMERO_FUNCION_SYSCALL(ZwWriteFile)];

	st = ObReferenceObjectByHandle(FileHandle, FILE_WRITE_DATA, *IoFileObjectType,
		KernelMode, &file, NULL);

	if (NT_SUCCESS(st))
	{
		st = IoQueryFileDosDeviceName(file, &objectname);
		if (NT_SUCCESS(st))
		{
			if (wcsstr(objectname->Name.Buffer, (PCWSTR)UserHelperVariable) != NULL)
			{

				return STATUS_ACCESS_DENIED;
			}
		}
	}

	return ZwWriteFileOriginal(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key);

}


NTSTATUS HookZwQueryInformationFile(
	HANDLE                 FileHandle,
	PIO_STATUS_BLOCK       IoStatusBlock,
	PVOID                  FileInformation,
	ULONG                  Length,
	FILE_INFORMATION_CLASS FileInformationClass
	)
{
	PFILE_OBJECT obj = NULL;
	NTSTATUS st = 0;
	POBJECT_NAME_INFORMATION info = { 0 };

	ZwQueryInformationFileOriginal = (fnZwQueryInformationFile)DireccionServicioOriginal[NUMERO_FUNCION_SYSCALL(ZwQueryInformationFile)];

	st = ObReferenceObjectByHandle(FileHandle, FILE_READ_DATA | FILE_WRITE_DATA,
		*IoFileObjectType, KernelMode, &obj, NULL);
	if (NT_SUCCESS(st))
	{
		if (NT_SUCCESS(IoQueryFileDosDeviceName(obj, &info)))
		{
			if (wcsstr(info->Name.Buffer, (PCWSTR)UserHelperVariable) != NULL)
			{
		
				return STATUS_ACCESS_DENIED;
			
			}
		}
	}

	return ZwQueryInformationFileOriginal(FileHandle, IoStatusBlock, FileInformation, Length, FileInformationClass);
}
#endif _WIN64