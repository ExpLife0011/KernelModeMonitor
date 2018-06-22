#pragma once


NTKERNELAPI NTSTATUS PsLookupThreadByThreadId(HANDLE handle, PETHREAD *Ethread);
NTKERNELAPI HANDLE PsGetThreadProcessId(__in PETHREAD Thread);
NTKERNELAPI HANDLE NTAPI PsGetThreadId(IN PETHREAD Thread);
typedef NTKERNELAPI NTSTATUS (NTAPI *fnZwOpenThread)(OUT PHANDLE Handle, IN ACCESS_MASK Mask, IN POBJECT_ATTRIBUTES ObjectAttributes, IN PCLIENT_ID Cid);

fnZwOpenThread ZwOpenThread;

typedef struct _THREAD_INFORMATION_ENTRY
{
#ifdef _WIN64
	UINT_PTR Ethread;
	UINT_PTR Win32StartAddress;
	HANDLE ThreadId;
	UINT8 State;
	UINT_PTR Teb;
	WCHAR CreateTime[MAX_PATH];
	UINT_PTR StartAddress;
#else
	UINT32 Ethread;
	UINT32 Win32StartAddress;
	HANDLE ThreadId;
	UINT8 State;
	UINT32 Teb;
	WCHAR CreateTime[MAX_PATH];
	UINT32 StartAddress;
#endif

}THREAD_INFORMATION_ENTRY,*PTHREAD_INFORMATION_ENTRY;

typedef struct _THREAD_INFORMATION
{
	ULONG NumberOfThreads;
	THREAD_INFORMATION_ENTRY Entry[1];
}THREAD_INFORMATION, *PTHREAD_INFORMATION;

NTSTATUS GetKernelThreadInformation(IN HANDLE ProcessId, OUT PVOID OutputBuffer, IN ULONG OutlengthSize);
VOID GetThreadInfo(IN PETHREAD Ethread, IN PEPROCESS Eprocess, OUT PTHREAD_INFORMATION pi, IN UINT32 Count);
NTSTATUS GetThreadListHead(IN PEPROCESS Eprocess, OUT PTHREAD_INFORMATION pi, IN UINT32 Count);

NTSTATUS TerminateKernelModeThread(IN HANDLE Tid);


#define TIME_FORMAT_WZPREFIX L"%02u/%02u/%04u %02u:%02u:%02u:%03u"
#define TIME_SHORT_FORMAT L"%02u:%02u:%02u:%03u"