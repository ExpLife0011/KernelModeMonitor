#include "driver.h"
#include "hook.h"

#ifndef _WIN64

void DisableWriteProtect(unsigned long x)
{
	_asm
	{
			mov eax, x
			mov cr0, eax
			sti;
	}
}

void EnableWriteProtect(unsigned long *z)
{
	unsigned long c;

	_asm
	{
		cli;
		mov  eax, cr0;
		mov  c, eax;
		and  eax, 0FFFEFFFFh; 
		mov  cr0, eax;
	};


	*z = c;
}


void BackupSysServicesTable()
{
	unsigned long i;

	for (i = 0; (i < KeServiceDescriptorTable->ntoskrnl.NumberOfService) && (i < MAX_SYSTEM_SERVICE_NUMBER); i++)
	{
		DireccionServicioOriginal[i] = KeServiceDescriptorTable->ntoskrnl.ServiceTableBase[i];
		
	}
}


long HOOK(unsigned long oldService, unsigned long newService)
{
	unsigned long uOldAttr = 0;

	EnableWriteProtect(&uOldAttr);	

	FUNCION_SYSCALL(oldService) = newService;

	DisableWriteProtect(uOldAttr);

	return STATUS_SUCCESS;
}


long UNHOOK(unsigned long oldService)
{
	unsigned long uOldAttr = 0;

	EnableWriteProtect(&uOldAttr);

	FUNCION_SYSCALL(oldService) = DireccionServicioOriginal[NUMERO_FUNCION_SYSCALL(oldService)];


	DisableWriteProtect(uOldAttr);

	return STATUS_SUCCESS;
}


NTSTATUS Funcion_Hookeada(unsigned long oldaddress, unsigned long newaddress)
{
	return HOOK((unsigned long)oldaddress, (unsigned long)newaddress);
}

NTSTATUS Funcion_Unhookeada(unsigned long oldaddress)
{
	return UNHOOK((unsigned long)oldaddress);
}

#endif _WIN64
