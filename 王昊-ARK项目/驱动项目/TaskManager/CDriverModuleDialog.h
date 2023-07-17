#pragma once


// CDriverModuleDialog 对话框

class CDriverModuleDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CDriverModuleDialog)

public:
	CDriverModuleDialog(ULONG pEprocess,CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDriverModuleDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = DriverModuleDialog };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl DriverModuleList;
	virtual BOOL OnInitDialog();
	ULONG my_pEprocess1;
};
