#pragma once


// CMPlayer �Ի���

class CMPlayer : public CDialog
{
	DECLARE_DYNAMIC(CMPlayer)

public:
	CMPlayer(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMPlayer();

// �Ի�������
	enum { IDD = IDD_POCKETPC_PLAYER };
public:
	HRESULT CMPlayer::ProcOnCreate(HWND hwnd);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog(void);
	HRESULT ProcOnOpen();
public:
	 CAxWindow m_wmplayer;
	 CComPtr<IWMPPlayer>         m_spWMPPlayer;
	 CComPtr<IConnectionPoint>   m_spConnectionPoint;
     DWORD                       m_dwAdviseCookie;
	 afx_msg void OnOpenFile();
	 afx_msg void OnOpenUrl();
	 afx_msg void OnExit();
};
