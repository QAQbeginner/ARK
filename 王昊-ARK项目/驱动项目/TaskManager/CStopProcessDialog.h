#pragma once


// CStopProcessDialog 对话框

class CStopProcessDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CStopProcessDialog)

public:
	CStopProcessDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CStopProcessDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = StopProcessDialog };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString StopProcessString;
	afx_msg void OnBnClickedButton1();
};
