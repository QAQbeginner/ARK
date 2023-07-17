// CDriverThreadDialog.cpp: 实现文件
//

#include "pch.h"
#include "TaskManager.h"
#include "CDriverThreadDialog.h"
#include "afxdialogex.h"
#include"CDriverTool.h"

// CDriverThreadDialog 对话框

IMPLEMENT_DYNAMIC(CDriverThreadDialog, CDialogEx)

CDriverThreadDialog::CDriverThreadDialog(ULONG pEprocess,CWnd* pParent /*=nullptr*/)
	: CDialogEx(DriverThreadDialog, pParent), my_pEprocess(pEprocess)
{

}

CDriverThreadDialog::~CDriverThreadDialog()
{
}

void CDriverThreadDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, ThreaList, DriverThreadList);
}


BEGIN_MESSAGE_MAP(CDriverThreadDialog, CDialogEx)
END_MESSAGE_MAP()


// CDriverThreadDialog 消息处理程序

BOOL CDriverThreadDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CDriverTool::InitThreadList(DriverThreadList);
	// TODO:  在此添加额外的初始化
	HANDLE hDevice = NULL;
	CDriverTool::ConnectDriver(hDevice);
	if (hDevice == NULL)
		return FALSE;
	CDriverTool::UserUueryThread(hDevice, DriverThreadList,&my_pEprocess);
	CloseHandle(hDevice);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
