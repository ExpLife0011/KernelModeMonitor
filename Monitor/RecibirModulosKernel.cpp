// RecibirModulosKernel.cpp : implementation file
//

#include "stdafx.h"
#include "Monitor.h"
#include "RecibirModulosKernel.h"
#include "afxdialogex.h"

#include "global.h"


// RecibirModulosKernel dialog

IMPLEMENT_DYNAMIC(RecibirModulosKernel, CDialogEx)

RecibirModulosKernel::RecibirModulosKernel(CWnd* pParent /*=NULL*/)
	: CDialogEx(RecibirModulosKernel::IDD, pParent)
{

}

RecibirModulosKernel::~RecibirModulosKernel()
{
}

void RecibirModulosKernel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_modulos);
}


BEGIN_MESSAGE_MAP(RecibirModulosKernel, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &RecibirModulosKernel::OnBnClickedButton1)
END_MESSAGE_MAP()


// RecibirModulosKernel message handlers

BOOL RecibirModulosKernel::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	m_modulos.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER| LVS_EX_FULLROWSELECT);
	m_modulos.InsertColumn(0, _T("Mapped"), LVCFMT_CENTER, 90);
	m_modulos.InsertColumn(1, _T("Section"), LVCFMT_CENTER, 90);
	m_modulos.InsertColumn(2, _T("InitOrder"), LVCFMT_CENTER, 90);
	m_modulos.InsertColumn(3, _T("LoadOrder"), LVCFMT_CENTER, 90);
	m_modulos.InsertColumn(4, _T("Size"), LVCFMT_CENTER, 100);


	return TRUE;
}



void RecibirModulosKernel::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here


	EnumerarModulos(MODULOS_CARGADOS);
}


void RecibirModulosKernel::EnumerarModulos(unsigned long _w64 CodigoIoctl)
{
	m_modulos.DeleteAllItems();

	bool ret = false;
	unsigned long bytes = 0;
	unsigned long cont = 0x1000;
	PKERNEL_MODULOS_CARGADOS ModuleInformation = NULL;

	do
	{
		unsigned long size = 0;
		if (ModuleInformation)
		{
			RtlFreeHeap(GetProcessHeap(), 0, ModuleInformation);
			ModuleInformation = NULL;
		
		}
		size = sizeof(PKERNEL_MODULOS_CARGADOS) + cont * sizeof(PKERNEL_MODULOS_CARGADOS_INFORMACION);
		ModuleInformation = (PKERNEL_MODULOS_CARGADOS)RtlAllocateHeap(GetProcessHeap(), 0,HEAP_ZERO_MEMORY);
		if (!ModuleInformation)
		{
			break;
		}
		RtlSecureZeroMemory(ModuleInformation, size);
		ret = DeviceIoControl(g_device, CodigoIoctl, NULL, NULL, ModuleInformation, size, &bytes, NULL);


	} while (ret == FALSE && GetLastError() == ERROR_INSUFFICIENT_BUFFER);

	if (ret && ModuleInformation)
	{
		CString mapped, init, load, section,size2;
		int item = 0;
		unsigned long i = 0;

		for (i > 0; i < ModuleInformation->NumeroDeModulos; ++i)
		{
			mapped.Format(_T("0x%p"), ModuleInformation->Entry[i].mapped);
			init.Format(_T("%ld"), ModuleInformation->Entry[i].init);
			load.Format(_T("%ld"), ModuleInformation->Entry[i].load);
			section.Format(_T("0x%p"), ModuleInformation->Entry[i].section);
			size2.Format(_T("%ld"), ModuleInformation->Entry[i].imagesize);

			m_modulos.InsertItem(0, mapped);
			m_modulos.SetItemText(item, 1, section);
			m_modulos.SetItemText(item, 2, init);
			m_modulos.SetItemText(item, 3, load);
			m_modulos.SetItemText(item, 4, size2);
		}


	}

	if (ModuleInformation) {
		RtlFreeHeap(GetProcessHeap(), 0, ModuleInformation);
		ModuleInformation = NULL;
	}
}