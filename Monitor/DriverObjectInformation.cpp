// DriverObjectInformation.cpp : implementation file
//

#include "stdafx.h"
#include "Monitor.h"
#include "DriverObjectInformation.h"
#include "afxdialogex.h"
#include "global.h"


// DriverObjectInformation dialog

/*///////////////////////////////////////
/
/	File: DriverObjectInformation.cpp
/
/	Goal:
/		Retrieve Driver Object
/		Information 
/
*///////////////////////////////////////	


IMPLEMENT_DYNAMIC(DriverObjectInformation, CDialogEx)

DriverObjectInformation::DriverObjectInformation(CWnd* pParent /*=NULL*/)
	: CDialogEx(DriverObjectInformation::IDD, pParent)
{

}

DriverObjectInformation::~DriverObjectInformation()
{
}

void DriverObjectInformation::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_driverobject);
}


BEGIN_MESSAGE_MAP(DriverObjectInformation, CDialogEx)

	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &DriverObjectInformation::OnNMRClickList1)
	ON_COMMAND(ID_ONDRIVEROBJECT_REFRESCAR, &DriverObjectInformation::OnOndriverobjectRefrescar)
END_MESSAGE_MAP()


// DriverObjectInformation message handlers


BOOL DriverObjectInformation::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CImageList *imgreg = new CImageList;
	imgreg->Create(16, 16, ILC_MASK | ILC_COLOR32, 2, 2);
	imgreg->Add(AfxGetApp()->LoadIconW(MAKEINTRESOURCE(IDI_ICON2)));

	m_driverobject.SetImageList(imgreg, LVSIL_SMALL);

	m_driverobject.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
	m_driverobject.InsertColumn(0, L"Name", LVCFMT_CENTER, 100);
	m_driverobject.InsertColumn(1, L"Drv", LVCFMT_LEFT, 120);
	m_driverobject.InsertColumn(2, L"DrvSec", LVCFMT_CENTER, 135);
	m_driverobject.InsertColumn(3, L"Bytes", LVCFMT_CENTER, 60);
	m_driverobject.InsertColumn(4, L"DrvStart", LVCFMT_LEFT, 140);
	m_driverobject.InsertColumn(5, L"DrvObj", LVCFMT_LEFT, 150);

	return TRUE;

}


void DriverObjectInformation::GetObjectKernelInformation(ULONG_PTR DeviceIoctl)
{
	ULONG size = 0;
	NTSTATUS st;
	ULONG cont = 0x1000;
	objectkernelinfo = NULL;

	m_driverobject.DeleteAllItems();

	size = sizeof(PKERNEL_OBJECT_INFORMATION) + cont*sizeof(PKERNEL_OBJECT_ENTRY);
	objectkernelinfo = (PKERNEL_OBJECT_INFORMATION)malloc(size * 0x500);
	if (objectkernelinfo != NULL)
	{
		RtlSecureZeroMemory(objectkernelinfo, size);
		
		st = NtDeviceIoControlFile(g_device,
			NULL,
			NULL,
			NULL,
			&io,
			DeviceIoctl,
			NULL,
			0,
			objectkernelinfo,
			size);
		if (st == STATUS_INVALID_HANDLE)
			::MessageBox(NULL, L"Error sending info to driver", L"Error", MB_ICONERROR);
		else
		{
			for (UINT i = 0; i < objectkernelinfo->NumberOfEntries; ++i)
			{
				CString nombre, drvobj, bytes, drvname, hard, obj;

				nombre.Format(L"%ws", objectkernelinfo->Entry[i].ObjectName);
				drvobj.Format(L"0x%p", objectkernelinfo->Entry[i].DriverSection);
				bytes.Format(L"%lu", objectkernelinfo->Entry[i].DriverSize);
				drvname.Format(L"%ws", objectkernelinfo->Entry[i].DriverName);
				hard.Format(L"0x%p", objectkernelinfo->Entry[i].DriverStart);
				obj.Format(L"0x%p", objectkernelinfo->Entry[i].DriverObject);

				UINT item = 0;

				m_driverobject.InsertItem(0, nombre);
				m_driverobject.SetItemText(item, 1, drvname);
				m_driverobject.SetItemText(item, 2, drvobj);
				m_driverobject.SetItemText(item, 3, bytes);
				m_driverobject.SetItemText(item, 4, hard);
				m_driverobject.SetItemText(item, 5, obj);
			}
		}
	}
	if (objectkernelinfo)
	{
		free(objectkernelinfo);
		objectkernelinfo = NULL;
	}

}



void DriverObjectInformation::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	CMenu menu;
	VERIFY(menu.LoadMenuW(IDR_MENU11));
	CMenu *pop = menu.GetSubMenu(0);

	UINT select = m_driverobject.GetNextItem(-1, LVNI_SELECTED);
	RECT rect;
	CPoint point;

	GetWindowRect(&rect);
	GetCursorPos(&point);
	pop->TrackPopupMenu(NULL, point.x, point.y, this);


}


void DriverObjectInformation::OnOndriverobjectRefrescar()
{

	GetObjectKernelInformation(KERNEL_OBJECT_MANAGER);
}


