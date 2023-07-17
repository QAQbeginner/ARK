#pragma once


// CDriverRegDialog 对话框

class CDriverRegDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CDriverRegDialog)

public:
	CDriverRegDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDriverRegDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = DriverRegDialog };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl DriverRegList;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	CTreeCtrl RegTree;
	afx_msg void OnNMClickTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void CreateReg();
	afx_msg void OnNMRClickTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void DelReg();
public:
	CString m_KeyName;
	CMenu m_Menu;
	afx_msg void OnTvnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
};
