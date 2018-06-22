#include "driver.h"
#include "Km.h"



/******************************************************
/	Funcion: driverAbrirCanal
/
/	Objetivo:
/		IRP_MJ_CREATE: abre canal para
/		poder comunicarnos con el driver
/
*******************************************************/


NTSTATUS driverAbrirCanal(PDEVICE_OBJECT pdeviceObject, IRP *Irp) 
{
	DbgPrint("\n[IRP_MJ_CREATE] Device opened");
	NTSTATUS st = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = st;
	IofCompleteRequest(Irp, IO_NO_INCREMENT);
	return st;

}


/******************************************************
/	Funcion: driverCerrarCanal
/
/	Objetivo:
/		IRP_MJ_CLOSE: cierra canal
/
*******************************************************/

NTSTATUS driverCerrarCanal(PDEVICE_OBJECT pdeviceObject, IRP *Irp) 
{
	DbgPrint("\n[IRP_MJ_CLOSE] Device closed");
	NTSTATUS st = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = st;
	IofCompleteRequest(Irp, IO_NO_INCREMENT);
	return st;

}


/******************************************************
/	Funcion: driverControlarComunicacion
/
/	Objetivo:
/		''Parsing'' del I/O con el usuario
/
*******************************************************/

NTSTATUS driverControlarComunicacion(PDEVICE_OBJECT pdeviceObject, IRP *Irp) 
{
	IO_STACK_LOCATION *StackLocation = Irp->Tail.Overlay.CurrentStackLocation;
	NTSTATUS st = STATUS_SUCCESS;

	ULONG control = StackLocation->Parameters.DeviceIoControl.IoControlCode;
	ULONG Inlength = StackLocation->Parameters.DeviceIoControl.InputBufferLength;
	ULONG OutlengthSize = StackLocation->Parameters.DeviceIoControl.OutputBufferLength;
	PVOID InBuffer = NULL;
	PVOID OutBuffer = NULL;

	InBuffer = StackLocation->Parameters.DeviceIoControl.Type3InputBuffer;
	OutBuffer = Irp->UserBuffer;


	switch (control) 
	{

#ifndef _WIN64

	case HOOK_REG_BORRAR_LLAVE:
		__try 
		{

			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			UserModeBuffer = InBuffer;
			RtlStringCchCopyW(UserHelperVariable, 0xFF, UserModeBuffer);
			Funcion_Hookeada(ZwDeleteKey, HookZwDeleteKey);
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = st;

		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}	

		break;
	case HOOK_REG_BORRAR_VALORES:
		__try 
		{

			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			UserModeBuffer = InBuffer;
			RtlStringCchCopyW(UserHelperVariable, 0xFF, UserModeBuffer);
			Funcion_Hookeada(ZwDeleteValueKey, HookZwDeleteValueKey);
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = st;

		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}

		break;

	case HOOK_REG_INTRODUCIR_VALORES:
		__try 
		{

			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			UserModeBuffer = InBuffer;
			RtlStringCchCopyW(UserHelperVariable, 0xFF, UserModeBuffer);
			Funcion_Hookeada(ZwSetValueKey, HookZwSetValueKey);
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = st;

		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}

		break;
	case HOOK_REG_VER_VALORES:
		__try 
		{

			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			UserModeBuffer = InBuffer;
			RtlStringCchCopyW(UserHelperVariable, 0xFF, UserModeBuffer);
			Funcion_Hookeada(ZwQueryValueKey, HookZwQueryValueKey);
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = st;

		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}

		break;
	case HOOK_REG_ESCONDER:
		__try 
		{

			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			UserModeBuffer = InBuffer;
			RtlStringCchCopyW(UserHelperVariable, 0xFF, UserModeBuffer);
			Funcion_Hookeada(ZwEnumerateKey, HookZwEnumerateKey);
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = st;

		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}

		break;

	case DESHACER_HOOKS_REGISTRO:
		DesProtegerRegistro();
		break;

	case HOOK_BORRAR_DRIVER:
		__try 
		{

			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			UserModeBuffer = InBuffer;
			RtlStringCbCopyW(UserHelperVariable, 0xFF, UserModeBuffer);
			Funcion_Hookeada(ZwSetInformationFile, HookZwSetInformationFileDriver);
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = st;


		}
		__except (EXCEPTION_EXECUTE_HANDLER) 
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}

		break;
	case HOOK_ACCESO_DIR:
		__try 
		{

			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			UserModeBuffer = InBuffer;
			RtlStringCbCopyW(UserHelperVariable, 0xFF, UserModeBuffer);
			Funcion_Hookeada(ZwCreateFile, HookZwCreateFile);
			Funcion_Hookeada(ZwSetSecurityObject, HookZwSetSecurityObject);
			Funcion_Hookeada(ZwQuerySecurityObject, HookZwQuerySecurityObject);
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = st;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) 
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}
		break;

	case HOOK_ACCESO_DEVICE:
		__try 
		{

			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			UserModeBuffer = InBuffer;
			RtlStringCbCopyW(UserHelperVariable, 0xFF, UserModeBuffer);
			Funcion_Hookeada(ZwOpenFile, HookZwOpenFile);
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = st;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) 
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}

		break;

	case DESHACER_HOOKS_DIR:
		DesprotegerDirectorioAcls();
		break;
	case HOOK_LECTURA_ESCRITURA:
		__try 
		{
			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			UserModeBuffer = InBuffer;
			RtlStringCbCopyW(UserHelperVariable, 0xFF, UserModeBuffer);
			Funcion_Hookeada(ZwReadFile, HookZwReadFile);
			Funcion_Hookeada(ZwWriteFile, HookZwWriteFile);
			Funcion_Hookeada(ZwQueryInformationFile, HookZwQueryInformationFile);
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = st;

		}
		__except (EXCEPTION_EXECUTE_HANDLER) 
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}

		break;

	case HOOK_OPEN_DEVICES:
		__try 
		{
			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			UserModeBuffer = InBuffer;
			RtlStringCbCopyW(UserHelperVariable, 0xFF, UserModeBuffer);
			Funcion_Hookeada(ZwOpenDirectoryObject, HookZwOpenDirectoryObject);
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = st;


		}
		__except (EXCEPTION_EXECUTE_HANDLER) 
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}
		break;
	case DESHACER_HOOK_OBJETOS:
		DeshacerProteccionesObjetos();
		break;
	case HOOK_LINK_SIMBOLICO:	
		__try 
		{
			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			UserModeBuffer = InBuffer;
			RtlStringCbCopyW(UserHelperVariable, 0xFF, UserModeBuffer);
			Funcion_Hookeada(ZwOpenSymbolicLinkObject, HookZwOpenSymbolicLinkObject);
			Irp->IoStatus.Status = st;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) 
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}

		break;

#endif _WIN64

	case NTFS_SCANNER:

		if (!MmIsAddressValid(OutBuffer)) 
		{
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			break;
		}

		__try 
		{

			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			ProbeForWrite(OutBuffer, OutlengthSize, sizeof(PVOID));
			
			st = NTFSLookupDirectoryByDirectoryId(InBuffer, OutBuffer, OutlengthSize);
			
			Irp->IoStatus.Information = OutlengthSize;
			Irp->IoStatus.Status = st;

		}
		__except (EXCEPTION_EXECUTE_HANDLER) 
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}

		break;

	case VOLUME_INFORMATION:

		if (!MmIsAddressValid(OutBuffer))
		{
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			break;
		}

		__try 
		{
			
			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));		
			st = NTFSLookupVolumeByVolumeId(InBuffer, OutBuffer, OutlengthSize);
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = st;

		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}

		break;


	case BORRAR_VIA_IRP:

		__try 
		{
			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			st = BorrarArchivoViaIrp(InBuffer);
			Irp->IoStatus.Status = st;

		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}


		break;

	case SUSPENDER_PROCESO:
		__try 
		{
			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			st = KernelModeSuspendProcess(*(PHANDLE)InBuffer);
			Irp->IoStatus.Status = st;

		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}


		break;

	case RESUMIR_PROCESO:

		__try 
		{
			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			st = KernelModeResumeProcess(*(PHANDLE)InBuffer);
			Irp->IoStatus.Status = st;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) 
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}


		break;
	case QUITAR_OFFSET_LIST_ENTRY:

		__try 
		{
			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			st = HideProcessByProcessId(*(PHANDLE)InBuffer);
			Irp->IoStatus.Status = st;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) 
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}

		break;
	case TERMINAR_PROCESO:

		__try
		{
			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			st = KernelModeTerminateProcess(*(PHANDLE)InBuffer);
			Irp->IoStatus.Status = st;

		}
		__except (EXCEPTION_EXECUTE_HANDLER) 
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}
		break;	

	case KERNEL_PROCESS_EXPLORER:

		if (!MmIsAddressValid(OutBuffer)) 
		{
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			break;
		}

		__try 
		{
			ProbeForWrite(OutBuffer, OutlengthSize, sizeof(PVOID));
			st = GetKernelProcessInformation(OutBuffer, OutlengthSize);
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = st;

		}
		__except (EXCEPTION_EXECUTE_HANDLER) 
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}

		break;

	case MEMORY_INFORMATION:

		if (!MmIsAddressValid(OutBuffer)) 
		{
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			break;
		}
		__try 
		{
			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			ProbeForWrite(OutBuffer, OutlengthSize, sizeof(PVOID));
			st = GetProcessMemoryInformationByProcessId(*(PHANDLE)InBuffer, OutBuffer, OutlengthSize);
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = st;

		}
		__except (EXCEPTION_EXECUTE_HANDLER) 
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}

		break;

	case KERNEL_REG_MON:

		if (!MmIsAddressValid(OutBuffer)) 
		{
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			break;
		}

		__try 
		{
			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			ProbeForWrite(OutBuffer, OutlengthSize, sizeof(PVOID));
			st = GetKernelRegInformation(InBuffer, OutBuffer, OutlengthSize);
			Irp->IoStatus.Status = st;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}

		break;

	case GET_REG_VALUE_INFORMATION:

		if (!MmIsAddressValid(OutBuffer))
		{
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			break;
		}

		__try 
		{
			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			ProbeForWrite(OutBuffer, OutlengthSize, sizeof(PVOID));
			st = GetKernelValueEnumeration(InBuffer, OutBuffer, OutlengthSize);
			Irp->IoStatus.Status = st;

		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}

		break;

	case KERNEL_OBJECT_MANAGER:

		if (!MmIsAddressValid(OutBuffer)) 
		{
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			break;
		}

		__try 
		{
			ProbeForWrite(OutBuffer, OutlengthSize, sizeof(PVOID));
			GetKernelDriverObjectInformation(OutBuffer, OutlengthSize);
			Irp->IoStatus.Status = st;

		}
		__except (EXCEPTION_EXECUTE_HANDLER) 
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}

		break;

	case KERNEL_DRIVER_LIST:

		if (!MmIsAddressValid(OutBuffer)) 
		{
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			break;
		}

		__try 
		{
			ProbeForWrite(OutBuffer, OutlengthSize, sizeof(PVOID));			
			st = EnumDriversByQueryInformation(OutBuffer, OutlengthSize);
			Irp->IoStatus.Status = st;

		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}


		break;

	case GET_KERNEL_FILE_INFORMATION:

		if (!MmIsAddressValid(OutBuffer)) {
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			break;
		}
		__try 
		{
			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			ProbeForWrite(OutBuffer, OutlengthSize, sizeof(PVOID));
			st = GetKernelFileInformation(InBuffer, OutBuffer, OutlengthSize);
			Irp->IoStatus.Status = st;

		}
		__except (EXCEPTION_EXECUTE_HANDLER) 
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}

		break;

	case TERMINATE_PROCESS_INTERNALLY:

		__try 
		{
			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			st = KernelModeTerminateInternally(*(PHANDLE)InBuffer);
			Irp->IoStatus.Status = st;

		}
		__except (EXCEPTION_EXECUTE_HANDLER) 
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}

		break;
	
	case THREAD_KERNEL_INFORMATION:
		if (!MmIsAddressValid(OutBuffer))
		{
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			break;
		}
		__try
		{
			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			ProbeForWrite(OutBuffer, OutlengthSize, sizeof(PVOID));
			st = GetKernelThreadInformation(*(PHANDLE)InBuffer,OutBuffer, OutlengthSize);
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = st;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}

		break;
	case THREAD_KILL_KERNEL:

		__try
		{
			ProbeForRead(InBuffer, Inlength, sizeof(ULONG));
			st = TerminateKernelModeThread(*(PHANDLE)InBuffer);
			Irp->IoStatus.Status = st;

		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return GetExceptionCode();
			Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		}

		break;

	default:
		return STATUS_INVALID_DEVICE_REQUEST;
	}


	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = st;
	IofCompleteRequest(Irp, IO_NO_INCREMENT);
	return st;

}