#pragma once
#include "afxcmn.h"



typedef struct _KERNEL_OBJECT_ENTRY
{
	WCHAR ObjectName[MAX_PATH];
	ULONG DriverSize;
	PVOID DriverSection;
	WCHAR DriverName[MAX_PATH];
	PVOID DriverStart;
	ULONG_PTR DriverObject;
}KERNEL_OBJECT_ENTRY, *PKERNEL_OBJECT_ENTRY;

typedef struct _KERNEL_OBJECT_INFORMATION
{
	ULONG_PTR NumberOfEntries;
	KERNEL_OBJECT_ENTRY Entry[1];
}KERNEL_OBJECT_INFORMATION, *PKERNEL_OBJECT_INFORMATION;


// DriverObjectInformation dialog

class DriverObjectInformation : public CDialogEx
{
	DECLARE_DYNAMIC(DriverObjectInformation)

public:
	DriverObjectInformation(CWnd* pParent = NULL);   // standard constructor
	virtual ~DriverObjectInformation();
	PKERNEL_OBJECT_INFORMATION objectkernelinfo;
	void GetObjectKernelInformation(ULONG_PTR DeviceIoctl);
// Dialog Data
	enum { IDD = IDD_DRIVEROBJECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
public:
	virtual BOOL DriverObjectInformation::OnInitDialog();
	DECLARE_MESSAGE_MAP()
	CListCtrl m_driverobject;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOndriverobjectRefrescar();
	afx_msg void OnOndriverobjectEliminardriver();
};
