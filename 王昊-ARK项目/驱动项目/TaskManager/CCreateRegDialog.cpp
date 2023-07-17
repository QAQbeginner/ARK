// CCreateRegDialog.cpp: 实现文件
//

#include "pch.h"
#include "TaskManager.h"
#include "CCreateRegDialog.h"
#include "afxdialogex.h"
#include"CDriverTool.h"


// CCreateRegDialog 对话框

IMPLEMENT_DYNAMIC(CCreateRegDialog, CDialogEx)

CCreateRegDialog::CCreateRegDialog(CString KeyName, CWnd* pParent /*=nullptr*/)
	: CDialogEx(CreateRegDialog, pParent),my_KeyName(KeyName)
	, RegName(_T(""))
{

}

CCreateRegDialog::~CCreateRegDialog()
{
}

void CCreateRegDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, RegName);
}


BEGIN_MESSAGE_MAP(CCreateRegDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CCreateRegDialog::OnBnClickedButton1)
END_MESSAGE_MAP()


// CCreateRegDialog 消息处理程序
// 添加注册表子项
void CCreateRegDialog::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	HANDLE hDevice = NULL;
	CDriverTool::ConnectDriver(hDevice);
	if (hDevice == NULL)
		return;
	CString LastPath;
	LastPath = my_KeyName + L"\\" + RegName + L"\\";
	BOOL a=MessageBox(LastPath,L"是否创建",MB_OKCANCEL);
	if (a)
	{
		//创建注册表
		CDriverTool::UserCreateReg(hDevice, LastPath.GetBuffer(), (LastPath.GetLength() + 1) * 2);
	}
	CloseHandle(hDevice);
	CDialogEx::OnOK();
}
