#include "stdafx.h"

#include "global.h"
#include "kernel.h"


/*////////////////////////////////
/
/	File: kernel.cpp
/
/	Goal:
/		Open handle to created device
/		so we can exchange commands
/		to our kernel mode driver
/
*//////////////////////////////////		


HANDLE g_device = NULL;
IO_STATUS_BLOCK io = { 0 };

NTSTATUS KERNEL::AbrirCanalComunicacion(IN PWSTR NombreDispositivo)
{
	NTSTATUS st=STATUS_UNSUCCESSFUL;
	UNICODE_STRING ustr1;
	OBJECT_ATTRIBUTES ObjectAttributes;
	WCHAR error[30];

	RtlSecureZeroMemory(&ustr1, sizeof(ustr1));
	RtlInitUnicodeString(&ustr1, NombreDispositivo);

	InitializeObjectAttributes(&ObjectAttributes, &ustr1, OBJ_CASE_INSENSITIVE, NULL, NULL);

	st = NtOpenFile(&g_device, FILE_GENERIC_WRITE | FILE_GENERIC_READ,
		&ObjectAttributes,
		&io,
		FILE_SHARE_READ |
		FILE_SHARE_WRITE |
		FILE_SHARE_DELETE,
		NULL);

	switch (st)
	{
	case STATUS_OBJECT_NAME_NOT_FOUND:
		::MessageBox(NULL, _T("Device not found, make sure to load the driver first"), _T("ERROR"), MB_ICONERROR);
		break;

	case STATUS_SUCCESS:
		::MessageBox(NULL, _T("Handle successfully opened to device driver"), _T("KEMM"), MB_ICONINFORMATION);
		break;
	default:
		wsprintf(error, L"0x%x", st);
		::MessageBox(NULL, error, error, NULL);
		break;
	}

	return st;
}

NTSTATUS KERNEL::CerrarCanalComunicacion(VOID)
{
	NTSTATUS st = NtClose(g_device);
	switch (st)
	{
	case STATUS_SUCCESS:
		::MessageBox(NULL, _T("Handle closed successfully"), _T("KEMM"), MB_ICONINFORMATION);
		break;
	case STATUS_INVALID_HANDLE:
		::MessageBox(NULL, _T("Device not found, error"), _T("ERROR"), MB_ICONERROR);
		break;

	default:
		break;
	}
	
	return st;
}