// NtfsScanner.cpp : implementation file
//

#include "stdafx.h"
#include "Monitor.h"
#include "NtfsScanner.h"
#include "afxdialogex.h"

#include "global.h"
#include "Volumenes.h"
#include "FileInformationNtfs.h"

// NtfsScanner dialog

/*/////////////////////////////////////////////////////////////////////////
/
/	File: NtfsScanner.cpp
/
/	Goal:
/		Gather file information by sending path.
/		Format must be C:\Windows or C:\Users\username
/		Functionality:
/			- Delete file by IRP
/			- File Information by IRP
/			- Hooking functionality in x86 only
/			- View Volume Information
/			- Detects hidden files 
/			- Uses internal Kernel Mode Function IopCreateFile to open handle
/
*//////////////////////////////////		//////////////////////////////////

IMPLEMENT_DYNAMIC(NtfsScanner, CDialogEx)

CString sharestr;

NtfsScanner::NtfsScanner(CWnd* pParent /*=NULL*/)
	: CDialogEx(NtfsScanner::IDD, pParent)
{

}

NtfsScanner::~NtfsScanner()
{
}

void NtfsScanner::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_insertntfs);
	DDX_Control(pDX, IDC_EDIT1, m_numerodelementos);
	DDX_Control(pDX, IDC_EDIT2, m_insertarruta);
}


BEGIN_MESSAGE_MAP(NtfsScanner, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &NtfsScanner::OnBnClickedScanner)
	ON_COMMAND(ID_OPCIONES_VOLUMENES, &NtfsScanner::OnOpcionesVolumenes)
	ON_COMMAND(ID_ONNTFS_BORRARARCHIVOVIAIRP, &NtfsScanner::OnOnntfsBorrararchivoviairp)
	ON_COMMAND(ID_ONNTFS_INFORMACIONARCHIVO, &NtfsScanner::OnOnntfsInformacionarchivo)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &NtfsScanner::OnNMRClickList1)
#ifndef _WIN64
	ON_COMMAND(ID_PROTECCIONESX86_PROTEGERCONTRAAPERTURAYRENOMBRE, &NtfsScanner::OnProteccionesx86Protegercontraaperturayrenombre)
	ON_COMMAND(ID_PROTECCIONESX86_PROTEGERCONTRAESCRITURA, &NtfsScanner::OnProteccionesx86Protegercontraescritura)
	ON_COMMAND(ID_PROTECCIONESX86_PROTEGERCONTRAELIMINACIONYRENOMBRE, &NtfsScanner::OnProteccionesx86Protegercontraeliminacionyrenombre)
	ON_COMMAND(ID_PROTECCIONESX86_DESHACERPROTECCIONES, &NtfsScanner::OnProteccionesx86Deshacerprotecciones)
#endif
END_MESSAGE_MAP()


// NtfsScanner message handlers

BOOL NtfsScanner::OnInitDialog() 
{
	CDialogEx::OnInitDialog();


	m_insertntfs.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
	m_insertntfs.InsertColumn(0, _T("Directory"), LVCFMT_CENTER, 160);
	m_insertntfs.InsertColumn(1, _T("Creation"),LVCFMT_CENTER,160);
	m_insertntfs.InsertColumn(2, _T("Change"), LVCFMT_CENTER, 160);
	m_insertntfs.InsertColumn(3, _T("Write"), LVCFMT_CENTER, 160);
	m_insertntfs.InsertColumn(4, _T("Bytes"), LVCFMT_CENTER, 80);
	m_insertntfs.InsertColumn(5, _T("Alloc"), LVCFMT_CENTER, 100);

	CImageList *ntfs = new CImageList();
	ntfs->Create(16, 16, ILC_MASK | ILC_COLOR32, 2, 2);
	ntfs->Add(AfxGetApp()->LoadIconW(MAKEINTRESOURCE(IDI_ICON16)));
	m_insertntfs.SetImageList(ntfs, LVSIL_SMALL);

	CMenu menu;
	menu.LoadMenuW(IDR_MENU5);
	SetMenu(&menu);

	return TRUE;

}

void NtfsScanner::OnBnClickedScanner() 
{
	// TODO: Add your control notification handler code here

	EnumerarNtfs(NTFS_SCANNER);
	

}

PCHAR NtfsScanner::UnicodeStringToAnsiStringWrapper(CString *str) 
{
	ANSI_STRING astr1;
	UNICODE_STRING ustr1;

	RtlSecureZeroMemory(&ustr1, sizeof(ustr1));
	RtlInitUnicodeString(&ustr1, (const wchar_t *)str->GetBuffer());
	RtlUnicodeStringToAnsiString(&astr1, &ustr1, TRUE);

	return astr1.Buffer;

}

#define KERNEL_BUFFER_DATA 0x2000 * 2

VOID NtfsScanner::EnumerarNtfs(ULONG DeviceIoctl) 
{
	m_insertntfs.DeleteAllItems();

	CString str;
	PCHAR AnsiBuffer = NULL;
	ULONG SizeOfBuffer = 0;
	ULONG cont = 0x2000;
	BOOL ret = FALSE;
	DWORD bytes = 0;
	FileInformation = NULL;

	m_insertarruta.GetWindowTextW(str);


	do 
	{
		ULONG size = 0;
		if (FileInformation) 
		{
			RtlFreeHeap(GetProcessHeap(), 0, FileInformation);
			FileInformation = NULL;
		}

		size = sizeof(PFILE_INFORMATION) + cont * sizeof(PFILE_INFORMATION_ENTRY);
		FileInformation = (PFILE_INFORMATION)RtlAllocateHeap(GetProcessHeap(), HEAP_ZERO_MEMORY, size * 0x1000);
		if (!FileInformation) 
		{
			break;
		}
		RtlSecureZeroMemory(FileInformation, size);
		ret = DeviceIoControl(g_device, DeviceIoctl,(PVOID)str.GetBuffer(), (ULONG)str.GetLength(), FileInformation, size, &bytes, NULL);
		cont = FileInformation->NumberOfEntries + 1000;

	} while (ret && GetLastError() == ERROR_INSUFFICIENT_BUFFER);

	if (ret && FileInformation)
	{
		CString numberofelements, directory, createtime, writetime, changetime, bytes, allocsize;
		ULONG i = 0;
		ULONG item = 0;
	
		for (i = 0; i < FileInformation->NumberOfEntries; ++i) 
		{
			directory.Format(_T("%ls"), FileInformation->Entry[i].DirectoryName);
			createtime.Format(_T("%ws"), FileInformation->Entry[i].CreationTime);
			writetime.Format(_T("%ws"), FileInformation->Entry[i].WriteTime);
			changetime.Format(_T("%ws"), FileInformation->Entry[i].ChangeTime);;
			bytes.Format(L"%I64d", FileInformation->Entry[i].EndOfFile);
			allocsize.Format(L"%I64d", FileInformation->Entry[i].AllocationSize);

			m_insertntfs.InsertItem(0, directory);
			m_insertntfs.SetItemText(item, 1, createtime);
			m_insertntfs.SetItemText(item, 2, changetime);
			m_insertntfs.SetItemText(item, 3, writetime);
			m_insertntfs.SetItemText(item, 4, bytes);
			m_insertntfs.SetItemText(item, 5, allocsize);

			numberofelements.Format(_T("%ld"), FileInformation->NumberOfEntries);
			m_numerodelementos.SetWindowTextW(numberofelements);
			
		}
		
	}
	if (FileInformation)
	{
		RtlFreeHeap(GetProcessHeap(), 0, FileInformation);
		FileInformation = NULL;
	}
}



void NtfsScanner::OnOpcionesVolumenes() 
{
	Volumenes vol;
	vol.DoModal();
}


void NtfsScanner::OnContextNtfsScannerOperation(ULONG_PTR Ioctl) 
{
	NTSTATUS st;
	CString windowstr;
	POSITION pos = m_insertntfs.GetFirstSelectedItemPosition();
	while (pos) {
		UINT item = m_insertntfs.GetNextSelectedItem(pos);
		sharestr = m_insertntfs.GetItemText(item, 0);
		m_insertarruta.GetWindowTextW(windowstr);

		if (wcscmp(windowstr.GetBuffer(), L"C:\\")==0)
		{
			windowstr += sharestr;
		}
		else
		{
			windowstr += L"\\";
			windowstr += sharestr;
		}

		//windowstr += L"\\";
		//windowstr += sharestr;
		
		if (windowstr.GetBuffer() != NULL) {
			st = NtDeviceIoControlFile(g_device,
				NULL,
				NULL,
				NULL,
				&io,
				Ioctl,
				(PVOID)windowstr.GetBuffer(),
				(ULONG)windowstr.GetLength(),
				NULL,
				0);
			if (st == STATUS_INVALID_HANDLE)
				::MessageBox(NULL, L"Error sending path to driver", L"ERROR", MB_ICONERROR);
			else
				::MessageBox(NULL, L"Path sent to driver", L"KEMM", MB_ICONINFORMATION);
		}
	}

}


void NtfsScanner::OnOnntfsBorrararchivoviairp() {
	OnContextNtfsScannerOperation(BORRAR_VIA_IRP);
}


void NtfsScanner::OnOnntfsInformacionarchivo() {	
	FileInformationNtfs file;
	CString str;
	POSITION pos = m_insertntfs.GetFirstSelectedItemPosition();
	while (pos) {
		UINT item = m_insertntfs.GetNextSelectedItem(pos);
		str = m_insertntfs.GetItemText(item, 0);
		m_insertarruta.GetWindowTextW(sharestr);

		sharestr += L"\\";
		sharestr += str;

		if (sharestr.GetBuffer() != NULL) {
			file.DoModal();
		}
	}
	

}


void NtfsScanner::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	CMenu menu;
	VERIFY(menu.LoadMenuW(IDR_MENU8));
	CMenu *pop = menu.GetSubMenu(0);
#ifdef _WIN64
	pop->RemoveMenu(0, MF_BYPOSITION);
#endif _WIN64


	UINT select = m_insertntfs.GetNextItem(-1, LVNI_SELECTED);
	RECT rect;
	CPoint point;

	GetWindowRect(&rect);
	GetCursorPos(&point);
	pop->TrackPopupMenu(NULL, point.x, point.y, this);


}

// FileInformationNtfs dialog

IMPLEMENT_DYNAMIC(FileInformationNtfs, CDialogEx)

FileInformationNtfs::FileInformationNtfs(CWnd* pParent /*=NULL*/)
	: CDialogEx(FileInformationNtfs::IDD, pParent)
{

}

FileInformationNtfs::~FileInformationNtfs()
{
}

void FileInformationNtfs::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(FileInformationNtfs, CDialogEx)
END_MESSAGE_MAP()


// FileInformationNtfs message handlers


BOOL FileInformationNtfs::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetFileNtfsInformation(&sharestr, GET_KERNEL_FILE_INFORMATION);


	return TRUE;

}

void FileInformationNtfs::GetFileNtfsInformation(CString *str, ULONG_PTR Ioctl) {
	NTSTATUS st;
	ULONG cont = 0x1000;
	ULONG size = 0;
	fileinfo = NULL;

	size = sizeof(PFILE_KERNEL_INFO) + cont;
	fileinfo = (PFILE_KERNEL_INFO)malloc(size);
	if (fileinfo != NULL) {
		st = NtDeviceIoControlFile(g_device,
			NULL, NULL, NULL,
			&io,
			Ioctl,
			(PVOID)str->GetBuffer(),
			(ULONG)str->GetLength(),
			fileinfo,
			size);
		if (st == STATUS_INVALID_HANDLE)
			::MessageBox(NULL, L"Error sending string", L"ERROR", MB_ICONERROR);
		else {
			CString bytes, write, read, del, drv, dev, fs;
			bytes.Format(L"%lu kb", fileinfo->FileBytes / 1024);
			write.Format(L"%ws", fileinfo->WriteAccess ? L"Yes" : L"No");
			read.Format(L"%ws", fileinfo->ReadAccess ? L"Yes" : L"No");
			del.Format(L"%ws", fileinfo->DeleteAccess ? L"Yes" : L"No");
			fs.Format(L"0x%p", fileinfo->FsContext);
			drv.Format(L"%lu", fileinfo->AccessMask);
			dev.Format(L"%lld", fileinfo->CurrentByteOffset);

			SetDlgItemText(IDC_STATIC1, bytes);
			SetDlgItemText(IDC_STATIC2, read);
			SetDlgItemText(IDC_STATIC3, write);
			SetDlgItemText(IDC_STATIC4, del);
			SetDlgItemText(IDC_STATIC5, fs);
			SetDlgItemText(IDC_STATIC6, drv);
			SetDlgItemText(IDC_STATIC7, dev);
		}
	}
	if (fileinfo) {
		free(fileinfo);
		fileinfo = NULL;
	}

}

#ifndef _WIN64

void NtfsScanner::OnProteccionesx86Protegercontraaperturayrenombre()
{
	OnContextNtfsScannerOperation(HOOK_ACCESO_DIR);
}


void NtfsScanner::OnProteccionesx86Protegercontraescritura()
{
	OnContextNtfsScannerOperation(HOOK_LECTURA_ESCRITURA);
}


void NtfsScanner::OnProteccionesx86Protegercontraeliminacionyrenombre()
{
	OnContextNtfsScannerOperation(HOOK_BORRAR_DRIVER);
}


void NtfsScanner::OnProteccionesx86Deshacerprotecciones()
{
	UTILS::__EnviarComandoDriver(g_device, DESHACER_HOOKS_DIR);
}

#endif