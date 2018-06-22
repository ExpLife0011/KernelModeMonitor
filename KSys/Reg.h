#pragma once

#define MAX_PATH 260

typedef NTKERNELAPI NTSTATUS(*fnZwDeleteKey)(HANDLE handle);
typedef NTKERNELAPI NTSTATUS(*fnZwDeleteValueKey)(HANDLE handle, PUNICODE_STRING Valuename);
typedef NTKERNELAPI NTSTATUS(*fnZwSetValueKey)(HANDLE keyhandle, PUNICODE_STRING Valuename, ULONG Titleindex, ULONG Type, PVOID Data, ULONG Datasize);
typedef NTKERNELAPI NTSTATUS(*fnZwQueryValueKey)(HANDLE keyhandle, PUNICODE_STRING Valuename, KEY_VALUE_INFORMATION_CLASS keyclass, PVOID Keyinfo, ULONG keylength, PULONG bytesreturned);
typedef NTKERNELAPI NTSTATUS(*fnZwEnumerateValueKey)(HANDLE keyhandle, ULONG index, KEY_VALUE_INFORMATION_CLASS KeyValClass, PVOID KeyValueInformation, ULONG Length, PULONG ResultLength);
typedef NTKERNELAPI NTSTATUS(*fnZwEnumerateKey)(HANDLE keyhandle, ULONG index, KEY_INFORMATION_CLASS KeyInfoClass, PVOID KeyInformation, ULONG Length, PULONG ResultLength);



fnZwDeleteKey ZwDeleteKeyOriginal;
fnZwDeleteValueKey ZwDeleteValueKeyOriginal;
fnZwSetValueKey ZwSetValueKeyOriginal;
fnZwQueryValueKey ZwQueryValueKeyOriginal;
fnZwEnumerateValueKey ZwEnumerateValueKeyOriginal;
fnZwEnumerateKey ZwEnumerateKeyOriginal;



NTSTATUS HookZwDeleteKey(HANDLE keyhandle);
NTSTATUS HookZwDeleteValueKey(HANDLE keyhandle, PUNICODE_STRING Valuename);
NTSTATUS HookZwSetValueKey(HANDLE keyhandle,
	PUNICODE_STRING Valuename,
	ULONG Title,
	ULONG Type,
	PVOID Data,
	ULONG Datasize);

NTSTATUS HookZwQueryValueKey(HANDLE keyhandle,
	PUNICODE_STRING Valuename,
	KEY_VALUE_INFORMATION_CLASS keyclass,
	PVOID keyinfo,
	ULONG keylength,
	PULONG bytesreturned);

NTSTATUS HookZwEnumerateKey(HANDLE handle,
	ULONG index,
	KEY_INFORMATION_CLASS KeyInfoClass,
	PVOID KeyInformation,
	ULONG Length,
	PULONG ResultLength);



void Log_NotificacionRegistro(const char *Formato, ...);
void DesProtegerRegistro();


typedef struct _REG_KERNEL_ENTRY
{
	WCHAR KeyName[MAX_PATH];
	WCHAR CreateTime[MAX_PATH];

}REG_KERNEL_ENTRY,*PREG_KERNEL_ENTRY;

typedef struct _REG_KERNEL_INFORMATION
{
	ULONG_PTR NumberOfEntries;
	REG_KERNEL_ENTRY Entry[1];

}REG_KERNEL_INFORMATION, *PREG_KERNEL_INFORMATION;


typedef struct _KERNEL_REG_VAL_ENTRY
{
	WCHAR Name[MAX_PATH];
	WCHAR DataName[MAX_PATH];
	ULONG TypeOfKey;
	ULONG udatalong;

}KERNEL_REG_VAL_ENTRY, *PKERNEL_REG_VAL_ENTRY;

typedef struct _KERNEL_REG_VAL_INFORMATION
{

	ULONG_PTR NumberOfEntries;
	KERNEL_REG_VAL_ENTRY Entry[1];

}KERNEL_REG_VAL_INFORMATION, *PKERNEL_REG_VAL_INFORMATION;

NTSTATUS GetKernelRegInformation(IN PWCHAR KeyBuffer, OUT PVOID OutputBuffer, IN ULONG Outlength);

NTSTATUS GetKernelValueEnumeration(IN PWCHAR KeyBuffer, OUT PVOID OutputBuffer, IN ULONG Outlength);
