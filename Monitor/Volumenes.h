#pragma once
#include "afxwin.h"

#include "global.h"


// Volumenes dialog

typedef struct _VOL_INFO
{
	WCHAR SymbolicName[MAX_PATH];
	ULONG BytesPerSector;
	LARGE_INTEGER ActualAvailableAllocationUnits;
	LARGE_INTEGER TotalAllocationUnits;
	LARGE_INTEGER CallerAvailableAllocationUnits;
	ULONG SectorsPerAllocationUnit;
	WCHAR		  VolumeCreationTime[MAX_PATH];
	ULONG         VolumeSerialNumber;
	BOOLEAN       SupportsObjects;
}VOL_INFO, *PVOL_INFO;

class Volumenes : public CDialogEx
{
	DECLARE_DYNAMIC(Volumenes)

public:
	Volumenes(CWnd* pParent = NULL);   // standard constructor
	virtual ~Volumenes();
	PVOL_INFO volinfo;

// Dialog Data
	enum { IDD = IDD_VOLUMENES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
public:
	virtual BOOL Volumenes::OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedViewVolInformation();
	CEdit m_setdatavol;
	CListBox m_listbox;
};
