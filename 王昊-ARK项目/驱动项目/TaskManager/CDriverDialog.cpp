// CDriverDialog.cpp: 实现文件
//

#include "pch.h"
#include "TaskManager.h"
#include "CDriverDialog.h"
#include "afxdialogex.h"
#include"CDriverTool.h"


// CDriverDialog 对话框

IMPLEMENT_DYNAMIC(CDriverDialog, CDialogEx)

CDriverDialog::CDriverDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG18, pParent)
{

}

CDriverDialog::~CDriverDialog()
{
}

void CDriverDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, DriverList);
}


BEGIN_MESSAGE_MAP(CDriverDialog, CDialogEx)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CDriverDialog::OnNMRClickList1)
	ON_COMMAND(ID_32808, &CDriverDialog::HideDriver)
	ON_COMMAND(ID_32809, &CDriverDialog::MyUpdate)
	ON_COMMAND(ID_32811, &CDriverDialog::ProtectProcess)
	ON_COMMAND(ID_32812, &CDriverDialog::FanTiaoshiProcess)
	ON_COMMAND(ID_32817, &CDriverDialog::OnNeiHeChongZai)
	ON_COMMAND(ID_32818, &CDriverDialog::CloseNeiHeChongZai)
	ON_COMMAND(ID_32821, &CDriverDialog::CancleProtect)
END_MESSAGE_MAP()


// CDriverDialog 消息处理程序

// 初始化函数
BOOL CDriverDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  在此添加额外的初始化
	CDriverTool::InitDriverList(DriverList);
	HANDLE hDevice = NULL;
	CDriverTool::ConnectDriver(hDevice);
	if (hDevice == NULL)
		return FALSE;
	CDriverTool::UserQueryDriver(hDevice, DriverList);
	CloseHandle(hDevice);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// 弹出菜单
void CDriverDialog::OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	POINT point;
	GetCursorPos(&point);

	// 2. 加载需要弹出的菜单项
	HMENU hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU3));
	HMENU hSubMenu = GetSubMenu(hMenu, 3);

	// 3. 通过函数弹出菜单
	TrackPopupMenu(hSubMenu, TPM_LEFTALIGN, point.x, point.y, 0, this->m_hWnd, NULL);
	*pResult = 0;
}

// 隐藏驱动
void CDriverDialog::HideDriver()
{
	// TODO: 在此添加命令处理程序代码
	//获取要隐藏的驱动名
	DWORD nNow = DriverList.GetSelectionMark();
	CString BaseName = DriverList.GetItemText(nNow, 0);
	HANDLE hDevice = NULL;
	CDriverTool::ConnectDriver(hDevice);
	if (hDevice == NULL)
		return;
	//向0环发送隐藏驱动请求
	CDriverTool::UserHideDriver(hDevice,BaseName.GetBuffer(0));
	CloseHandle(hDevice);
}

// 刷新
void CDriverDialog::MyUpdate()
{
	// TODO: 在此添加命令处理程序代码
	DriverList.DeleteAllItems();
	HANDLE hDevice = NULL;
	CDriverTool::ConnectDriver(hDevice);
	if (hDevice == NULL)
		return;
	CDriverTool::UserQueryDriver(hDevice, DriverList);
	CloseHandle(hDevice);
}

// 保护该进程不被调试
void CDriverDialog::ProtectProcess()
{
	// TODO: 在此添加命令处理程序代码
	HANDLE hDevice = NULL;
	CDriverTool::ConnectDriver(hDevice);
	if (hDevice == NULL)
		return;
	CDriverTool::SYSENTER_HOOK(hDevice);
	CloseHandle(hDevice);
}


void CDriverDialog::FanTiaoshiProcess()
{
	// TODO: 在此添加命令处理程序代码
	HANDLE hDevice = NULL;
	CDriverTool::ConnectDriver(hDevice);
	if (hDevice == NULL)
		return;
	CDriverTool::FanTiao(hDevice);
	CloseHandle(hDevice);
}

// 开启内核重载
void CDriverDialog::OnNeiHeChongZai()
{
	// TODO: 在此添加命令处理程序代码
	MessageBox(L"转至SSDT表中开启");
;	
}

// 关闭内核重载
void CDriverDialog::CloseNeiHeChongZai()
{
	MessageBox(L"转至SSDT表中开启");
}

// 取消保护
void CDriverDialog::CancleProtect()
{
	// TODO: 在此添加命令处理程序代码
	HANDLE hDevice = NULL;
	CDriverTool::ConnectDriver(hDevice);
	if (hDevice == NULL)
		return;
	DWORD size = 0;
	DeviceIoControl(hDevice, CTL_CancleProtectDriver, 0, 0, NULL, 0, &size, NULL);
	CloseHandle(hDevice);
}
