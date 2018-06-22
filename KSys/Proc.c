#include "driver.h"

#include "Proc.h"


/*//////////////////////////////////////////////
/
/	Function: GetFullProcessPathViaSectionObject
/
/	Purpose: Gather process path by Eprocess
/   Input param
/
/	Environment:
/		kernel mode only
/
*//////////////////////////////////////////////

NTSTATUS GetFullProcessPathViaSectionObject(IN PEPROCESS Eprocess, OUT PWCHAR ProcessPath) 
{
	NTSTATUS st = STATUS_UNSUCCESSFUL;
	POBJECT_NAME_INFORMATION Object = NULL;
	PFILE_OBJECT FileObject = NULL;
#ifndef _WIN64
	PSEGMENT_OBJECT32 Segment = NULL;
	PCONTROL_AREA32 ControlArea = NULL;
	PSECTION_OBJECT32 SectionObject = NULL;
#else
	PSEGMENT_OBJECT64 Segment = NULL;
	PCONTROL_AREA64 ControlArea = NULL;
	PSECTION_OBJECT64 SectionObject = NULL;
#endif _WIN64


	/*
		kd> dt _eprocess	
			nt!_EPROCESS
			+0x000 Pcb              : _KPROCESS
			+0x098 ProcessLock      : _EX_PUSH_LOCK
			...
			+0x128 SectionObject    : Ptrr32 Void
	*/


		/*
		/ kd> dt -b nt!_SECTION_OBJECT 0x8b1b65d8
				+0x000 StartingVa       : 0x89001cf8 
				+0x004 EndingVa         : 0x00000001 
				+0x008 Parent           : (null) 
				+0x00c LeftChild        : (null) 
				+0x010 RightChild       : 0x89001140 
				+0x014 Segment          : 0x8b3c9008 
		*/
#ifndef _WIN64
	SectionObject = (PSECTION_OBJECT32)(*(PUINT_PTR)((PUINT8)Eprocess + 0x128));
#else
	SectionObject = (PSECTION_OBJECT64)(*(PUINT_PTR)((PUINT8)Eprocess + 0x268));
#endif _WIN64
	if (SectionObject && MmIsAddressValid(SectionObject))
	{

			/*
				kd> dt -b nt!_SEGMENT  0x8b3c9008 
					+0x000 ControlArea      : 0x85ac2dd0 
					+0x004 TotalNumberOfPtes : 0x281
					+0x008 SegmentFlags     : _SEGMENT_FLAGS
			*/

		Segment = SectionObject->Segment;
		if (Segment && MmIsAddressValid(Segment)) 
		{
				/*
					kd> dt -b nt!_CONTROL_AREA 0x85ac2dd0 
						+0x000 Segment          : 0x8b3c9008 
						+0x004 DereferenceList  : _LIST_ENTRY [ 0x0 - 0x0 ]
						...
						+0x000 Object           : 0x85ac2c8f
				*/

			ControlArea = Segment->ControlArea;
			if (ControlArea && MmIsAddressValid(ControlArea))
			{
					/*
						kd> dt -b nt!_FILE_OBJECT (0x85ac2c8f & 0xFFFFFFF8)
							+0x000 Type             : 0n5
							+0x002 Size             : 0n128
							...
							+0x030 FileName         : _UNICODE_STRING "\Windows\explorer.exe"
					*/
#ifndef _WIN64
				FileObject = (PFILE_OBJECT)((UINT_PTR)ControlArea->FilePointer & 0xFFFFFFF8);
#else
				FileObject = (PFILE_OBJECT)((UINT_PTR)ControlArea->FilePointer & ~0xf);
#endif _WIN64
				if (FileObject && MmIsAddressValid(FileObject)) 
				{

					ULONG bytes = 0;

					st = ObQueryNameString((PVOID)FileObject, NULL, 0, &bytes);
					if (st == STATUS_INFO_LENGTH_MISMATCH) 
					{
						Object = (POBJECT_NAME_INFORMATION)ExAllocatePoolWithTag(NonPagedPool, bytes, 'Obj');
						if (Object != NULL) 
						{
							st = ObQueryNameString((PVOID)FileObject, Object, bytes, &bytes);
							if (NT_SUCCESS(st)) 
							{
#ifndef _WIN64
								UINT32 ProcessFullPathLength = 0;
#else
								UINT64 ProcessFullPathLength = 0;
#endif _WIN64
								if (Object->Name.Length >= MAX_PATH) 
								{
									ProcessFullPathLength = MAX_PATH - 1;
								}
								else 
								{
									ProcessFullPathLength = Object->Name.Length;
								}

								RtlStringCchCopyW(ProcessPath, ProcessFullPathLength + 1, Object->Name.Buffer);

								st = STATUS_SUCCESS;

							}
							ExFreePoolWithTag(Object, 'Obj');
						}
					}
				}
			}
		}
	}



	return st;
}


/*//////////////////////////////////////////////
/
/	Function: KernelModeSuspendProcess
/
/	Purpose: Suspend Process by Process ID
/
/	Environment:
/		kernel mode only
/
*//////////////////////////////////////////////

NTSTATUS KernelModeSuspendProcess(HANDLE pid)
{
	PEPROCESS Eprocess = NULL;
	NTSTATUS st = STATUS_UNSUCCESSFUL;
	UNICODE_STRING DynamicAddress;

	RtlInitUnicodeString(&DynamicAddress, L"PsSuspendProcess");
	_PsSuspendProcess = (PsSuspendProcess)MmGetSystemRoutineAddress(&DynamicAddress);
	if (_PsSuspendProcess) 
	{
		st = PsLookupProcessByProcessId((HANDLE)pid, &Eprocess);
		if (NT_SUCCESS(st)) 
		{
			if (NT_SUCCESS(_PsSuspendProcess(Eprocess))) 
			{
				st = STATUS_SUCCESS;
			}
			ObfDereferenceObject(Eprocess);
		}

	}

	return st;
}


/*//////////////////////////////////////////////
/
/	Function: KernelModeResumeProcess
/
/	Purpose: Resume Process by ID
/
/	Environment:
/		kernel mode only
/
*//////////////////////////////////////////////

NTSTATUS KernelModeResumeProcess(HANDLE pid)
{
	PEPROCESS Eprocess = NULL;
	NTSTATUS st = STATUS_UNSUCCESSFUL;
	UNICODE_STRING DynamicAddress;

	RtlInitUnicodeStringEx(&DynamicAddress, L"PsResumeProcess");
	_PsResumeProcess = (PsResumeProcess)MmGetSystemRoutineAddress(&DynamicAddress);
	if (_PsResumeProcess)
	{
		st = PsLookupProcessByProcessId((HANDLE)pid, &Eprocess);
		if (NT_SUCCESS(st))
		{
			if (NT_SUCCESS(_PsResumeProcess(Eprocess)))
			{
				st = STATUS_SUCCESS;
			}
			ObfDereferenceObject(Eprocess);
		}
	}

	return st;
}


/*//////////////////////////////////////////////
/
/	Function: HideProcessById
/
/	Purpose: Hide process by given ID
/
/	Environment:
/		kernel mode only
/
*//////////////////////////////////////////////

NTSTATUS HideProcessByProcessId(HANDLE pid) 
{
	NTSTATUS st;
	ULONG mjv = 0, mvr = 0;
	PEPROCESS eprocess = NULL;
	ULONG offset;
#ifndef _WIN64
	PLIST_ENTRY32 current = NULL, prev = NULL, next = NULL;
#else
	PLIST_ENTRY64 current = NULL, prev = NULL, next = NULL;
#endif _WIN64

	st = PsLookupProcessByProcessId((HANDLE)pid, &eprocess);
	if (NT_SUCCESS(st)) 
	{
		PsGetVersion(&mjv, &mvr, 0, 0);
		switch (mvr) 
		{

		case 1:	

#ifndef _WIN64
			offset = 0x0b8;		// WIN 7 X786 ActiveProcessLinks Kernel
#else	
			offset = 0x188;	// WIN 7 X64 ActiveProcessLinks Kernel
#endif _WIN64
			break;

		default:
			break;
		}
#ifndef _WIN64
		current = (PLIST_ENTRY32)((PUCHAR)eprocess + offset);
		prev = (PLIST_ENTRY32)current->Blink;
		next = (PLIST_ENTRY32)current->Flink;
		prev->Flink = (PLIST_ENTRY32)current->Flink;
		next->Blink = (PLIST_ENTRY32)current->Blink;

		current->Flink = (PLIST_ENTRY32)&(current->Flink);
		current->Blink = (PLIST_ENTRY32)&(current->Flink);
#else
		current = (PLIST_ENTRY64)((PUCHAR)eprocess + offset);
		prev = (PLIST_ENTRY64)current->Blink;
		next = (PLIST_ENTRY64)current->Flink;
		prev->Flink = (PLIST_ENTRY64)current->Flink;
		next->Blink = (PLIST_ENTRY64)current->Blink;

		current->Flink = (PLIST_ENTRY64)&(current->Flink);
		current->Blink = (PLIST_ENTRY64)&(current->Flink);
#endif _WIN64
		ObfDereferenceObject(eprocess);

	}

	return st;
}


/*//////////////////////////////////////////////
/
/	Function: KernelModeTerminateProcess
/
/	Purpose: Terminate Process by ID
/
/	Environment:
/		kernel mode only
/
*//////////////////////////////////////////////


NTSTATUS KernelModeTerminateProcess(HANDLE pid) 
{
	PEPROCESS Eprocess = NULL;
	NTSTATUS st = STATUS_UNSUCCESSFUL;
	HANDLE handle = 0;

	st = PsLookupProcessByProcessId((HANDLE)pid, &Eprocess);
	if (NT_SUCCESS(st)) 
	{
		st = ObOpenObjectByPointer(Eprocess, OBJ_KERNEL_HANDLE, NULL, PROCESS_ALL_ACCESS, *PsProcessType, KernelMode, &handle);
		if (NT_SUCCESS(st)) 
		{
			if (NT_SUCCESS(ZwTerminateProcess(handle, 1)))
			{
				st = STATUS_SUCCESS;
			}
		}
	}

	return st;

}


/*//////////////////////////////////////////////
/
/	Function: GetKerneProcessInformation
/
/	Purpose: Gather process informationm allocate
/   it in struct to user-space
/
/	Environment:
/		kernel mode only
/
*//////////////////////////////////////////////

NTSTATUS GetKernelProcessInformation(OUT PVOID OutputBuffer, IN ULONG OutlengthSize) 
{
	NTSTATUS st = STATUS_UNSUCCESSFUL;
	UNICODE_STRING Dynamic = { 0 };
	HANDLE handle;
	CLIENT_ID cid;
	OBJECT_ATTRIBUTES oa;
	ULONG Size = (OutlengthSize - sizeof(KERNEL_PROCESS)) / sizeof(KERNEL_PROCESS_ENTRY);
	ULONG bytes = 0;
	PEPROCESS Eprocess = NULL;
	PSYSTEM_PROCESS_INFORMATION sysproc = NULL;
	PKERNEL_PROCESS kproc = (PKERNEL_PROCESS)OutputBuffer;

	RtlInitUnicodeString(&Dynamic, L"ZwQuerySystemInformation");
	ZwQuerySystemInformation = MmGetSystemRoutineAddress(&Dynamic);
	if (ZwQuerySystemInformation && MmIsAddressValid(ZwQuerySystemInformation)) 
	{
		st = ZwQuerySystemInformation(SystemProcessInformation, NULL, 0, &bytes);
		if (st == STATUS_INFO_LENGTH_MISMATCH) 
		{
			PVOID base = ExAllocatePoolWithTag(NonPagedPool, bytes, 'kEM');
			if (base != NULL) 
			{
				st = ZwQuerySystemInformation(SystemProcessInformation, base, bytes, &bytes);
				if (NT_SUCCESS(st)) 
				{
					sysproc = (PSYSTEM_PROCESS_INFORMATION)base;
					if (sysproc && MmIsAddressValid(sysproc)) 
					{
						while (sysproc->NextEntryOffset)
						{
							sysproc = (PSYSTEM_PROCESS_INFORMATION)(((PUCHAR)sysproc) + sysproc->NextEntryOffset);

							ULONG contador = kproc->NumberOfEntries;
							
							
							kproc->Entry[contador].Pid = (ULONG)sysproc->ProcessId;
							kproc->Entry[contador].PPid = (ULONG)sysproc->ParentProcessId;
							RtlCopyMemory(kproc->Entry[contador].ProcessName, sysproc->ImageName.Buffer, sysproc->ImageName.Length);
							kproc->Entry[contador].HandleCount = sysproc->HandleCount;
							kproc->Entry[contador].NumberOfThreads = sysproc->NumberOfThreads;

							kproc->NumberOfEntries++;

							cid.UniqueProcess = (HANDLE)sysproc->ProcessId;
							cid.UniqueThread = 0;
							st = ZwOpenProcess(&handle, GENERIC_ALL, &oa, &cid);
							if (NT_SUCCESS(st)) 
							{
								ObReferenceObjectByHandle(handle, GENERIC_ALL, *PsProcessType, KernelMode, (PVOID*)&Eprocess, NULL);

								kproc->Entry[contador].Eprocess = (ULONG_PTR)Eprocess;												  // Eprocess address
								kproc->Entry[contador].CreationTimeQuadPart = (LONGLONG)PsGetProcessCreateTimeQuadPart(Eprocess);	 // creation time

								GetFullProcessPathViaSectionObject(Eprocess, kproc->Entry[contador].ProcessPath);

								ObfDereferenceObject(Eprocess);
								ZwClose(handle);

								st = STATUS_SUCCESS;
							}
							RtlSecureZeroMemory(&oa, sizeof(OBJECT_ATTRIBUTES));
						}
					}
				}
				ExFreePoolWithTag(base, 'kEM');
				base = NULL;
			}
		}
	}
	return st;
}

NTSTATUS KernelModeTerminateInternally(HANDLE pid) 
{
	PEPROCESS Eprocess = NULL;
	PEPROCESS pEprocess = NULL;
	PETHREAD Ethread = NULL;
	NTSTATUS st = STATUS_UNSUCCESSFUL;

	st = PsLookupProcessByProcessId((HANDLE)pid, &Eprocess);
	if (NT_SUCCESS(st)) 
	{
		for (ULONG i = 4; i < 0x40000; i += 4)
		{
			st = PsLookupThreadByThreadId((HANDLE)i, &Ethread);
			if (NT_SUCCESS(st))
			{
				pEprocess = IoThreadToProcess(Ethread);
				if (pEprocess == Eprocess)
				{
					PspTerminateThreadByPointer(Ethread, 0, 1);
					st = STATUS_SUCCESS;
				}
				ObfDereferenceObject(Ethread);
			}
		}
		ObfDereferenceObject(Eprocess);
	}

	return st;

}