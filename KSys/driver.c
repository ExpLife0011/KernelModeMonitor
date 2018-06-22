#include "driver.h"
#include "Km.h"


UNICODE_STRING dispositivo;
UNICODE_STRING link;
PDEVICE_OBJECT pdeviceObject = NULL;
PCHAR RegValoresBuffer = NULL;
CHAR HelperVariable[0xFF] = {0};
WCHAR UserHelperVariable[0xFF] = { 0 };
PWCHAR UserModeBuffer=NULL;
PDRIVER_OBJECT g_DriverObject = NULL;
PsSuspendProcess _PsSuspendProcess = NULL;
PsResumeProcess _PsResumeProcess = NULL;
IOPCREATEFILE IopCreateFile = NULL;
IOCREATEFILE IoCreateFileOriginal = NULL;
PSPTERMINATETHREADBYPOINTER PspTerminateThreadByPointer=NULL;
PSTERMINATESYSTEMTHREAD PsTerminateSystemThreadOriginal = NULL;
PSLOOKUPPROCESSBYPROCESSID PsLookupProcessByProcessIdOriginal=NULL;



void ConvertirSysTimeToLocalTime(PLARGE_INTEGER localtime, PTIME_FIELDS timefields) 
{
	LARGE_INTEGER systime = { 0 };
	KeQuerySystemTime(&systime);
	ExSystemTimeToLocalTime(&systime, localtime);
	RtlTimeToTimeFields(localtime, timefields);
	
}


VOID Unload(PDRIVER_OBJECT pdriverObject) 
{
	DbgPrint("\n[KEMM-MONITOR] Driver Unloaded");
#ifndef _WIN64
	DesProtegerRegistro();
	DeshacerProteccionesObjetos();
	DesprotegerDirectorioAcls();
#endif _WIN64
	IoDeleteDevice(pdeviceObject);
	IoDeleteSymbolicLink(&link);
}


NTSTATUS DriverEntry(PDRIVER_OBJECT pdriverObject, PUNICODE_STRING Registry) {

	ULONG majorver=0, minorver=0, buildnumber=0;
	LARGE_INTEGER localtime;
	TIME_FIELDS timefields = {0};

	ConvertirSysTimeToLocalTime(&localtime, &timefields);

	DbgPrint("\n[KEMM-MONITOR] Driver Loaded");
	DbgPrint("\n[KEMM-MONITOR] System time: %02u-%02u-%04u %02u:%02u:%02u:%03u", timefields.Day,
		timefields.Month, timefields.Year, timefields.Hour, timefields.Minute, timefields.Second, timefields.Milliseconds);

	PsGetVersion(&majorver, &minorver, &buildnumber, NULL);
	switch (minorver) {
	case 1:
		DbgPrint("\n[KEMM-MONITOR] Software: Windows 7");
		break;
	default:
		break;
	}
#ifndef _WIN64
	BackupSysServicesTable();
#endif _WIN64


	pdriverObject->DriverUnload = Unload;

	NTSTATUS st;

	RtlInitUnicodeString(&dispositivo, DISPOSITIVO);
	RtlInitUnicodeString(&link, LINK);


	st = IoCreateDevice(pdriverObject, NULL, &dispositivo, FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN, FALSE, (DEVICE_OBJECT**)&pdeviceObject);
	if (!NT_SUCCESS(st)) {
		DbgPrint("[KEMM-MONITOR] Error creating device: 0x%x",st);
		return st;
	}

	st = IoCreateSymbolicLink(&link, &dispositivo);
	if (!NT_SUCCESS(st)) {
		DbgPrint("[KEMM-MONITOR] Error creating symbolic link: 0x%x", st);
		IoDeleteDevice(pdeviceObject);
		return st;
	}


	LoadInternalAPI();

	pdriverObject->MajorFunction[IRP_MJ_CREATE] = driverAbrirCanal;
	pdriverObject->MajorFunction[IRP_MJ_CLOSE] = driverCerrarCanal;
	pdriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = driverControlarComunicacion;

	pdeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;


	return st;
}