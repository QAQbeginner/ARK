// CStopFileDialog.cpp: 实现文件
//

#include "pch.h"
#include "TaskManager.h"
#include "CStopFileDialog.h"
#include "afxdialogex.h"
#include"CDriverTool.h"


// CStopFileDialog 对话框

IMPLEMENT_DYNAMIC(CStopFileDialog, CDialogEx)

CStopFileDialog::CStopFileDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(StopFileDialog, pParent)
	, StopFileString(_T(""))
{

}

CStopFileDialog::~CStopFileDialog()
{
}

void CStopFileDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, StopFileName);
	DDX_Text(pDX, IDC_EDIT1, StopFileString);
}


BEGIN_MESSAGE_MAP(CStopFileDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CStopFileDialog::OnBnClickedButton1)
END_MESSAGE_MAP()


// CStopFileDialog 消息处理程序
// 让指定文件无法创建
void CStopFileDialog::OnBnClickedButton1()
{
	//(LastPath.GetLength() + 1) * 2
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	HANDLE hDevice = NULL;
	CDriverTool::ConnectDriver(hDevice);
	if (hDevice == NULL)
		return;
	DWORD Select=MessageBox(StopFileString,0,MB_OKCANCEL);
	if (Select== IDOK)
	{
		CDriverTool::StopFile(hDevice, StopFileString.GetBuffer(), (StopFileString.GetLength() + 1) * 2);
	}
	CloseHandle(hDevice);
}
