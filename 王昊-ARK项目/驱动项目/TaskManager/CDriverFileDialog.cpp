// CDriverFileDialog.cpp: 实现文件
//

#include "pch.h"
#include "TaskManager.h"
#include "CDriverFileDialog.h"
#include "afxdialogex.h"
#include"CDriverTool.h"
#include<strsafe.h>
#include"CStopFileDialog.h"

// CDriverFileDialog 对话框

IMPLEMENT_DYNAMIC(CDriverFileDialog, CDialogEx)

CDriverFileDialog::CDriverFileDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(DriverFileDialog, pParent)
{

}

CDriverFileDialog::~CDriverFileDialog()
{
}

void CDriverFileDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, DriverFileList);
    DDX_Control(pDX, IDC_EDIT2, FilePathEdit);
    DDX_Control(pDX, IDC_RADIO1, FilePathButton);
}


BEGIN_MESSAGE_MAP(CDriverFileDialog, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_RADIO1, &CDriverFileDialog::OnBnClickedRadio1)
    ON_BN_CLICKED(IDC_BUTTON1, &CDriverFileDialog::OnBnClickedButton1)
    ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CDriverFileDialog::OnNMRClickList1)
    ON_COMMAND(ID_32805, &CDriverFileDialog::DelFile)
    ON_COMMAND(ID_32813, &CDriverFileDialog::StopFileCreat)
    ON_COMMAND(ID_32815, &CDriverFileDialog::CloseHOOK)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CDriverFileDialog::MouseDouble)
END_MESSAGE_MAP()


// CDriverFileDialog 消息处理程序
// 进行初始化
void CDriverFileDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow)
	{
        DriverFileList.DeleteAllItems();
	}
	// TODO: 在此处添加消息处理程序代码
}
/*
    获取指定目录下路径
*/
CString GetPathFrom1()
{
    TCHAR szBuffer[MAX_PATH] = { 0 };
    BROWSEINFO bi;
    ZeroMemory(&bi, sizeof(BROWSEINFO));
    bi.hwndOwner = NULL;
    bi.pszDisplayName = szBuffer;
    bi.lpszTitle = _T("从下面选文件夹目录:");
    bi.ulFlags = BIF_RETURNFSANCESTORS;
    LPITEMIDLIST idl = SHBrowseForFolder(&bi);
    if (NULL == idl)
    {
        return CString("");
    }
    SHGetPathFromIDList(idl, szBuffer);
    return szBuffer;
}

// 选择路径
void CDriverFileDialog::OnBnClickedRadio1()
{
	// TODO: 在此添加控件通知处理程序代码
    m_ThisPath = GetPathFrom1();
    if (m_ThisPath == TEXT(""))
    {
        FilePathButton.SetCheck(FALSE);
        return;
    }
    FilePathEdit.SetWindowText(m_ThisPath);
}

// 遍历文件
void CDriverFileDialog::OnBnClickedButton1()
{
    // TODO: 在此添加控件通知处理程序代码
    DriverFileList.DeleteAllItems();
    HANDLE hDevice = NULL;
    CDriverTool::ConnectDriver(hDevice);
    if (hDevice == NULL)
        return;
    CDriverTool::UserQueryFile(hDevice, DriverFileList, m_ThisPath.GetBuffer(), (m_ThisPath.GetLength()+1)* sizeof(TCHAR));
    CloseHandle(hDevice);
}

// 初始化
BOOL CDriverFileDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  在此添加额外的初始化
    CDriverTool::InitFileList(DriverFileList);
    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

// 弹出文件菜单
void CDriverFileDialog::OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    *pResult = 0;
    POINT point;
    GetCursorPos(&point);

    // 2. 加载需要弹出的菜单项
    HMENU hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU3));
    HMENU hSubMenu = GetSubMenu(hMenu, 1);

    // 3. 通过函数弹出菜单
    TrackPopupMenu(hSubMenu, TPM_LEFTALIGN, point.x, point.y, 0, this->m_hWnd, NULL);
}

// 删除文件
void CDriverFileDialog::DelFile()
{
    // TODO: 在此添加命令处理程序代码
    //获取到列表名字
    HANDLE hDevice = NULL;
    CDriverTool::ConnectDriver(hDevice);
    if (hDevice == NULL)
        return;
    DWORD nNow = DriverFileList.GetSelectionMark();
    CString BaseName = DriverFileList.GetItemText(nNow, 1);
    //拼接
    WCHAR szFullPath[MAX_PATH] = { 0 };
    StringCbPrintf(szFullPath, MAX_PATH, L"%s\\%s", m_ThisPath, BaseName);

    //向0环发送删除文件请求
    CDriverTool::UserDelFile(hDevice,szFullPath, wcslen(szFullPath) * 2 + 2);
    CloseHandle(hDevice);
}

// 让指定文件无法创建
void CDriverFileDialog::StopFileCreat()
{
    // TODO: 在此添加命令处理程序代码
    CStopFileDialog StopDialog;
    StopDialog.DoModal();
}

// 关闭HOOK
void CDriverFileDialog::CloseHOOK()
{
    // TODO: 在此添加命令处理程序代码
    HANDLE hDevice = NULL;
    CDriverTool::ConnectDriver(hDevice);
    if (hDevice == NULL)
        return;
    CDriverTool::CloseHook(hDevice);
    CloseHandle(hDevice);
}

// 双击鼠标左键，进入文件进行遍历
void CDriverFileDialog::MouseDouble(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    HANDLE hDevice = NULL;
    CDriverTool::ConnectDriver(hDevice);
    if (hDevice == NULL)
        return;
    CString Buffer;
    // 获取被选中项
    for (int i = 0; i < DriverFileList.GetItemCount(); i++)
    {
        if (DriverFileList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
        {
            Buffer.Format(L"%s%s", m_ThisPath.GetBuffer(),DriverFileList.GetItemText(i, 1));
            break;
        }
    }
    DriverFileList.DeleteAllItems();

    CDriverTool::UserQueryFile(hDevice, DriverFileList, Buffer.GetBuffer(), (Buffer.GetLength() + 1) * sizeof(TCHAR));
    FilePathEdit.SetWindowText(Buffer.GetBuffer());
    m_ThisPath.Format(L"%s\\",Buffer.GetBuffer());
    // 测试
    // MessageBox(m_ThisPath, 0);
    CloseHandle(hDevice);



    *pResult = 0;
}
