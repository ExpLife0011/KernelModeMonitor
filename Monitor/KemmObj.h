#pragma once
#include "afxcmn.h"


// KemmObj dialog


class KemmObj : public CDialogEx
{
	DECLARE_DYNAMIC(KemmObj)

public:
	KemmObj(CWnd* pParent = NULL);   // standard constructor
	virtual ~KemmObj();
	BOOL EnumerarObjetos(IN PWSTR Objeto, IN PWSTR bufferObj);
	void OnContextMenuObjOperation(CString wzPrefix, ULONG_PTR ControlCode);
	void InsertIconByIconId(ULONG_PTR IconID);
// Dialog Data
	enum { IDD = IDD_KOBJ };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
public:
	virtual BOOL KemmObj::OnInitDialog();
	DECLARE_MESSAGE_MAP()
	CTreeCtrl m_treeinsert;
	CListCtrl m_object;
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOnkobjectProtegerdispositivo();
	afx_msg void OnOnkobjectProtegerlinksimbolico();
	afx_msg void OnNMRClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOnkobjectPropiedades();
	

};

