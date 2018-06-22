
// MonitorDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


typedef struct _KERNEL_MODULES_ENTRY {
	HANDLE Section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR FullPathName[256];
	CHAR ModuleName[256];
} KERNEL_MODULES_ENTRY, *PKERNEL_MODULES_ENTRY;

typedef struct _KERNEL_MODULES_INFORMATION {
	ULONG NumberOfModules;
	KERNEL_MODULES_ENTRY Modules[1];
} KERNEL_MODULES_INFORMATION, *PKERNEL_MODULES_INFORMATION;



// CMonitorDlg dialog
class CMonitorDlg : public CDialogEx
{
// Construction
public:
	CMonitorDlg(CWnd* pParent = NULL);	// standard constructor
	PKERNEL_MODULES_INFORMATION kernelinfo;

// Dialog Data
	enum { IDD = IDD_MONITOR_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listarmodulos;
	afx_msg void BotonActualizarModulos();
	afx_msg void OnArchivoEjecutarcomoadministrador();
	afx_msg void OnArchivoSalir();
	afx_msg void OnOpcionesExtraerdriver();

	afx_msg void OnOpcionesCargardriver();
	afx_msg void OnOpcionesDescargardriver();
	afx_msg void OnDriverIrpAbrir();
	afx_msg void OnDriverIrpCerrar();
	afx_msg void OnDriverModulosproceso();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnOpcionesInformacionhost();
	afx_msg void OnDriverModulosregistro();

	afx_msg void OnArchivoDesinstalarkemm();
	afx_msg void OnDriverModulosObjetos();

	afx_msg void OnDriverModulosdirectorio();
	CEdit m_numerodemodulos;
	afx_msg void OnDriverNtfs();
	afx_msg void OnDriverEnumerardriverskernel();
	afx_msg void OnDriverExploradordeprocesoskernel();
	afx_msg void OnDriverRegmon();
	afx_msg void OnDriverK();
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOnloadRecibirdriverskernel32855();
	afx_msg void OnOnloadSalir();
	afx_msg void OnOnloadInformaciondriverobject();
	afx_msg void OnDriverK32858();
	afx_msg void OnDriverExploradordehilos();
	afx_msg void OnDriverExploradordrivers();
};
