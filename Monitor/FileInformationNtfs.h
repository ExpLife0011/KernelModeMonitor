#pragma once
#include "afxwin.h"


// FileInformationNtfs dialog

typedef struct _FILE_KERNEL_INFO
{
	ULONG FileBytes;
	BOOLEAN DeleteAccess;
	BOOLEAN ReadAccess;
	BOOLEAN WriteAccess;
	ULONG AccessMask;
	LONGLONG CurrentByteOffset;
	PVOID FsContext;
}FILE_KERNEL_INFO,*PFILE_KERNEL_INFO;


class FileInformationNtfs : public CDialogEx
{
	DECLARE_DYNAMIC(FileInformationNtfs)

public:
	FileInformationNtfs(CWnd* pParent = NULL);   // standard constructor
	virtual ~FileInformationNtfs();
	PFILE_KERNEL_INFO fileinfo;
	void GetFileNtfsInformation(CString *str, ULONG_PTR Ioctl);

// Dialog Data
	enum { IDD = IDD_FILEINFORMATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
public:
	virtual BOOL FileInformationNtfs::OnInitDialog();
	DECLARE_MESSAGE_MAP()
	CEdit m_fileeditinformation;
};
