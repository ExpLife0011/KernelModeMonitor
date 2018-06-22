#pragma once

#define MONITOR_FORMATO TEXT("{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}")
#define MONITOR_REGISTRO TEXT("\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services\\KEMM_MONITOR")
#define SERVICIOS_REGISTRO TEXT("\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services")


extern WCHAR driver_extraccion[MAX_PATH];
extern WCHAR dir[MAX_PATH];


namespace API_COMPRESION
{
	PBYTE ComprimirBuffer(PBYTE DataIn, SIZE_T SizeIn, PULONG SizeOut);			//hecho
	PBYTE DescomprimirBuffer(PBYTE DataIn, SIZE_T SizeIn, PULONG SizeOut);		//hecho
};

namespace UTILS
{
	BOOL MiWriteFile(PWSTR RutaArchivo, PBYTE Data, ULONG Datasize);			//hecho
	BOOL MiCreateDirectory(PWSTR RutaDirectorio);								//hecho
	PBYTE AccederRecurso(ULONG_PTR IdRecurso, PBYTE Data, PULONG Datasize);			//hecho
	BOOL DetectarArquitectura(PROCESSINFOCLASS processinfo);					//hecho
	NTSTATUS ObtenerPrivilegiosDriver(ULONG Privilegio);						//hecho
	BOOL CrearParametrosDriverServicio(PWSTR Nombredriver, PWSTR Nombreservicio);  //hecho
	BOOL CargarDriverKernel(VOID);												//hecho
	BOOL ComprobarAdministrador(TOKEN_INFORMATION_CLASS Tokenclass);			//hecho
	NTSTATUS EliminarDirectoriosyArchivosKEMM(VOID);							//hecho
	BOOLEAN EliminarEntradasRegistro(IN PWSTR Entrada1, IN PWSTR Entrada2);		//hecho
	BOOLEAN ExtraerDriverWindows7(PWSTR NombreDriver);				//hecho
	BOOLEAN ExtraerDriverWindowsXP(PWSTR NombreDriver);
	BOOLEAN DescomprimirDriverEnFuncionWinVer(ULONG IdRecurso);
	NTSTATUS __EnviarComandoDriver(HANDLE DeviceHandle, unsigned long _w64 CodigoIoctl);
};