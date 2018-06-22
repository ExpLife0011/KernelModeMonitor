// InfoBasicaHost.cpp : implementation file
//

#include "stdafx.h"
#include "Monitor.h"
#include "InfoBasicaHost.h"
#include "afxdialogex.h"

#include "global.h"

// InfoBasicaHost dialog

/*////////////////////////////////
/
/	File: InfoBasicaHost.cpp
/
/	Goal:
/		Gather basic host 
/		information
*//////////////////////////////////		


IMPLEMENT_DYNAMIC(InfoBasicaHost, CDialogEx)

InfoBasicaHost::InfoBasicaHost(CWnd* pParent /*=NULL*/)
	: CDialogEx(InfoBasicaHost::IDD, pParent)
{

}

InfoBasicaHost::~InfoBasicaHost()
{
}

void InfoBasicaHost::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFO, m_informacionhost);
}


BEGIN_MESSAGE_MAP(InfoBasicaHost, CDialogEx)
END_MESSAGE_MAP()


// InfoBasicaHost message handlers


BOOL InfoBasicaHost::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_informacionhost.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
	m_informacionhost.InsertColumn(0, _T("Host"),LVCFMT_CENTER,110);
	m_informacionhost.InsertColumn(1, _T("User"),LVCFMT_CENTER,110);
	m_informacionhost.InsertColumn(2, _T("Windows"), LVCFMT_CENTER, 100);
	m_informacionhost.InsertColumn(3, _T("Build"), LVCFMT_CENTER, 70);
	m_informacionhost.InsertColumn(4, _T("Arch"), LVCFMT_CENTER, 100);
	m_informacionhost.InsertColumn(5, _T("Kernel debugger"), LVCFMT_CENTER, 130);
	m_informacionhost.InsertColumn(6, _T("Administrator"), LVCFMT_CENTER, 110);


	ListarInfoBasica();

	return TRUE;

}


void InfoBasicaHost::ListarInfoBasica()
{
	WCHAR			user[MAX_PATH];
	WCHAR			pc[MAX_PATH];
	WCHAR			admin[MAX_PATH];
	WCHAR			arch[20];
	WCHAR			os[MAX_PATH];
	DWORD usersize = 0, pcsize = 0;
	RTL_OSVERSIONINFOW verinfo;
	SYSTEM_KERNEL_DEBUGGER_INFORMATION kernelinfo;
	NTSTATUS st;
	INT item = 0;


	pcsize = sizeof pc;
	usersize = sizeof user;

	GetComputerName(pc, &pcsize);
	GetUserName(user, &usersize);

	CString host, usr, win, b, pri, debug, v;

	host.Format(_T("%ws"), pc);
	usr.Format(_T("%ws"), user);
	win.Format(_T("%ws"), os);

	m_informacionhost.InsertItem(0, host);
	m_informacionhost.SetItemText(item, 1, usr);

	RtlSecureZeroMemory(&verinfo, sizeof(verinfo));
	verinfo.dwOSVersionInfoSize = sizeof(verinfo);
	RtlGetVersion(&verinfo);
	if (verinfo.dwMajorVersion == 5 && verinfo.dwMinorVersion == 1) wcscpy_s(os,sizeof(os), L"Windows XP");
	else if (verinfo.dwMajorVersion == 6 && verinfo.dwMinorVersion == 0) wcscpy_s(os,sizeof(os), L"Windows Vista");
	else if (verinfo.dwMajorVersion == 6 && verinfo.dwMinorVersion == 1) wcscpy_s(os,sizeof(os), L"Windows 7");
	else if (verinfo.dwMajorVersion == 6 && verinfo.dwMinorVersion == 2) wcscpy_s(os,sizeof(os), L"Windows 8");
	else if (verinfo.dwMajorVersion == 6 && verinfo.dwMinorVersion == 3) wcscpy_s(os,sizeof(os), L"Windows 8.1");
	else if (verinfo.dwMajorVersion == 10 && verinfo.dwMinorVersion == 0) wcscpy_s(os,sizeof(os), L"Windows 10");
	else wcscpy_s(os,sizeof(os), L"Not supported");


	m_informacionhost.SetItemText(item, 2, os);

	b.Format(_T("%ld"), verinfo.dwBuildNumber);
	m_informacionhost.SetItemText(item, 3, b);

	if (UTILS::DetectarArquitectura(ProcessBasicInformation) == TRUE)
	{
#ifndef _WIN64
		v.Format(_T("%ws"), _T("64-Bits"));
#else
		v.Format(L"%ws", L"32-Bits");
#endif

	}
	else
	{
#ifndef _WIN64
		v.Format(_T("%ws"), _T("32-Bits"));
	
#else
		v.Format(L"%ws", L"64-Bits");
#endif
	}

	st = ZwQuerySystemInformation(SystemKernelDebuggerInformation, &kernelinfo, sizeof(SYSTEM_KERNEL_DEBUGGER_INFORMATION), 0);
	if (NT_SUCCESS(st)) {
		if (kernelinfo.KernelDebuggerEnabled)
			debug.Format(_T("%ws"), _T("On"));
		else
			debug.Format(_T("%ws"), _T("Off"));
	}

	m_informacionhost.SetItemText(item, 4, v);
	m_informacionhost.SetItemText(item, 5, debug);


	if (UTILS::ComprobarAdministrador(TokenGroups) == TRUE)
		pri.Format(_T("%ws"), _T("Yes"));
	else
		pri.Format(_T("%ws"), _T("No"));


	m_informacionhost.SetItemText(item, 6, pri);

}