#pragma once

NTSTATUS driverAbrirCanal(PDEVICE_OBJECT pdeviceObject, IRP *Irp);
NTSTATUS driverCerrarCanal(PDEVICE_OBJECT pdeviceObject, IRP *Irp);
NTSTATUS driverControlarComunicacion(PDEVICE_OBJECT pdeviceObject, IRP *Irp);