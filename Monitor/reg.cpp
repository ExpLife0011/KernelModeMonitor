#include "stdafx.h"

#include "global.h"
#include "reg.h"


/*///////////////////////////////////
/	Function: OpenKey
/
/	Goal: Wrapper of ZwOpenKey
/
///////////////////////////////////*/

NTSTATUS REGISTRO::OpenKey(PHANDLE keyhandle, ACCESS_MASK Mask, PWSTR Valuename)
{
	UNICODE_STRING ustr1;
	OBJECT_ATTRIBUTES ObjectAttributes;

	RtlSecureZeroMemory(&ustr1, sizeof(ustr1));
	RtlInitUnicodeString(&ustr1, Valuename);

	InitializeObjectAttributes(&ObjectAttributes, &ustr1, OBJ_CASE_INSENSITIVE, NULL, NULL);

	return ZwOpenKey(keyhandle, Mask, &ObjectAttributes);
}

/*///////////////////////////////////
/	Function: CreateKey
/
/	Goal: Wrapper of ZwCreateKey
/
///////////////////////////////////*/

NTSTATUS REGISTRO::CreateKey(PHANDLE keyhandle, ACCESS_MASK Mask, PWSTR Valuename)
{
	UNICODE_STRING ustr1;
	OBJECT_ATTRIBUTES ObjectAttributes;

	RtlSecureZeroMemory(&ustr1, sizeof(ustr1));
	RtlInitUnicodeString(&ustr1, Valuename);

	InitializeObjectAttributes(&ObjectAttributes, &ustr1, OBJ_CASE_INSENSITIVE, NULL, NULL);

	return ZwCreateKey(keyhandle, Mask, &ObjectAttributes, NULL, 0, NULL, 0);

}

/*///////////////////////////////////
/	Function: SetValueKey
/
/	Goal: Wrapper of ZwSetValueKey
/
///////////////////////////////////*/

NTSTATUS REGISTRO::SetValueKey(HANDLE keyhandle, PWSTR Valuename, DWORD Type, PVOID Data, DWORD Datasize)
{
	UNICODE_STRING ustr1;
	RtlSecureZeroMemory(&ustr1, sizeof(ustr1));

	RtlInitUnicodeString(&ustr1, Valuename);

	return ZwSetValueKey(keyhandle, &ustr1, NULL, Type, Data, Datasize);

}

/*///////////////////////////////////
/	Function: LoadDriver
/
/	Goal: Wrapper of ZwLoadDriver
/
///////////////////////////////////*/

NTSTATUS REGISTRO::LoadDriver(PWSTR Servicio)
{
	UNICODE_STRING ustr1;
	RtlInitUnicodeString(&ustr1, Servicio);

	return ZwLoadDriver(&ustr1);
}

/*///////////////////////////////////
/	Function: UnloadDriver
/
/	Goal: Wrapper of ZwUnloadDriver
/
///////////////////////////////////*/

NTSTATUS REGISTRO::UnloadDriver(PWSTR Servicio)
{
	UNICODE_STRING ustr1;
	RtlInitUnicodeString(&ustr1, Servicio);

	return ZwUnloadDriver(&ustr1);
}
/*///////////////////////////////////
/	Function: DelValKey
/
/	Goal: Wrapper of ZwDeleteValueKey
/
///////////////////////////////////*/

NTSTATUS REGISTRO::DelValKey(HANDLE Keyhandle, PWSTR Valuename)
{
	UNICODE_STRING ustr1;
	RtlInitUnicodeString(&ustr1, Valuename);

	return ZwDeleteValueKey(Keyhandle, &ustr1);
}