#pragma once


// COptionDlg 对话框

class COptionDlg : public CDialog
{
	DECLARE_DYNAMIC(COptionDlg)

public:
	COptionDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COptionDlg();

// 对话框数据
	enum { IDD = IDD_DIGOPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_sUsrName;
	CString m_sPassword;
	CString m_sDomain;
	CString m_sProxy;
	CString m_sPort;
	afx_msg void OnBnClickedOk();
	
	CString m_sName;
};
