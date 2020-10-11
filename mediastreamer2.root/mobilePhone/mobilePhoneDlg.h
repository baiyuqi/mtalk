// mobilePhoneDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
class CPhoneCore;
class CVideoDlg;
class CDIALDLG;
class CStaticManager;
class CGPSDlg;
#define IDD_MOBILEPHONE_DIALOG          102
// CmobilePhoneDlg �Ի���
class CmobilePhoneDlg : public CDialog
{
// ����
public:
	CmobilePhoneDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MOBILEPHONE_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()
public:
	CString m_sUsrName;
	CString m_sPassword;
	CString m_sDomain;
	CString m_sProxy;
	CString m_sPort;
	//HWND             g_hWndMenuBar;     // menu bar handle
	//HINSTANCE        g_hInst;           // current instance
public:
	CPhoneCore* m_pPhoneCore;
	afx_msg void OnDestroy();

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);

	afx_msg void OnLongin();
	afx_msg void OnDialpanel();
	afx_msg void OnExit();
	afx_msg void OnVideoTest();
	CTreeCtrl m_pFriends;
	void initFriendTree(void);
	afx_msg void OnMediaPlay();

	CString GetStartupPath(void);
	CString m_strStartupPath;
public:
	CVideoDlg  *m_pCallDlg;
	CDIALDLG   *m_pDialDlg;
	CGPSDlg    *m_pGpsDlg;
	CString m_sSysState;

	CStaticManager *m_pStateManager;
	void UpdateSysState(CString state);
	afx_msg void OnOpenGpsDlg();
};
