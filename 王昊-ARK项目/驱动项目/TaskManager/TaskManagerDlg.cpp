
// TaskManagerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "TaskManager.h"
#include "TaskManagerDlg.h"
#include "afxdialogex.h"

#include<PowrProf.h>
#pragma comment(lib,"PowrProf.lib")
#include"CDriverDialog.h"
#include"CDriverProcessDialog.h"
#include"CDriverFileDialog.h"
#include"CDriverRegDialog.h"
#include"CIDTDialog.h"
#include"CGDTDialog.h"
#include"CSSDTDialog.h"

/*
	定义一个全局变量用于函数内部切换窗口
	再定义一个全局变量用于保存到指定进程窗口的ID
*/

DWORD MyProcessID;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void On32787();
//	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_COMMAND(ID_32787, &CAboutDlg::On32787)
END_MESSAGE_MAP()


// CTaskManagerDlg 对话框



CTaskManagerDlg::CTaskManagerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TASKMANAGER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTaskManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, TabItem);
}

BEGIN_MESSAGE_MAP(CTaskManagerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CTaskManagerDlg::OnTcnSelchangeTab1)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CTaskManagerDlg 消息处理程序
HANDLE hDevice;
BOOL CTaskManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 老板键设置
	::RegisterHotKey(this->m_hWnd, 0x1234, MOD_CONTROL | MOD_SHIFT, 'L');

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	/*
	初始化状态栏
	*/
	My_StasusBar.Create(this);
	unsigned int Arry[3] = { 4001,4002,4003 };
	My_StasusBar.SetIndicators(Arry, 3);
	My_StasusBar.SetPaneInfo(0, 4001, SBPS_STRETCH, 100);
	My_StasusBar.SetPaneInfo(1, 4001, SBPS_STRETCH, 100);
	My_StasusBar.SetPaneInfo(2, 4001, SBPS_STRETCH, 100);
	SetTimer(1, 1000, NULL);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	/*
	初始化Tab栏
	*/
	TabItem.InsertItem(0, L"驱动项");
	TabItem.InsertItem(1, L"驱动进程项");
	TabItem.InsertItem(2, L"驱动文件项");
	TabItem.InsertItem(3, L"驱动注册表项");
	TabItem.InsertItem(4, L"IDT表");
	TabItem.InsertItem(5, L"GDT表");
	TabItem.InsertItem(6, L"SSDT表");

	My_Dialog[0] = new CDriverDialog;
	My_Dialog[0]->Create(IDD_DIALOG18, &TabItem);
	My_Dialog[1] = new CDriverProcessDialog;
	My_Dialog[1]->Create(DriverProcessDialog, &TabItem);
	My_Dialog[2] = new CDriverFileDialog;
	My_Dialog[2]->Create(DriverFileDialog, &TabItem);
	My_Dialog[3] = new CDriverRegDialog;
	My_Dialog[3]->Create(DriverRegDialog, &TabItem);
	My_Dialog[4] = new CIDTDialog;
	My_Dialog[4]->Create(DriverIDTDialog, &TabItem);
	My_Dialog[5] = new CGDTDialog;
	My_Dialog[5]->Create(GDTDilalog, &TabItem);
	My_Dialog[6] = new CSSDTDialog;
	My_Dialog[6]->Create(SSDTDialog, &TabItem);

	CRect MyRect = { 0 };
	TabItem.GetClientRect(MyRect);
	MyRect.DeflateRect(8, 33, 8, 8);

	My_Dialog[0]->MoveWindow(MyRect);
	My_Dialog[1]->MoveWindow(MyRect);
	My_Dialog[2]->MoveWindow(MyRect);
	My_Dialog[3]->MoveWindow(MyRect);
	My_Dialog[4]->MoveWindow(MyRect);
	My_Dialog[5]->MoveWindow(MyRect);
	My_Dialog[6]->MoveWindow(MyRect);
	
	
	My_Dialog[0]->ShowWindow(SW_SHOW);
	My_Dialog[1]->ShowWindow(SW_HIDE);
	My_Dialog[2]->ShowWindow(SW_HIDE);
	My_Dialog[3]->ShowWindow(SW_HIDE);
	My_Dialog[4]->ShowWindow(SW_HIDE);
	My_Dialog[5]->ShowWindow(SW_HIDE);
	My_Dialog[6]->ShowWindow(SW_HIDE);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTaskManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTaskManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTaskManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTaskManagerDlg::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码

	int idex = TabItem.GetCurSel();

	for (int i = 0; i < 7; i++)
		My_Dialog[i]->ShowWindow((i == idex) ? SW_NORMAL : SW_HIDE);
	*pResult = 0;
}

// CPU占用率
void CTaskManagerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
	{
		FILETIME newIdleTime, newKernelTime, newUserTime;
		GetSystemTimes(&newIdleTime, &newKernelTime, &newUserTime);
		// 转换时间
		double dOldIdleTime = FILETIME2Double(m_idleTime);
		double dNewIdleTime = FILETIME2Double(newIdleTime);
		double dOldKernelTime = FILETIME2Double(m_kernelTime);
		double dNewKernelTime = FILETIME2Double(newKernelTime);
		double dOldUserTime = FILETIME2Double(m_userTime);
		double dNewUserTime = FILETIME2Double(newUserTime);
		// 给成员赋值
		m_idleTime = newIdleTime;
		m_kernelTime = newKernelTime;
		m_userTime = newUserTime;

		// 计算出使用率
		int CPU = (100.0 - ((dNewIdleTime - dOldIdleTime) / (dNewKernelTime - dOldKernelTime + dNewUserTime - dOldUserTime)) * 100.0);
		// 输出
		CString NowCpu;
		NowCpu.Format(L"当前CPU：%d%%", CPU);
		My_StasusBar.SetPaneText(0, NowCpu);

		MEMORYSTATUS memStatus;
		GlobalMemoryStatus(&memStatus);
		CString Buffer;
		Buffer.Format(L"当前内存占用率为：%d%%", memStatus.dwMemoryLoad);
		My_StasusBar.SetPaneText(1, Buffer);
	}
	CDialogEx::OnTimer(nIDEvent);
}


double CTaskManagerDlg::FILETIME2Double(const FILETIME& fileTime)
{
	return double(fileTime.dwHighDateTime * 4.294967296e6) + double(fileTime.dwLowDateTime);
}


BOOL CTaskManagerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if ((pMsg->message == WM_HOTKEY) && (pMsg->wParam == 0x1234))
	{
		if (IsWindowVisible() == TRUE)
		{
			ShowWindow(SW_HIDE);
		}
		else
		{
			ShowWindow(SW_SHOW);
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


