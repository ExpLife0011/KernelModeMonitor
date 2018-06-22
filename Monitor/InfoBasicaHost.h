#pragma once
#include "afxcmn.h"


// InfoBasicaHost dialog

class InfoBasicaHost : public CDialogEx
{
	DECLARE_DYNAMIC(InfoBasicaHost)

public:
	InfoBasicaHost(CWnd* pParent = NULL);   // standard constructor
	virtual ~InfoBasicaHost();

// Dialog Data
	enum { IDD = IDD_INFOHOST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
public:
	virtual BOOL InfoBasicaHost::OnInitDialog();
	void ListarInfoBasica();

	DECLARE_MESSAGE_MAP()
	CListCtrl m_informacionhost;
};
