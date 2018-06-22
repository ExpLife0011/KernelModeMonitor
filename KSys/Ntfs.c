#include "driver.h"
#include "Ntfs.h"


/*///////////////////////////////////////
/	Function:  QueryCompletion
/
/	Purpose: Complete IRP callback for
/	IoSetCompletionRoutine routine
/
/	Environment:
/		Kernel Mode only
/
*///////////////////////////////////////

NTSTATUS QueryCompletion(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp,IN PVOID Context)
{
	PIO_STATUS_BLOCK ioStatus;

	ioStatus = Irp->UserIosb;
	ioStatus->Status = Irp->IoStatus.Status;
	ioStatus->Information = Irp->IoStatus.Information;

	KeSetEvent(Irp->UserEvent, 0, FALSE);
	IoFreeIrp(Irp);

	return STATUS_MORE_PROCESSING_REQUIRED;
}

/*///////////////////////////////////////
/	Function: NTFSLookupDirectoryByDirectoryId
/
/	Purpose: Traverse system internal 
/		directory linked list emulating
/		NtQueryDirectoryFile routine
/
/	Environment:
/		Kernel Mode only
/
/	Remarks:
/		Detect hidden files 
/		
*///////////////////////////////////////

NTSTATUS NTFSLookupDirectoryByDirectoryId(IN PWCHAR pszDirectory, OUT PVOID OutputBuffer, IN ULONG Outlengthsize)
{
	NTSTATUS ntStatus;
	WCHAR Buffer[260] = { 0 };
	OBJECT_ATTRIBUTES ObAttributes;
	UNICODE_STRING FileName;
	HANDLE FileHandle;
	IO_STATUS_BLOCK ioStatus;
	PFILE_OBJECT FileObject;
	PDEVICE_OBJECT FileDevice;
	PIRP NewIrp = NULL;
	PIO_STACK_LOCATION IrpSp;
	KEVENT waitEvent;
	PVOID FileUserBuffer;
	PFILE_BOTH_DIR_INFORMATION FileDirectory;
	TIME_FIELDS timer;
	ULONG size = (Outlengthsize - sizeof(FILE_INFORMATION)) / sizeof(FILE_INFORMATION_ENTRY);
	PFILE_INFORMATION fileinfo = (PFILE_INFORMATION)OutputBuffer;

	RtlSecureZeroMemory(&FileName, sizeof(FileName));

	wcscpy_s(Buffer, 260, L"\\GLOBAL??\\");
	wcscat_s(Buffer, 260, pszDirectory);

	RtlInitUnicodeString(&FileName, Buffer);
	InitializeObjectAttributes(&ObAttributes, &FileName, OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE, NULL, NULL);
	
	ntStatus = IopCreateFile(&FileHandle, FILE_LIST_DIRECTORY | FILE_ANY_ACCESS | SYNCHRONIZE, &ObAttributes, &ioStatus, 0, FILE_ATTRIBUTE_NORMAL, 
		FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE, FILE_OPEN, FILE_DIRECTORY_FILE |
		FILE_SYNCHRONOUS_IO_ALERT, NULL, 0, CreateFileTypeNone, NULL, IO_NO_PARAMETER_CHECKING, 0, NULL);
	if (NT_SUCCESS(ntStatus))
	{
		ntStatus = ObReferenceObjectByHandle(FileHandle, FILE_LIST_DIRECTORY | SYNCHRONIZE, *IoFileObjectType, KernelMode, (PVOID*)&FileObject, NULL);
		if (NT_SUCCESS(ntStatus))
		{
			FileDevice = IoGetRelatedDeviceObject(FileObject);
			if (FileDevice)
			{
				NewIrp = IoAllocateIrp(FileDevice->StackSize, FALSE);
				if (NewIrp != NULL)
				{
					KeInitializeEvent(&waitEvent, NotificationEvent, FALSE);
					FileUserBuffer = ExAllocatePoolWithTag(NonPagedPool, 655350, MEM_TAG);

					NewIrp->UserEvent = &waitEvent;
					NewIrp->UserBuffer = FileUserBuffer;
					NewIrp->AssociatedIrp.SystemBuffer = FileUserBuffer;
					NewIrp->MdlAddress = NULL;
					NewIrp->Flags = 0;
					NewIrp->UserIosb = &ioStatus;
					NewIrp->Tail.Overlay.OriginalFileObject = FileObject;
					NewIrp->Tail.Overlay.Thread = PsGetCurrentThread();
					NewIrp->RequestorMode = KernelMode;

					IrpSp = IoGetNextIrpStackLocation(NewIrp);
					IrpSp->MajorFunction = IRP_MJ_DIRECTORY_CONTROL;
					IrpSp->MinorFunction = IRP_MN_QUERY_DIRECTORY;
					IrpSp->FileObject = FileObject;
					IrpSp->DeviceObject = FileDevice;
					IrpSp->Flags = SL_RESTART_SCAN;
					IrpSp->Control = 0;
					IrpSp->Parameters.QueryDirectory.FileIndex = 0;
					IrpSp->Parameters.QueryDirectory.FileInformationClass = FileBothDirectoryInformation;
					IrpSp->Parameters.QueryDirectory.FileName = NULL;
					IrpSp->Parameters.QueryDirectory.Length = 655350;

					IoSetCompletionRoutine(NewIrp, QueryCompletion, NULL, TRUE, TRUE, TRUE);
					ntStatus = IoCallDriver(FileDevice, NewIrp);
					if (ntStatus == STATUS_PENDING)
					{
						KeWaitForSingleObject(&waitEvent, Executive, KernelMode, TRUE, NULL);
					}
					
					FileDirectory = (PFILE_BOTH_DIR_INFORMATION)FileUserBuffer;
					if (!FileDirectory || MmIsAddressValid(FileDirectory) == FALSE)
						return STATUS_UNSUCCESSFUL;

					while (TRUE)
					{


						ULONG contador = fileinfo->NumberOfEntries;


						if ((FileDirectory->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (FileDirectory->FileName)[0] == L'.')  
							goto exit;
						

							RtlTimeToTimeFields(&FileDirectory->CreationTime, &timer);
							RtlStringCchPrintfW(fileinfo->Entry[contador].CreationTime, MAX_PATH, L"%02u/%02u/%04u %02u:%02u:%02u:%03u", timer.Day, timer.Month, timer.Year, timer.Hour, timer.Minute, timer.Second, timer.Milliseconds);

							RtlTimeToTimeFields(&FileDirectory->ChangeTime, &timer);
							RtlStringCchPrintfW(fileinfo->Entry[contador].ChangeTime, MAX_PATH, L"%02u/%02u/%04u %02u:%02u:%02u:%03u", timer.Day, timer.Month, timer.Year, timer.Hour, timer.Minute, timer.Second, timer.Milliseconds);

							RtlTimeToTimeFields(&FileDirectory->LastWriteTime, &timer);
							RtlStringCchPrintfW(fileinfo->Entry[contador].WriteTime, MAX_PATH, L"%02u/%02u/%04u %02u:%02u:%02u:%03u", timer.Day, timer.Month, timer.Year, timer.Hour, timer.Minute, timer.Second, timer.Milliseconds);

							RtlCopyMemory(fileinfo->Entry[contador].DirectoryName, FileDirectory->FileName, FileDirectory->FileNameLength);

							fileinfo->Entry[contador].EndOfFile = FileDirectory->EndOfFile;
							fileinfo->Entry[contador].AllocationSize = FileDirectory->AllocationSize;
							fileinfo->NumberOfEntries++;
						

					exit:
						if (FileDirectory->NextEntryOffset == 0)  
							break;

						FileDirectory = (PFILE_BOTH_DIR_INFORMATION)((PUCHAR)FileDirectory + FileDirectory->NextEntryOffset);
					}

					ExFreePoolWithTag(FileUserBuffer, MEM_TAG);
				}
			}
			ObDereferenceObject(FileObject);
		}
		ZwClose(FileHandle);
	}

	return ntStatus;
}

