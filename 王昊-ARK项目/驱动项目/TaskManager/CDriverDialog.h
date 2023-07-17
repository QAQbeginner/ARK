#pragma once


// CDriverDialog 对话框

class CDriverDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CDriverDialog)

public:
	CDriverDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDriverDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG18 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl DriverList;
	virtual BOOL OnInitDialog();
	afx_msg void OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void HideDriver();
	afx_msg void MyUpdate();
	afx_msg void ProtectProcess();
	afx_msg void FanTiaoshiProcess();
	afx_msg void OnNeiHeChongZai();
	afx_msg void CloseNeiHeChongZai();
	afx_msg void CancleProtect();
};
