#pragma once


// MemInformation dialog

typedef struct _KERNEL_MEM_INFO
{
	ULONG		 PeakVirtualSize;
	ULONG		 VirtualSize;
	ULONG        PageFaultCount;
	ULONG        PeakWorkingSetSize;
	ULONG        WorkingSetSize;
	ULONG        QuotaPeakPagedPoolUsage;
	ULONG        QuotaPagedPoolUsage;
	ULONG        QuotaPeakNonPagedPoolUsage;
	ULONG        QuotaNonPagedPoolUsage;
	ULONG        PagefileUsage;
	ULONG        PeakPagefileUsage;
	ULONGLONG	 ReadOperation;
	ULONGLONG    ReadTransferOperation;
	ULONGLONG    WriteOperation;
	ULONGLONG    WriteTransferOperation;
}KERNEL_MEM_INFO,*PKERNEL_MEM_INFO;


class MemInformation : public CDialogEx
{
	DECLARE_DYNAMIC(MemInformation)

public:
	MemInformation(CWnd* pParent = NULL);   // standard constructor
	virtual ~MemInformation();
	void GetKernelMemoryInformation(ULONG DeviceIoctl);
	PKERNEL_MEM_INFO meminfo;

// Dialog Data
	enum { IDD = IDD_EXTRAINFORMACION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
public:
	virtual BOOL MemInformation::OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
