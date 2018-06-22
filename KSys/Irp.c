#include "driver.h"
#include "Irp.h"


NTSTATUS IrpCompleteRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp, IN PVOID Context) 
{
	KeSetEvent(Irp->UserEvent, IO_NO_INCREMENT, FALSE);

	IoFreeIrp(Irp);

	return STATUS_MORE_PROCESSING_REQUIRED;
}

NTSTATUS BorrarArchivoViaIrp(IN WCHAR *RutaArchivo) 
{
	NTSTATUS st = STATUS_UNSUCCESSFUL;
	UNICODE_STRING ustr1;
	OBJECT_ATTRIBUTES ObjectAttributes;
	IO_STATUS_BLOCK IoStatus;
	HANDLE handle = 0;
	PFILE_OBJECT FileObject = NULL;
	PDEVICE_OBJECT DeviceObject = NULL;
	PIRP Irp = NULL;
	KEVENT Event = { 0 };
	PIO_STACK_LOCATION pio = NULL;
	FILE_DISPOSITION_INFORMATION fileinfo = { 0 };
	WCHAR wzFile[MAX_PATH] = L"\\??\\";

	RtlStringCchCatW(wzFile, sizeof(wzFile), RutaArchivo);
	RtlInitUnicodeString(&ustr1, wzFile);
	InitializeObjectAttributes(&ObjectAttributes, &ustr1, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	st = IopCreateFile(&handle, FILE_READ_ATTRIBUTES, &ObjectAttributes, &IoStatus, 0, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_DELETE,
		FILE_OPEN, FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0, CreateFileTypeNone, NULL, IO_NO_PARAMETER_CHECKING,0,NULL);
	if (NT_SUCCESS(st)) 
	{
		st = ObReferenceObjectByHandle(handle, DELETE, *IoFileObjectType, KernelMode, (PVOID*)&FileObject, NULL);
		if (NT_SUCCESS(st)) 
		{
			DeviceObject = IoGetRelatedDeviceObject(FileObject);
			if (DeviceObject && MmIsAddressValid(DeviceObject)) 
			{
				Irp = IoAllocateIrp(DeviceObject->StackSize, TRUE);
				if (Irp) 
				{
					fileinfo.DeleteFile = TRUE;

		
					KeInitializeEvent(&Event, SynchronizationEvent, FALSE);
					Irp->AssociatedIrp.SystemBuffer = &fileinfo;
					Irp->UserEvent = &Event;
					Irp->UserIosb = &IoStatus;
					Irp->Tail.Overlay.OriginalFileObject = FileObject;
					Irp->Tail.Overlay.Thread = KeGetCurrentThread();
					Irp->RequestorMode = KernelMode;

					pio = IoGetNextIrpStackLocation(Irp);
					pio->DeviceObject = DeviceObject;
					pio->FileObject = FileObject;
					pio->MajorFunction = IRP_MJ_SET_INFORMATION;
					pio->Parameters.SetFile.Length = sizeof(FILE_DISPOSITION_INFORMATION);
					pio->Parameters.SetFile.FileInformationClass = FileDispositionInformation;
				
					
					IoSetCompletionRoutine(Irp, IrpCompleteRoutine, &Event, TRUE,TRUE,TRUE);
					
					st = IofCallDriver(DeviceObject, Irp);
					if (st == STATUS_PENDING) 
					{
						KeWaitForSingleObject(&Event, Executive, KernelMode, TRUE, NULL);

					}

					st = STATUS_SUCCESS;

				}

			}
			ObfDereferenceObject(FileObject);
		}
		ZwClose(handle);
		handle = NULL;
	}


	return st;
}

NTSTATUS IopGetFileInformationInternal(IN PFILE_OBJECT FileObject, IN ULONG Length,IN FILE_INFORMATION_CLASS FileInfoClass,OUT PVOID Buffer,OUT PULONG ReturnedLength)
{
	PIRP Irp;
	NTSTATUS st;
	KEVENT Event;
	PIO_STACK_LOCATION pio = NULL;
	PDEVICE_OBJECT DeviceObject = NULL;
	IO_STATUS_BLOCK io;


	ObfReferenceObject(FileObject);

	DeviceObject = IoGetRelatedDeviceObject(FileObject);


	Irp = IoAllocateIrp(DeviceObject->StackSize, FALSE);
	if (!Irp)
	{
		ObfDereferenceObject(FileObject);
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	KeInitializeEvent(&Event, SynchronizationEvent, FALSE);


	Irp->UserIosb = &io;
	Irp->UserEvent = &Event;
	Irp->Overlay.AsynchronousParameters.UserApcContext = NULL;
	Irp->RequestorMode = KernelMode;
	Irp->AssociatedIrp.SystemBuffer = Buffer;
	Irp->Flags = IRP_SYNCHRONOUS_API | IRP_BUFFERED_IO | IRP_OB_QUERY_NAME;
	Irp->Tail.Overlay.OriginalFileObject = FileObject;
	Irp->Tail.Overlay.Thread = PsGetCurrentThread();

	pio = IoGetNextIrpStackLocation(Irp);
	pio->MajorFunction = IRP_MJ_QUERY_INFORMATION;
	pio->FileObject = FileObject;
	pio->Parameters.QueryFile.FileInformationClass = FileInfoClass;
	pio->Parameters.QueryFile.Length = Length;
	
	IoQueueThreadIrp(Irp);
	
	st = IofCallDriver(DeviceObject, Irp);
	if (st == STATUS_PENDING) 
	{
		KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);
		st = io.Status;
	}


	*ReturnedLength = io.Information;

	return st;
}


NTSTATUS GetKernelFileInformation(IN PWCHAR File, OUT PVOID OutputBuffer, IN ULONG Outlengthsize)
{
	NTSTATUS st = STATUS_UNSUCCESSFUL;
	UNICODE_STRING file;
	HANDLE handle = 0;
	OBJECT_ATTRIBUTES oa;
	PFILE_OBJECT FileObject = NULL;
	IO_STATUS_BLOCK io;
	PFILE_ALL_INFORMATION fileallinfo = NULL;
	WCHAR wzPrefix[MAX_PATH];
	ULONG bytesreturned = 0;
	PFILE_KERNEL_INFO kernelinfo = (PFILE_KERNEL_INFO)OutputBuffer;


	RtlStringCchCopyW(wzPrefix, MAX_PATH, L"\\??\\");
	RtlStringCchCatW(wzPrefix, MAX_PATH, File);

	RtlSecureZeroMemory(&file, sizeof(file));
	RtlInitUnicodeStringEx(&file, wzPrefix);
	InitializeObjectAttributes(&oa, &file, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	


	st = IopCreateFile(&handle, FILE_GENERIC_READ, &oa, &io, 0, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN, FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
		NULL, 0, CreateFileTypeNone, NULL, IO_NO_PARAMETER_CHECKING, 0, NULL);
	if (NT_SUCCESS(st)) 
	{
		st = ObReferenceObjectByHandle(handle, FILE_READ_DATA, *IoFileObjectType, KernelMode, (PVOID*)&FileObject, NULL);
		if (NT_SUCCESS(st)) 
		{
			fileallinfo = (PFILE_ALL_INFORMATION)ExAllocatePool(NonPagedPool, 0x1F4);
			if (fileallinfo != NULL) 
			{
				st = IopGetFileInformationInternal(FileObject, 0x1F4, FileAllInformation, fileallinfo, &bytesreturned);
				if (NT_SUCCESS(st)) 
				{
						kernelinfo->FileBytes = fileallinfo->StandardInformation.EndOfFile.QuadPart;
						kernelinfo->DeleteAccess = FileObject->DeleteAccess;
						kernelinfo->ReadAccess = FileObject->ReadAccess;
						kernelinfo->WriteAccess = FileObject->WriteAccess;
						kernelinfo->FsContext = FileObject->FsContext;
						kernelinfo->AccessMask = fileallinfo->AccessInformation.AccessFlags;
						kernelinfo->CurrentByteOffset = fileallinfo->PositionInformation.CurrentByteOffset.QuadPart;

						st = STATUS_SUCCESS;

				}
				ExFreePool(fileallinfo);
			}
			ObfDereferenceObject(FileObject);

		}
		ZwClose(handle);
		handle = NULL;
		
	}

	return st;
}