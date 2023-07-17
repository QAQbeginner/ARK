#pragma once


// CGDTDialog 对话框

class CGDTDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CGDTDialog)

public:
	CGDTDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CGDTDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = GDTDilalog };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl GDTList;
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
