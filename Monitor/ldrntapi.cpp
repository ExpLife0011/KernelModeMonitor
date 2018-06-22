#include "stdafx.h"

#include "global.h"
#include "ldrntapi.h"


/*////////////////////////////////
/
/	File: ldrntapi.cpp
/
/	Goal:
/		Load NTDLL.DLL and its
/		functions dynamically
/
*//////////////////////////////////		



fnZwAllocateVirtualMemory ZwAllocateVirtualMemory = 0;
fnZwFreeVirtualMemory ZwFreeVirtualMemory = 0;
fnZwQuerySystemInformation ZwQuerySystemInformation = 0;
fnRtlUnicodeStringToAnsiString RtlUnicodeStringToAnsiString = 0;
fnRtlInitUnicodeString RtlInitUnicodeString = 0;
fnRtlInitString RtlInitString = 0;

fnRtlCompressBuffer RtlCompressBuffer = 0;
fnRtlDecompressBuffer RtlDecompressBuffer=0;
fnRtlGetCompressionWorkSpaceSize RtlGetCompressionWorkSpaceSize=0;
fnRtlTimeToTimeFields RtlTimeToTimeFields=0;
fnRtlExpandEnvironmentStrings_U RtlExpandEnvironmentStrings_U=0;
fnLdrAccessResource LdrAccessResource=0;
fnLdrFindResource_U LdrFindResource_U=0;
fnLdrLoadDll LdrLoadDll=0;
fnLdrGetDllHandle LdrGetDllHandle=0;
fnNtTerminateProcess NtTerminateProcess=0;
fnRtlAllocateHeap RtlAllocateHeap = 0;
fnRtlFreeHeap RtlFreeHeap=0;
fnRtlDosPathNameToNtPathName_U RtlDosPathNameToNtPathName_U=0;
fnNtCreateFile NtCreateFile=0;
fnNtWriteFile NtWriteFile=0;
fnNtOpenFile NtOpenFile = 0;
fnNtQueryInformationFile NtQueryInformationFile=0;
fnNtClose NtClose = 0;
fnRtlRandomEx RtlRandomEx=0;
fnRtlGetVersion RtlGetVersion=0;
fnNtOpenProcessToken NtOpenProcessToken=0;
fnNtAdjustPrivilegesToken NtAdjustPrivilegesToken=0;
fnZwQueryInformationProcess ZwQueryInformationProcess=0;
fnNtQueryInformationToken NtQueryInformationToken=0;
fnRtlEqualSid RtlEqualSid=0;
fnRtlFreeSid RtlFreeSid=0;

fnZwOpenKey ZwOpenKey=0;
fnZwCreateKey ZwCreateKey=0;
fnZwSetValueKey ZwSetValueKey=0;
fnZwFlushKey ZwFlushKey=0;
fnZwLoadDriver ZwLoadDriver=0;
fnZwUnoadDriver ZwUnloadDriver=0;
fnZwYieldExecution ZwYieldExecution=0;
fnNtDeviceIoControlFile NtDeviceIoControlFile=0;
fnZwOpenProcess ZwOpenProcess=0;
fnZwEnumerateKey ZwEnumerateKey=0;
fnZwDeleteFile ZwDeleteFile;
fnZwDeleteValueKey ZwDeleteValueKey=0;
fnZwDeleteKey ZwDeleteKey=0;
fnZwOpenDirectoryObject ZwOpenDirectoryObject=0;
fnZwQueryDirectoryObject ZwQueryDirectoryObject=0;
fnZwOpenSymbolicLinkObject ZwOpenSymbolicLinkObject=0;
fnZwQuerySymbolicLinkObject ZwQuerySymbolicLinkObject=0;
fnMessageBoxA _MessageBoxA=0;
fnZwReadVirtualMemory ZwReadVirtualMemory=0;
fnNtReadFile NtReadFile=0;
fnRtlSystemTimeToLocalTime RtlSystemTimeToLocalTime=0;
fnRtlAnsiStringToUnicodeString RtlAnsiStringToUnicodeString = 0;
fnNtQueryDirectoryFile NtQueryDirectoryFile = 0;

VOID LDR_APIS::CargarFuncionesNT()
{
	HMODULE ntdll = GetModuleHandle(L"NTDLL.DLL");
	if (ntdll != NULL)
	{

		ZwAllocateVirtualMemory = (fnZwAllocateVirtualMemory)GetProcAddress(ntdll, "NtAllocateVirtualMemory");
		ZwFreeVirtualMemory = (fnZwFreeVirtualMemory)GetProcAddress(ntdll, "NtFreeVirtualMemory");
		ZwQuerySystemInformation = (fnZwQuerySystemInformation)GetProcAddress(ntdll, "NtQuerySystemInformation");
		RtlUnicodeStringToAnsiString = (fnRtlUnicodeStringToAnsiString)GetProcAddress(ntdll, "RtlUnicodeStringToAnsiString");
		RtlInitUnicodeString = (fnRtlInitUnicodeString)GetProcAddress(ntdll, "RtlInitUnicodeString");
		RtlInitString = (fnRtlInitString)GetProcAddress(ntdll, "RtlInitString");
		RtlCompressBuffer = (fnRtlCompressBuffer)GetProcAddress(ntdll, "RtlCompressBuffer");
		RtlGetCompressionWorkSpaceSize = (fnRtlGetCompressionWorkSpaceSize)GetProcAddress(ntdll, "RtlGetCompressionWorkSpaceSize");
		RtlDecompressBuffer = (fnRtlDecompressBuffer)GetProcAddress(ntdll, "RtlDecompressBuffer");
		RtlTimeToTimeFields = (fnRtlTimeToTimeFields)GetProcAddress(ntdll, "RtlTimeToTimeFields");
		NtTerminateProcess = (fnNtTerminateProcess)GetProcAddress(ntdll, "NtTerminateProcess");
		RtlExpandEnvironmentStrings_U = (fnRtlExpandEnvironmentStrings_U)GetProcAddress(ntdll, "RtlExpandEnvironmentStrings_U");
		LdrFindResource_U = (fnLdrFindResource_U)GetProcAddress(ntdll, "LdrFindResource_U");
		LdrAccessResource = (fnLdrAccessResource)GetProcAddress(ntdll, "LdrAccessResource");
		RtlAllocateHeap = (fnRtlAllocateHeap)GetProcAddress(ntdll, "RtlAllocateHeap");
		RtlFreeHeap = (fnRtlFreeHeap)GetProcAddress(ntdll, "RtlFreeHeap");
		RtlDosPathNameToNtPathName_U = (fnRtlDosPathNameToNtPathName_U)GetProcAddress(ntdll, "RtlDosPathNameToNtPathName_U");
		NtCreateFile = (fnNtCreateFile)GetProcAddress(ntdll, "NtCreateFile");
		NtWriteFile = (fnNtWriteFile)GetProcAddress(ntdll, "NtWriteFile");
		NtOpenFile = (fnNtOpenFile)GetProcAddress(ntdll, "NtOpenFile");
		NtQueryInformationFile = (fnNtQueryInformationFile)GetProcAddress(ntdll, "NtQueryInformationFile");
		NtClose = (fnNtClose)GetProcAddress(ntdll, "NtClose");
		RtlRandomEx = (fnRtlRandomEx)GetProcAddress(ntdll, "RtlRandomEx");
		NtOpenProcessToken = (fnNtOpenProcessToken)GetProcAddress(ntdll, "NtOpenProcessToken");
		NtAdjustPrivilegesToken = (fnNtAdjustPrivilegesToken)GetProcAddress(ntdll, "NtAdjustPrivilegesToken");
		ZwQueryInformationProcess = (fnZwQueryInformationProcess)GetProcAddress(ntdll, "NtQueryInformationProcess");
		RtlGetVersion = (fnRtlGetVersion)GetProcAddress(ntdll, "RtlGetVersion");
		RtlEqualSid = (fnRtlEqualSid)GetProcAddress(ntdll, "RtlEqualSid");
		NtQueryInformationToken = (fnNtQueryInformationToken)GetProcAddress(ntdll, "NtQueryInformationToken");
		RtlFreeSid = (fnRtlFreeSid)GetProcAddress(ntdll, "RtlFreeSid");
		ZwOpenKey = (fnZwOpenKey)GetProcAddress(ntdll, "NtOpenKey");
		ZwCreateKey = (fnZwCreateKey)GetProcAddress(ntdll, "NtCreateKey");
		ZwSetValueKey = (fnZwSetValueKey)GetProcAddress(ntdll, "NtSetValueKey");
		ZwFlushKey = (fnZwFlushKey)GetProcAddress(ntdll, "NtFlushKey");
		ZwLoadDriver = (fnZwLoadDriver)GetProcAddress(ntdll, "NtLoadDriver");
		ZwUnloadDriver = (fnZwUnoadDriver)GetProcAddress(ntdll, "NtUnloadDriver");
		ZwYieldExecution = (fnZwYieldExecution)GetProcAddress(ntdll, "NtYieldExecution");
		NtDeviceIoControlFile = (fnNtDeviceIoControlFile)GetProcAddress(ntdll, "NtDeviceIoControlFile");
		ZwOpenProcess = (fnZwOpenProcess)GetProcAddress(ntdll, "NtOpenProcess");
		ZwEnumerateKey = (fnZwEnumerateKey)GetProcAddress(ntdll, "NtEnumerateKey");
		ZwDeleteFile = (fnZwDeleteFile)GetProcAddress(ntdll, "NtDeleteFile");
		ZwDeleteValueKey = (fnZwDeleteValueKey)GetProcAddress(ntdll,"NtDeleteValueKey");
		ZwDeleteKey = (fnZwDeleteKey)GetProcAddress(ntdll, "NtDeleteKey");
		ZwOpenDirectoryObject = (fnZwOpenDirectoryObject)GetProcAddress(ntdll, "NtOpenDirectoryObject");
		ZwQueryDirectoryObject = (fnZwQueryDirectoryObject)GetProcAddress(ntdll, "NtQueryDirectoryObject");
		ZwOpenSymbolicLinkObject = (fnZwOpenSymbolicLinkObject)GetProcAddress(ntdll, "NtOpenSymbolicLinkObject");
		ZwQuerySymbolicLinkObject = (fnZwQuerySymbolicLinkObject)GetProcAddress(ntdll, "NtQuerySymbolicLinkObject");
		ZwReadVirtualMemory = (fnZwReadVirtualMemory)GetProcAddress(ntdll, "NtReadVirtualMemory");
		NtReadFile = (fnNtReadFile)GetProcAddress(ntdll, "NtReadFile");
		RtlSystemTimeToLocalTime = (fnRtlSystemTimeToLocalTime)GetProcAddress(ntdll, "RtlSystemTimeToLocalTime");
		RtlAnsiStringToUnicodeString = (fnRtlAnsiStringToUnicodeString)GetProcAddress(ntdll, "RtlAnsiStringToUnicodeString");
		NtQueryDirectoryFile = (fnNtQueryDirectoryFile)GetProcAddress(ntdll, "NtQueryDirectoryFile");
	}

	// test

	HMODULE user = GetModuleHandle(L"USER32.DLL");
	if (user != NULL)
	{
		_MessageBoxA = (fnMessageBoxA)GetProcAddress(user, "MessageBoxA");
	}

}