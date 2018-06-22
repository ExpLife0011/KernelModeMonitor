// KemmObj.cpp : implementation file
//

#include "stdafx.h"
#include "Monitor.h"
#include "KemmObj.h"
#include "afxdialogex.h"
#include "global.h"


// KemmObj dialog

CString wzBuffer;

IMPLEMENT_DYNAMIC(KemmObj, CDialogEx)

KemmObj::KemmObj(CWnd* pParent /*=NULL*/)
	: CDialogEx(KemmObj::IDD, pParent)
{

}

KemmObj::~KemmObj()
{
}

void KemmObj::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_treeinsert);
	DDX_Control(pDX, IDC_LIST1, m_object);
}


BEGIN_MESSAGE_MAP(KemmObj, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &KemmObj::OnNMDblclkTree1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &KemmObj::OnNMRClickList1)
	ON_COMMAND(ID_ONKOBJECT_PROTEGERDISPOSITIVO, &KemmObj::OnOnkobjectProtegerdispositivo)
	ON_COMMAND(ID_ONKOBJECT_PROTEGERLINKSIMBOLICO, &KemmObj::OnOnkobjectProtegerlinksimbolico)
END_MESSAGE_MAP()


// KemmObj message handlers

// OBJECT MANAGER

/*////////////////////////////////
/
/	File: KemmObj
/
/	Goal:
/		List Objects similar
/		to WinObj (systinternals)
/
*//////////////////////////////////		


BOOL KemmObj::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	

	HTREEITEM root = m_treeinsert.InsertItem(L"\\",0,0, TVI_ROOT);
	HTREEITEM item;
	item = m_treeinsert.InsertItem(L"ArcName",1,1, root);
	item = m_treeinsert.InsertItem(L"Device", root);
	item = m_treeinsert.InsertItem(L"GLOBAL??", root);
	item = m_treeinsert.InsertItem(L"FileSystem", root);
	item = m_treeinsert.InsertItem(L"KernelObjects", root);
	item = m_treeinsert.InsertItem(L"KnownDlls", root);
	item = m_treeinsert.InsertItem(L"KnownDlls32", root);
	item = m_treeinsert.InsertItem(L"ObjectTypes", root);
	item = m_treeinsert.InsertItem(L"RPC Control", root);
	item = m_treeinsert.InsertItem(L"Callback", root);
	item = m_treeinsert.InsertItem(L"BaseNamedObjects", root);

	m_object.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP | LVS_EX_DOUBLEBUFFER |LVS_EX_GRIDLINES);
	m_object.InsertColumn(0, L"Nombre", LVCFMT_CENTER, 400);
	m_object.InsertColumn(1, L"Tipo", LVCFMT_CENTER, 100);
	m_object.InsertColumn(2, L"SymLink", LVCFMT_LEFT, 160);

	

	return TRUE;
}

void KemmObj::InsertIconByIconId(ULONG_PTR IconID) {
	CImageList *image = new CImageList();
	if (image != ERROR) {
		image->Create(16, 16, ILC_MASK | ILC_COLOR32, 2, 2);
		image->Add(AfxGetApp()->LoadIconW(MAKEINTRESOURCE(IconID)));
		m_object.SetImageList(image, LVSIL_SMALL);
	}
}

void KemmObj::OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult) {
	// TODO: Add your control notification handler code here
	*pResult = 0;


	PWCHAR base[] = {L"Event",L"Mutant",L"Section",L"SymbolicLink",L"ALPC Port",NULL};
	PWCHAR sys[] = {L"Device",L"Driver",NULL};
	PWCHAR ev[] = {L"Event",L"KeyedEvent",L"Session",L"Mutant",NULL};
	PWCHAR sec[] = {L"\\KnownDlls",L"\\KnownDlls32",NULL};

	UINT i = 0;

	HTREEITEM item;

	m_object.DeleteAllItems();

	item = m_treeinsert.GetSelectedItem();
	CString str = m_treeinsert.GetItemText(item);
	
	
	
	if (wcscmp(str.GetBuffer(), L"GLOBAL??") == 0) { 
		InsertIconByIconId(IDI_ICON5);
		EnumerarObjetos(L"\\GLOBAL??", L"SymbolicLink"); 
	}
	
	else if (wcscmp(str.GetBuffer(), L"Device") == 0){ EnumerarObjetos(L"\\Device", L"Device"); }
	else if (wcscmp(str.GetBuffer(), L"ArcName") == 0){ EnumerarObjetos(L"\\ArcName", L"SymbolicLink"); }

	else if (!wcscmp(str.GetBuffer(), L"KnownDlls")) {
		InsertIconByIconId(IDI_ICON3);
		EnumerarObjetos(L"\\KnownDlls", L"Section");
	}
	else if (!wcscmp(str.GetBuffer(), L"KnownDlls32")) {
		EnumerarObjetos(L"\\KnownDlls32", L"Section");
	}
	
	else if (wcscmp(str.GetBuffer(), L"ObjectTypes") == 0) { 
		EnumerarObjetos(L"\\ObjectTypes", L"Type"); 
	}
	
	else if (wcscmp(str.GetBuffer(), L"BaseNamedObjects") == 0) {
		for (i = 0; base[i] != NULL; ++i) {
			InsertIconByIconId(IDI_ICON15);
			EnumerarObjetos(L"\\BaseNamedObjects", base[i]);
		}
	}
	else if (wcscmp(str.GetBuffer(), L"FileSystem") ==0 ) {
		for (i = 0; sys[i] != NULL; ++i) {
			InsertIconByIconId(IDI_ICON7);
			EnumerarObjetos(L"\\FileSystem", sys[i]);
		}
	}
	else if (wcscmp(str.GetBuffer(), L"Callback") == 0) { 
		InsertIconByIconId(IDI_ICON6);
		EnumerarObjetos(L"\\Callback", L"Callback"); 
	}
	
	else if (wcscmp(str.GetBuffer(), L"RPC Control") == 0) { 
		InsertIconByIconId(IDI_ICON8);
		EnumerarObjetos(L"\\RPC Control", L"ALPC Port"); 
	}
	
	
	else if (wcscmp(str.GetBuffer(), L"KernelObjects") == 0) { 
		for (i = 0; ev[i] != NULL; ++i) {
			InsertIconByIconId(IDI_ICON8);
			EnumerarObjetos(L"\\KernelObjects", ev[i]);
		}
	
	}

}

BOOL KemmObj::EnumerarObjetos(IN PWSTR Objeto, IN PWSTR bufferObj)
{
	HANDLE Objhandle;
	UNICODE_STRING ustr1;
	OBJECT_ATTRIBUTES ObjectAttributes;
	BOOL ret = TRUE;
	NTSTATUS st;
	SIZE_T size = 0x8000;	// 32768 Bytes
	ULONG context = 0, length = 0;
	POBJECT_DIRECTORY_INFORMATION info;
	WCHAR DataBuffer[MAX_PATH * 2];	// 260 * 2
	UNICODE_STRING DataUni, SymUni;
	OBJECT_ATTRIBUTES ObjectAttributesData;
	HANDLE SymHandle = 0;
	DWORD bytes = 0;

	RtlSecureZeroMemory(&ustr1, sizeof(ustr1));
	RtlInitUnicodeString(&ustr1, Objeto);

	InitializeObjectAttributes(&ObjectAttributes, &ustr1, OBJ_CASE_INSENSITIVE, NULL, NULL);
	st = ZwOpenDirectoryObject(&Objhandle, DIRECTORY_QUERY, &ObjectAttributes);
	if (!NT_SUCCESS(st))
		return ret;
	do
	{
		info = (POBJECT_DIRECTORY_INFORMATION)RtlAllocateHeap(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
		if (info == NULL)
			break;

		while ((st = ZwQueryDirectoryObject(Objhandle, info, size, FALSE, ret, &context, &length)) == STATUS_MORE_ENTRIES)
		{
			if (info[0].Name.Buffer)
				break;
			RtlFreeHeap(GetProcessHeap(), 0, info);
			size *= 2;
			info = (POBJECT_DIRECTORY_INFORMATION)RtlAllocateHeap(GetProcessHeap(), 0, size);
		}

		SIZE_T i = 0;
		for (i = 0; info->Name.Buffer[0] != NULL; ++i)
		{
			POBJECT_DIRECTORY_INFORMATION buffer;
			buffer = &info[i];
			if (buffer->Name.Buffer == NULL)
				break;

			if (wcscmp(bufferObj, buffer->TypeName.Buffer) == 0)
			{
				int item = 0;
				CString name, type;

				name.Format(_T("%ws"), buffer->Name.Buffer);
				type.Format(_T("%ws"), buffer->TypeName.Buffer);

				m_object.InsertItem(0, name);
				m_object.SetItemText(item, 1, type);


				if (wcscmp(Objeto, L"\\ArcName") == 0)
				{
					wcscpy_s(DataBuffer, sizeof(DataBuffer), L"\\ArcName\\");
				}
				else
				{
					wcscpy_s(DataBuffer, sizeof(DataBuffer), L"\\GLOBAL??\\");
				}
		
				wcscat_s(DataBuffer, sizeof(DataBuffer), buffer->Name.Buffer);		

				RtlInitUnicodeString(&DataUni, DataBuffer);
				InitializeObjectAttributes(&ObjectAttributesData, &DataUni, OBJ_CASE_INSENSITIVE, NULL, NULL);

				st = ZwOpenSymbolicLinkObject(&SymHandle, GENERIC_READ, &ObjectAttributesData);
				if (NT_SUCCESS(st))
				{
					SymUni.Length = 0;
					SymUni.MaximumLength = 256;
					SymUni.Buffer = (PWSTR)RtlAllocateHeap(GetProcessHeap(), HEAP_ZERO_MEMORY, SymUni.MaximumLength);


					st = ZwQuerySymbolicLinkObject(SymHandle, &SymUni, &bytes);
					if (NT_SUCCESS(st))
					{
						CString addinfo;
						addinfo.Format(_T("%ws"), SymUni.Buffer);
						m_object.SetItemText(item, 2, addinfo);	
					
					}
				}

			}

		}

		if (st != STATUS_MORE_ENTRIES)
			break;

		ret = FALSE;


	} while (ret);


	if (Objhandle != NULL) {
		NtClose(Objhandle);
	}

	if (SymHandle != NULL) {
		NtClose(SymHandle);
	}
	if (info) {
		RtlFreeHeap(GetProcessHeap(), 0, info);
	}

	return ret;
}


void KemmObj::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
#ifndef _WIN64
	CMenu menu;
	VERIFY(menu.LoadMenuW(IDR_MENU2));
	CMenu *pop = menu.GetSubMenu(0);

	UINT select = m_object.GetNextItem(-1, LVNI_SELECTED);
	RECT rect;
	CPoint point;

	GetWindowRect(&rect);
	GetCursorPos(&point);
	pop->TrackPopupMenu(NULL, point.x, point.y, this);
#endif _WIN64
}


void KemmObj::OnContextMenuObjOperation(CString wzPrefix, ULONG_PTR ControlCode)
{
	NTSTATUS st;
	POSITION pos = m_object.GetFirstSelectedItemPosition();
	while (pos)
	{
		UINT item = m_object.GetNextSelectedItem(pos);
		CString str = m_object.GetItemText(item, 0);

		wzPrefix += L"\\";
		wzPrefix += str;
		if (wzPrefix.GetBuffer() != NULL)
		{
			st = NtDeviceIoControlFile(g_device,
				NULL,
				NULL, NULL,
				&io,
				ControlCode,
				(PVOID)wzPrefix.GetBuffer(),
				wzPrefix.GetLength(),
				NULL,
				0);
			if (st == STATUS_INVALID_HANDLE)
				::MessageBox(NULL, L"Error enviando orden", L"Error", MB_ICONERROR);
			else
			{
				::MessageBox(NULL, L"String enviado al driver", L"KEMM", MB_ICONINFORMATION);
			}
		}
	}
}

void KemmObj::OnOnkobjectProtegerdispositivo()
{
	// TODO: Add your command handler code here

	OnContextMenuObjOperation(L"\\Device", HOOK_ACCESO_DEVICE);
}


void KemmObj::OnOnkobjectProtegerlinksimbolico()
{
	// TODO: Add your command handler code here
	OnContextMenuObjOperation(L"\\GLOBAL??", HOOK_LINK_SIMBOLICO);
}


