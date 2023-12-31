﻿
// TaskManagerDlg.h: 头文件
//

#pragma once


// CTaskManagerDlg 对话框
class CTaskManagerDlg : public CDialogEx
{
// 构造
public:
	CTaskManagerDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TASKMANAGER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	double FILETIME2Double(const FILETIME& fileTime);
public:
	CTabCtrl TabItem;
	CDialogEx* My_Dialog[7];
	CStatusBar My_StasusBar;
	FILETIME m_idleTime, m_kernelTime, m_userTime;
	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
