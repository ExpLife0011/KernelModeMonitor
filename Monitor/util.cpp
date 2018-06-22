#include "stdafx.h"

#include "global.h"
#include "util.h"
#include "resource.h"

WCHAR driver_extraccion[MAX_PATH] = { 0 };
WCHAR dir[MAX_PATH] = { 0 };


/*////////////////////////////////////////////////
/
/	File: Util.cpp
/
/	Goal:
/		Provide basic Utility and helper
/		Functions that are gonna be used
/		through the activity of the 
/		application
/		
/	Functionality:
/		- Buffer compression / decompression
/		- Working directory for extraction
/		- Resource extraction at runtime
/		- Driver loading
/		- Uninstall functionality
/	    - Extras
*////////////////////////////////////////////////

PBYTE API_COMPRESION::ComprimirBuffer(PBYTE DataIn, SIZE_T SizeIn, PULONG SizeOut)
{
	NTSTATUS st;
	PBYTE Data = NULL;
	PBYTE mem = NULL;
	DWORD dwsize = 0, dwret = 0;
	SIZE_T totalsize = 16 * SizeIn;


	st = RtlGetCompressionWorkSpaceSize(COMPRESSION_FORMAT_LZNT1 | COMPRESSION_ENGINE_MAXIMUM,
		&dwsize, &dwret);
	if (!NT_SUCCESS(st)) {
		return NULL;
	}


	Data = (PBYTE)RtlAllocateHeap(GetProcessHeap(), HEAP_ZERO_MEMORY, totalsize);
	mem = (PBYTE)RtlAllocateHeap(GetProcessHeap(), HEAP_ZERO_MEMORY, (SIZE_T)dwsize);

	if (Data == NULL || mem == NULL) {
		return NULL;
	}


	st = RtlCompressBuffer(COMPRESSION_FORMAT_LZNT1 | COMPRESSION_ENGINE_MAXIMUM,
		DataIn,
		SizeIn,
		Data,
		totalsize,
		0x1000,   
		SizeOut,
		mem);
	if (NT_SUCCESS(st)) {
		RtlFreeHeap(GetProcessHeap(), 0, mem);
	}

	return Data;
}


PBYTE API_COMPRESION::DescomprimirBuffer(PBYTE DataIn, SIZE_T SizeIn, PULONG SizeOut)
{
	NTSTATUS st;
	PBYTE Data = NULL;
	PBYTE mem = NULL;
	DWORD dwsize = 0, dwret = 0;
	SIZE_T totalsize = 16 * SizeIn;


	st = RtlGetCompressionWorkSpaceSize(COMPRESSION_FORMAT_LZNT1 | COMPRESSION_ENGINE_MAXIMUM,
		&dwsize, &dwret);
	if (!NT_SUCCESS(st)) {
		return NULL;
	}

	Data = (PBYTE)RtlAllocateHeap(GetProcessHeap(), HEAP_ZERO_MEMORY, totalsize);
	if (Data == NULL) {
		return NULL;
	}


	st = RtlDecompressBuffer(COMPRESSION_FORMAT_LZNT1 | COMPRESSION_ENGINE_MAXIMUM,
		Data,
		totalsize,
		DataIn,
		SizeIn,
		SizeOut);
	if (!NT_SUCCESS(st)) {
		return NULL;
	}

	return Data;
}


BOOL UTILS::MiCreateDirectory(PWSTR RutaDirectorio)
{
	UNICODE_STRING ustr1;
	OBJECT_ATTRIBUTES ObjectAttributes;
	LARGE_INTEGER large;
	NTSTATUS st;
	HANDLE handle=NULL;
	IO_STATUS_BLOCK io;
	BOOL ret;
	WCHAR error[40];

	RtlSecureZeroMemory(&ustr1, sizeof(ustr1));

	RtlInitUnicodeString(&ustr1, RutaDirectorio);

	if (RtlDosPathNameToNtPathName_U(RutaDirectorio, &ustr1, NULL, NULL) == FALSE)
		return FALSE;

	InitializeObjectAttributes(&ObjectAttributes, &ustr1, OBJ_CASE_INSENSITIVE, NULL, NULL);
	large.QuadPart = 2048;
	st = NtCreateFile(&handle,FILE_GENERIC_WRITE,&ObjectAttributes,&io,&large,FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN,FILE_SHARE_WRITE,FILE_CREATE,FILE_DIRECTORY_FILE |
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL, NULL);
	switch (st)
	{

	case STATUS_SUCCESS:
		::MessageBox(NULL, L"Extraction completed", L"KEMM", MB_ICONINFORMATION);
		ret = TRUE;
		break;

	case STATUS_ACCESS_DENIED:
		::MessageBox(NULL, L"Access denied", L"ERROR", MB_ICONERROR);
		ret = FALSE;
		break;

	case STATUS_OBJECT_NAME_COLLISION:
		::MessageBox(NULL, L"Existing folder", L"ERROR", MB_ICONERROR);
		ret = FALSE;
	default:
		break;
	}
	
	if (handle != NULL) {
		NtClose(handle);
	}

	return ret;
}



BOOL UTILS::MiWriteFile(PWSTR RutaArchivo, PBYTE Data, ULONG Datasize)
{
	UNICODE_STRING ustr1;
	OBJECT_ATTRIBUTES ObjectAttributes;
	LARGE_INTEGER large;
	NTSTATUS st;
	HANDLE handle=NULL;
	IO_STATUS_BLOCK io;
	BOOL ret = FALSE, g_cond = FALSE;
	
	do
	{
		RtlSecureZeroMemory(&ustr1, sizeof(ustr1));
		RtlInitUnicodeString(&ustr1, RutaArchivo);

		if (RtlDosPathNameToNtPathName_U(RutaArchivo, &ustr1, NULL, NULL) == FALSE)
			break;

		InitializeObjectAttributes(&ObjectAttributes, &ustr1, OBJ_CASE_INSENSITIVE, NULL, NULL);
		large.QuadPart = 2048;

		__try
		{
			st = NtCreateFile(&handle, FILE_GENERIC_WRITE,&ObjectAttributes,&io,&large,FILE_ATTRIBUTE_NORMAL,FILE_SHARE_WRITE,FILE_CREATE,FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
				NULL, NULL);
			if (NT_SUCCESS(st))
			{
				NtWriteFile(handle, NULL,NULL,NULL,&io,Data,Datasize,NULL,0);
				if (NT_SUCCESS(st)) {
					g_cond = TRUE;
					break;
				}
			}

		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return GetExceptionCode();
		}

	} while (ret);

	if (handle != NULL) {
		NtClose(handle);
	}

	return g_cond;
}



PBYTE UTILS::AccederRecurso(ULONG_PTR IdRecurso, PBYTE Data, PULONG Datasize)
{
	NTSTATUS st;
	PIMAGE_RESOURCE_DATA_ENTRY DataEntry;
	PBYTE Buffer = NULL;
	ULONG_PTR Array[3];
	DWORD Size = 0;

	if (Data == NULL)
		return NULL;

	Array[0] = (ULONG_PTR)RT_RCDATA;
	Array[1] = IdRecurso;
	Array[2] = 0;

	st = LdrFindResource_U(Data, (ULONG_PTR*)&Array, 3, &DataEntry);
	if (NT_SUCCESS(st))
	{
		st = LdrAccessResource(Data, DataEntry, (PVOID*)&Buffer, &Size);
		if (NT_SUCCESS(st)) {
			if (Datasize)	{
				*Datasize = Size;
			}
		}

	}

	return Buffer;
}



BOOL UTILS::DetectarArquitectura(PROCESSINFOCLASS processinfo)
{
	NTSTATUS st;
	PROCESS_EXTENDED_BASIC_INFORMATION extinfo;

	RtlSecureZeroMemory(&extinfo, sizeof(extinfo));
	extinfo.Size = sizeof(extinfo);

	st = ZwQueryInformationProcess(ZwCurrentProcess(),
		processinfo,
		&extinfo,
		sizeof(PROCESS_EXTENDED_BASIC_INFORMATION),
		0);
	if (NT_SUCCESS(st)) {
		return extinfo.IsWow64Process == TRUE;
	}

	return FALSE;
}



NTSTATUS UTILS::ObtenerPrivilegiosDriver(ULONG Privilegio)
{
	NTSTATUS st=STATUS_UNSUCCESSFUL;
	TOKEN_PRIVILEGES token;
	LUID lui;
	HANDLE handle=NULL;

	lui.LowPart = Privilegio;
	lui.HighPart = 0;

	token.PrivilegeCount = 1;
	token.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	token.Privileges[0].Luid = lui;

	st = NtOpenProcessToken(NtCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &handle);
	if (NT_SUCCESS(st)) {
		st = NtAdjustPrivilegesToken(handle, FALSE, &token, sizeof(token), NULL, 0);
		if (NT_SUCCESS(st)) {
			st = STATUS_SUCCESS;
		}
	}
	NtClose(handle);

	return st;

}

BOOLEAN UTILS::DescomprimirDriverEnFuncionWinVer(ULONG IdRecurso)
{
	HINSTANCE hInt = GetModuleHandle(NULL);
	PBYTE Recurso = NULL;
	PBYTE DataBuffer = NULL;
	WCHAR Ruta[MAX_PATH];
	BOOL ret = FALSE, g_cond = FALSE;
	DWORD size = 0, outsize = 0;

	do
	{

		Recurso = UTILS::AccederRecurso(IdRecurso, (PBYTE)hInt, &size);
		if (Recurso == NULL)
			break;

		DataBuffer = API_COMPRESION::DescomprimirBuffer(Recurso, (SIZE_T)size, &outsize);
		if (DataBuffer == NULL)
			break;
#ifndef _WIN64
		wcscpy_s(Ruta,sizeof(Ruta), L"C:\\Windows\\MONITOR\\KEMM.sys");
#else
		wcscpy_s(Ruta, sizeof(Ruta), L"C:\\Windows\\MONITOR\\KEMM64.sys");
#endif
		g_cond = UTILS::MiWriteFile(Ruta, DataBuffer, outsize);
		if (!g_cond)
			break;
#ifndef _WIN64
		::MessageBox(NULL, L"Driver decompressed", L"KEMM", MB_ICONINFORMATION);
#else
		::MessageBox(NULL, L"Driver decompressed", L"KEMM64", MB_ICONINFORMATION);
#endif
	} while (ret);


	return g_cond;

}



BOOL UTILS::ComprobarAdministrador(TOKEN_INFORMATION_CLASS Tokenclass)
{
	NTSTATUS st;
	BOOL ret = FALSE, g_cond = FALSE;
	PTOKEN_GROUPS token;
	HANDLE handle;
	PSID Admin = NULL;
	DWORD bytes = 0, i = 0, attrib = 0;
	SID_IDENTIFIER_AUTHORITY nt = SECURITY_NT_AUTHORITY;

	st = NtOpenProcessToken(NtCurrentProcess(), TOKEN_QUERY, &handle);
	if (!NT_SUCCESS(st))
		return ret;

	do
	{
		if (AllocateAndInitializeSid(
			&nt, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
			0, 0, 0, 0, 0, 0, (PSID*)&Admin) == FALSE)

			break;

		st = NtQueryInformationToken(handle, Tokenclass, NULL, 0, &bytes);
		if (st != STATUS_BUFFER_TOO_SMALL)
			break;

		token = (PTOKEN_GROUPS)RtlAllocateHeap(GetProcessHeap(), HEAP_ZERO_MEMORY, (SIZE_T)bytes);
		if (token == NULL)
			break;

		st = NtQueryInformationToken(handle, Tokenclass, token, bytes, &bytes);
		if (NT_SUCCESS(st))
		{
			if (token->GroupCount > 0)
				for (i = 0; i < token->GroupCount; ++i)
				{
					attrib = token->Groups[i].Attributes;
					if (RtlEqualSid(Admin, token->Groups[i].Sid) == TRUE)
					{
						if ((attrib & SE_GROUP_ENABLED) && (!(attrib & SE_GROUP_USE_FOR_DENY_ONLY)))
						{
							g_cond = TRUE;
							break;
						}
					}
				}
		}
		RtlFreeHeap(GetProcessHeap(), 0, token);

	} while (ret);
	
	if (Admin != NULL) {
		RtlFreeSid(Admin);
	}

	if (handle != NULL) {
		NtClose(handle);
	}

	return g_cond;
}



BOOL UTILS::CrearParametrosDriverServicio(PWSTR Nombredriver, PWSTR Nombreservicio)
{
	NTSTATUS st;
	WCHAR displayname[MAX_PATH];
	DWORD data, driversize, displaysize;
	BOOL ret = FALSE, g_cond = FALSE;
	HANDLE handle;
	WCHAR error[MAX_PATH];
	do
	{
		st = REGISTRO::CreateKey(&handle, KEY_ALL_ACCESS, Nombreservicio);
		if (!NT_SUCCESS(st))
			break;

		driversize = (DWORD)(1 + wcslen(Nombredriver))*sizeof(WCHAR);
		st = REGISTRO::SetValueKey(handle, L"ImagePath", REG_EXPAND_SZ, Nombredriver, driversize);
		if (!NT_SUCCESS(st))
			break;

		data = 1;
		st = REGISTRO::SetValueKey(handle, L"Type", REG_DWORD, &data, sizeof(DWORD));
		if (!NT_SUCCESS(st))
			break;


		data = 3;
		st = REGISTRO::SetValueKey(handle, L"Start", REG_DWORD, &data, sizeof(DWORD));
		if (!NT_SUCCESS(st))
			break;


		data = SERVICE_ERROR_IGNORE;
		st = REGISTRO::SetValueKey(handle, L"ErrorControl", REG_DWORD, &data, sizeof(DWORD));
		if (!NT_SUCCESS(st))
			break;
#ifndef _WIN64
		wcscpy_s(displayname, sizeof(displayname), L"KEMM");
#else
		wcscpy_s(displayname, sizeof(displayname), L"KEMM64");
#endif
		displaysize = (DWORD)(1 + wcslen(displayname))*sizeof(WCHAR);
		st = REGISTRO::SetValueKey(handle, L"DisplayName", REG_SZ, displayname, displaysize);
		if (!NT_SUCCESS(st))
			break;

		ZwFlushKey(handle);
		NtClose(handle);

		st = REGISTRO::LoadDriver(Nombreservicio);
		switch (st)
		{
		case STATUS_SUCCESS:
			::MessageBox(NULL, L"Driver successfully loaded", L"KEMM", MB_ICONINFORMATION);
			g_cond = TRUE;
			break;

		case STATUS_IMAGE_ALREADY_LOADED:
			REGISTRO::UnloadDriver(Nombreservicio);
			ZwYieldExecution();
			st = REGISTRO::LoadDriver(Nombreservicio);
			if (NT_SUCCESS(st))
			{
				::MessageBox(NULL, L"Detected loaded image, updating it...", L"KEMM", MB_ICONEXCLAMATION);
				g_cond = TRUE;
				break;
			}
			break;

		default:
			wsprintf(error, L"%x", st);
			::MessageBox(NULL, error, error, MB_ICONERROR);

			break;
		}


	} while (ret);

	if (handle != NULL) {
		NtClose(handle);
	}

	return g_cond;
}


BOOL UTILS::CargarDriverKernel(VOID)
{
	HANDLE keyhandle = NULL;
	WCHAR Buffer[MAX_PATH + 1];
	BOOL ret = FALSE, g_cond = FALSE;
	NTSTATUS st;
	RTL_OSVERSIONINFOW verinfo;

	RtlSecureZeroMemory(&verinfo, sizeof(verinfo));
	verinfo.dwOSVersionInfoSize = sizeof(verinfo);
	RtlGetVersion(&verinfo);

	do
	{

		st = UTILS::ObtenerPrivilegiosDriver(SE_LOAD_DRIVER_PRIVILEGE);
		if (!NT_SUCCESS(st))
			break;

		st = REGISTRO::OpenKey(&keyhandle, KEY_ALL_ACCESS, SERVICIOS_REGISTRO);
		if (!NT_SUCCESS(st))
			break;
#ifndef _WIN64
		wcscpy_s(Buffer,sizeof(Buffer), L"\\??\\C:\\Windows\\MONITOR\\KEMM.sys");
#else
		wcscpy_s(Buffer, sizeof(Buffer), L"\\??\\C:\\Windows\\MONITOR\\KEMM64.sys");
#endif
		g_cond = UTILS::CrearParametrosDriverServicio(Buffer, MONITOR_REGISTRO);
		if (!g_cond)
			break;


	} while (ret);

	if (keyhandle != NULL) {
		NtClose(keyhandle);
	}

	return g_cond;

}



NTSTATUS UTILS::EliminarDirectoriosyArchivosKEMM(VOID)
{
	OBJECT_ATTRIBUTES ObjectAttributes;
	UNICODE_STRING file;
	NTSTATUS st = STATUS_UNSUCCESSFUL;
	
	PWSTR files[] = {
		
		L"\\??\\C:\\Windows\\MONITOR\\Registro.log",
		L"\\??\\C:\\Windows\\MONITOR\\Objetos.log",
		L"\\??\\C:\\Windows\\MONITOR\\Procesos.log",
		L"\\??\\C:\\Windows\\MONITOR\\Imagen.log",
		L"\\??\\C:\\Windows\\MONITOR\\Directorio.log",
#ifndef _WIN64
		L"\\??\\C:\\Windows\\MONITOR\\KEMM.sys",
#else
		L"\\??\\C:\\Windows\\MONITOR\\KEMM64.sys",
#endif
		NULL
	};

	RtlSecureZeroMemory(&file, sizeof(file));

	for (unsigned long i = 0; files[i] != NULL; ++i)
	{
		RtlInitUnicodeString(&file, files[i]);
		InitializeObjectAttributes(&ObjectAttributes, &file, OBJ_CASE_INSENSITIVE, NULL, NULL);
		st = ZwDeleteFile(&ObjectAttributes);
		if (NT_SUCCESS(st))
		{
			if (i == 3)
				goto exit;
		}
	}

exit:
	RtlInitUnicodeString(&file, L"\\??\\C:\\Windows\\MONITOR");
	InitializeObjectAttributes(&ObjectAttributes, &file, OBJ_CASE_INSENSITIVE, NULL, NULL);
	st = ZwDeleteFile(&ObjectAttributes);
	if (NT_SUCCESS(st))
	{
		st = STATUS_SUCCESS;
	}

	return st;

}



BOOLEAN UTILS::EliminarEntradasRegistro(IN PWSTR Entrada1, IN PWSTR Entrada2)
{
	NTSTATUS st;
	HANDLE keyhandle1=NULL, keyhandle2=NULL;
	BOOL ret = FALSE, g_cond = FALSE;
	PWSTR Enum[] = { L"0", L"Count", L"NextInstance",NULL };
	PWSTR Enum2[] = { L"ImagePath", L"Type", L"Start", L"ErrorControl", L"DisplayName",NULL };
	DWORD i = 0, z = 0;
	RTL_OSVERSIONINFOW verinfo = { 0 };

	RtlSecureZeroMemory(&verinfo, sizeof(verinfo));
	verinfo.dwOSVersionInfoSize = sizeof(verinfo);
	RtlGetVersion(&verinfo);

	do
	{

		switch (verinfo.dwMinorVersion)
		{
		case 1:	// WIN 7

			st = REGISTRO::OpenKey(&keyhandle1, KEY_ALL_ACCESS, Entrada1);
			if (!NT_SUCCESS(st))
				break;

			for (i = 0; Enum[i] != NULL; ++i)
			{
				
				st = REGISTRO::DelValKey(keyhandle1, Enum[i]);
				if (NT_SUCCESS(st))
				{
					ZwDeleteKey(keyhandle1);
				}
			}

		break;

		}


		st = REGISTRO::OpenKey(&keyhandle2, KEY_ALL_ACCESS, Entrada2);
		if (!NT_SUCCESS(st))
			break;

		for (z = 0; Enum2[z] != NULL; ++z)
		{
	

			st = REGISTRO::DelValKey(keyhandle2, Enum2[z]);
			if (NT_SUCCESS(st))
			{
	

				st = ZwDeleteKey(keyhandle2);
				if (NT_SUCCESS(st))
				{
					g_cond = TRUE;
					break;
				}
			}
		}

	} while (ret);

	if (keyhandle1 != NULL) {
		NtClose(keyhandle1);
	}

	if (keyhandle2 != NULL) {
		NtClose(keyhandle2);
	}

	return g_cond;
}



NTSTATUS UTILS::__EnviarComandoDriver(HANDLE DeviceHandle, unsigned long _w64 CodigoIoctl)
{

	NTSTATUS st = 0;

	st = NtDeviceIoControlFile(DeviceHandle, NULL, NULL, NULL, &io, CodigoIoctl,
		NULL, 0, NULL, 0);
	switch (st)
	{
	case STATUS_ACCESS_DENIED:
		::MessageBox(NULL, L"Access denied", L"ERROR", MB_ICONERROR);
		break;

	case STATUS_INVALID_HANDLE:
		::MessageBox(NULL, L"Error sending command", L"ERROR", MB_ICONERROR);
		break;

	case STATUS_SUCCESS:
		::MessageBox(NULL, L"Command sent", L"KEMM", MB_ICONINFORMATION);
		break;

	default:
		break;

	}

	return st;

}
