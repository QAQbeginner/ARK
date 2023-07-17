// CGDTDialog.cpp: 实现文件
//

#include "pch.h"
#include "TaskManager.h"
#include "CGDTDialog.h"
#include "afxdialogex.h"
#include"CDriverTool.h"

// CGDTDialog 对话框

IMPLEMENT_DYNAMIC(CGDTDialog, CDialogEx)

CGDTDialog::CGDTDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(GDTDilalog, pParent)
{

}

CGDTDialog::~CGDTDialog()
{
}

void CGDTDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, GDTList);
}


BEGIN_MESSAGE_MAP(CGDTDialog, CDialogEx)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CGDTDialog 消息处理程序
// 初始化
BOOL CGDTDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CDriverTool::InitGDTList(GDTList);
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// 窗口显示
void CGDTDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow)
	{
		GDTList.DeleteAllItems();
		HANDLE hDevice = NULL;
		CDriverTool::ConnectDriver(hDevice);
		CDriverTool::UserQueryGDT(hDevice,GDTList);
		CloseHandle(hDevice);
	}
	// TODO: 在此处添加消息处理程序代码
}
