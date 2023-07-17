#pragma once


// CDriverThreadDialog 对话框

class CDriverThreadDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CDriverThreadDialog)

public:
	CDriverThreadDialog(ULONG pEprocess, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDriverThreadDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = DriverThreadDialog };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl DriverThreadList;
	ULONG my_pEprocess;
	virtual BOOL OnInitDialog();
};
