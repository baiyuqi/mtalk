// MPlayer.cpp : 实现文件
//

#include "stdafx.h"
#include "mobilePhone.h"
#include "MPlayer.h"
#include "WMPEventSink.h"
#include  "macros.h"

// CMPlayer 对话框

IMPLEMENT_DYNAMIC(CMPlayer, CDialog)

CMPlayer::CMPlayer(CWnd* pParent /*=NULL*/)
: CDialog(CMPlayer::IDD, pParent)
{

}

CMPlayer::~CMPlayer()
{
}

void CMPlayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMPlayer, CDialog)
	ON_COMMAND(ID_32805, &CMPlayer::OnOpenFile)
	ON_COMMAND(ID_32806, &CMPlayer::OnOpenUrl)
	ON_COMMAND(ID_32807, &CMPlayer::OnExit)
END_MESSAGE_MAP()


// CMPlayer 消息处理程序

HRESULT CMPlayer::ProcOnCreate(HWND hwnd)
{
	HRESULT hr = S_OK;
	RECT rcClient;
	RECT rcMainWindow = { 0 };
	CComPtr<IConnectionPointContainer>  spConnectionContainer;
	CComWMPEventDispatch        *pEventListener = NULL;
	CComPtr<IWMPEvents>         spEventListener;

	if( !AtlAxWinInit())
	{
		return -1;
	}
	GetClientRect(&rcClient);
	//VERIFY(SystemParametersInfo(SPI_GETWORKAREA, 0, &rcMainWindow, 0));
	//rcClient.bottom -= rcMainWindow.top;

	//create WMP control
	m_wmplayer.Create(this->m_hWnd, rcClient, TEXT("{6BF52A52-394A-11d3-B153-00C04F79FAA6}"), 
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
	CBR(m_wmplayer.m_hWnd != NULL);

	hr =m_wmplayer.QueryControl(&(m_spWMPPlayer));
	CHR(hr);

	hr = CComWMPEventDispatch::CreateInstance(&pEventListener);
	CHR(hr);

	spEventListener = pEventListener;        
	hr =m_spWMPPlayer->QueryInterface(&spConnectionContainer);
	CHR(hr);

	// See if OCX supports the IWMPEvents interface
	hr = spConnectionContainer->FindConnectionPoint(__uuidof(IWMPEvents), 
		&(m_spConnectionPoint));
	if (FAILED(hr))
	{
		// If not, try the _WMPOCXEvents interface, which will use IDispatch
		hr = spConnectionContainer->FindConnectionPoint(__uuidof(_WMPOCXEvents), 
			&(m_spConnectionPoint));
	}

	hr = m_spConnectionPoint->Advise(spEventListener, 
		&(m_dwAdviseCookie));
	  CHR(hr);

	Error:
	return SUCCEEDED(hr) ? 0 : -1;
}

BOOL CMPlayer::OnInitDialog(void)
{
	CDialog::OnInitDialog();
	RECT rc;
	GetWindowRect(&rc);
	rc.top -= DIALOGTOP;
	MoveWindow(rc.left,rc.top,rc.right,rc.bottom,FALSE);
	SHFullScreen(this->m_hWnd,SHFS_HIDETASKBAR);

	SHMENUBARINFO mbi;

	memset(&mbi, 0, sizeof(SHMENUBARINFO));
	mbi.cbSize     = sizeof(SHMENUBARINFO);
	mbi.hwndParent = this->m_hWnd;
	mbi.dwFlags |= SHCMBF_HMENU;
	mbi.nToolBarId = IDR_MAIN_MENUBAR;
	mbi.hInstRes   = AfxGetInstanceHandle();
	SHCreateMenuBar(&mbi);
	this->ProcOnCreate(this->GetSafeHwnd());

	return 0;
}

HRESULT CMPlayer::ProcOnOpen()
{
	HRESULT hr = S_OK;
	OPENFILENAMEEX  ofn;
	TCHAR           szFileName[_MAX_PATH];

	memset(&ofn, 0, sizeof(ofn)); // initialize structure to 0/NULL
	szFileName[0] = TEXT('\0');
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = ARRAYSIZE(szFileName);
	ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_EXPLORER;
	ofn.lpstrFilter = TEXT("All Media files\0*.wma;*.mp3;*.3gp;*.mpeg;*.avi;*.wmv;*.asf;*.wav;*.mid;*.dvr-ms;*.mid;*.rmi;*.midi;*.wm;*.snd;*.au;*.aif;*.mpg;*.m1v;*.mp2;*.mpa\0");
	ofn.hwndOwner = m_hWnd;
	ofn.hInstance = _AtlBaseModule.GetResourceInstance();

	CBR(GetOpenFileNameEx(&ofn));

	//EndDialog(IDOK);
	hr = m_spWMPPlayer->put_URL(CComBSTR(szFileName));

Error:
	return SUCCEEDED(hr) ? 0 : -1;
}
void CMPlayer::OnOpenFile()
{
	ProcOnOpen();
}

void CMPlayer::OnOpenUrl()
{
	// TODO: 在此添加命令处理程序代码
	m_spWMPPlayer->put_URL(CComBSTR(_T("mms://192.168.100.250/c06/test.wmv")));
}

void CMPlayer::OnExit()
{
	if (m_spConnectionPoint)
	{
		if (0 != m_dwAdviseCookie)
		{
			m_spConnectionPoint->Unadvise(m_dwAdviseCookie);
		}
		m_spConnectionPoint.Release();
	}

	// close the OCX
	if (m_spWMPPlayer)
	{
		m_spWMPPlayer->close();
		m_spWMPPlayer.Release();
	}
	m_wmplayer.Detach();

	CDialog::OnCancel();
}
