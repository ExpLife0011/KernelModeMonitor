#pragma once
#include "afxcmn.h"



typedef struct _KERNEL_MODULOS_CARGADOS_INFORMACION
{
	void *mapped;
	void *imagebase;
	unsigned long imagesize;
	HANDLE section;
	unsigned long init;
	unsigned long load;

}KERNEL_MODULOS_CARGADOS_INFORMACION, *PKERNEL_MODULOS_CARGADOS_INFORMACION;


typedef struct _KERNEL_MODULOS_CARGADOS
{
	unsigned long NumeroDeModulos;
	KERNEL_MODULOS_CARGADOS_INFORMACION Entry[1];

}KERNEL_MODULOS_CARGADOS, *PKERNEL_MODULOS_CARGADOS;

// RecibirModulosKernel dialog

class RecibirModulosKernel : public CDialogEx
{
	DECLARE_DYNAMIC(RecibirModulosKernel)

public:
	RecibirModulosKernel(CWnd* pParent = NULL);   // standard constructor
	virtual ~RecibirModulosKernel();

// Dialog Data
	enum { IDD = IDD_DIALOG5 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
public:
	virtual BOOL RecibirModulosKernel::OnInitDialog();
	DECLARE_MESSAGE_MAP()
	CListCtrl m_modulos;
	afx_msg void OnBnClickedButton1();
	void EnumerarModulos(unsigned long _w64 CodigoIoctl);
};
