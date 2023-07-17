#pragma once


// CSSDTDialog 对话框

class CSSDTDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CSSDTDialog)

public:
	CSSDTDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSSDTDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = SSDTDialog };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl SSDTList;
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void PopMenu(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OpenNeiHe();
	afx_msg void CloseNeiHe();
};
