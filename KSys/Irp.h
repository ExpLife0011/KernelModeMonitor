#pragma once

typedef struct _FILE_KERNEL_INFO
{
	ULONG FileBytes;
	BOOLEAN DeleteAccess;
	BOOLEAN ReadAccess;
	BOOLEAN WriteAccess;
	PVOID FsContext;
	ULONG AccessMask;
	LONGLONG CurrentByteOffset;
}FILE_KERNEL_INFO, *PFILE_KERNEL_INFO;

NTSTATUS BorrarArchivoViaIrp(IN WCHAR *RutaArchivo);

NTSTATUS IrpCompleteRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp, IN PVOID Context);

NTSTATUS GetKernelFileInformation(IN PWCHAR File, OUT PVOID OutputBuffer, IN ULONG Outlengthsize);

NTSTATUS IopGetFileInformationInternal(IN PFILE_OBJECT FileObject, IN ULONG Length,IN FILE_INFORMATION_CLASS FileInfoClass,OUT PVOID Buffer,OUT PULONG ReturnedLength);

