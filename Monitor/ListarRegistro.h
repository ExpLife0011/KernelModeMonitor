#pragma once
#include "afxcmn.h"
#include "afxwin.h"


typedef struct _REG_KERNEL_ENTRY
{
	WCHAR KeyName[MAX_PATH];
	WCHAR CreateTime[MAX_PATH];

}REG_KERNEL_ENTRY, *PREG_KERNEL_ENTRY;

typedef struct _REG_KERNEL_INFORMATION
{
	ULONG_PTR NumberOfEntries;
	REG_KERNEL_ENTRY Entry[1];

}REG_KERNEL_INFORMATION, *PREG_KERNEL_INFORMATION;

typedef struct _KERNEL_REG_VAL_ENTRY
{
	WCHAR Name[MAX_PATH];
	WCHAR DataName[MAX_PATH];
	ULONG TypeOfKey;
	ULONG udatalong;

}KERNEL_REG_VAL_ENTRY, *PKERNEL_REG_VAL_ENTRY;

typedef struct _KERNEL_REG_VAL_INFORMATION
{

	ULONG_PTR NumberOfEntries;
	KERNEL_REG_VAL_ENTRY Entry[1];

}KERNEL_REG_VAL_INFORMATION, *PKERNEL_REG_VAL_INFORMATION;

// ListarRegistro dialog

class ListarRegistro : public CDialogEx
{
	DECLARE_DYNAMIC(ListarRegistro)

public:
	ListarRegistro(CWnd* pParent = NULL);   // standard constructor
	virtual ~ListarRegistro();
	void GetKernelValueBuffer(ULONG DeviceIoctl);
	void GetServicesNodeBuffer(ULONG DeviceIoctl);
	void OnRegContextRegistryOperation(ULONG DeviceIoctl);
	PKERNEL_REG_VAL_INFORMATION valinfo;
	PREG_KERNEL_INFORMATION kernelinfo;
// Dialog Data
	enum { IDD = IDD_LISTARREGISTRO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
public:
	virtual BOOL ListarRegistro::OnInitDialog();
	BOOL EnumerarServiciosRegistro();
	DECLARE_MESSAGE_MAP()
	CListCtrl m_listarregistro;
	afx_msg void OnNMRClickListarentradasregistro(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRegcontextEliminarLlave();
	afx_msg void OnRegcontextEliminarValores();
	afx_msg void OnRegcontextMonitorearintroducciondatos();
	afx_msg void OnRegcontextMonitorearvisualizacionvalores();
	afx_msg void OnRegcontextEsconder();
	afx_msg void OnRegcontextPropiedades();
	afx_msg void OnRegcontextDeshacerprotecciones();
	CListCtrl m_regval;
	CString proper;
	afx_msg void OnRegcontextRefrescar();
	afx_msg void OnProteccionexx86Protegerllavedeborrado();
	afx_msg void OnProteccionexx86Protegervaloresdeborrado();
	afx_msg void OnProteccionexx86Protegerllavedeintroducciondevalores();
	afx_msg void OnProteccionexx86Protegerllavevisualizaciondevalores();
	afx_msg void OnProteccionexx86Ocultarllave();
	afx_msg void OnProteccionexx86Deshacerprotecciones();
};
