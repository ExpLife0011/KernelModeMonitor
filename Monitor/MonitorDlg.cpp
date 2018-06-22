
// MonitorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Monitor.h"
#include "MonitorDlg.h"
#include "afxdialogex.h"

#include "global.h"
#include "InfoBasicaHost.h"
#include "ListarRegistro.h"
#include "NtfsScanner.h"
#include "KernelProcessExplorer.h"
#include "DriverObjectInformation.h"
#include "KemmObj.h"
#include "KernelThreadExplorer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMonitorDlg dialog



CMonitorDlg::CMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMonitorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON3);
}

void CMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listarmodulos);

}

BEGIN_MESSAGE_MAP(CMonitorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_ARCHIVO_EJECUTARCOMOADMINISTRADOR, &CMonitorDlg::OnArchivoEjecutarcomoadministrador)
	ON_COMMAND(ID_ARCHIVO_SALIR, &CMonitorDlg::OnArchivoSalir)
	ON_COMMAND(ID_OPCIONES_EXTRAERDRIVER, &CMonitorDlg::OnOpcionesExtraerdriver)
	ON_COMMAND(ID_OPCIONES_CARGARDRIVER, &CMonitorDlg::OnOpcionesCargardriver)
	ON_COMMAND(ID_OPCIONES_DESCARGARDRIVER, &CMonitorDlg::OnOpcionesDescargardriver)
	ON_COMMAND(ID_DRIVER_IRP, &CMonitorDlg::OnDriverIrpAbrir)
	ON_COMMAND(ID_DRIVER_IRP32777, &CMonitorDlg::OnDriverIrpCerrar)
	ON_COMMAND(ID_OPCIONES_INFORMACIONHOST, &CMonitorDlg::OnOpcionesInformacionhost)
	ON_COMMAND(ID_DRIVER_MODULOSREGISTRO, &CMonitorDlg::OnDriverModulosregistro)
	ON_COMMAND(ID_ARCHIVO_DESINSTALARKEMM, &CMonitorDlg::OnArchivoDesinstalarkemm)
	ON_COMMAND(ID_DRIVER_NTFS, &CMonitorDlg::OnDriverNtfs)
	ON_COMMAND(ID_DRIVER_EXPLORADORDEPROCESOSKERNEL, &CMonitorDlg::OnDriverExploradordeprocesoskernel)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CMonitorDlg::OnNMRClickList1)
	ON_COMMAND(ID_ONLOAD_RECIBIRDRIVERSKERNEL32855, &CMonitorDlg::OnOnloadRecibirdriverskernel32855)
	ON_COMMAND(ID_ONLOAD_SALIR32856, &CMonitorDlg::OnOnloadSalir)
	ON_COMMAND(ID_DRIVER_K32858, &CMonitorDlg::OnDriverK32858)
	ON_COMMAND(ID_DRIVER_EXPLORADORDRIVERS, &CMonitorDlg::OnDriverExploradordrivers)
END_MESSAGE_MAP()


// CMonitorDlg message handlers

BOOL CMonitorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	// antes que nada cargar todas las funciones NT 

	LDR_APIS::CargarFuncionesNT();


	// iniciar menu

	CMenu menu;
	menu.LoadMenuW(IDR_MENU1);
	SetMenu(&menu);

	CImageList *image = new CImageList;
	image->Create(16, 16, ILC_COLOR32|ILC_MASK,2,2);
	image->Add(AfxGetApp()->LoadIconW(MAKEINTRESOURCE(IDI_ICON1)));
	
	m_listarmodulos.SetImageList(image, LVSIL_SMALL);
	
	m_listarmodulos.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER|LVS_EX_FULLROWSELECT|LVS_EX_LABELTIP|LVS_EX_SUBITEMIMAGES);
	m_listarmodulos.InsertColumn(0, _T("ImageBase"), LVCFMT_CENTER, 145);
	m_listarmodulos.InsertColumn(1, _T("Name"), LVCFMT_CENTER, 135);
	m_listarmodulos.InsertColumn(2, _T("Path"), LVCFMT_CENTER, 280);
	m_listarmodulos.InsertColumn(3, _T("Bytes"), LVCFMT_CENTER, 70);
	m_listarmodulos.InsertColumn(4, _T("#load"), LVCFMT_LEFT, 100);


	if (UTILS::ComprobarAdministrador(TokenGroups) == FALSE)
#ifndef _WIN64
		SetWindowText(TEXT("KEMM"));
#else
		SetWindowText(TEXT("KEMM64"));
#endif
	else
#ifndef _WIN64
		SetWindowText(TEXT("KEMM - Administrator"));
#else
		SetWindowText(TEXT("KEMM64 - Administrator"));
#endif


	//CMenu *z = GetMenu();
	//CMenu *pop = z->GetSubMenu(2);
	//pop->RemoveMenu(0, MF_BYPOSITION);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMonitorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMonitorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//boton ejecutar como administrador

void CMonitorDlg::OnArchivoEjecutarcomoadministrador()
{
	WCHAR szBuffer[MAX_PATH + 1];

	GetModuleFileName(NULL, szBuffer, MAX_PATH);

	SHELLEXECUTEINFO info;
	RtlSecureZeroMemory(&info, sizeof(info));

	info.cbSize = sizeof(info);
	info.hProcess = 0;
	info.lpFile = szBuffer;
	info.lpDirectory = 0;
	info.lpVerb = L"runas";
	info.nShow = SW_SHOW;

	if (ShellExecuteEx(&info))
	{
		NtTerminateProcess(NtCurrentProcess(), 1);
	}

}

// boton salir

void CMonitorDlg::OnArchivoSalir()
{
	NtTerminateProcess(NtCurrentProcess(), 1);
}

void CMonitorDlg::OnOpcionesExtraerdriver()
{
	RTL_OSVERSIONINFOW verinfo;
	RtlSecureZeroMemory(&verinfo, sizeof(verinfo));
	verinfo.dwOSVersionInfoSize = sizeof(verinfo);
	RtlGetVersion(&verinfo);

	if (UTILS::MiCreateDirectory(L"C:\\Windows\\MONITOR") == TRUE)
	{
#ifndef _WIN64
		if (verinfo.dwMajorVersion == 6 && verinfo.dwMinorVersion == 1)	//win7
			UTILS::DescomprimirDriverEnFuncionWinVer(IDR_RCDATA1);
		else if (verinfo.dwMajorVersion == 6 && verinfo.dwMinorVersion == 3)
			UTILS::DescomprimirDriverEnFuncionWinVer(IDR_RCDATA1);				//win 8.1
		else
			UTILS::DescomprimirDriverEnFuncionWinVer(IDR_RCDATA1);			// win 10
#else
		if (verinfo.dwMajorVersion == 6 && verinfo.dwMinorVersion == 1)	//win7
			UTILS::DescomprimirDriverEnFuncionWinVer(IDR_RCDATA2);
		else if (verinfo.dwMajorVersion == 6 && verinfo.dwMinorVersion == 3)
			UTILS::DescomprimirDriverEnFuncionWinVer(IDR_RCDATA2);				//win 8.1
		else
			UTILS::DescomprimirDriverEnFuncionWinVer(IDR_RCDATA2);			// win 10		

#endif
	}

}


// boton cargar driver en el kernel

void CMonitorDlg::OnOpcionesCargardriver()
{

	UTILS::CargarDriverKernel();
		
}


void CMonitorDlg::OnOpcionesDescargardriver()
{
	WCHAR error[40];

	
	NTSTATUS st = UTILS::ObtenerPrivilegiosDriver(SE_LOAD_DRIVER_PRIVILEGE);
	if (!NT_SUCCESS(st))
		return;

	st = REGISTRO::UnloadDriver(MONITOR_REGISTRO);
	switch (st)
	{
	case STATUS_SUCCESS:
		::MessageBox(NULL, _T("Driver unloaded successfully"), _T("KEMM"), MB_ICONINFORMATION);
		break;

	case STATUS_PRIVILEGE_NOT_HELD:
		::MessageBox(NULL, _T("Error, need privileges for unloading"), _T("KEMM"), MB_ICONERROR);
		break;

	default:
		break;
	}
}


void CMonitorDlg::OnDriverIrpAbrir()
{
	KERNEL::AbrirCanalComunicacion(KERNEL_DISPOSITIVO);
}


void CMonitorDlg::OnDriverIrpCerrar()
{
	KERNEL::CerrarCanalComunicacion();
}


void CMonitorDlg::OnBnClickedButton2()
{
	NtTerminateProcess(NtCurrentProcess(), 0);
}


void CMonitorDlg::OnOpcionesInformacionhost()
{
	InfoBasicaHost host;
	host.DoModal();
}


void CMonitorDlg::OnDriverModulosregistro()
{
	ListarRegistro reg;
	reg.DoModal();
}


void CMonitorDlg::OnArchivoDesinstalarkemm()
{
	
	if (UTILS::EliminarEntradasRegistro(L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services\\KEMM_MONITOR\\Enum", 
		L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services\\KEMM_MONITOR") == TRUE)
	{
		UTILS::EliminarDirectoriosyArchivosKEMM();
	}
	
}



void CMonitorDlg::OnDriverNtfs()
{

	NtfsScanner ntfs;
	ntfs.DoModal();
}


void CMonitorDlg::OnDriverExploradordeprocesoskernel()
{
	// TODO: Add your command handler code here
	KernelProcessExplorer kkk;
	kkk.DoModal();
}


void CMonitorDlg::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	CMenu menu;
	VERIFY(menu.LoadMenuW(IDR_MENU3));
	CMenu *pop = menu.GetSubMenu(0);

	UINT select = m_listarmodulos.GetNextItem(-1, LVNI_SELECTED);
	RECT rect;
	CPoint point;

	GetWindowRect(&rect);
	GetCursorPos(&point);
	pop->TrackPopupMenu(NULL, point.x, point.y, this);

}


void CMonitorDlg::OnOnloadRecibirdriverskernel32855()
{
	m_listarmodulos.DeleteAllItems();

	ULONG size = 0, cont = 0x1000, i = 0;
	kernelinfo = NULL;
	size = sizeof(PKERNEL_MODULES_INFORMATION) + cont*sizeof(PKERNEL_MODULES_ENTRY);
	kernelinfo = (PKERNEL_MODULES_INFORMATION)malloc(size * 0x1000);
	if (kernelinfo != NULL)
	{
		RtlSecureZeroMemory(kernelinfo, size);
		NTSTATUS st = NtDeviceIoControlFile(g_device,
			NULL,
			NULL,
			NULL,
			&io,
			KERNEL_DRIVER_LIST,
			NULL,
			0,
			kernelinfo,
			size);
		if (!NT_SUCCESS(st))
		{
			WCHAR err[20];
			wsprintf(err, L"0x%x", st);
			::MessageBox(NULL, err, err, MB_ICONERROR);
		}
		else
		{
			UINT item = 0;
			for (i = 0; i < kernelinfo->NumberOfModules; ++i)
			{
				CString base, bytes, path, name, carga, n;
				WCHAR wzRuta[MAX_PATH + 1];
				WCHAR wzTest[MAX_PATH + 1];

				MultiByteToWideChar(CP_ACP, 0, (LPCSTR)&kernelinfo->Modules[i].FullPathName, -1, wzRuta, MAX_PATH);
				MultiByteToWideChar(CP_ACP, 0, (LPCSTR)&kernelinfo->Modules[i].ModuleName, -1, wzTest, MAX_PATH);

				base.Format(L"0x%p", kernelinfo->Modules[i].ImageBase);
				bytes.Format(L"%lu", kernelinfo->Modules[i].ImageSize);
				path.Format(L"%ws", wzRuta);
				name.Format(L"%ws", wzTest);
				carga.Format(L"%lu", kernelinfo->Modules[i].LoadOrderIndex);
				n.Format(L"%lu", kernelinfo->NumberOfModules);

				m_listarmodulos.InsertItem(0, base);
				m_listarmodulos.SetItemText(item, 1, name);
				m_listarmodulos.SetItemText(item, 2, path);
				m_listarmodulos.SetItemText(item, 3, bytes);
				m_listarmodulos.SetItemText(item, 4, carga);

			}
		}
	}

	if (kernelinfo)
	{
		free(kernelinfo);
		kernelinfo = NULL;
	}

}


void CMonitorDlg::OnOnloadSalir()
{

	NtTerminateProcess(NtCurrentProcess(), 0);
}

void CMonitorDlg::OnDriverK32858()
{

	KemmObj obj;
	obj.DoModal();
}



void CMonitorDlg::OnDriverExploradordrivers()
{
	DriverObjectInformation obj;
	obj.DoModal();
}
