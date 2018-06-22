#pragma once
#include "afxcmn.h"
#include "afxwin.h"



typedef struct _FILE_INFORMATION_ENTRY
{
	WCHAR DirectoryName[MAX_PATH];
	WCHAR CreationTime[MAX_PATH];
	WCHAR WriteTime[MAX_PATH];
	WCHAR ChangeTime[MAX_PATH];
	LARGE_INTEGER AllocationSize;
	LARGE_INTEGER EndOfFile;
}FILE_INFORMATION_ENTRY, *PFILE_INFORMATION_ENTRY;

typedef struct _FILE_INFORMATION
{
	ULONG_PTR NumberOfEntries;
	FILE_INFORMATION_ENTRY Entry[1];

}FILE_INFORMATION, *PFILE_INFORMATION;

// NtfsScanner dialog

class NtfsScanner : public CDialogEx
{
	DECLARE_DYNAMIC(NtfsScanner)

public:
	NtfsScanner(CWnd* pParent = NULL);   // standard constructor
	virtual ~NtfsScanner();
	VOID EnumerarNtfs(ULONG DeviceIoCtl);
	PFILE_INFORMATION FileInformation;
	void OnContextNtfsScannerOperation(ULONG_PTR Ioctl);

// Dialog Data
	enum { IDD = IDD_NTFS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
public:
	virtual BOOL NtfsScanner::OnInitDialog();
	DECLARE_MESSAGE_MAP()
	CListCtrl m_insertntfs;
	afx_msg void OnBnClickedScanner();
	CEdit m_numerodelementos;
	CEdit m_insertarruta;
	ULONG SearchDirectory(LPWSTR lpPath);
	PCHAR UnicodeStringToAnsiStringWrapper(CString *str);
	afx_msg void OnOpcionesVolumenes();
	afx_msg void OnOpcionesArchivo();
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOnntfsProtegercontraborradoyrenombre();
	afx_msg void OnOnntfsProtegercontraapertura();
	afx_msg void OnOnntfsProtegercontraescritura();
	afx_msg void OnOnntfsBorrararchivoviairp();
	afx_msg void OnOnntfsInformacionarchivo();
	afx_msg void OnOnntfsDeshacerprotecciones();
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOnntfsEsconderarchivo();
	afx_msg void OnProteccionesx86Protegercontraaperturayrenombre();
	afx_msg void OnProteccionesx86Protegercontraescritura();
	afx_msg void OnProteccionesx86Protegercontraeliminacionyrenombre();
	afx_msg void OnProteccionesx86Ocultararchivo();
	afx_msg void OnProteccionesx86Deshacerprotecciones();
};


//extern CString sharestr;