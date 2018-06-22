#pragma once
#include "afxcmn.h"
#include "afxwin.h"


typedef struct _KERNEL_PROCESS_ENTRY
{
	WCHAR ProcessName[50];
	WCHAR ProcessPath[MAX_PATH];
	ULONG Pid;
	ULONG PPid;
	ULONG NumberOfThreads;
	ULONG HandleCount;
	WCHAR CreateTime[100];
	ULONG_PTR Eprocess;
	LONGLONG CreationTimeQuadpart;
}KERNEL_PROCESS_ENTRY, *PKERNEL_PROCESS_ENTRY;

typedef struct _KERNEL_PROCESS
{
	ULONG NumberOfEntries;
	KERNEL_PROCESS_ENTRY Entry[1];

}KERNEL_PROCESS, *PKERNEL_PROCESS;

// KernelProcessExplorer dialog

class KernelProcessExplorer : public CDialogEx
{
	DECLARE_DYNAMIC(KernelProcessExplorer)

public:
	KernelProcessExplorer(CWnd* pParent = NULL);   // standard constructor
	virtual ~KernelProcessExplorer();
	
	PKERNEL_PROCESS kernelinfo;
	void GetKernelProcess(ULONG DeviceIoctl);
	void OnContextMenuOperation(ULONG DeviceIoctl);
// Dialog Data
	enum { IDD = IDD_KERNELPROCESSEXPLORER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
public:
	virtual BOOL KernelProcessExplorer::OnInitDialog();
	DECLARE_MESSAGE_MAP()
	CListCtrl m_kernelprocessexplorer;
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnProcessSuspender();
	afx_msg void OnProcessResumir();
	afx_msg void OnProcessEsconder();
	afx_msg void OnProcessInformacionExtra();
	afx_msg void OnCallbacksMonitorearcreacuib();
	afx_msg void OnCallbacksMonitorearcargadeimagenes();
	afx_msg void OnMonitorearcreacionDetener();
	afx_msg void OnMonitorearcreacionActivar();
	afx_msg void OnCallbacksMonitorearcreacion();
	afx_msg void OnCallbacksMonitorearprocesos();
	afx_msg void OnProcessRecibirprocesoskernel();
	afx_msg void OnTerminacionViaobopenobjectpointer();
	afx_msg void OnTerminacionViapspterminatesystemthread();
	afx_msg void OnProcessInformacionhilos();
};


extern ULONG pid;