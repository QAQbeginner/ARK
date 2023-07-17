#pragma once


// CIDTDialog 对话框

class CIDTDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CIDTDialog)

public:
	CIDTDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CIDTDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = DriverIDTDialog };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl IDTList;
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
