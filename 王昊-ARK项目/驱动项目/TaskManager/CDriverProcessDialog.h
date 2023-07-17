#pragma once


// CDriverProcessDialog 对话框

class CDriverProcessDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CDriverProcessDialog)

public:
	CDriverProcessDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDriverProcessDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = DriverProcessDialog };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl DriverProcessList;
	virtual BOOL OnInitDialog();
	afx_msg void HideDriverProcess();
	afx_msg void OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void DelProcess();
	afx_msg void QueryThread();
	afx_msg void QueryModule();
	afx_msg void ShuaXin();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void StopProcessCreat();
	afx_msg void CloseHook();
};
