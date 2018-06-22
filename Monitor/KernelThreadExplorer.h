#pragma once
#include "afxcmn.h"

typedef enum _KTHREAD_STATE
{
	Initialized,
	Ready,
	Running,
	Standby,
	Terminated,
	Waiting,
	Transition,
	DeferredReady,
	GateWait
} KTHREAD_STATE;

typedef struct _THREAD_INFORMATION_ENTRY
{
#ifdef _WIN64
	UINT_PTR Ethread;
	UINT_PTR Win32StartAddress;
	HANDLE ThreadId;
	UINT8 State;
	UINT_PTR Teb;
	WCHAR CreateTime[MAX_PATH];
	UINT_PTR StartAddress;
#else
	UINT32 Ethread;
	UINT32 Win32StartAddress;
	HANDLE ThreadId;
	UINT8 State;
	UINT32 Teb;
	WCHAR CreateTime[MAX_PATH];
	UINT32 StartAddress;
#endif

}THREAD_INFORMATION_ENTRY, *PTHREAD_INFORMATION_ENTRY;

typedef struct _THREAD_INFORMATION
{
	ULONG NumberOfThreads;
	THREAD_INFORMATION_ENTRY Entry[1];
}THREAD_INFORMATION, *PTHREAD_INFORMATION;

// KernelThreadExplorer dialog

class KernelThreadExplorer : public CDialogEx
{
	DECLARE_DYNAMIC(KernelThreadExplorer)

public:
	KernelThreadExplorer(CWnd* pParent = NULL);   // standard constructor
	virtual ~KernelThreadExplorer();
	PTHREAD_INFORMATION thrinfo;
	VOID GetThreadInformationKernelMode(ULONG_PTR Controlcode);
	VOID OnThreadContextMenuOperation(ULONG_PTR ControlCode);

// Dialog Data
	enum { IDD = IDD_THREADINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
public:
	virtual BOOL KernelThreadExplorer::OnInitDialog();
	DECLARE_MESSAGE_MAP()
	CListCtrl m_threadinfo;
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOnthreadoperationSuspenderhilo();
	afx_msg void OnOnthreadoperationResumirhilo();
	afx_msg void OnOnthreadoperationTerminarhilo();
	afx_msg void OnOnthreadoperationSuspenderhiloExplorer();
	afx_msg void OnOnthreadoperationTerminarhilo32897();
};
