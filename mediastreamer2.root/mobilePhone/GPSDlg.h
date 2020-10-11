#pragma once


// CGPSDlg 对话框
class	CGPSSink;
class	CGPSController;
class CGPSDlg : public CDialog
{
	DECLARE_DYNAMIC(CGPSDlg)

public:
	CGPSDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGPSDlg();

// 对话框数据
	enum { IDD = IDD_POCKETPC_GPS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
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
