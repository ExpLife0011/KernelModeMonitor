#pragma once

typedef struct _HANDLE_TABLE_ENTRY_INFO
{
	ULONG AuditMask;
} HANDLE_TABLE_ENTRY_INFO, *PHANDLE_TABLE_ENTRY_INFO;

typedef struct _HANDLE_TABLE_ENTRY
{
	union
	{
		PVOID Object;
		ULONG ObAttributes;
		PHANDLE_TABLE_ENTRY_INFO InfoTable;
		ULONG Value;
	};
	union
	{
		ULONG GrantedAccess;
		struct
		{
			USHORT GrantedAccessIndex;
			USHORT CreatorBackTraceIndex;
		};
		LONG NextFreeTableEntry;
	};
} HANDLE_TABLE_ENTRY, *PHANDLE_TABLE_ENTRY;

typedef struct _HANDLE_TRACE_DB_ENTRY
{
	CLIENT_ID ClientId;
	PVOID Handle;
	ULONG Type;
	VOID * StackTrace[16];
} HANDLE_TRACE_DB_ENTRY, *PHANDLE_TRACE_DB_ENTRY;


typedef struct _HANDLE_TRACE_DEBUG_INFO
{
	LONG RefCount;
	ULONG TableSize;
	ULONG BitMaskFlags;
	FAST_MUTEX CloseCompactionLock;
	ULONG CurrentStackIndex;
	HANDLE_TRACE_DB_ENTRY TraceDb[1];
} HANDLE_TRACE_DEBUG_INFO, *PHANDLE_TRACE_DEBUG_INFO;


typedef struct _HANDLE_TABLE
{
	ULONG TableCode;
	PEPROCESS QuotaProcess;
	PVOID UniqueProcessId;
	EX_PUSH_LOCK HandleLock;
	LIST_ENTRY HandleTableList;
	EX_PUSH_LOCK HandleContentionEvent;
	PHANDLE_TRACE_DEBUG_INFO DebugInfo;
	LONG ExtraInfoPages;
	ULONG Flags;
	ULONG StrictFIFO : 1;
	LONG FirstFreeHandle;
	PHANDLE_TABLE_ENTRY LastFreeHandleEntry;
	LONG HandleCount;
	ULONG NextHandleNeedingPool;
} HANDLE_TABLE, *PHANDLE_TABLE;


UINT_PTR GetInternalOsFunctionAddressByOffsetAndFunctionName(
	IN PVOID FunctionAddress,
	IN PCWSTR FunctionName,
	IN UINT8 FirstOffset, 
	IN UINT8 SecondOffset, 
	IN OPTIONAL UINT8 ThirdOffset);


UINT_PTR GetInternalOsFunctionWithNTFunction(IN PVOID FunctionAddress, IN UINT8 FirstOffset, IN UINT8 SecondOffset);


UINT_PTR GetPspCidTable();

VOID LoadInternalAPI();


typedef NTKERNELAPI NTSTATUS(*PSLOOKUPPROCESSBYPROCESSID)(HANDLE Handle, PEPROCESS *Eprocess);
typedef NTKERNELAPI NTSTATUS(*PSTERMINATESYSTEMTHREAD)(NTSTATUS ExitStatus);
typedef NTKERNELAPI NTSTATUS(*PSPTERMINATETHREADBYPOINTER)(IN PETHREAD Ethread, IN NTSTATUS ExitStatus, IN BOOLEAN DirectTerminate);
typedef PHANDLE_TABLE_ENTRY (NTAPI *FNExMapHandleToPointer)(IN PHANDLE_TABLE HandleTable, IN HANDLE Handle);



typedef NTKERNELAPI NTSTATUS(*IOCREATEFILE)(
	PHANDLE                   FileHandle,
	ACCESS_MASK               DesiredAccess,
	POBJECT_ATTRIBUTES        ObjectAttributes,
	PIO_STATUS_BLOCK          IoStatusBlock,
	PLARGE_INTEGER            AllocationSize,
	ULONG                     FileAttributes,
	ULONG                     ShareAccess,
	ULONG                     Disposition,
	ULONG                     CreateOptions,
	PVOID                     EaBuffer,
	ULONG                     EaLength,
	CREATE_FILE_TYPE          CreateFileType,
	PVOID                     InternalParameters,
	ULONG                     Options
	);

typedef NTKERNELAPI NTSTATUS(*IOPCREATEFILE)(PHANDLE pHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES pObjectAttributes,
	PIO_STATUS_BLOCK pIoStatusBlock,
	PLARGE_INTEGER pAllocationSize,
	ULONG FileAttributes,
	ULONG ShareAccess,
	ULONG Disposition,
	ULONG CreateOptions,
	PVOID pEaBuffer,
	ULONG EaLength,
	CREATE_FILE_TYPE CreateFileType,
	PVOID pExtraCreateParameters,
	ULONG Options,
	ULONG Flags,
	PVOID pIoDriverCreateContext);

extern IOCREATEFILE IoCreateFileOriginal;
extern IOPCREATEFILE IopCreateFile;
extern PSPTERMINATETHREADBYPOINTER PspTerminateThreadByPointer;
extern PSTERMINATESYSTEMTHREAD PsTerminateSystemThreadOriginal;
extern UINT_PTR PspCidTable;
extern PSLOOKUPPROCESSBYPROCESSID PsLookupProcessByProcessIdOriginal;
extern FNExMapHandleToPointer ExMapHandleToPointer;



