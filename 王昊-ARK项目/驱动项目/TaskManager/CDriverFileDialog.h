#pragma once


// CDriverFileDialog 对话框

class CDriverFileDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CDriverFileDialog)

public:
	CDriverFileDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDriverFileDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = DriverFileDialog };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl DriverFileList;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedRadio1();
	CEdit FilePathEdit;
	CButton FilePathButton;
	CString m_ThisPath;
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	afx_msg void OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void DelFile();
	afx_msg void StopFileCreat();
	afx_msg void CloseHOOK();
	afx_msg void MouseDouble(NMHDR* pNMHDR, LRESULT* pResult);
};
