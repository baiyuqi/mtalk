#pragma once


// COptionDlg �Ի���

class COptionDlg : public CDialog
{
	DECLARE_DYNAMIC(COptionDlg)

public:
	COptionDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~COptionDlg();

// �Ի�������
	enum { IDD = IDD_DIGOPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
