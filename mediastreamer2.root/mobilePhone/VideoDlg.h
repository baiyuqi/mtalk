#pragma once

#define IDD_POCKETPC_VIDEO              131
// CVideoDlg 对话框
class CmobilePhoneDlg;
class CVideoDlg : public CDialog
{
	DECLARE_DYNAMIC(CVideoDlg)

public:
	CVideoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CVideoDlg();

// 对话框数据
	enum { IDD = IDD_POCKETPC_VIDEO };
protected:
virtual BOOL	OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedComplete();
	afx_msg void OnReturn();
	BOOL call(CString name);
private:
	CmobilePhoneDlg*  m_pMainDlg;
public:
	bool videoPreview(void);
	afx_msg void OnBnClickedHangup();
	void OnCallEnd(void);
};
