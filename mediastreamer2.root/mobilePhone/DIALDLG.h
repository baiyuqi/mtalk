#pragma once


// CDIALDLG �Ի���
class CmobilePhoneDlg;

class CDIALDLG : public CDialog
{
	DECLARE_DYNAMIC(CDIALDLG)

public:
	CDIALDLG(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDIALDLG();

// �Ի�������
	enum { IDD = IDD_DIAlPANEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_sPhoneNumber;
	afx_msg void OnBnClickedDial1();
protected:
	virtual BOOL OnInitDialog(void);
public:
	afx_msg void On32800();
	afx_msg void OnBnClickedBtnvideo();
private:
	CmobilePhoneDlg* m_pMianDlg;
	void setMode();
	
public:
	afx_msg void OnBnClickedBtndel();
	afx_msg void OnBnClickedHangup();
	afx_msg void OnBnClickedAnswer();
	bool  isDial;
	
};
