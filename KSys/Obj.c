#include "driver.h"

#include "Obj.h"	



/*////////////////////////////////////////////
/
/	Funcion: HookZwOpenFile
/	Formato: \\Device\\[Nombre dispositivo]
/	Objetivo:
/		Denegar Acceso al dispositivo
/
*/////////////////////////////////////////////
#ifndef _WIN64
NTSTATUS HookZwOpenFile(
	OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN ULONG ShareAccess,
	IN ULONG OpenOptions
	)
{

	LARGE_INTEGER loctime;
	TIME_FIELDS timef = { 0 };
	UNICODE_STRING unicode;
	
	RtlInitUnicodeString(&unicode, UserHelperVariable);

	ConvertirSysTimeToLocalTime(&loctime, &timef);


	ZwOpenFileOriginal = (fnZwOpenFile)DireccionServicioOriginal[NUMERO_FUNCION_SYSCALL(ZwOpenFile)];


	if (RtlCompareUnicodeString(ObjectAttributes->ObjectName, &unicode, TRUE) == 0x00) {
		return STATUS_ACCESS_DENIED;
	}

	return ZwOpenFileOriginal(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions);
}

/*////////////////////////////////////////////
/
/	Funcion: HookZwOpenDirectoryObject
/	Formato: \\[Nombre de Objeto]
/	Objetivo:
/		Denegar Acceso al directorio de
/		Objetos
*/////////////////////////////////////////////

NTSTATUS HookZwOpenDirectoryObject(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes
	)
{

	LARGE_INTEGER loctime;
	TIME_FIELDS timef = { 0 };
	UNICODE_STRING ustr1;
	
	RtlInitUnicodeString(&ustr1, UserHelperVariable);

	ConvertirSysTimeToLocalTime(&loctime, &timef);

	ZwOpenDirectoryObjectOriginal = (fnZwOpenDirectoryObject)DireccionServicioOriginal[NUMERO_FUNCION_SYSCALL(ZwOpenDirectoryObject)];

	if (RtlCompareUnicodeString(ObjectAttributes->ObjectName, &ustr1, TRUE) == 0x00) {
		return STATUS_ACCESS_DENIED;
	}

	return ZwOpenDirectoryObjectOriginal(FileHandle, DesiredAccess, ObjectAttributes);
}


/*////////////////////////////////////////////
/
/	Funcion: HookZwOpenSymbolicLinkObject
/	Objetivo:
/		Denegar la apertura de cuatro links
/		simbolicos en WinObj
*/////////////////////////////////////////////

NTSTATUS HookZwOpenSymbolicLinkObject(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes
	)
{
	ZwOpenSymbolicLinkObjectOriginal = (fnZwOpenSymbolicLinkObject)DireccionServicioOriginal[NUMERO_FUNCION_SYSCALL(ZwOpenSymbolicLinkObject)];

	LARGE_INTEGER loctime;
	TIME_FIELDS timef = { 0 };
	UNICODE_STRING ustr1;
	
	RtlSecureZeroMemory(&ustr1, sizeof(ustr1));

	ConvertirSysTimeToLocalTime(&loctime, &timef);


	RtlInitUnicodeString(&ustr1, UserHelperVariable);
	if (RtlCompareUnicodeString(ObjectAttributes->ObjectName, &ustr1, TRUE) == 0x00)
	{

		return STATUS_OBJECT_NAME_NOT_FOUND;
	}
	
	return ZwOpenSymbolicLinkObjectOriginal(FileHandle, DesiredAccess, ObjectAttributes);

}

void DeshacerProteccionesObjetos()
{
	Funcion_Unhookeada(ZwOpenFile);
	Funcion_Unhookeada(ZwOpenDirectoryObject);
	Funcion_Unhookeada(ZwOpenSymbolicLinkObject);
}

#endif _WIN64


BOOLEAN GetKernelDriverObjectInformation(OUT PVOID OutputBuffer, IN ULONG Outlengthsize) 
{
	ULONG lengthsize = (Outlengthsize - sizeof(PKERNEL_OBJECT_INFORMATION)) / sizeof(PKERNEL_OBJECT_ENTRY);
	HANDLE Objhandle;
	UNICODE_STRING ustr1;
	OBJECT_ATTRIBUTES ObjectAttributes;
	BOOLEAN ret = TRUE;
	NTSTATUS st;
	SIZE_T size = 0x9C4;	
	ULONG context = 0, length = 0;
	POBJECT_DIRECTORY_INFORMATION info;
	UNICODE_STRING funaddress;
	OBJECT_ATTRIBUTES ObjectAttributesData;
	UNICODE_STRING obstring;
	PDRIVER_OBJECT pdriverobject = NULL;
	PKERNEL_OBJECT_INFORMATION kernelobject = (PKERNEL_OBJECT_INFORMATION)OutputBuffer;


	RtlSecureZeroMemory(&ustr1, sizeof(ustr1));
	RtlInitUnicodeString(&ustr1, L"\\Driver");

	RtlInitUnicodeString(&funaddress, L"ZwQueryDirectoryObject");
	ZwQueryDirectoryObject = MmGetSystemRoutineAddress(&funaddress);
	if (ZwQueryDirectoryObject == NULL)
		return FALSE;


	InitializeObjectAttributes(&ObjectAttributes, &ustr1, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	st = ZwOpenDirectoryObject(&Objhandle, DIRECTORY_QUERY, &ObjectAttributes);
	if (!NT_SUCCESS(st))
		return ret;
	do 
	{
		info = (POBJECT_DIRECTORY_INFORMATION)ExAllocatePool(NonPagedPool, size);
		if (info == NULL)
			break;

		while ((st = ZwQueryDirectoryObject(Objhandle, info, size, FALSE, ret, &context, &length)) == STATUS_MORE_ENTRIES) 
		{
			if (info[0].Name.Buffer)	// in case buffer is too small
				break;
			ExFreePool(info);		// free it
			size *= 2;	// allocate more memory
			info = (POBJECT_DIRECTORY_INFORMATION)ExAllocatePool(NonPagedPool, size);
		}

		SIZE_T i = 0;
		
		// loop through all the object namespace -> \\drivers\\

		for (i = 0; info->Name.Buffer[0] != NULL; ++i) 
		{

			ULONG conta = kernelobject->NumberOfEntries;

			POBJECT_DIRECTORY_INFORMATION buffer;
			buffer = &info[i];
			if (buffer->Name.Buffer == NULL)
				break;

			if (wcscmp(L"Driver", buffer->TypeName.Buffer) == 0) {
				
				RtlCopyMemory(kernelobject->Entry[conta].ObjectName, buffer->Name.Buffer, buffer->Name.Length);
				kernelobject->Entry[conta].ObjectName[buffer->Name.Length / sizeof(WCHAR)] = 0;

				DbgPrint("%ws", buffer->Name.Buffer);

				WCHAR wzBuffer[MAX_PATH];
				RtlStringCchCopyW(wzBuffer, MAX_PATH, L"\\Driver\\");
				RtlStringCchCatW(wzBuffer, MAX_PATH, buffer->Name.Buffer);

				RtlInitUnicodeString(&obstring, wzBuffer);
				st = ObReferenceObjectByName(&obstring, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, 0, *IoDriverObjectType, KernelMode, NULL, (PVOID*)&pdriverobject);
				if (NT_SUCCESS(st))
				{
					kernelobject->Entry[conta].DriverSection = pdriverobject->DriverSection;
					kernelobject->Entry[conta].DriverSize = pdriverobject->DriverSize;
					kernelobject->Entry[conta].DriverStart = pdriverobject->DriverStart;
					kernelobject->Entry[conta].DriverObject = (ULONG_PTR)pdriverobject;
				
					RtlCopyMemory(kernelobject->Entry[conta].DriverName, pdriverobject->DriverName.Buffer, pdriverobject->DriverName.Length);
				
					ObfDereferenceObject(pdriverobject);
				}

			}

			kernelobject->NumberOfEntries++;

		}

		if (st != STATUS_MORE_ENTRIES)
			break;

		ret = FALSE;


	} while (ret);


	if (Objhandle != NULL) {
		ZwClose(Objhandle);
	}

	if (info) {
		ExFreePool(info);
	}

	return ret;
}

