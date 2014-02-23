#include "stdafx.h"
#include "iperfgui.h"
#include "iperfguiDlg.h"
#include "IperfThread.h"
#include "iperfguidlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

    enum { IDD = IDD_ABOUTBOX };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CiperfguiDlg::CiperfguiDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CiperfguiDlg::IDD, pParent)
    , m_cmdline(_T("iperf -c 192.168.0.195 -i1 -w1m"))
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CiperfguiDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT1, m_log);
    DDX_Text(pDX, IDC_EDITCMD, m_cmdline);
}

BEGIN_MESSAGE_MAP(CiperfguiDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
    ON_MESSAGE(WM_CONSOLE_MESG,OnIperfMessage)
END_MESSAGE_MAP()


// CiperfguiDlg メッセージ ハンドラ

BOOL CiperfguiDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

	m_view.SubclassDlgItem(IDC_IPERFVIEW,this);
    return TRUE;
}

BOOL CiperfguiDlg::DestroyWindow()
{
    CIperfThread *pThread; 

    for(int i = 0 ; i < pThList.GetSize() ; i ++ )
    {
        pThread =(CIperfThread*)pThList.ElementAt(i);
        pThread->Delete();
    }

    return CDialog::DestroyWindow();
}

void CiperfguiDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialog::OnSysCommand(nID, lParam);
    }
}

void CiperfguiDlg::OnPaint() 
{
    if (IsIconic())
    {
        CPaintDC dc(this);

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

HCURSOR CiperfguiDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CiperfguiDlg::OnBnClickedOk()
{
    // TODO : ここにコントロール通知ハンドラ コードを追加します。
    CIperfThread *pThread; 
    UpdateData(TRUE);

    CRuntimeClass *pRuntime = RUNTIME_CLASS(CIperfThread);
    pThread = (CIperfThread *)pRuntime->CreateObject();
    pThread->m_hParent = this;
    strcpy(pThread->m_CmdLine, m_cmdline);
    pThread->CreateThread(0, 0, NULL);
    pThList.Add(pThread);

    UpdateData(FALSE);
}

void CiperfguiDlg::Parse(double start,double end, double speed)
{
	TRACE("l:%s p:%s %f - %f bps:%f\n", m_local, m_peer, start, end, speed );
}

//パース処理
void CiperfguiDlg::ParseLine(CString line)
{
	double start =0;
	double end = 0;
	double bps = 0.0;

	if( line == _T("")) return;
	if( line == _T("[ ID] Interval       Transfer     Bandwidth")) return;
	if( line == _T("------------------------------------------------------------")) return; 
// [000] local 111.111.111 port 11111 connected with 111.111.1111.111 port 5001
#define FindNextWord(_t, _s, _f) \
	int _t = _s.Find(_T(_f)); \
	if( _t != -1 ) _t +=(int) _tcslen(_T(_f));

	FindNextWord(t, line, "] ");

	do {
		FindNextWord(s1, line, "local ");
		FindNextWord(s2, line, "port ");
		FindNextWord(s3, line, "connected with ");

		if( s2 != -1 && s3 != -1 ) {
			if(m_cmdline.Find("-c")) {
				m_peer = line.Mid(s1, line.GetLength() - s3);
				m_local = line.Mid(s3, line.GetLength() - s3);
			}
			if(m_cmdline.Find("-s")) {
				m_local = line.Mid(s1, line.GetLength() - s3);
				m_peer = line.Mid(s3, line.GetLength() - s3);
			}
			//TRACE("l:%s p:%s\n", local, peer );
		}
	}while(0);

	do {
		FindNextWord(k, line, "sec ");
		if( k != -1 )
		{
			CString sec = line.Mid( t, k - t - _tcslen(_T("sec ")) );
			int sec_s = sec.Find(_T("-"));
			if ( sec_s != -1) {
				start = _tcstod( sec.Left( sec_s ), NULL );
				end = _tcstod( sec.Right( sec_s + 1 ), NULL );
			}
		}

	}while(0);

	do {
		FindNextWord(s4, line, "Bytes ");
		FindNextWord(s5, line, "bits/sec");
		//	TRACE("4:%d 5:%d\n", s4, s5 );
		if( s4 != -1 && s5 != -1 ) {
			if( s4 < s5 ) {
				CString speed = line.Mid( s4, s5 -s4 - _tcslen(_T("bits/sec")) );
				speed.Trim(_T(" "));
				int len = speed.GetLength() - 1;
				TCHAR unite = speed[len]; // M K 
				bps = _tcstod( speed.Left(len), NULL );
				switch(unite)
				{
					case 'G':bps *= (1024 * 1024);break;
					case 'M':bps *= 1024;break;
					case 'K':bps *= 1.024;break;
					case 'g':bps *= (1000 * 1000);break;
					case 'm':bps *= 1000;break;
					case 'k':bps *= 1;break;
					case ' ':bps *= 0;break;
					case '\0':bps *= 0;break;
				}

				Parse(start, end, bps);
			//TRACE("bps:%f\n", bps );

			}
		}
	}while(0);


}

int CiperfguiDlg::Split( const TCHAR *pattern, TCHAR *expr, TCHAR **param, int param_size, int limit )
{
	int length;
	int expr_remain, param_pos, param_tail;
	TCHAR *p, *s, *end_point;

	param_pos = 0;
	length = (int)_tcslen( expr );
	expr_remain = (int)length;
	p = s = expr;
	end_point = expr + length;

	for( ; expr_remain >= 0; expr_remain--, p++ )
	{
		if( _tcschr( pattern, *p ) != NULL )
		{
			param[ param_pos ] = s;
			param_pos++;
			if( param_pos >= limit ) 
				break;
			if( param_pos >= param_size ) 
				break;
			*p = '\0';
			s =  p + 1;
		}
	}

	for( param_tail = param_pos ; param_tail < param_size; param_tail++ )
		param[ param_tail ] = end_point;

	return param_pos;
}

// Iperf からのメッセージはここに来る
LRESULT CiperfguiDlg::OnIperfMessage(WPARAM wParam, LPARAM lParam)
{
	TCHAR *param[1024];
  //  TRACE0((LPCTSTR)(lParam));
    m_log.SetSel(m_log.GetWindowTextLength(), -1);
    m_log.ReplaceSel((LPCTSTR)(lParam));

	CString lines;
	lines += (LPCTSTR)(lParam);
	int elem = Split( _T("\r\n"), lines.GetBuffer(), param, 1024, 1024 );
	if( elem > 0 )
	{
		for( int i = 0; i < elem ; i ++)
		{
		//	TRACE("%d: %s\n", i, param[i] );
			ParseLine(param[i]);
		}
	}
	//後続の\nなしとか保持しないといけないんだけど省略
    return 0;
}

