#include "driver.h"
#include "vol.h"

/*////////////////////////////////////////
/
/	File: vol.c
/
/	Goal: Gather volume information
/	via IRP
/
/
*/////////////////////////////////////////

NTSTATUS IrpQueryVolumeInformation(IN HANDLE handle, IN FS_INFORMATION_CLASS FileInformationClass, OUT PVOID Buffer, IN ULONG Length, OUT PULONG ReturnedLength)
{
	NTSTATUS st;
	PDEVICE_OBJECT DeviceObject = NULL;
	PFILE_OBJECT FileObject = NULL;
	PIRP Irp;
	PIO_STACK_LOCATION pio;
	KEVENT Event;
	IO_STATUS_BLOCK io;

	st = ObReferenceObjectByHandle(handle, GENERIC_ALL, NULL, KernelMode, &FileObject, NULL);
	if (NT_SUCCESS(st))
	{
		DeviceObject = IoGetRelatedDeviceObject(FileObject);
		
		Irp = IoAllocateIrp(DeviceObject->StackSize, FALSE);
		if (Irp == NULL)
		{
			ObDereferenceObject(FileObject);
			return STATUS_INSUFFICIENT_RESOURCES;
		}

		KeInitializeEvent(&Event, NotificationEvent, FALSE);

		Irp->AssociatedIrp.SystemBuffer = Buffer;
		Irp->UserBuffer = Buffer;
		Irp->UserIosb = &io;
		Irp->UserEvent = &Event;
		Irp->RequestorMode = KernelMode;
		Irp->Tail.Overlay.OriginalFileObject = FileObject;
		Irp->Tail.Overlay.Thread = (PETHREAD)KeGetCurrentThread();

		pio = IoGetNextIrpStackLocation(Irp);
		pio->Parameters.QueryVolume.FsInformationClass = FileInformationClass;
		pio->Parameters.QueryVolume.Length = Length;
		pio->DeviceObject = DeviceObject;
		pio->FileObject = FileObject;
		pio->MajorFunction = IRP_MJ_QUERY_VOLUME_INFORMATION;

		IoSetCompletionRoutine(Irp, QueryCompletion, &Event, TRUE, TRUE, TRUE);

		IoCallDriver(DeviceObject, Irp);

		KeWaitForSingleObject(&Event, Executive, KernelMode, TRUE, NULL);

		ObDereferenceObject(FileObject);

		st = io.Status;

		*ReturnedLength = io.Information;
	}

	return st;
}


NTSTATUS NTFSLookupVolumeByVolumeId(IN PWCHAR Volume, OUT PVOID OutputBuffer, IN ULONG Outlength)
{
	NTSTATUS st;
	UNICODE_STRING ustr1, linker;
	OBJECT_ATTRIBUTES oa;
	WCHAR Buffer[MAX_PATH];
	HANDLE handle, Symhandle;
	PFILE_FS_FULL_SIZE_INFORMATION size = NULL;
	PFILE_FS_VOLUME_INFORMATION control = NULL;
	ULONG sizeofbuffer = (Outlength - sizeof(PVOL_INFO));
	IO_STATUS_BLOCK io;
	ULONG bytes = 0;
	TIME_FIELDS timer;

	
	wcscpy_s(Buffer, MAX_PATH, L"\\GLOBAL??\\");
	wcscat_s(Buffer, MAX_PATH, Volume);

	RtlInitUnicodeStringEx(&ustr1, Buffer);
	InitializeObjectAttributes(&oa, &ustr1, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	// call internal dynamic function

	st = IopCreateFile(&handle, FILE_GENERIC_READ, &oa, &io, 0, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | 
		FILE_SHARE_DELETE | FILE_SHARE_WRITE, FILE_OPEN, FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
		NULL, 0, CreateFileTypeNone, NULL, IO_NO_PARAMETER_CHECKING, 0, NULL);
	if (NT_SUCCESS(st))
	{
		
		control = (PFILE_FS_VOLUME_INFORMATION)ExAllocatePool(NonPagedPool, 0x500);
		if (control != NULL)
		{
			st = IrpQueryVolumeInformation(handle, FileFsVolumeInformation, control, 0x500, &bytes);
			if (NT_SUCCESS(st))
			{
				size = (PFILE_FS_FULL_SIZE_INFORMATION)ExAllocatePool(NonPagedPool, 0x500);
				if (size)
				{
					st = IrpQueryVolumeInformation(handle, FileFsFullSizeInformation, size, 0x500, &bytes);
					if (NT_SUCCESS(st))
					{
						PVOL_INFO info = (PVOL_INFO)OutputBuffer;
						if (info && MmIsAddressValid(info))
						{
							info->ActualAvailableAllocationUnits = size->ActualAvailableAllocationUnits;
							info->BytesPerSector = size->BytesPerSector;
							info->CallerAvailableAllocationUnits = size->CallerAvailableAllocationUnits;
							info->SectorsPerAllocationUnit = size->SectorsPerAllocationUnit;
							info->TotalAllocationUnits = size->TotalAllocationUnits;
							info->SupportsObjects = control->SupportsObjects;
							info->VolumeSerialNumber = control->VolumeSerialNumber;

							RtlTimeToTimeFields(&control->VolumeCreationTime, &timer);
							RtlStringCchPrintfW(info->VolumeCreationTime, MAX_PATH, L"%02u/%02u/%04u %02u:%02u:%02u:%03u", 
								timer.Day, timer.Month, timer.Year, timer.Hour, timer.Minute, timer.Second, timer.Milliseconds);

							st = ZwOpenSymbolicLinkObject(&Symhandle, SYMBOLIC_LINK_ALL_ACCESS, &oa);
							if (NT_SUCCESS(st))
							{
								linker.Length = 0;
								linker.MaximumLength = 256;
								linker.Buffer = (PCWCH)ExAllocatePool(NonPagedPool, linker.MaximumLength);
								st = ZwQuerySymbolicLinkObject(Symhandle, &linker, &bytes);
								if (NT_SUCCESS(st))
								{
									RtlCopyMemory(info->SymbolicName, linker.Buffer, linker.MaximumLength);

									st = STATUS_SUCCESS;
								}
								ZwClose(Symhandle);
							}

						}
					}
					ExFreePool(size);
				}
			}
			ExFreePool(control);
		}
		ZwClose(handle);		
	}

	return st;
}