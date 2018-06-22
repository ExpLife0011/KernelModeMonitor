#include "driver.h"
#include "thr.h"


#ifdef _WIN64

VOID GetThreadInfo(IN PETHREAD Ethread, IN PEPROCESS Eprocess, OUT PTHREAD_INFORMATION pi, IN UINT32 Count)
{

	PEPROCESS EProcess = NULL;
	UINT32 CurrentCount = 0;
	LARGE_INTEGER Time;
	TIME_FIELDS timer;

	if (Ethread || MmIsAddressValid(Ethread))
	{

		if (IoThreadToProcess)
		{

			EProcess = IoThreadToProcess(Ethread);
		}
		
		if (NT_SUCCESS(ObReferenceObjectByPointer(Ethread,0,*PsThreadType,KernelMode) && Eprocess == EProcess))
		{
			CurrentCount = pi->NumberOfThreads;
			if (Count > CurrentCount)
			{

				if (PsGetThreadId)
				{

					pi->Entry[CurrentCount].ThreadId = (UINT32)(UINT_PTR)PsGetThreadId(Ethread);
				}

				pi->Entry[CurrentCount].Ethread = (UINT_PTR)Ethread;
				pi->Entry[CurrentCount].Teb = *(PUINT_PTR)((UINT_PTR)Ethread + 0x0b8);
				pi->Entry[CurrentCount].StartAddress = *(PUINT_PTR)((PUINT8)Ethread + 0x388);
				pi->Entry[CurrentCount].Win32StartAddress = *(PUINT_PTR)((PUINT8)Ethread + 0x410);
				pi->Entry[CurrentCount].State = *((PUINT8)Ethread + 0x164);

				Time = *(LARGE_INTEGER*)((UINT_PTR)Ethread + 0x360);
				RtlTimeToTimeFields(&Time, &timer);
				RtlStringCchPrintfW(pi->Entry[CurrentCount].CreateTime, MAX_PATH, TIME_FORMAT_WZPREFIX, timer.Day, timer.Month, timer.Year, timer.Hour + 2, timer.Minute, timer.Second, timer.Milliseconds);

			}
			pi->NumberOfThreads++;

			ObfDereferenceObject(Ethread);
		}

	}


}


NTSTATUS GetThreadListHead(IN PEPROCESS Eprocess, OUT PTHREAD_INFORMATION pi, IN UINT32 Count)
{
	NTSTATUS st = STATUS_UNSUCCESSFUL;
	PLIST_ENTRY ThreadListHead = (PLIST_ENTRY)((PUINT8)Eprocess + 0x030);
	if (ThreadListHead && MmIsAddressValid(ThreadListHead) && MmIsAddressValid(ThreadListHead->Flink))
	{
		UINT_PTR Max = 2 * PAGE_SIZE;
		for (PLIST_ENTRY ThreadListEntry = ThreadListHead->Flink; MmIsAddressValid(ThreadListEntry) && ThreadListEntry != ThreadListHead && Max--;
			ThreadListEntry = ThreadListEntry->Flink)
		{
			PETHREAD Ethread = (PETHREAD)((PUINT8)ThreadListEntry - 0x2f8);
			GetThreadInfo(Ethread, Eprocess, pi, Count);
		}
	}
	if (pi->NumberOfThreads)
	{
		st = STATUS_SUCCESS;
	}

	return st;

}

NTSTATUS GetKernelThreadInformation(IN HANDLE ProcessId, OUT PVOID OutputBuffer, IN ULONG OutlengthSize)
{
	NTSTATUS st = STATUS_UNSUCCESSFUL;
	PTHREAD_INFORMATION pi = (PTHREAD_INFORMATION)OutputBuffer;
	UINT32 Count = (OutlengthSize - sizeof(PTHREAD_INFORMATION)) / sizeof(PTHREAD_INFORMATION_ENTRY);
	PEPROCESS Eprocess = NULL;

	st = PsLookupProcessByProcessId((HANDLE)ProcessId, &Eprocess);
	if (NT_SUCCESS(st))
	{
		st = GetThreadListHead(Eprocess, pi, Count);
		if (NT_SUCCESS(st))
		{
			if (Count >= pi->NumberOfThreads)
			{
				st = STATUS_SUCCESS;
			}
			else
			{
				st = STATUS_BUFFER_TOO_SMALL;
			}
		}
	}
	return st;
}

#endif

NTSTATUS TerminateKernelModeThread(IN HANDLE Tid)
{
	NTSTATUS st = STATUS_UNSUCCESSFUL;
	PETHREAD Ethread = NULL;

	st = PsLookupThreadByThreadId((HANDLE)Tid, &Ethread);
	if (NT_SUCCESS(st))
	{
		st = PspTerminateThreadByPointer(Ethread, 0, TRUE);
		if (NT_SUCCESS(st))
		{
			st = STATUS_SUCCESS;
		}
		ObfDereferenceObject(Ethread);
	}

	return st;
}


#ifndef _WIN64

NTSTATUS GetKernelThreadInformation(IN HANDLE ProcessId, OUT PVOID OutputBuffer, IN ULONG OutlengthSize)
{
	NTSTATUS st;
	ULONG Size = (OutlengthSize - sizeof(PTHREAD_INFORMATION)) / sizeof(PTHREAD_INFORMATION_ENTRY);
	PSYSTEM_PROCESS_INFORMATION sysproc = NULL;
	PTHREAD_INFORMATION info = (PTHREAD_INFORMATION)OutputBuffer;
	ULONG bytes = 0, i = 0;
	UNICODE_STRING func = { 0 };
	UNICODE_STRING ustr1 = { 0 };
	PVOID buffer = NULL;
	PETHREAD Ethread = NULL;
	OBJECT_ATTRIBUTES oa;
	CLIENT_ID cid;
	HANDLE handle = 0;
	LARGE_INTEGER CreateTime;
	TIME_FIELDS time, ktime, utime;
	LARGE_INTEGER systime, localtime;


	UINT32 CreateTimeOffset = 0x200;
	UINT32 StartAddressOffset = 0x218;
	UINT32 Win32StartAddressOffset = 0x260;
	UINT32 StateOffset = 0x068;
	UINT32 TebOffset = 0x088;



	RtlInitUnicodeString(&ustr1, L"ZwOpenThread");
	ZwOpenThread = (fnZwOpenThread)MmGetSystemRoutineAddress(&ustr1);
	if (ZwOpenThread == NULL)
	{
		return NULL;
	}


	RtlInitUnicodeString(&func, L"ZwQuerySystemInformation");
	ZwQuerySystemInformation = (fnZwQuerySystemInformation)MmGetSystemRoutineAddress(&func);
	if (ZwQuerySystemInformation && MmIsAddressValid(ZwQuerySystemInformation))
	{

		st = ZwQuerySystemInformation(SystemProcessInformation, NULL, 0, &bytes);
		if (st == STATUS_INFO_LENGTH_MISMATCH)
		{

			buffer = ExAllocatePool(NonPagedPool, bytes);
			if (buffer)
			{

				st = ZwQuerySystemInformation(SystemProcessInformation, buffer, bytes, &bytes);
				if (NT_SUCCESS(st))
				{
					sysproc = (PSYSTEM_PROCESS_INFORMATION)buffer;
					if (sysproc && MmIsAddressValid(sysproc))
					{
						while (sysproc->NextEntryOffset)
						{
							sysproc = (PSYSTEM_PROCESS_INFORMATION)(((PUCHAR)sysproc) + sysproc->NextEntryOffset);

							if (sysproc->ProcessId == (HANDLE)ProcessId)
							{
								for (i = 0; i < sysproc->NumberOfThreads; i++)
								{
									ULONG usermode = info->NumberOfThreads;

									cid.UniqueProcess = (HANDLE)0;
									cid.UniqueThread = sysproc->Threads[i].ClientId.UniqueThread;
									InitializeObjectAttributes(&oa, NULL, NULL, NULL, NULL);
									st = ZwOpenThread(&handle, GENERIC_ALL, &oa, &cid);
									if (NT_SUCCESS(st))
									{
										st = ObReferenceObjectByHandle(handle, GENERIC_ALL, *PsThreadType, KernelMode, &Ethread, NULL);
										if (NT_SUCCESS(st))
										{

											info->Entry[usermode].Ethread = (UINT32)Ethread;
											info->Entry[usermode].Teb = *(PUINT32)((UINT32)Ethread + TebOffset);
											info->Entry[usermode].StartAddress = *(PUINT32)((UINT32)Ethread + StartAddressOffset);
											info->Entry[usermode].Win32StartAddress = *(PUINT32)((UINT32)Ethread + Win32StartAddressOffset);
											info->Entry[usermode].ThreadId = PsGetThreadId(Ethread);
											info->Entry[usermode].State = sysproc->Threads[i].State;


											CreateTime = *(LARGE_INTEGER*)((UINT32)Ethread + CreateTimeOffset);
											RtlTimeToTimeFields(&CreateTime, &time);
											RtlStringCchPrintfW(info->Entry[usermode].CreateTime, MAX_PATH, TIME_FORMAT_WZPREFIX, 
												time.Day,
												time.Month, 
												time.Year,
												time.Hour + 2, 
												time.Minute,
												time.Second,
												time.Milliseconds);


											ObfDereferenceObject(Ethread);
										}
										ZwClose(handle);
									}

									info->NumberOfThreads++;
								}
							}

						}
					}
				}
				ExFreePool(buffer);

			}
		}
	}
	return st;
}

#endif