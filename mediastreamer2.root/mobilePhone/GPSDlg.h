#pragma once


// CGPSDlg �Ի���
class	CGPSSink;
class	CGPSController;
class CGPSDlg : public CDialog
{
	DECLARE_DYNAMIC(CGPSDlg)

public:
	CGPSDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CGPSDlg();

// �Ի�������
	enum { IDD = IDD_POCKETPC_GPS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	int InitGPS(void);
private :
    CGPSSink *pGPSSink;
	CGPSController *pGPSController;
public:
	afx_msg void OnReturn();
	afx_msg void OnDestroy();
};
