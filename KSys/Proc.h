#pragma once

#define MAX_PATH 260

NTKERNELAPI NTSTATUS ZwQueryInformationProcess(
	_In_      HANDLE           ProcessHandle,
	_In_      PROCESSINFOCLASS ProcessInformationClass,
	_Out_     PVOID            ProcessInformation,
	_In_      ULONG            ProcessInformationLength,
	_Out_opt_ PULONG           ReturnLength
	);

// Gestor de Procesos kernel / Hilos **FUNCIONES Ps**

NTKERNELAPI PCHAR PsGetProcessImageFileName(PEPROCESS Process);
NTKERNELAPI NTSTATUS PsLookupProcessByProcessId(HANDLE ProcessId, PEPROCESS *Process);
NTKERNELAPI NTSTATUS PsLookupThreadByThreadId(HANDLE ThreadId, PETHREAD *Ethread);
NTKERNELAPI HANDLE PsGetProcessInheritedFromUniqueProcessId(IN PEPROCESS Process);
NTKERNELAPI BOOLEAN PsIsProcessBeingDebugged(IN PEPROCESS Process);
NTKERNELAPI BOOLEAN PsIsSystemProcess(IN PEPROCESS Process);
NTKERNELAPI PVOID PsGetProcessSectionBaseAddress(PEPROCESS Process);
NTKERNELAPI BOOLEAN PsIsProtectedProcess(IN PEPROCESS Process);
NTKERNELAPI PEPROCESS IoThreadToProcess(PETHREAD Thread);
NTKERNELAPI ULONG PsGetProcessSessionId(IN PEPROCESS Process);
NTKERNELAPI PPEB PsGetProcessPeb(IN PEPROCESS Process);
typedef NTKERNELAPI NTSTATUS (NTAPI *PsSuspendProcess)(IN PEPROCESS Process);
typedef NTKERNELAPI NTSTATUS (NTAPI *PsResumeProcess)(IN PEPROCESS Process);

extern PsSuspendProcess _PsSuspendProcess;
extern PsResumeProcess _PsResumeProcess;


NTSTATUS KernelModeSuspendProcess(HANDLE pid);
NTSTATUS KernelModeResumeProcess(HANDLE pid);
NTSTATUS HideProcessByProcessId(HANDLE pid);
NTSTATUS KernelModeTerminateProcess(HANDLE pid);
NTSTATUS GetFullProcessPathViaSectionObject(IN PEPROCESS Eprocess, OUT PWCHAR ProcessPath);
NTSTATUS KernelModeTerminateInternally(HANDLE pid);



typedef struct _CONTROL_AREA32
{
	struct _SEGMENT*	Segment;
	LIST_ENTRY			DereferenceList;
	UINT_PTR			NumberOfSectionReferences;
	UINT_PTR			NumberOfPfnReferences;
	UINT_PTR			NumberOfMappedViews;
	UINT_PTR			NumberOfUserReferences;
	UINT32				u;
	UINT32				FlushInProgressCount;
	PFILE_OBJECT		FilePointer;
	UINT32				ControlAreaLock;
	UINT32				ModifiedWriteCount;
	UINT_PTR			WaitingForDeletion;
	UINT_PTR			u2;
	UINT64				Padding;
	UINT64   			LockedPages;
	LIST_ENTRY			ViewList;
} CONTROL_AREA32, *PCONTROL_AREA32;


typedef struct _SEGMENT_OBJECT32
{
	PCONTROL_AREA32	ControlArea;
	UINT32			TotalNumberOfPtes;
	UINT32			SegmentFlags;
	UINT_PTR		NumberOfCommittedPages;
	UINT64			SizeOfSegment;
	union
	{
		UINT_PTR ExtendInfo;
		UINT_PTR BasedAddress;
	} u;
	UINT_PTR	SegmentLock;
	UINT_PTR	u1;
	UINT_PTR	u2;
	UINT_PTR	PrototypePte;
#ifndef _WIN64
	UINT32		Padding;
#endif // !_WIN64
	UINT_PTR	ThePtes;
} SEGMENT_OBJECT32, *PSEGMENT_OBJECT32;


typedef struct _SECTION_OBJECT32
{
	PVOID	 StartingVa;
	PVOID	 EndingVa;
	PVOID	 Parent;
	PVOID	 LeftChild;
	PVOID	 RightChild;
	PSEGMENT_OBJECT32 Segment;
} SECTION_OBJECT32, *PSECTION_OBJECT32;


typedef  struct _CONTROL_AREA64
{
	PVOID Segment;
	LIST_ENTRY DereferenceList;
	ULONG64 NumberOfSectionReferences;
	ULONG64 NumberOfPfnReferences;
	ULONG64 NumberOfMappedViews;
	ULONG64 NumberOfUserReferences;
	union
	{
		ULONG LongFlags;
		ULONG Flags;
	} u;
	PFILE_OBJECT FilePointer;
}CONTROL_AREA64, *PCONTROL_AREA64;

typedef struct  _SEGMENT_OBJECT64
{
	struct _CONTROL_AREA64* ControlArea;
	ULONG32  TotalNumberOfPtes;
	ULONG32  SegmentFlags;
	ULONG64  NumberOfCommittedPages;
	ULONG64  SizeOfSegment;
	union
	{
		ULONG64 ExtendInfo;
		ULONG64 BasedAddress;
	}Unknow;
	ULONG64 SegmentLock;
	ULONG64 u1;
	ULONG64 u2;
	ULONG64 PrototypePte;
	ULONG64 ThePtes;
}SEGMENT_OBJECT64, *PSEGMENT_OBJECT64;

typedef struct _SECTION_OBJECT64
{
	PVOID StartingVa;
	PVOID EndingVa;
	PVOID Parent;
	PVOID LeftChild;
	PVOID RightChild;
	struct _SEGMENT_OBJECT64* Segment;
}SECTION_OBJECT64, *PSECTION_OBJECT64;

typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemBasicInformation = 0,
	SystemProcessorInformation = 1,             // obsolete...delete
	SystemPerformanceInformation = 2,
	SystemTimeOfDayInformation = 3,
	SystemPathInformation = 4,
	SystemProcessInformation = 5,
	SystemCallCountInformation = 6,
	SystemDeviceInformation = 7,
	SystemProcessorPerformanceInformation = 8,
	SystemFlagsInformation = 9,
	SystemCallTimeInformation = 10,
	SystemModuleInformation = 11,
	SystemLocksInformation = 12,
	SystemStackTraceInformation = 13,
	SystemPagedPoolInformation = 14,
	SystemNonPagedPoolInformation = 15,
	SystemHandleInformation = 16,
	SystemObjectInformation = 17,
	SystemPageFileInformation = 18,
	SystemVdmInstemulInformation = 19,
	SystemVdmBopInformation = 20,
	SystemFileCacheInformation = 21,
	SystemPoolTagInformation = 22,
	SystemInterruptInformation = 23,
	SystemDpcBehaviorInformation = 24,
	SystemFullMemoryInformation = 25,
	SystemLoadGdiDriverInformation = 26,
	SystemUnloadGdiDriverInformation = 27,
	SystemTimeAdjustmentInformation = 28,
	SystemSummaryMemoryInformation = 29,
	SystemMirrorMemoryInformation = 30,
	SystemPerformanceTraceInformation = 31,
	SystemObsolete0 = 32,
	SystemExceptionInformation = 33,
	SystemCrashDumpStateInformation = 34,
	SystemKernelDebuggerInformation = 35,
	SystemContextSwitchInformation = 36,
	SystemRegistryQuotaInformation = 37,
	SystemExtendServiceTableInformation = 38,
	SystemPrioritySeperation = 39,
	SystemVerifierAddDriverInformation = 40,
	SystemVerifierRemoveDriverInformation = 41,
	SystemProcessorIdleInformation = 42,
	SystemLegacyDriverInformation = 43,
	SystemCurrentTimeZoneInformation = 44,
	SystemLookasideInformation = 45,
	SystemTimeSlipNotification = 46,
	SystemSessionCreate = 47,
	SystemSessionDetach = 48,
	SystemSessionInformation = 49,
	SystemRangeStartInformation = 50,
	SystemVerifierInformation = 51,
	SystemVerifierThunkExtend = 52,
	SystemSessionProcessInformation = 53,
	SystemLoadGdiDriverInSystemSpace = 54,
	SystemNumaProcessorMap = 55,
	SystemPrefetcherInformation = 56,
	SystemExtendedProcessInformation = 57,
	SystemRecommendedSharedDataAlignment = 58,
	SystemComPlusPackage = 59,
	SystemNumaAvailableMemory = 60,
	SystemProcessorPowerInformation = 61,
	SystemEmulationBasicInformation = 62,
	SystemEmulationProcessorInformation = 63,
	SystemExtendedHandleInformation = 64,
	SystemLostDelayedWriteInformation = 65,
	SystemBigPoolInformation = 66,
	SystemSessionPoolTagInformation = 67,
	SystemSessionMappedViewInformation = 68,
	SystemHotpatchInformation = 69,
	SystemObjectSecurityMode = 70,
	SystemWatchdogTimerHandler = 71,
	SystemWatchdogTimerInformation = 72,
	SystemLogicalProcessorInformation = 73,
	SystemWow64SharedInformation = 74,
	SystemRegisterFirmwareTableInformationHandler = 75,
	SystemFirmwareTableInformation = 76,
	SystemModuleInformationEx = 77,
	SystemVerifierTriageInformation = 78,
	SystemSuperfetchInformation = 79,
	SystemMemoryListInformation = 80,
	SystemFileCacheInformationEx = 81,
	MaxSystemInfoClass = 82  // MaxSystemInfoClass should always be the last enum

} SYSTEM_INFORMATION_CLASS;


typedef enum _THREAD_STATE {
	StateInitialized,
	StateReady,
	StateRunning,
	StateStandby,
	StateTerminated,
	StateWait,
	StateTransition,
	StateUnknown
}THREAD_STATE;

struct VM_COUNTERS {
	SIZE_T        PeakVirtualSize;
	SIZE_T        VirtualSize;
	ULONG         PageFaultCount;
	SIZE_T        PeakWorkingSetSize;
	SIZE_T        WorkingSetSize;
	SIZE_T        QuotaPeakPagedPoolUsage;
	SIZE_T        QuotaPagedPoolUsage;
	SIZE_T        QuotaPeakNonPagedPoolUsage;
	SIZE_T        QuotaNonPagedPoolUsage;
	SIZE_T        PagefileUsage;
	SIZE_T        PeakPagefileUsage;
	SIZE_T        PrivatePageCount;
}; 

typedef struct _SYSTEM_THREAD {
	LARGE_INTEGER   KernelTime;
	LARGE_INTEGER   UserTime;
	LARGE_INTEGER   CreateTime;
	ULONG           WaitTime;
	PVOID          StartAddress;
	CLIENT_ID       ClientId;
	DWORD           Priority;
	LONG            BasePriority;
	ULONG           ContextSwitchCount;
	THREAD_STATE    State;
	ULONG           WaitReason;
}SYSTEM_THREAD, *PSYSTEM_THREAD; 


typedef struct _SYSTEM_PROCESS_INFORMATION {
	ULONG           NextEntryOffset;
	ULONG           NumberOfThreads;
	LARGE_INTEGER   Reserved[3];
	LARGE_INTEGER   CreateTime;
	LARGE_INTEGER   UserTime;
	LARGE_INTEGER   KernelTime;
	UNICODE_STRING  ImageName;
	DWORD           BasePriority;
	HANDLE          ProcessId;
	HANDLE          ParentProcessId;
	ULONG           HandleCount;
	ULONG           Reserved2[2];
	VM_COUNTERS     VMCounters;
	IO_COUNTERS     IOCounters;
	SYSTEM_THREAD   Threads[1];
}SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;


typedef struct _KERNEL_PROCESS_ENTRY
{
	WCHAR ProcessName[50];
	WCHAR ProcessPath[MAX_PATH];
	ULONG Pid;
	ULONG PPid;
	ULONG NumberOfThreads;
	ULONG HandleCount;
	WCHAR CreateTime[100];
	ULONG_PTR Eprocess;
	LONGLONG CreationTimeQuadPart;
}KERNEL_PROCESS_ENTRY, *PKERNEL_PROCESS_ENTRY;

typedef struct _KERNEL_PROCESS
{
	ULONG NumberOfEntries;
	KERNEL_PROCESS_ENTRY Entry[1];

}KERNEL_PROCESS, *PKERNEL_PROCESS;

typedef NTKERNELAPI NTSTATUS (*fnZwQuerySystemInformation)(
	_In_      SYSTEM_INFORMATION_CLASS SystemInformationClass,
	_Inout_   PVOID                    SystemInformation,
	_In_      ULONG                    SystemInformationLength,
	_Out_opt_ PULONG                   ReturnLength
	);


fnZwQuerySystemInformation ZwQuerySystemInformation;


NTSTATUS GetKernelProcessInformation(OUT PVOID OutputBuffer, IN ULONG OutlengthSize);