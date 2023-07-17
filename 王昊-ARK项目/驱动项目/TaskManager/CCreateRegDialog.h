#pragma once


// CCreateRegDialog 对话框

class CCreateRegDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CCreateRegDialog)

public:
	CCreateRegDialog(CString KeyName,CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCreateRegDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = CreateRegDialog };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString RegName;
	afx_msg void OnBnClickedButton1();
	CString my_KeyName;
};
