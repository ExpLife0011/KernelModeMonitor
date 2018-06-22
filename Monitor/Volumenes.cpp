// Volumenes.cpp : implementation file
//

#include "stdafx.h"
#include "Monitor.h"
#include "Volumenes.h"
#include "afxdialogex.h"
#include "global.h"

// Volumenes dialog

IMPLEMENT_DYNAMIC(Volumenes, CDialogEx)

Volumenes::Volumenes(CWnd* pParent /*=NULL*/)
	: CDialogEx(Volumenes::IDD, pParent)
{

}

Volumenes::~Volumenes()
{
}

void Volumenes::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, m_setdatavol);
	DDX_Control(pDX, IDC_LIST1, m_listbox);
}


BEGIN_MESSAGE_MAP(Volumenes, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &Volumenes::OnBnClickedViewVolInformation)
END_MESSAGE_MAP()


// Volumenes message handlers


BOOL Volumenes::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_listbox.AddString(L"C:");	
	return TRUE;

}

void Volumenes::OnBnClickedViewVolInformation()
{
	// TODO: Add your control notification handler code here

	CString str;
	NTSTATUS st;
	ULONG bytes = 0;
	ULONG size = 0;
	BOOLEAN ret = FALSE;
	volinfo = NULL;
	ULONG cont = 0x1000;
	WCHAR error[20];

	CListBox *p = (CListBox*)GetDlgItem(IDC_LIST1);
	int sel = p->GetCurSel();
	if (sel == LB_ERR)
		return;

	p->GetText(sel, str);


	size = sizeof(PVOL_INFO) + cont;
	volinfo = (PVOL_INFO)malloc(size * 0x250);
	if (volinfo != NULL)
	{
		RtlSecureZeroMemory(volinfo, size);
		st = NtDeviceIoControlFile(g_device, NULL, NULL, NULL, &io, VOLUME_INFORMATION, (PVOID)str.GetBuffer(), (ULONG)str.GetLength(), volinfo, size);
		if (NT_SUCCESS(st))
		{
			CString string;


			string.Format(L"Real Path: %ws\r\nBytesPerSector: %lu\r\nActualAvailableAllocationUnits: %I64d\r\nCallerAvailableAllocationUnits: %I64d\r\nSectorsPerAllocationUnit: %lu\r\nTotalAllocationUnits: %I64d\r\nVolumeCreationTime: %ws\r\nVolumeSerialNumber: %lu\r\nSupportObjects: %ws",
				volinfo->SymbolicName,
				volinfo->BytesPerSector,
				volinfo->ActualAvailableAllocationUnits,
				volinfo->CallerAvailableAllocationUnits,
				volinfo->SectorsPerAllocationUnit,
				volinfo->TotalAllocationUnits,
				volinfo->VolumeCreationTime,
				volinfo->VolumeSerialNumber,
				volinfo->SupportsObjects ? L"Yes" : L"No"
				);


			m_setdatavol.SetWindowTextW(string);

		}
		else
		{
			wsprintf(error, L"0x%x", st);
			::MessageBox(NULL, error, L"ERROR", MB_ICONERROR);
		}
	}
	if (volinfo)
	{
		free(volinfo);
		volinfo = NULL;
	}



}
