// CDriverProcessDialog.cpp: 实现文件
//

#include "pch.h"
#include "TaskManager.h"
#include "CDriverProcessDialog.h"
#include "afxdialogex.h"
#include"CDriverTool.h"
#include"resource.h"
#include"CDriverThreadDialog.h"
#include"CDriverModuleDialog.h"
#include"CStopProcessDialog.h"
// CDriverProcessDialog 对话框

IMPLEMENT_DYNAMIC(CDriverProcessDialog, CDialogEx)

CDriverProcessDialog::CDriverProcessDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(DriverProcessDialog, pParent)
{

}

CDriverProcessDialog::~CDriverProcessDialog()
{
}

void CDriverProcessDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, DriverProcessList);
}


BEGIN_MESSAGE_MAP(CDriverProcessDialog, CDialogEx)
	ON_COMMAND(ID_32800, &CDriverProcessDialog::HideDriverProcess)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CDriverProcessDialog::OnNMRClickList1)
	ON_COMMAND(ID_32801, &CDriverProcessDialog::DelProcess)
	ON_COMMAND(ID_32802, &CDriverProcessDialog::QueryThread)
	ON_COMMAND(ID_32803, &CDriverProcessDialog::QueryModule)
	ON_COMMAND(ID_32804, &CDriverProcessDialog::ShuaXin)
	ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_32814, &CDriverProcessDialog::StopProcessCreat)
	ON_COMMAND(ID_32816, &CDriverProcessDialog::CloseHook)
END_MESSAGE_MAP()


// CDriverProcessDialog 消息处理程序
// 初始化函数
BOOL CDriverProcessDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  在此添加额外的初始化
	CDriverTool::InitProcessList(DriverProcessList);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// 隐藏进程
void CDriverProcessDialog::HideDriverProcess()
{
	// TODO: 在此添加命令处理程序代码
	//获取要隐藏的进程ID
	DWORD nNow = DriverProcessList.GetSelectionMark();
	CString BaseName = DriverProcessList.GetItemText(nNow, 0);
	ULONG ProcessId = _tcstoul(BaseName, 0, 10);
	HANDLE hDevice = NULL;
	CDriverTool::ConnectDriver(hDevice);
	if (hDevice == NULL)
		return;
	//向0环发送隐藏进程请求
	CDriverTool::HideProcess(hDevice, &ProcessId);
	CloseHandle(hDevice);
}

// 弹出菜单
void CDriverProcessDialog::OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	// TODO: 在此添加控件通知处理程序代码
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	POINT point;
	GetCursorPos(&point);

	// 2. 加载需要弹出的菜单项
	HMENU hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU3));
	HMENU hSubMenu = GetSubMenu(hMenu, 0);

	// 3. 通过函数弹出菜单
	TrackPopupMenu(hSubMenu, TPM_LEFTALIGN, point.x, point.y, 0, this->m_hWnd, NULL);
}

// 结束进程
void CDriverProcessDialog::DelProcess()
{
	// TODO: 在此添加命令处理程序代码
	DWORD nNow = DriverProcessList.GetSelectionMark();
	CString BaseName = DriverProcessList.GetItemText(nNow, 0);
	ULONG ProcessId = _tcstoul(BaseName, 0, 10);
	HANDLE hDevice = NULL;
	CDriverTool::ConnectDriver(hDevice);
	if (hDevice == NULL)
		return;
	//向0环发送结束进程请求
	CDriverTool::DelProcess(hDevice, &ProcessId);
	CloseHandle(hDevice);
}

// 弹出遍历线程的框
void CDriverProcessDialog::QueryThread()
{
	// TODO: 在此添加命令处理程序代码
	DWORD nNow = DriverProcessList.GetSelectionMark();
	CString BaseName = DriverProcessList.GetItemText(nNow, 0);
	ULONG pEprocess = _tcstoul(BaseName, 0, 10);

	CDriverThreadDialog ThreadDialog1(pEprocess);
	ThreadDialog1.DoModal();
}

// 弹出遍历模块的框
void CDriverProcessDialog::QueryModule()
{
	// TODO: 在此添加命令处理程序代码
	DWORD nNow = DriverProcessList.GetSelectionMark();
	CString BaseName = DriverProcessList.GetItemText(nNow, 2);
	ULONG pEprocess = _tcstoul(BaseName, 0, 16);

	CDriverModuleDialog ModuleDialog1(pEprocess);
	ModuleDialog1.DoModal();
}

// 刷新
void CDriverProcessDialog::ShuaXin()
{
	// TODO: 在此添加命令处理程序代码
	DriverProcessList.DeleteAllItems();
	HANDLE hDevice = NULL;
	CDriverTool::ConnectDriver(hDevice);
	if (hDevice == NULL)
		return;
	CDriverTool::UserQueryProcess(hDevice, DriverProcessList);
	CloseHandle(hDevice);
}


void CDriverProcessDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow)
	{
		DriverProcessList.DeleteAllItems();
		// TODO:  在此添加额外的初始化
		HANDLE hDevice = NULL;
		CDriverTool::ConnectDriver(hDevice);
		if (hDevice == NULL)
			return;
		CDriverTool::UserQueryProcess(hDevice, DriverProcessList);
		CloseHandle(hDevice);
	}
	// TODO: 在此处添加消息处理程序代码
}

// 让指定进程无法创建
void CDriverProcessDialog::StopProcessCreat()
{
	// TODO: 在此添加命令处理程序代码
	CStopProcessDialog StopProcessDialog1;
	StopProcessDialog1.DoModal();
}


void CDriverProcessDialog::CloseHook()
{
	// TODO: 在此添加命令处理程序代码
	HANDLE hDevice = NULL;
	CDriverTool::ConnectDriver(hDevice);
	if (hDevice == NULL)
		return;
	CDriverTool::CloseHook(hDevice);
	CloseHandle(hDevice);
}
