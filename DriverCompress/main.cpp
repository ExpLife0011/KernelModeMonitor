#include "..\Monitor\global.h"
#include "..\Monitor\ntapi.h"
#include "..\Monitor\ldrntapi.h"
#include "..\Monitor\kernel.h"
#include "..\Monitor\util.h"
#include "..\Monitor\reg.h"


/*
/	Win32 Driver Compressor
/
/	Native Compressor version https://github.com/alex9191/NativeFileCompressor/blob/master/Compressor.c
/
/
*/


int main(int argc, char *argv[])
{

	LDR_APIS::CargarFuncionesNT();

	HANDLE handle = CreateFileA(argv[1], GENERIC_READ, NULL, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);	
	if (handle != INVALID_HANDLE_VALUE)																			
	{
		DWORD size = GetFileSize(handle, NULL);	
		if (size > 0)
		{
			PBYTE mem = (BYTE*)LocalAlloc(LPTR, size);
			if (mem != NULL)
			{
				DWORD bytesread = 0;
				BOOL bret = ReadFile(handle, mem, size, &bytesread, NULL);	
				if (bret == TRUE && size == bytesread)
				{
					CloseHandle(handle);
					DWORD outsize = 0;
					PBYTE Buffer = API_COMPRESION::ComprimirBuffer(mem, (SIZE_T)size, &outsize);
					if (Buffer != NULL)		
					{
						HANDLE handle2 = CreateFileA(argv[2],	
							GENERIC_WRITE,
							NULL,
							0,
							CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL);
						if (handle2 != INVALID_HANDLE_VALUE)
						{
							DWORD byteswritten = 0;
							BOOL write = WriteFile(handle2, Buffer, outsize, &byteswritten, NULL);
							if (write == TRUE && outsize == byteswritten)
							{
								CloseHandle(handle2);
							}
						}

					}
				}
				LocalFree(mem);
			}
		}
	}

	return EXIT_SUCCESS;
}