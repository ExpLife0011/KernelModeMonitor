// KernelProcessExplorer.cpp : implementation file
//

#include "stdafx.h"
#include "Monitor.h"
#include "KernelProcessExplorer.h"
#include "afxdialogex.h"

#include "global.h"
#include "MemInformation.h"
#include "KernelThreadExplorer.h"


// KernelProcessExplorer dialog

/*//////////////////////////////////////////////////////////
/
/	File: KernelProcessExplorer
/
/	Goal:
/		- Recibir procesos kernel
/		- suspender procesos
/		- resumir procesos
/		- Información VM Counters
/		- Información Hilos
/		- Terminar Proceso vía ObOpenObjectByPointer
/		- Terminar Proceso vía PspTerminateThreadByPointer
/
*///////////////////////////////////////////////////////////	


ULONG pid = 0;

IMPLEMENT_DYNAMIC(KernelProcessExplorer, CDialogEx)

KernelProcessExplorer::KernelProcessExplorer(CWnd* pParent /*=NULL*/)
	: CDialogEx(KernelProcessExplorer::IDD, pParent)
{

}

KernelProcessExplorer::~KernelProcessExplorer()
{
}

void KernelProcessExplorer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_kernelprocessexplorer);
}


BEGIN_MESSAGE_MAP(KernelProcessExplorer, CDialogEx)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &KernelProcessExplorer::OnNMRClickList1)
	ON_COMMAND(ID_PROCESS_SUSPENDER, &KernelProcessExplorer::OnProcessSuspender)
	ON_COMMAND(ID_PROCESS_RESUMIR, &KernelProcessExplorer::OnProcessResumir)
	ON_COMMAND(ID_PROCESS_ESCONDER, &KernelProcessExplorer::OnProcessEsconder)
	ON_COMMAND(ID_PROCESS_INFORMACI32835, &KernelProcessExplorer::OnProcessInformacionExtra)
	ON_COMMAND(ID_PROCESS_RECIBIRPROCESOSKERNEL, &KernelProcessExplorer::OnProcessRecibirprocesoskernel)
	ON_COMMAND(ID_TERMINACION_VIAOBOPENOBJECTPOINTER, &KernelProcessExplorer::OnTerminacionViaobopenobjectpointer)
	ON_COMMAND(ID_TERMINACION_VIAPSPTERMINATESYSTEMTHREAD, &KernelProcessExplorer::OnTerminacionViapspterminatesystemthread)
	ON_COMMAND(ID_PROCESS_INFORMACIONHILOS, &KernelProcessExplorer::OnProcessInformacionhilos)
END_MESSAGE_MAP()


// KernelProcessExplorer message handlers

BOOL KernelProcessExplorer::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_kernelprocessexplorer.SetExtendedStyle(LVS_EX_FULLROWSELECT| LVS_EX_GRIDLINES| LVS_EX_LABELTIP | LVS_EX_DOUBLEBUFFER);
	m_kernelprocessexplorer.InsertColumn(0, L"Name", LVCFMT_LEFT, 120);
	m_kernelprocessexplorer.InsertColumn(1, L"Id", LVCFMT_CENTER, 60);
	m_kernelprocessexplorer.InsertColumn(2, L"Ppid", LVCFMT_CENTER, 60);
	m_kernelprocessexplorer.InsertColumn(3, L"Handles", LVCFMT_CENTER, 60);
	m_kernelprocessexplorer.InsertColumn(4, L"Threads", LVCFMT_CENTER, 60);
	m_kernelprocessexplorer.InsertColumn(5, L"Time", LVCFMT_CENTER, 120);
	m_kernelprocessexplorer.InsertColumn(6, L"Eprocess", LVCFMT_CENTER, 130);
	m_kernelprocessexplorer.InsertColumn(7, L"Path", LVCFMT_LEFT, 418);

	CImageList *img = new CImageList;
	img->Create(16, 16, ILC_MASK | ILC_COLOR32, 2, 2);
	img->Add(AfxGetApp()->LoadIconW(MAKEINTRESOURCE(IDI_ICON17)));
	m_kernelprocessexplorer.SetImageList(img, LVSIL_SMALL);


	return TRUE;
}


void KernelProcessExplorer::GetKernelProcess(ULONG DeviceIoctl)
{

	m_kernelprocessexplorer.DeleteAllItems();

	ULONG bytes = 0;
	ULONG size = 0;
	ULONG cont = 0x1000;
	LARGE_INTEGER systime = { 0 };
	TIME_FIELDS timetime = { 0 };
	kernelinfo = NULL;
	
	size = sizeof(PKERNEL_PROCESS) + cont*sizeof(PKERNEL_PROCESS_ENTRY);
	kernelinfo = (PKERNEL_PROCESS)malloc(size * 0x500);
	if (kernelinfo != NULL)
	{
		RtlSecureZeroMemory(kernelinfo, size);
		NTSTATUS st = NtDeviceIoControlFile(g_device, NULL, NULL, NULL, &io, DeviceIoctl, NULL, 0, kernelinfo, size);
		if (!NT_SUCCESS(st))
		{
			::MessageBox(NULL, L"Error connecting to kernel-mode driver", L"ERROR", MB_ICONERROR);
		}
		else
		{
			ULONG i = 0;
			for (i > 0; i < kernelinfo->NumberOfEntries; ++i)
			{
				CString pid, ppid, nb, name, handles, threads, time, eprocess, path;

				name.Format(L" %ws", kernelinfo->Entry[i].ProcessName);
				pid.Format(L"%lu", kernelinfo->Entry[i].Pid);
				ppid.Format(L"%lu", kernelinfo->Entry[i].PPid);
				handles.Format(L"%lu", kernelinfo->Entry[i].HandleCount);
				threads.Format(L"%lu", kernelinfo->Entry[i].NumberOfThreads);
				eprocess.Format(L"0x%p", kernelinfo->Entry[i].Eprocess);
				path.Format(L"%ws", kernelinfo->Entry[i].ProcessPath);


				RtlSystemTimeToLocalTime((LARGE_INTEGER*)&kernelinfo->Entry[i].CreationTimeQuadpart, &systime);
				RtlTimeToTimeFields(&systime, &timetime);
				time.Format(L"%02u/%02u/%04u %02u:%02u:%02u", timetime.Day, timetime.Month, timetime.Year, timetime.Hour, timetime.Minute, timetime.Second);

				UINT item = 0;
				m_kernelprocessexplorer.InsertItem(0,name);
				m_kernelprocessexplorer.SetItemText(item, 1, pid);
				m_kernelprocessexplorer.SetItemText(item, 2, ppid);
				m_kernelprocessexplorer.SetItemText(item, 3, handles);
				m_kernelprocessexplorer.SetItemText(item, 4, threads);
				m_kernelprocessexplorer.SetItemText(item, 5, time);
				m_kernelprocessexplorer.SetItemText(item, 6, eprocess);
				m_kernelprocessexplorer.SetItemText(item, 7, path);
			}
		}
	}
	if (kernelinfo)
	{
		free(kernelinfo);
		kernelinfo = NULL;
	}

}

void KernelProcessExplorer::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	CMenu menu;
	VERIFY(menu.LoadMenuW(IDR_MENU6));
	CMenu *pop = menu.GetSubMenu(0);

	UINT select = m_kernelprocessexplorer.GetNextItem(-1, LVNI_SELECTED);
	RECT rect;
	CPoint point;

	GetWindowRect(&rect);
	GetCursorPos(&point);
	pop->TrackPopupMenu(NULL, point.x, point.y, this);

}


void KernelProcessExplorer::OnContextMenuOperation(ULONG DeviceIoctl)
{
	NTSTATUS st;
	POSITION pos = m_kernelprocessexplorer.GetFirstSelectedItemPosition();
	while (pos)
	{
		UINT item = m_kernelprocessexplorer.GetNextSelectedItem(pos);
		ULONG IdProceso = _ttoi(m_kernelprocessexplorer.GetItemText(item, 1).GetBuffer());
		if (IdProceso <= 4)
		{
			return;
		}
		st = NtDeviceIoControlFile(g_device, NULL,
			NULL, 
			NULL, 
			&io,
			DeviceIoctl,
			&IdProceso,
			sizeof(ULONG),
			NULL,
			0);
		if (NT_SUCCESS(st))
		{
			::MessageBox(NULL, L"Id sent to driver", L"KEMM", MB_ICONINFORMATION);
		}
		else
		{
			::MessageBox(NULL, L"Error sending id", L"ERROR", MB_ICONERROR);
		}
	}
}

void KernelProcessExplorer::OnProcessSuspender()
{
	OnContextMenuOperation(SUSPENDER_PROCESO);
}


void KernelProcessExplorer::OnProcessResumir()
{
	OnContextMenuOperation(RESUMIR_PROCESO);
}


void KernelProcessExplorer::OnProcessEsconder()
{
	OnContextMenuOperation(QUITAR_OFFSET_LIST_ENTRY);
}


void KernelProcessExplorer::OnProcessInformacionExtra()
{
	POSITION pos = m_kernelprocessexplorer.GetFirstSelectedItemPosition();
	while (pos)
	{
		UINT item = m_kernelprocessexplorer.GetNextSelectedItem(pos);
		pid = _ttoi(m_kernelprocessexplorer.GetItemText(item, 1).GetBuffer());
		if (pid <= 4)
		{
			return;
		}

		MemInformation mem;
		mem.DoModal();
	}
}




void KernelProcessExplorer::OnProcessRecibirprocesoskernel()
{
	GetKernelProcess(KERNEL_PROCESS_EXPLORER);
}


void KernelProcessExplorer::OnTerminacionViaobopenobjectpointer()
{
	OnContextMenuOperation(TERMINAR_PROCESO);
}


void KernelProcessExplorer::OnTerminacionViapspterminatesystemthread()
{
	// TODO: Add your command handler code here
	OnContextMenuOperation(TERMINATE_PROCESS_INTERNALLY);
}


void KernelProcessExplorer::OnProcessInformacionhilos()
{
	// TODO: Add your command handler code here
	POSITION pos = m_kernelprocessexplorer.GetFirstSelectedItemPosition();
	while (pos)
	{
		UINT item = m_kernelprocessexplorer.GetNextSelectedItem(pos);
		pid = _ttoi(m_kernelprocessexplorer.GetItemText(item, 1).GetBuffer());
		if (pid <= 4)
		{
			return;
		}

		KernelThreadExplorer kte;
		kte.DoModal();
	}
}
