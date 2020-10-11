#pragma once
#include "afxwin.h"


// CSIPLoginDlg �Ի���

class CSIPLoginDlg : public CDialog
{
	DECLARE_DYNAMIC(CSIPLoginDlg)

public:
	CSIPLoginDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSIPLoginDlg();

// �Ի�������
	enum { IDD = IDD_POCKETPC_LOGIN };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog(void);
	afx_msg void OnLogin();
	afx_msg void OnCancel();
	CComboBox m_cboNetList;
public:
	void InitNetList(void);
	CString m_strStartupPath;
	afx_msg void OnAcountSet();
};
