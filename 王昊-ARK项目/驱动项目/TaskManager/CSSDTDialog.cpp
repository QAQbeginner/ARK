// CSSDTDialog.cpp: 实现文件
//

#include "pch.h"
#include "TaskManager.h"
#include "CSSDTDialog.h"
#include "afxdialogex.h"
#include"CDriverTool.h"


// CSSDTDialog 对话框

IMPLEMENT_DYNAMIC(CSSDTDialog, CDialogEx)

CSSDTDialog::CSSDTDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(SSDTDialog, pParent)
{

}

CSSDTDialog::~CSSDTDialog()
{
}

void CSSDTDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, SSDT_List, SSDTList);
}


BEGIN_MESSAGE_MAP(CSSDTDialog, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_RCLICK, SSDT_List, &CSSDTDialog::PopMenu)
	ON_COMMAND(ID_SSDT_32819, &CSSDTDialog::OpenNeiHe)
	ON_COMMAND(ID_SSDT_32820, &CSSDTDialog::CloseNeiHe)
END_MESSAGE_MAP()


// CSSDTDialog 消息处理程序

BOOL CSSDTDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CDriverTool::InitSSDTList(SSDTList);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CSSDTDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow)
	{
		HANDLE hDevice = NULL;
		CDriverTool::ConnectDriver(hDevice);
		if (hDevice == NULL)
			return;
		SSDTList.DeleteAllItems();
		CDriverTool::UserQuerySSDT(hDevice, SSDTList);
		CloseHandle(hDevice);
	}
	// TODO: 在此处添加消息处理程序代码
}

// 右键弹出菜单
void CSSDTDialog::PopMenu(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	POINT point;
	GetCursorPos(&point);

	// 2. 加载需要弹出的菜单项
	HMENU hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU3));
	HMENU hSubMenu = GetSubMenu(hMenu, 4);

	// 3. 通过函数弹出菜单
	TrackPopupMenu(hSubMenu, TPM_LEFTALIGN, point.x, point.y, 0, this->m_hWnd, NULL);
	*pResult = 0;
}

// 开启内核重载
void CSSDTDialog::OpenNeiHe()
{
	// TODO: 在此添加命令处理程序代码
	HANDLE hDevice = NULL;
	CDriverTool::ConnectDriver(hDevice);
	if (hDevice == NULL)
		return;
	CDriverTool::OpenNeiHe(hDevice, SSDTList);
	CloseHandle(hDevice);
}

// 关闭内核重载
void CSSDTDialog::CloseNeiHe()
{
	// TODO: 在此添加命令处理程序代码
	HANDLE hDevice = NULL;
	CDriverTool::ConnectDriver(hDevice);
	if (hDevice == NULL)
		return;
	CDriverTool::CloseNeiHe(hDevice);
	CloseHandle(hDevice);
}
