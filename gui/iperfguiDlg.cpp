#include "stdafx.h"
#include "iperfgui.h"
#include "iperfguiDlg.h"
#include "IperfThread.h"
#include "iperfguidlg.h"

#define DEFAULT_CMDLINE _T("iperf -c 192.168.0.195 -i1 -w1m -t60")

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
    , m_cmdline(DEFAULT_CMDLINE)
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
    pThread->m_pMainWnd = this;
    strcpy(pThread->m_CmdLine, m_cmdline);
    pThread->CreateThread(0, 0, NULL);
    pThList.Add(pThread);

    UpdateData(FALSE);
}

//パース処理
void CiperfguiDlg::ParseLine(WPARAM wParam, CString line)
{
	CIperfViewItem *item;
	double start =0;
	double end = 0;
	double speed = 0.0;
	WORD uniqid = (WORD)wParam;

	if( line == _T("")) return;
	if( line == _T("[ ID] Interval       Transfer     Bandwidth")) return;
	if( line == _T("------------------------------------------------------------")) return; 
// [000] local 111.111.111 port 11111 connected with 111.111.1111.111 port 5001
#define FindNextWord(_t, _s, _f) \
	int _t = _s.Find(_T(_f)); \
	if( _t != -1 ) _t +=(int) _tcslen(_T(_f));

	FindNextWord(t1, line, "[");
	FindNextWord(t2, line, "] ");

	// item を探す。
	// サーバモード時は同一threadなのでiperf pidを使用
	// 上記ハッシュに対するものは peer, localアドレスのみ
	// 計測結果に対する ハッシュはpeer + local も加えuniq さを上げる
	//
	//item = m_view.FindItem(uniqid);
	uniqid |= _tstoi( line.Mid(t1, line.GetLength() - t2));
	item = m_view.FindItem(uniqid);
	if(item)
	{
		uniqid = mkhash(item->m_peer, uniqid) ;
		uniqid = mkhash(item->m_local, uniqid) ;
	}
	item = m_view.FindItem(uniqid);

	// アドレス引きを行う
	do {
		FindNextWord(s1, line, "local ");
		FindNextWord(s2, line, "port ");
		FindNextWord(s3, line, "connected with ");

		if( s2 != -1 && s3 != -1 ) {
			CString local;
			CString peer;
			if(m_cmdline.Find("-c")) {
				peer = line.Mid(s1, line.GetLength() - s3 - 2);
				local = line.Mid(s3, line.GetLength() - s3);
			}
			if(m_cmdline.Find("-s")) {
				local = line.Mid(s1, line.GetLength() - s3 - 2);
				peer = line.Mid(s3, line.GetLength() - s3);
				//mkhash(peer);
			//	process
			}
				m_view.AddItemPeer(wParam, peer);
				m_view.AddItemLocal(wParam, local);


		//	m_Process[process] = peer + _T("/") + local;
			//TRACE("l:%s p:%s\n", local, peer );

			//
			item = m_view.FindItem(uniqid);
			if(item)
			{
				wParam = mkhash(item->m_peer, uniqid) ;
				wParam = mkhash(item->m_local, uniqid) ;
				m_view.AddItemPeer(uniqid, peer);
				m_view.AddItemLocal(uniqid, local);
			}
		}
	}while(0);

	//[xxxx] 59.0-60.0 sec  11.3 MBytes  94.4 Mbits/sec
	do {
		FindNextWord(k, line, "sec ");
		if( k != -1 )
		{
			CString sec = line.Mid( t2, k - t2 - (int)_tcslen(_T("sec ")) );
			int sec_s = sec.Find(_T("-"));
			if ( sec_s != -1) {
				start = _tcstod( sec.Left( sec_s ), NULL );
				end = _tcstod( sec.Right( sec_s + 1 ), NULL );
			}
		}

		FindNextWord(s4, line, "Bytes ");
		FindNextWord(s5, line, "bits/sec");
		if( s4 != -1 && s5 != -1 ) {
			if( s4 < s5 ) {
				CString tmp = line.Mid( s4, s5 -s4 - (int)_tcslen(_T("bits/sec")) );
				tmp.Trim(_T(" "));
				int len = tmp.GetLength() - 1;
				TCHAR unite = tmp[len]; // M K 
				speed = _tcstod( tmp.Left(len), NULL );
				switch(unite)
				{
					case 'G':speed *= (1024 * 1024);break;
					case 'M':speed *= 1024;break;
					case 'K':speed *= 1.024;break;
					case 'g':speed *= (1000 * 1000);break;
					case 'm':speed *= 1000;break;
					case 'k':speed *= 1;break;
					case ' ':speed *= 0;break;
					case '\0':speed *= 0;break;
				}

				m_view.AddItem(uniqid, start, end, speed);

				if(item)
				{
				CString log;
				log.Format(_T("%s->%s %s\r\n"), item->m_local, item->m_peer,
					line.Mid(t2, line.GetLength() - t2 ));
				m_log.SetSel(m_log.GetWindowTextLength(), -1);
				m_log.ReplaceSel((LPCTSTR)log);
				}
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
	CString lines;
	lines += (LPCTSTR)(lParam); // ASCII to MBSTR or UNICODE
	int elem = Split( _T("\r\n"), lines.GetBuffer(), param, 1024, 1024 );
	if( elem > 0 )
	{
		for( int i = 0; i < elem ; i ++)
		{
			ParseLine(wParam, param[i]);
		}
	}
	//後続の\nなしとか保持しないといけないんだけど省略
    return 0;
}

