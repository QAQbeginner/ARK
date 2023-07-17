#pragma once


// CStopFileDialog 对话框

class CStopFileDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CStopFileDialog)

public:
	CStopFileDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CStopFileDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = StopFileDialog };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit StopFileName;
	afx_msg void OnBnClickedButton1();
	CString StopFileString;
};
