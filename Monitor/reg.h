#pragma once

namespace REGISTRO
{
	NTSTATUS OpenKey(PHANDLE keyhandle, ACCESS_MASK Mask, PWSTR Valuename);
	NTSTATUS CreateKey(PHANDLE keyhandle, ACCESS_MASK Mask, PWSTR Valuename);
	NTSTATUS SetValueKey(HANDLE keyhandle, PWSTR Valuename, DWORD Type, PVOID Data, DWORD Datasize);
	NTSTATUS DelValKey(HANDLE keyhandle, PWSTR Valuename);
	NTSTATUS LoadDriver(PWSTR Servicio);
	NTSTATUS UnloadDriver(PWSTR Servicio);
};