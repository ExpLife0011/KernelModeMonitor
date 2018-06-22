#include "driver.h"

#include "Reg.h"


#ifndef _WIN64

NTSTATUS HookZwDeleteKey(HANDLE keyhandle)
{
	PKEY_NAME_INFORMATION info;
	CHAR buffer[1024];
	ULONG bytes = 0;
	LARGE_INTEGER loctime;
	TIME_FIELDS timef = { 0 };

	ConvertirSysTimeToLocalTime(&loctime, &timef);


	info = (PKEY_NAME_INFORMATION)&buffer;

	ZwDeleteKeyOriginal = (fnZwDeleteKey)DireccionServicioOriginal[NUMERO_FUNCION_SYSCALL(ZwDeleteKey)];

	if (NT_SUCCESS(ZwQueryKey(keyhandle, KeyNameInformation, buffer, 1024, &bytes)))
	{
		if (wcsstr(info->Name,UserHelperVariable) != NULL)
		{
			
			return STATUS_ACCESS_DENIED;
			
		}
	}


	return ZwDeleteKeyOriginal(keyhandle);

}

NTSTATUS HookZwDeleteValueKey(HANDLE keyhandle, PUNICODE_STRING Valuename)
{
	PKEY_NAME_INFORMATION info;
	CHAR buffer[1024];
	ULONG bytes = 0;
	LARGE_INTEGER loctime;
	TIME_FIELDS timef = { 0 };

	ConvertirSysTimeToLocalTime(&loctime, &timef);


	info = (PKEY_NAME_INFORMATION)&buffer;

	ZwDeleteValueKeyOriginal = (fnZwDeleteValueKey)DireccionServicioOriginal[NUMERO_FUNCION_SYSCALL(ZwDeleteValueKey)];

	if (NT_SUCCESS(ZwQueryKey(keyhandle, KeyNameInformation, buffer, 1024, &bytes)))
	{
		if (wcsstr(info->Name, UserHelperVariable) != NULL)
		{
			
			return STATUS_ACCESS_DENIED;
		}
	}

	return ZwDeleteValueKeyOriginal(keyhandle, Valuename);
}

NTSTATUS HookZwSetValueKey(HANDLE keyhandle,
	PUNICODE_STRING Valuename,
	ULONG Title,
	ULONG Type,
	PVOID Data,
	ULONG Datasize)
{
	PKEY_NAME_INFORMATION info;
	CHAR buffer[1024];
	ULONG bytes = 0;
	LARGE_INTEGER loctime;
	TIME_FIELDS timef = { 0 };

	ConvertirSysTimeToLocalTime(&loctime, &timef);


	info = (PKEY_NAME_INFORMATION)&buffer;

	ZwSetValueKeyOriginal = (fnZwSetValueKey)DireccionServicioOriginal[NUMERO_FUNCION_SYSCALL(ZwSetValueKey)];

	if (NT_SUCCESS(ZwQueryKey(keyhandle, KeyNameInformation, buffer, 1024, &bytes)))
	{
		if (wcsstr(info->Name,UserHelperVariable) != NULL)
		{
			
			return STATUS_ACCESS_DENIED;
		}
	}

	return ZwSetValueKeyOriginal(keyhandle, Valuename, Title, Type, Data, Datasize);

}


NTSTATUS HookZwQueryValueKey(HANDLE keyhandle,
	PUNICODE_STRING Valuename,
	KEY_VALUE_INFORMATION_CLASS keyclass,
	PVOID keyinfo,
	ULONG keylength,
	PULONG bytesreturned)
{
	PKEY_NAME_INFORMATION info;
	CHAR Buffer[1024];
	ULONG bytes = 0;
	LARGE_INTEGER loctime;
	TIME_FIELDS timef = { 0 };


	ConvertirSysTimeToLocalTime(&loctime, &timef);


	info = (PKEY_NAME_INFORMATION)&Buffer;


	ZwQueryValueKeyOriginal = (fnZwQueryValueKey)DireccionServicioOriginal[NUMERO_FUNCION_SYSCALL(ZwQueryValueKey)];

	if (NT_SUCCESS(ZwQueryKey(keyhandle, KeyNameInformation, Buffer, 1024, &bytes)))
	{
		
		if (wcsstr(info->Name,UserHelperVariable) != NULL)
		{
			
			return STATUS_ACCESS_DENIED;
		}
		
	}

	return ZwQueryValueKeyOriginal(keyhandle, Valuename, keyclass, keyinfo, keylength, bytesreturned);
}


NTSTATUS HookZwEnumerateKey(HANDLE handle,
	ULONG index,
	KEY_INFORMATION_CLASS KeyInfoClass,
	PVOID KeyInformation,
	ULONG Length,
	PULONG ResultLength)
{
	NTSTATUS st = 0;
	PCWSTR KeyBufferName = NULL;

	
	ZwEnumerateKeyOriginal = (fnZwEnumerateKey)DireccionServicioOriginal[NUMERO_FUNCION_SYSCALL(ZwEnumerateKey)];

	st = ((fnZwEnumerateKey)(ZwEnumerateKeyOriginal))
		(
			handle,
			index,
			KeyInfoClass,
			KeyInformation,
			Length,
			ResultLength
		);

	if (!NT_SUCCESS(st))
	{
		return st;
	}

	switch (KeyInfoClass)
	{
	case KeyBasicInformation:
		KeyBufferName = ((PKEY_BASIC_INFORMATION)KeyInformation)->Name;
		break;
	case KeyNameInformation:
		KeyBufferName= ((PKEY_NAME_INFORMATION)KeyInformation)->Name;
		break;
	case KeyNodeInformation:
		KeyBufferName = ((PKEY_NODE_INFORMATION)KeyInformation)->Name;
		break;
		
	}

	if ((KeyBufferName != NULL) 
		&& (wcsstr(KeyBufferName,UserHelperVariable) != NULL))
	{
		index++;

		st = ((fnZwEnumerateKey)(ZwEnumerateKeyOriginal))
			(
			handle,
			index,
			KeyInfoClass,
			KeyInformation,
			Length,
			ResultLength
			);


	}

	return st;
}


void DesProtegerRegistro()
{
	Funcion_Unhookeada(ZwDeleteKey);
	Funcion_Unhookeada(ZwDeleteValueKey);
	Funcion_Unhookeada(ZwSetValueKey);
	Funcion_Unhookeada(ZwQueryValueKey);
	Funcion_Unhookeada(ZwEnumerateKey);

}
#endif _WIN64

/*//////////////////////////////////////////////
/
/	Function: GetKernelRegInformation
/
/	Purpose: Enumerate all registry keys inside
/    Services, allocate it in structure to user space
/
/	Environment:
/		kernel mode only
/
*//////////////////////////////////////////////

NTSTATUS GetKernelRegInformation(IN PWCHAR KeyBuffer, OUT PVOID OutputBuffer, IN ULONG Outlength) 
{
	NTSTATUS st;
	OBJECT_ATTRIBUTES ObjectAttrib;
	UNICODE_STRING ustr1;
	HANDLE handle;
	ULONG index = 0;
	ULONG bytes = 0;
	ULONG Size = (Outlength - sizeof(REG_KERNEL_INFORMATION)) / sizeof(REG_KERNEL_ENTRY);
	PKEY_BASIC_INFORMATION key = NULL;
	WCHAR wzRegPrefix[MAX_PATH] = L"\\Registry\\Machine\\";
	WCHAR timer[MAX_PATH];
	TIME_FIELDS timetime = { 0 };


	RtlStringCchCatW(wzRegPrefix, MAX_PATH, KeyBuffer);
	RtlInitUnicodeStringEx(&ustr1, wzRegPrefix);
	InitializeObjectAttributes(&ObjectAttrib, &ustr1, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	st = ZwOpenKey(&handle, KEY_ALL_ACCESS, &ObjectAttrib);
	if (NT_SUCCESS(st)) 
	{
		while (st != STATUS_NO_MORE_ENTRIES) 
		{

			ULONG contador = ((PREG_KERNEL_INFORMATION)OutputBuffer)->NumberOfEntries;

			st = ZwEnumerateKey(handle, index, KeyBasicInformation, NULL, 0, &bytes);
			if (st == STATUS_BUFFER_TOO_SMALL) 
			{
				key = (PKEY_BASIC_INFORMATION)ExAllocatePoolWithTag(NonPagedPool, bytes * 0x1000, 'rEg');
				if (key != NULL)
				{
					st = ZwEnumerateKey(handle, index, KeyBasicInformation, key, bytes, &bytes);
					if (NT_SUCCESS(st)) 
					{
						RtlCopyMemory(((PREG_KERNEL_INFORMATION)OutputBuffer)->Entry[contador].KeyName, key->Name, key->NameLength);

						RtlTimeToTimeFields((PLARGE_INTEGER)&key->LastWriteTime, &timetime);
						RtlStringCchPrintfW(timer, MAX_PATH, L"%02u/%02u/%04u %02u:%02u:%02u:%03u", timetime.Day, timetime.Month,
							timetime.Year, timetime.Hour,
							timetime.Minute, timetime.Second,
							timetime.Milliseconds);

						RtlStringCchCopyW(((PREG_KERNEL_INFORMATION)OutputBuffer)->Entry[contador].CreateTime, MAX_PATH, timer);

					}
				}
				ExFreePoolWithTag(key, 'rEg');
			}
			((PREG_KERNEL_INFORMATION)OutputBuffer)->NumberOfEntries++;

			index++;
		}
	}
	if (handle != NULL) {
		ZwClose(handle);
	}

	return st;
}

/*//////////////////////////////////////////////
/
/	Function: GetKernelValueEnumeration
/
/	Purpose: Gather reg value information
/	and allocate it in structure to user space
/
/	Environment:
/		kernel mode only
/
*//////////////////////////////////////////////

NTSTATUS GetKernelValueEnumeration(IN PWCHAR KeyBuffer, OUT PVOID OutputBuffer, IN ULONG Outlength) 
{
	HANDLE handle;
	UNICODE_STRING ustr1;
	OBJECT_ATTRIBUTES oa;
	NTSTATUS st;
	ULONG bytes = 0;
	WCHAR wzRegPrefix[MAX_PATH] = L"\\Registry\\Machine\\";
	ULONG i = 0;
	ULONG size = (Outlength - sizeof(PKERNEL_REG_VAL_INFORMATION)) / sizeof(PKERNEL_REG_VAL_ENTRY);
	PKEY_VALUE_FULL_INFORMATION full = NULL;
	PKEY_VALUE_PARTIAL_INFORMATION partial = NULL;
	UNICODE_STRING buffer;
	PKERNEL_REG_VAL_INFORMATION info = (PKERNEL_REG_VAL_INFORMATION)OutputBuffer;


	RtlStringCchCatW(wzRegPrefix, MAX_PATH, KeyBuffer);
	RtlInitUnicodeString(&ustr1, wzRegPrefix);
	InitializeObjectAttributes(&oa, &ustr1, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	st = ZwOpenKey(&handle, KEY_ALL_ACCESS, &oa);
	if (NT_SUCCESS(st))
	{
		while (st != STATUS_NO_MORE_ENTRIES)
		{
			ULONG contador = info->NumberOfEntries;

			__try 
			{

				st = ZwEnumerateValueKey(handle, i, KeyValueFullInformation, NULL, 0, &bytes);
				if (st == STATUS_BUFFER_TOO_SMALL)
				{
					full = (PKEY_VALUE_FULL_INFORMATION)ExAllocatePool(NonPagedPool, bytes);
					if (full != NULL) 
					{
						st = ZwEnumerateValueKey(handle, i, KeyValueFullInformation, full, bytes, &bytes);
						if (NT_SUCCESS(st)) 
						{
							PWCHAR alloc = (WCHAR*)ExAllocatePool(NonPagedPool, full->NameLength + sizeof(WCHAR));
							if (alloc != NULL) 
							{
								RtlSecureZeroMemory(alloc, full->NameLength + sizeof(WCHAR));
								memcpy(alloc, full->Name, full->NameLength);

								ULONG bb = 0;
								RtlInitUnicodeString(&buffer, alloc);
								st = ZwQueryValueKey(handle, &buffer, KeyValuePartialInformation, NULL, 0, &bb);
								if (st == STATUS_BUFFER_TOO_SMALL) 
								{
									partial = (PKEY_VALUE_PARTIAL_INFORMATION)ExAllocatePool(NonPagedPool, bb);
									if (partial != NULL)
									{
										st = ZwQueryValueKey(handle, &buffer, KeyValuePartialInformation, partial, bb, &bb);
										if (NT_SUCCESS(st)) 
										{

											if (partial->Type == REG_DWORD) 
											{
												RtlCopyMemory(info->Entry[contador].Name, alloc, full->NameLength);
												RtlCopyMemory(&info->Entry[contador].udatalong, &partial->Data[0], sizeof(ULONG));
											}
											else if (partial->Type == REG_SZ ||
												partial->Type == REG_MULTI_SZ ||
												partial->Type == REG_EXPAND_SZ) 
											{
												PWCHAR mem = (PWCHAR)ExAllocatePool(NonPagedPool, bb);
												if (mem != NULL) 
												{
													RtlSecureZeroMemory(mem, partial->DataLength + sizeof(WCHAR));
													memcpy(mem, partial->Data, partial->DataLength);
													RtlCopyMemory(info->Entry[contador].Name, alloc, full->NameLength);
													RtlCopyMemory(info->Entry[contador].DataName, mem, partial->DataLength);
													ExFreePool(mem);
												}

											}
											info->Entry[contador].TypeOfKey = partial->Type;

										}
										ExFreePool(partial);
									}
								}
								ExFreePool(alloc);
							}
						}
						ExFreePool(full);
					}
				}

			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				return GetExceptionCode();
			}

			info->NumberOfEntries++;
			
			i++;
		}
	}
	if (handle != NULL) {
		ZwClose(handle);
	}

	return st;
}

