// CIDTDialog.cpp: 实现文件
//

#include "pch.h"
#include "TaskManager.h"
#include "CIDTDialog.h"
#include "afxdialogex.h"
#include"CDriverTool.h"

// CIDTDialog 对话框

IMPLEMENT_DYNAMIC(CIDTDialog, CDialogEx)

CIDTDialog::CIDTDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(DriverIDTDialog, pParent)
{

}

CIDTDialog::~CIDTDialog()
{
}

void CIDTDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, IDTList);
}


BEGIN_MESSAGE_MAP(CIDTDialog, CDialogEx)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CIDTDialog 消息处理程序
// 初始化
BOOL CIDTDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CDriverTool::InitIDTList(IDTList);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CIDTDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow)
	{
		IDTList.DeleteAllItems();
		HANDLE hDevice = NULL;
		CDriverTool::ConnectDriver(hDevice);
		if (hDevice == NULL)
			return;
		CDriverTool::UserQueryIDT(hDevice, IDTList);
		CloseHandle(hDevice);
	}
	// TODO: 在此处添加消息处理程序代码
}
