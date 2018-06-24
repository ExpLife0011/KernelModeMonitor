# KernelModeMonitor
Kernel-Mode driver and User-Mode application communication project. Useful to interact with the operating system itself at a lower level because of the existence of a driver that loads into the kernel and exchange data with a usermode application. Available plugins
- Process Explorer: List process information extracting some data directly from the EPROCESS structure <br />
- Thread Explorer: Get Thread information by process id <br />
- Ntfs Viewer: File explorer that lists data using IRP not standard API functions, thus bypassing the existence of hooks and hidden files on some folders <br />
- Registry Explorer: Simulates regedit.exe but getting the data directly from the driver <br />
- Object explorer: Simulates WinObj tool from Microsoft <br />
- Modules: Lists loaded modules (.dll and .sys files) <br />
- Driver Object: Lists all drivers inside the directory object <br />
# VS version
Project is compiled with Visual Studio 2013 Ultimate and WDK (Windows Driver Kits) version 8.1.
If you use another version it will NOT work if you dont migrate or update the project first
# DriverCompress
Command-line tool to compress driver files and insert the given compressed-file inside Monitor (stored as a resource). i.e DriverCompress.exe KEMM.sys driver32.dat Or DriverCompress.exe KEMM64 driver64.sys
. Where both resulting files will be included as a resource (imported) for later run-time extraction
# Monitor
User-mode application that communicates with the driver. Compile Monitor for both x64 and x86 platforms
# KSys
Kernel-mode driver. Compile KSys for both x64 and x86 platforms
# Platforms
Tested in windows 7 x86 and Windows 7 x64. Wont work on another Windows version because driver uses private data offsets
# Photos
![alt tag](https://github.com/alex9191/KernelModeMonitor/blob/master/Capture.PNG)
![alt tag](https://github.com/alex9191/KernelModeMonitor/blob/master/1.PNG)
![alt tag](https://github.com/alex9191/KernelModeMonitor/blob/master/2.PNG)
