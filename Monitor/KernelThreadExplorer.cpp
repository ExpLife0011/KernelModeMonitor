// KernelThreadExplorer.cpp : implementation file
//

#include "stdafx.h"
#include "Monitor.h"
#include "KernelThreadExplorer.h"
#include "afxdialogex.h"
#include "global.h"

#include "KernelProcessExplorer.h"
// KernelThreadExplorer dialog


/*////////////////////////////////
/
/	File: KernelThreadExplorer.cpp
/
/	Goal:
/		Gather Thread Information By Process id
/		 - ETHREAD address of the specified address
/		 - Number of ContextSwitches
/		 - Thread ID
/		 - TEB address (Thread Environment Block)
/        - CreationTime of the specified Thread
/		 - Start Address
/		 - Win32 Start Address
/		 - State of Thread
*//////////////////////////////////		


IMPLEMENT_DYNAMIC(KernelThreadExplorer, CDialogEx)

KernelThreadExplorer::KernelThreadExplorer(CWnd* pParent /*=NULL*/)
	: CDialogEx(KernelThreadExplorer::IDD, pParent)
{

}

KernelThreadExplorer::~KernelThreadExplorer()
{
}

void KernelThreadExplorer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_threadinfo);
}


BEGIN_MESSAGE_MAP(KernelThreadExplorer, CDialogEx)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &KernelThreadExplorer::OnNMRClickList1)

	ON_COMMAND(ID_ONTHREADOPERATION_TERMINARHILO32897, &KernelThreadExplorer::OnOnthreadoperationTerminarhilo32897)
END_MESSAGE_MAP()


// KernelThreadExplorer message handlers


BOOL KernelThreadExplorer::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_threadinfo.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP | LVS_EX_DOUBLEBUFFER);

	m_threadinfo.InsertColumn(0, L"Ethread", LVCFMT_LEFT, 140);
	m_threadinfo.InsertColumn(1, L"Id", LVCFMT_CENTER, 52);
	m_threadinfo.InsertColumn(2, L"Teb", LVCFMT_CENTER, 145);
	m_threadinfo.InsertColumn(3, L"Creation", LVCFMT_CENTER, 150);
	m_threadinfo.InsertColumn(4, L"Address", LVCFMT_CENTER, 142);
	m_threadinfo.InsertColumn(5, L"Win32Address", LVCFMT_CENTER, 155);
	m_threadinfo.InsertColumn(6, L"State", LVCFMT_CENTER, 90);

	CImageList *img = new CImageList;
	img->Create(16, 16, ILC_MASK | ILC_COLOR32, 2, 2);
	img->Add(AfxGetApp()->LoadIconW(MAKEINTRESOURCE(IDI_ICON2)));
	m_threadinfo.SetImageList(img, LVSIL_SMALL);

	KernelThreadExplorer::GetThreadInformationKernelMode(THREAD_KERNEL_INFORMATION);

	return TRUE;

}


VOID KernelThreadExplorer::GetThreadInformationKernelMode(ULONG_PTR Controlcode)
{
	NTSTATUS st;
	ULONG cont = 0x1000;
	ULONG size = sizeof(PTHREAD_INFORMATION) + cont * sizeof(PTHREAD_INFORMATION_ENTRY);
	thrinfo = NULL;

	thrinfo = (PTHREAD_INFORMATION)malloc(size * 0x250);
	if (thrinfo != NULL)
	{
		RtlSecureZeroMemory(thrinfo, size);
		st = NtDeviceIoControlFile(g_device,
			NULL,
			NULL,
			NULL,
			&io,
			Controlcode,
			&pid,
			sizeof(ULONG),
			thrinfo,
			size);
		if (!NT_SUCCESS(st))
		{
			WCHAR error[20];
			wsprintf(error, L"0x%x", st);
			::MessageBox(NULL,error, L"ERROR", MB_ICONERROR);
		}
			
		else
		{
			CString win32, id, ethread, state, ctx, teb, time, addr;

			for (ULONG i = 0; i < thrinfo->NumberOfThreads; i++)
			{
				
				UINT item = 0;

				id.Format(L"%d", thrinfo->Entry[i].ThreadId);
				ethread.Format(L" 0x%p", thrinfo->Entry[i].Ethread);
				teb.Format(L"0x%p", thrinfo->Entry[i].Teb);
				time.Format(L"%ws", thrinfo->Entry[i].CreateTime);
				addr.Format(L"0x%p", thrinfo->Entry[i].StartAddress);
				win32.Format(L"0x%p", thrinfo->Entry[i].Win32StartAddress);

			
				m_threadinfo.InsertItem(0, ethread);
				m_threadinfo.SetItemText(item, 1, id);
				m_threadinfo.SetItemText(item, 2, teb);
				m_threadinfo.SetItemText(item, 3, time);
				m_threadinfo.SetItemText(item, 4, addr);
				m_threadinfo.SetItemText(item, 5, win32);

			
				switch (thrinfo->Entry[i].State)
				{
				case Initialized:
					state = L"Initialized";
					break;
				case Ready:
					state = L"Ready";
					break;
				case Running:
					state = L"Executing";
					break;
				case Standby:
					state = L"StandBy";
					break;
				case Terminated:
					state = L"Terminated";
					break;
				case Waiting:
					state = L"Waiting";
					break;
				case Transition:
					state = L"Transition";
					break;
				case DeferredReady:
					state = L"DeferredReady";
					break;
				case GateWait:
					state = L"GateWait";
					break;
				default:
					state = L"";
					break;
				}


				m_threadinfo.SetItemText(item, 6, state);


			}
			
		}
	}
	if (thrinfo)
	{
		free(thrinfo);
		thrinfo = NULL;
	}
}



void KernelThreadExplorer::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	CMenu menu;
	VERIFY(menu.LoadMenuW(IDR_MENU10));
	CMenu *pop = menu.GetSubMenu(0);

	UINT select = m_threadinfo.GetNextItem(-1, LVNI_SELECTED);
	RECT rect;
	CPoint point;

	GetWindowRect(&rect);
	GetCursorPos(&point);
	pop->TrackPopupMenu(NULL, point.x, point.y, this);


}

VOID KernelThreadExplorer::OnThreadContextMenuOperation(ULONG_PTR ControlCode)
{
	NTSTATUS st;
	POSITION pos = m_threadinfo.GetFirstSelectedItemPosition();
	while (pos)
	{
		UINT item = m_threadinfo.GetNextSelectedItem(pos);
		if (item != NULL)
		{
			ULONG Tid = _ttoi(m_threadinfo.GetItemText(item, 1).GetBuffer());
			if (Tid != NULL)
			{
				st = NtDeviceIoControlFile(g_device,
					NULL,
					NULL,
					NULL,
					&io,
					ControlCode,
					&Tid,
					sizeof(ULONG),
					NULL,
					0);
				if (!NT_SUCCESS(st))
					::MessageBox(NULL, L"Error sending Thread id", L"ERROR", MB_ICONERROR);
				else
					::MessageBox(NULL, L"Thread id sent", L"KEMM", MB_ICONINFORMATION);
			}
		}
	}
}


void KernelThreadExplorer::OnOnthreadoperationTerminarhilo32897()
{
	// TODO: Add your command handler code here
	OnThreadContextMenuOperation(THREAD_KILL_KERNEL);
}
