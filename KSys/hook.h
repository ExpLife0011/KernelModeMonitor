#pragma once

#define MAX_SYSTEM_SERVICE_NUMBER 1024

typedef struct _KSYSTEM_SERVICE_TABLE
{
	PULONG  ServiceTableBase;				
	PULONG  ServiceCounterTableBase;			
	ULONG   NumberOfService;				
	ULONG   ParamTableBase;					

} KSYSTEM_SERVICE_TABLE, *PKSYSTEM_SERVICE_TABLE;

typedef struct _KSERVICE_TABLE_DESCRIPTOR
{
	KSYSTEM_SERVICE_TABLE   ntoskrnl;			
	KSYSTEM_SERVICE_TABLE   win32k;				
	KSYSTEM_SERVICE_TABLE   notUsed1;
	KSYSTEM_SERVICE_TABLE   notUsed2;

} KSERVICE_TABLE_DESCRIPTOR, *PKSERVICE_TABLE_DESCRIPTOR;


extern PKSERVICE_TABLE_DESCRIPTOR KeServiceDescriptorTable;

#define NUMERO_FUNCION_SYSCALL(ServiceFunction) (*(unsigned long*)((unsigned char*)ServiceFunction + 1))
#define FUNCION_SYSCALL(ServiceFunction) KeServiceDescriptorTable->ntoskrnl.ServiceTableBase[NUMERO_FUNCION_SYSCALL(ServiceFunction)]

unsigned long DireccionServicioOriginal[MAX_SYSTEM_SERVICE_NUMBER];

void DisableWriteProtect(unsigned long oldAttr);

void BackupSysServicesTable();
void EnableWriteProtect(unsigned long *pOldAttr);

long HOOK(unsigned long oldService, unsigned long newService);
long UNHOOK(unsigned long oldService);



// extras

NTSTATUS Funcion_Hookeada(unsigned long FuncionVieja, unsigned long FuncionNueva);
NTSTATUS Funcion_Unhookeada(unsigned long FuncionVieja);


