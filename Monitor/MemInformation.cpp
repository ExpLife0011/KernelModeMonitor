// MemInformation.cpp : implementation file
//

#include "stdafx.h"
#include "Monitor.h"
#include "MemInformation.h"
#include "afxdialogex.h"
#include "global.h"

#include "KernelProcessExplorer.h"
// MemInformation dialog


/*////////////////////////////////
/
/	File: MemInformation.cpp
/
/	Goal:
/		Gather memory information 
/		by Process id.
/		Uses Undocumented structure
/		VM Counters in kernel driver
/
*//////////////////////////////////		

IMPLEMENT_DYNAMIC(MemInformation, CDialogEx)

MemInformation::MemInformation(CWnd* pParent /*=NULL*/)
	: CDialogEx(MemInformation::IDD, pParent)
{

}

MemInformation::~MemInformation()
{
}

void MemInformation::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(MemInformation, CDialogEx)
END_MESSAGE_MAP()


// MemInformation message handlers


BOOL MemInformation::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetKernelMemoryInformation(MEMORY_INFORMATION);

	return TRUE;
}

void MemInformation::GetKernelMemoryInformation(ULONG DeviceIoctl)
{
	NTSTATUS st;
	ULONG cont = 0x1000;
	ULONG size = 0;
	meminfo = NULL;

	size = sizeof(PKERNEL_MEM_INFO) + cont;
	meminfo = (PKERNEL_MEM_INFO)malloc(size);
	if (meminfo != NULL)
	{
		RtlSecureZeroMemory(meminfo, size);
		st = NtDeviceIoControlFile(g_device,
			NULL,
			NULL,
			NULL,
			&io,
			DeviceIoctl,
			&pid,
			sizeof(ULONG),
			meminfo,
			size);

		if (st == STATUS_INVALID_HANDLE)
		{
			::MessageBox(NULL, L"Invalid handle", L"ERROR", MB_ICONERROR);
		}
		else
		{
			CString a, b, c, d, e, f, g, i, j, m, p, q, r;

			a.Format(L"%lu kb",meminfo->PeakVirtualSize / 1024);
			b.Format(L"%lu kb", meminfo->VirtualSize / 1024);
			c.Format(L"%lu kb", meminfo->PeakWorkingSetSize / 1024);
			d.Format(L"%lu kb", meminfo->WorkingSetSize / 1024);
			e.Format(L"%lu kb", meminfo->QuotaPeakPagedPoolUsage / 1024);
			f.Format(L"%lu kb", meminfo->QuotaPagedPoolUsage / 1024);
			g.Format(L"%lu kb", meminfo->QuotaPeakNonPagedPoolUsage / 1024);
			i.Format(L"%lu kb", meminfo->QuotaNonPagedPoolUsage / 1024);
			j.Format(L"%lu kb", meminfo->PagefileUsage / 1024);

			m.Format(L"%llu kb", meminfo->ReadOperation / 1024);
			p.Format(L"%llu kb", meminfo->ReadTransferOperation / 1024);
			q.Format(L"%llu kb", meminfo->WriteOperation / 1024);
			r.Format(L"%llu kb", meminfo->WriteTransferOperation / 1024);

			SetDlgItemText(IDC_STATIC1, a);
			SetDlgItemText(IDC_STATIC2, b);
			SetDlgItemText(IDC_STATIC3, c);
			SetDlgItemText(IDC_STATIC4, d);
			SetDlgItemText(IDC_STATIC5, e);
			SetDlgItemText(IDC_STATIC6, f);
			SetDlgItemText(IDC_STATIC7, g);
			SetDlgItemText(IDC_STATIC8, i);
			SetDlgItemText(IDC_STATIC9, j);
			
			SetDlgItemText(IDC_STATIC10, m);
			SetDlgItemText(IDC_STATIC11, p);
			SetDlgItemText(IDC_STATIC12, q);
			SetDlgItemText(IDC_STATIC13, r);
		}

	}
	if (meminfo)
	{
		free(meminfo);
		meminfo = NULL;
	}
}