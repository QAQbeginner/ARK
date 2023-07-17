// CDriverModuleDialog.cpp: 实现文件
//

#include "pch.h"
#include "TaskManager.h"
#include "CDriverModuleDialog.h"
#include "afxdialogex.h"
#include"CDriverTool.h"

// CDriverModuleDialog 对话框

IMPLEMENT_DYNAMIC(CDriverModuleDialog, CDialogEx)

CDriverModuleDialog::CDriverModuleDialog(ULONG pEprocess,CWnd* pParent /*=nullptr*/)
	: CDialogEx(DriverModuleDialog, pParent), my_pEprocess1(pEprocess)
{

}

CDriverModuleDialog::~CDriverModuleDialog()
{
}

void CDriverModuleDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, DriverModuleList);
}


BEGIN_MESSAGE_MAP(CDriverModuleDialog, CDialogEx)
END_MESSAGE_MAP()


// CDriverModuleDialog 消息处理程序

BOOL CDriverModuleDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  在此添加额外的初始化
	CDriverTool::InitModuleLst(DriverModuleList);
	HANDLE hDevice = NULL;
	CDriverTool::ConnectDriver(hDevice);
	if (hDevice == NULL)
		return FALSE;
	CDriverTool::UserQueryModule(hDevice, DriverModuleList, &my_pEprocess1);
	CloseHandle(hDevice);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
