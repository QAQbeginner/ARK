// CDriverRegDialog.cpp: 实现文件
//

#include "pch.h"
#include "TaskManager.h"
#include "CDriverRegDialog.h"
#include "afxdialogex.h"
#include"CDriverTool.h"
#include"CCreateRegDialog.h"

// CDriverRegDialog 对话框

IMPLEMENT_DYNAMIC(CDriverRegDialog, CDialogEx)

CDriverRegDialog::CDriverRegDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(DriverRegDialog, pParent)
{

}

CDriverRegDialog::~CDriverRegDialog()
{
}

void CDriverRegDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, DriverRegList);
	DDX_Control(pDX, IDC_TREE1, RegTree);
}


BEGIN_MESSAGE_MAP(CDriverRegDialog, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CDriverRegDialog::OnNMClickTree1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CDriverRegDialog::OnNMRClickList1)
	ON_COMMAND(ID_32806, &CDriverRegDialog::CreateReg)
	ON_NOTIFY(NM_RCLICK, IDC_TREE1, &CDriverRegDialog::OnNMRClickTree1)
	ON_COMMAND(ID_32807, &CDriverRegDialog::DelReg)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CDriverRegDialog::OnTvnSelchangedTree1)
END_MESSAGE_MAP()


// CDriverRegDialog 消息处理程序
// 遍历注册表
void CDriverRegDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	// TODO: 在此处添加消息处理程序代码
	if (bShow)
	{

	}
}

// 初始化
BOOL CDriverRegDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  在此添加额外的初始化
	CDriverTool::InitRegList(DriverRegList);

	//初始化数控件
	HTREEITEM hItem1 = RegTree.InsertItem(L"我的电脑", NULL);
	//设定指定项的数据
	CString Root = L"\\Registry";
	wchar_t* pBuff = _wcsdup(Root);
	RegTree.SetItemData(hItem1, (DWORD_PTR)pBuff);
	HANDLE hDevice = NULL;
	CDriverTool::ConnectDriver(hDevice);
	if (hDevice != NULL)
		CDriverTool::UserQueryReg(hDevice, RegTree, Root.GetBuffer(), (Root.GetLength() + 1) * 2, hItem1);
	CloseHandle(hDevice);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// 左键单击
void CDriverRegDialog::OnNMClickTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

// 弹出菜单[错的]
void CDriverRegDialog::OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

// 点击菜单选项 创建子项
void CDriverRegDialog::CreateReg()
{
	// TODO: 在此添加命令处理程序代码
	//获取选中的树控件的项
		//获取选中的树控件的项
	CCreateRegDialog CreateRegDialog1(m_KeyName);
	CreateRegDialog1.DoModal();
}

// 弹出菜单
void CDriverRegDialog::OnNMRClickTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	// 获取选中的树控件的项
	CString Path = (WCHAR*)RegTree.GetItemData(RegTree.GetSelectedItem());
	if (Path.IsEmpty() ||
		Path == L"\\Registry")
	{
		return;
	}
	m_KeyName = Path;

	POINT point;
	GetCursorPos(&point);
	// MessageBox(Path, Path);
	// 2. 加载需要弹出的菜单项
	HMENU hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU3));
	HMENU hSubMenu = GetSubMenu(hMenu, 2);

	// 3. 通过函数弹出菜单
	TrackPopupMenu(hSubMenu, TPM_LEFTALIGN, point.x, point.y, 0, this->m_hWnd, NULL);
	*pResult = 0;
}

// 删除注册表子项
void CDriverRegDialog::DelReg()
{
	// TODO: 在此添加命令处理程序代码
	HANDLE hDevice = NULL;
	CDriverTool::ConnectDriver(hDevice);
	if (hDevice == NULL)
		return;
	CString LastPath;
	LastPath = m_KeyName + L"\\";
	BOOL Select=MessageBox(LastPath, L"确认删除？",MB_OKCANCEL);
	if (Select)
	{
		//删除注册表
		CDriverTool::UserDelReg(hDevice, LastPath.GetBuffer(), (LastPath.GetLength() + 1) * 2);
	}
	CloseHandle(hDevice);
}

// 切换选项
void CDriverRegDialog::OnTvnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	HTREEITEM hItem = RegTree.GetSelectedItem();
	if (!hItem)
	{
		return;
	}
	//获取选中的数控件的项的数据（之前设置为对应文件的路径）
	CString Path = (WCHAR*)RegTree.GetItemData(RegTree.GetSelectedItem());
	if (Path.IsEmpty() ||
		Path == L"\\Registry")
	{
		return;
	}
	HTREEITEM hChild = RegTree.GetChildItem(hItem);
	HTREEITEM hTempItem;
	while (hChild)//如果有子项
	{
		hTempItem = hChild;
		hChild = RegTree.GetNextSiblingItem(hChild);//得到下一个子项
		RegTree.DeleteItem(hTempItem);//删除它
	}

	//清空list列表
	DriverRegList.DeleteAllItems();
	HANDLE hDevice = NULL;
	CDriverTool::ConnectDriver(hDevice);
	if (hDevice == NULL)
		return;
	// 遍历子项
	CDriverTool::UserQueryChildReg(hDevice, RegTree, Path.GetBuffer(), 2 * (Path.GetLength() + 1), hItem, Path, DriverRegList);
	CloseHandle(hDevice);
	*pResult = 0;
}
