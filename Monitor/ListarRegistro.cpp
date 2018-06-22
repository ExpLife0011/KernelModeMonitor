// ListarRegistro.cpp : implementation file
//

#include "stdafx.h"
#include "Monitor.h"
#include "ListarRegistro.h"
#include "afxdialogex.h"

#include "global.h"



// ListarRegistro dialog

/*////////////////////////////////
/
/	File: ListarRegistro.cpp
/
/	Goal:
/		Gather Registry\\Services Information
/		View Properties functionality of each key
/		Hooking functionality in x86 version
/
*//////////////////////////////////		

CString externstring;

IMPLEMENT_DYNAMIC(ListarRegistro, CDialogEx)

ListarRegistro::ListarRegistro(CWnd* pParent /*=NULL*/)
	: CDialogEx(ListarRegistro::IDD, pParent)
{

}

ListarRegistro::~ListarRegistro()
{
}

void ListarRegistro::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTARENTRADASREGISTRO, m_listarregistro);
	DDX_Control(pDX, IDC_GETREGVALUE, m_regval);
}


BEGIN_MESSAGE_MAP(ListarRegistro, CDialogEx)

	ON_NOTIFY(NM_RCLICK, IDC_LISTARENTRADASREGISTRO, &ListarRegistro::OnNMRClickListarentradasregistro)
	ON_COMMAND(ID_REGCONTEXT_PROPIEDADES, &ListarRegistro::OnRegcontextPropiedades)
	ON_COMMAND(ID_REGCONTEXT_REFRESCAR, &ListarRegistro::OnRegcontextRefrescar)
#ifndef _WIN64
	ON_COMMAND(ID_PROTECCIONEXX86_PROTEGERLLAVEDEBORRADO, &ListarRegistro::OnProteccionexx86Protegerllavedeborrado)
	ON_COMMAND(ID_PROTECCIONEXX86_PROTEGERVALORESDEBORRADO, &ListarRegistro::OnProteccionexx86Protegervaloresdeborrado)
	ON_COMMAND(ID_PROTECCIONEXX86_PROTEGERLLAVEDEINTRODUCCIONDEVALORES, &ListarRegistro::OnProteccionexx86Protegerllavedeintroducciondevalores)
	ON_COMMAND(ID_PROTECCIONEXX86_PROTEGERLLAVEVISUALIZACIONDEVALORES, &ListarRegistro::OnProteccionexx86Protegerllavevisualizaciondevalores)
	ON_COMMAND(ID_PROTECCIONEXX86_OCULTARLLAVE, &ListarRegistro::OnProteccionexx86Ocultarllave)
	ON_COMMAND(ID_PROTECCIONEXX86_DESHACERPROTECCIONES, &ListarRegistro::OnProteccionexx86Deshacerprotecciones)
#endif
END_MESSAGE_MAP()


// ListarRegistro message handlers

BOOL ListarRegistro::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CImageList *imgreg = new CImageList;
	imgreg->Create(16, 16, ILC_MASK | ILC_COLOR32, 2, 2);
	imgreg->Add(AfxGetApp()->LoadIconW(MAKEINTRESOURCE(IDI_ICON4)));

	m_listarregistro.SetImageList(imgreg, LVSIL_SMALL);

	m_listarregistro.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER|LVS_EX_FULLROWSELECT|LVS_EX_LABELTIP|LVS_EX_TRACKSELECT);
	m_listarregistro.InsertColumn(0, _T("Service"),LVCFMT_CENTER,350);
	m_listarregistro.InsertColumn(1, _T("Modification"),LVCFMT_CENTER,350);


	m_regval.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
	m_regval.InsertColumn(0, _T("Name"), LVCFMT_CENTER, 200);
	m_regval.InsertColumn(1, _T("Type"), LVCFMT_CENTER, 100);
	m_regval.InsertColumn(2, _T("Data"), LVCFMT_LEFT, 400);



	return TRUE;


}

void ListarRegistro::OnNMRClickListarentradasregistro(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_MENU7));
	CMenu* pPopup = menu.GetSubMenu(0);

#ifdef _WIN64
	pPopup->RemoveMenu(0, MF_BYPOSITION);
#endif _WIN64

	int selected = m_listarregistro.GetNextItem(-1, LVNI_SELECTED);

	RECT rect;
	GetWindowRect(&rect);
	CPoint mousepos;
	GetCursorPos(&mousepos);
	pPopup->TrackPopupMenu(NULL, mousepos.x, mousepos.y, this);

}


void ListarRegistro::OnRegContextRegistryOperation(ULONG DeviceIoctl)
{
	POSITION pos = m_listarregistro.GetFirstSelectedItemPosition();
	while (pos)
	{
		UINT item = m_listarregistro.GetNextSelectedItem(pos);
		if (item != NULL)
		{
			CString buf = m_listarregistro.GetItemText(item, 0);
			if (buf.GetBuffer() != NULL)
			{
				NTSTATUS st = NtDeviceIoControlFile(g_device,
					NULL,
					NULL,
					NULL,
					&io,
					DeviceIoctl,
					(PVOID)buf.GetBuffer(),
					(ULONG)buf.GetLength(),
					NULL,
					0);
				if (st == STATUS_INVALID_HANDLE)
					::MessageBox(NULL, L"Error, Invalid handle", L"ERROR", MB_ICONERROR);
				else
					::MessageBox(NULL, L"Key Path sent to driver", L"KEMM", MB_ICONINFORMATION);
			}
		}
	}
}

void ListarRegistro::OnRegcontextPropiedades()
{
	POSITION pos = m_listarregistro.GetFirstSelectedItemPosition();
	while (pos)
	{
		UINT item = m_listarregistro.GetNextSelectedItem(pos);
		proper = m_listarregistro.GetItemText(item, 0);
		if (proper.GetBuffer() != NULL)
		{
			GetKernelValueBuffer(GET_REG_VALUE_INFORMATION);

		}

	}
}

void ListarRegistro::GetKernelValueBuffer(ULONG DeviceIoctl)
{
	valinfo = NULL;
	ULONG size = 0;
	NTSTATUS st;
	ULONG cont = 0x1000;
	CString str;

	m_regval.DeleteAllItems();

	str.Format(L"SYSTEM\\CurrentControlSet\\Services\\");
	str += proper;

	size = sizeof(PKERNEL_REG_VAL_INFORMATION) + cont*sizeof(PKERNEL_REG_VAL_ENTRY);
	valinfo = (PKERNEL_REG_VAL_INFORMATION)malloc(size);
	if (valinfo != NULL)
	{
		RtlSecureZeroMemory(valinfo, size);
		st = NtDeviceIoControlFile(g_device, NULL,
			NULL,
			NULL,
			&io,
			DeviceIoctl,
			(PVOID)str.GetBuffer(),
			(ULONG)str.GetLength(),
			valinfo,
			size);
		if (st == STATUS_INVALID_HANDLE)
			::MessageBox(NULL, L"Error, Invalid handle", L"Error", MB_ICONERROR);
		else
		{
			for (UINT i = 0; i < valinfo->NumberOfEntries; ++i)
			{
				CString name, type, data, udata, cc;

				name.Format(L"%ws", valinfo->Entry[i].Name);
				data.Format(L"%ws", valinfo->Entry[i].DataName);
				udata.Format(L"0x%08lu", valinfo->Entry[i].udatalong);

				UINT item = 0;

				m_regval.InsertItem(0, name);
				
				switch (valinfo->Entry[i].TypeOfKey)
				{
				case REG_DWORD:
					cc = L"REG_DWORD";
					m_regval.SetItemText(item, 2, udata);
					break;

				case REG_MULTI_SZ:
					cc = L"REG_MULTI_SZ";
					m_regval.SetItemText(item, 2, data);
					break;

				case REG_SZ:
					cc = L"REG_SZ";
					m_regval.SetItemText(item, 2, data);
					break;

				case REG_EXPAND_SZ:
					cc = L"REG_EXPAND_SZ";
					m_regval.SetItemText(item, 2, data);
					break;

				default:
					cc = L"(default)";
					break;
				}

				type.Format(L"%ws", cc);
				m_regval.SetItemText(item, 1, type);


			}
		}
	}
	if (valinfo)
	{
		free(valinfo);
		valinfo = NULL;
	}
	
}

void ListarRegistro::OnRegcontextRefrescar()
{
	// TODO: Add your command handler code here

	m_listarregistro.DeleteAllItems();

	GetServicesNodeBuffer(KERNEL_REG_MON);
}


void ListarRegistro::GetServicesNodeBuffer(ULONG DeviceIoctl)
{
	ULONG cont = 0x1000;
	ULONG size = sizeof(PREG_KERNEL_INFORMATION) + cont*sizeof(PREG_KERNEL_ENTRY);
	kernelinfo = NULL;
	CString aaa;

	aaa.Format(L"SYSTEM\\CurrentControlSet\\Services");

	kernelinfo = (PREG_KERNEL_INFORMATION)malloc(size * 0x1000);
	if (kernelinfo != NULL)
	{
		RtlSecureZeroMemory(kernelinfo, size);
		NTSTATUS st = NtDeviceIoControlFile(g_device,
			NULL,
			NULL,
			NULL,
			&io,
			DeviceIoctl,
			(PVOID)aaa.GetBuffer(),
			(ULONG)aaa.GetLength(),
			kernelinfo,
			size);

		if (st == STATUS_INVALID_HANDLE)
			::MessageBox(NULL, L"Error Invalid handle", L"ERROR", MB_ICONERROR);
		else
		{
			for (UINT i = 0; i < kernelinfo->NumberOfEntries; ++i)
			{
				CString name, time;
				name.Format(L"%ws", kernelinfo->Entry[i].KeyName);
				time.Format(L"%ws", kernelinfo->Entry[i].CreateTime);

				UINT item = 0;
				m_listarregistro.InsertItem(0, name);
				m_listarregistro.SetItemText(item,1, time);
			}

		}
	}
	if (kernelinfo)
	{
		free(kernelinfo);
		kernelinfo = NULL;
	}
}




#ifndef _WIN64

void ListarRegistro::OnProteccionexx86Protegerllavedeborrado()
{
	OnRegContextRegistryOperation(HOOK_REG_BORRAR_LLAVE);
}


void ListarRegistro::OnProteccionexx86Protegervaloresdeborrado()
{
	OnRegContextRegistryOperation(HOOK_REG_BORRAR_VALORES);
}


void ListarRegistro::OnProteccionexx86Protegerllavedeintroducciondevalores()
{
	OnRegContextRegistryOperation(HOOK_REG_INTRODUCIR_VALORES);
}


void ListarRegistro::OnProteccionexx86Protegerllavevisualizaciondevalores()
{
	OnRegContextRegistryOperation(HOOK_REG_VER_VALORES);
}


void ListarRegistro::OnProteccionexx86Ocultarllave()
{
	OnRegContextRegistryOperation(HOOK_REG_ESCONDER);
}


void ListarRegistro::OnProteccionexx86Deshacerprotecciones()
{
	OnRegContextRegistryOperation(DESHACER_HOOKS_REGISTRO);
}

#endif