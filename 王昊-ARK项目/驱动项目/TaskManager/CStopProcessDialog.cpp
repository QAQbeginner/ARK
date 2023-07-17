// CStopProcessDialog.cpp: 实现文件
//

#include "pch.h"
#include "TaskManager.h"
#include "CStopProcessDialog.h"
#include "afxdialogex.h"
#include "CDriverTool.h"


// CStopProcessDialog 对话框

IMPLEMENT_DYNAMIC(CStopProcessDialog, CDialogEx)

CStopProcessDialog::CStopProcessDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(StopProcessDialog, pParent)
	, StopProcessString(_T(""))
{

}

CStopProcessDialog::~CStopProcessDialog()
{
}

void CStopProcessDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, StopProcessString);
}


BEGIN_MESSAGE_MAP(CStopProcessDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CStopProcessDialog::OnBnClickedButton1)
END_MESSAGE_MAP()


// CStopProcessDialog 消息处理程序

// 向0环发出让指定进程无法创建的消息
void CStopProcessDialog::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	HANDLE hDevice = NULL;
	CDriverTool::ConnectDriver(hDevice);
	if (hDevice == NULL)
		return;
	DWORD Select = MessageBox(StopProcessString, 0, MB_OKCANCEL);
	if (Select == IDOK)
	{
		CDriverTool::StopProcess(hDevice, StopProcessString.GetBuffer(), (StopProcessString.GetLength() + 1) * 2);
	}
	CloseHandle(hDevice);
}

