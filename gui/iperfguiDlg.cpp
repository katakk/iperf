#include "stdafx.h"
#include "iperfgui.h"
#include "iperfguiDlg.h"
#include "IperfThread.h"
#include "iperfguidlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DEFAULT_CMDLINE1 _T("iperf -c 192.168.0.195 -i1 -w1m -t60")
#define DEFAULT_CMDLINE2 _T("ping 192.168.0.195")
UINT CiperfguiDlg::WM_CONSOLE_MESG =      (WM_USER+200);
UINT CiperfguiDlg::WM_CONSOLE_QUIT =      (WM_USER+201);

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

{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	WM_CONSOLE_MESG = RegisterWindowMessage(TEXT("ConsoleMesg"));
	WM_CONSOLE_QUIT = RegisterWindowMessage(TEXT("ConsoleQuit"));

}

void CiperfguiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDITLOG, m_log);
	DDX_Control(pDX, IDC_CMDLINE, m_combo);
	DDX_Control(pDX, IDOK, m_submit);
}

BEGIN_MESSAGE_MAP(CiperfguiDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_REGISTERED_MESSAGE(WM_CONSOLE_MESG,OnIperfMessage)
    ON_REGISTERED_MESSAGE(WM_CONSOLE_QUIT,OnIperfQuit)
	ON_WM_SIZE()
	ON_WM_CLOSE()
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

	GetHistory();
	UpdateData(FALSE);
    return TRUE;
}

//コマンド経歴
void CiperfguiDlg::GetHistory()
{
	TCHAR *param[1024];
	CMapStringToOb temp;
	CString str;
	CObject *pob;
	POSITION pos;
	int i;

	CString lines;

	// 入力データを積んでいく
	lines += m_cmdline;
	lines += _T("\n");
	// pull comboxtext
	m_combo.GetWindowText(str);
	lines += str;
	lines += _T("\n");
	for(i = 0; i < m_combo.GetCount(); i++)
	{
		m_combo.GetLBText(i, str) ;
		lines += str;
		lines += _T("\n");
	}
	// from file
	TRY {
		CStdioFile f(_T("iperfcmd.txt"), CFile::typeText | CFile::modeRead | CFile::modeNoTruncate ); 
		// uniq
		while( f.ReadString(str) ) {
			lines += str;
			lines += _T("\n");
		}
	} CATCH (CFileException, e) { }
    END_CATCH

	lines += _T("\n"); //番兵
	int elem = Split( _T("\r\n"), lines.GetBuffer(), param, 1024, 1024 );
	if( elem > 0 )
	{
		for( int i = 0; i < elem ; i ++)
		{
			temp.SetAt(param[i], NULL );
		}
	}
	temp.RemoveKey(_T(""));


	//コマンド経歴に書き込む
	TRY {
		CStdioFile f(_T("iperfcmd.txt"), CFile::typeText | CFile::modeCreate | CFile::modeWrite); 

		for(pos = temp.GetStartPosition( );pos != NULL;)
		{
			temp.GetNextAssoc( pos, str, pob );
			if(! str.IsEmpty() ) {
				f.WriteString(str);
				f.WriteString(_T("\n"));
			}
		}

	} CATCH (CFileException, e) { }
    END_CATCH

	// コンボボックスをリセット
	m_combo.ResetContent();
	for( pos = temp.GetStartPosition( ); pos != NULL; )
	{
		temp.GetNextAssoc( pos, str, pob );
		if(! str.IsEmpty() ) {
			m_combo.AddString(str);
			m_combo.SetWindowText(str);
		}
	}
	//セットがなければ初期値
	m_combo.GetWindowText(str);
	if( str == _T("") )
	{
		m_cmdline = DEFAULT_CMDLINE1;
		m_combo.AddString(m_cmdline);
		m_cmdline = DEFAULT_CMDLINE2;
		m_combo.AddString(m_cmdline);
	}
	m_combo.SetWindowText(m_cmdline);
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

void CiperfguiDlg::OnOK()
{
	CString tmp;
    CIperfThread *pThread; 
    UpdateData(TRUE);
	m_combo.GetWindowText(m_cmdline);
	tmp.Format(_T("プログラム'%s' は開始しました -->\r\n"), m_cmdline);
	m_log.SetWindowText(tmp);

    CRuntimeClass *pRuntime = RUNTIME_CLASS(CIperfThread);
    pThread = (CIperfThread *)pRuntime->CreateObject();
    pThread->m_pMainWnd = this;
    pThread->m_pActiveWnd = this;
    strcpy(pThread->m_CmdLine, m_cmdline);
    pThread->CreateThread(0, 0, NULL);
    pThList.Add(pThread);

	GetHistory();
	GetThreadTitle();
    UpdateData(FALSE);

	//CDialog::OnOK();
}

void CiperfguiDlg::GetThreadTitle()
{
	CIperfThread *pThread;
	CString str =  AfxGetApp()->m_pszAppName;
    for(int i = 0 ; i < pThList.GetSize() ; i ++ )
    {
        pThread =(CIperfThread*)pThList.ElementAt(i);
		str += _T(" [");
		str += pThread->m_CmdLine;
		str += _T("]");
    }
	SetWindowText(str);
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

				peer = line.Mid(s1, s3 - s1 - (int)_tcslen(_T("connected with ")));
				local = line.Mid(s3, line.GetLength() - s3);
			}
			if(m_cmdline.Find("-s")) {
				local = line.Mid(s1, s3 - s1 - (int)_tcslen(_T("connected with ")));
				peer = line.Mid(s3, line.GetLength() - s3);
			}
			local.Replace(_T(" port "),_T(":"));
			peer.Replace(_T(" port "),_T(":"));

			m_view.AddItemPeer(uniqid, peer);
			m_view.AddItemLocal(uniqid, local);

			item = m_view.FindItem(uniqid);
			if(item)
			{
				uniqid = mkhash(item->m_peer, uniqid) ;
				uniqid = mkhash(item->m_local, uniqid) ;
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
					CTime timeData = CTime::GetTickCount();
					double user = 0, kernel = 0;
					m_cpuload.GetKernelPerf(&user,&kernel);

					log.Format(_T("%s(%s)%s %s (user=%f sys=%f)\n"), 
						timeData.Format(_T("[%Y/%m/%d %H:%M:%S]")),
						item->m_colorname,
						item->m_peer,
						line.Mid(s4, line.GetLength() - s4 ),
						user,kernel);

					// LOGGING
					CString filename;
					filename.Format(_T("%s.log"), timeData.Format(_T("%Y%m%d")));
					TRY {
						CStdioFile f(filename, CFile::typeText | CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite); 

						f.SeekToEnd();
						f.WriteString(log);
					} CATCH (CFileException, e) { }
					END_CATCH
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
			*p = _T('\0');
			s =  p + 1;
		}
	}

	for( param_tail = param_pos ; param_tail < param_size; param_tail++ )
		param[ param_tail ] = end_point;

	return param_pos;
}
// Iperf 終了
LRESULT CiperfguiDlg::OnIperfQuit(WPARAM wParam, LPARAM lParam)
{
	CIperfThread *pThread = (CIperfThread *) lParam; 

    for(int i = 0 ; i < pThList.GetSize() ; i ++ )
    {
        if( pThread == (CIperfThread*) pThList.ElementAt(i) )
		{
			CString tmp;
			tmp.Format(_T("<-- プログラム'%s' はコード %d (%x) で終了しました"),
				pThread->m_CmdLine, pThread->m_exitCode, pThread->m_exitCode);
			pThList.RemoveAt(i);
			m_log.SetSel(m_log.GetWindowTextLength(), -1);
			m_log.ReplaceSel((LPCTSTR)tmp);
			GetThreadTitle();
			return 0;
		}
    }
	return 0;
}

// Iperf からのメッセージはここに来る
LRESULT CiperfguiDlg::OnIperfMessage(WPARAM wParam, LPARAM lParam)
{
	TCHAR *param[1024];
	CString lines;
	GetThreadTitle();

	lines = (LPCTSTR)(lParam); // ASCII to MBSTR or UNICODE
	m_log.SetSel(m_log.GetWindowTextLength(), -1);
	m_log.ReplaceSel((LPCTSTR)lines);

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


void CiperfguiDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

    if (IsWindow(GetSafeHwnd()))
    {
		// TODO : ここにメッセージ ハンドラ コードを追加します。
		int sx = 1, sy = 1;
		const int combo = 24;
		const int button = 64;
		cy --;
		cx --;
		cx --;
		m_combo.MoveWindow(sx + 2, sy + 2, cx - button - 2, cy/2); // heigth はひらいたときの
		m_submit.MoveWindow(cx - button + 3, sy, button - 3, combo);
		sy += combo;
		cy/=2;
        m_view.MoveWindow(sx, sy, cx, cy);
		sy += cy;
		cy -= combo;
        m_log.MoveWindow(sx, sy, cx, cy);
		Invalidate();
    }
}

// ESC キーだけで閉じちゃうのはどうかと
void CiperfguiDlg::OnClose()
{
	EndDialog(IDCANCEL);
	CDialog::OnClose();
}
void CiperfguiDlg::OnCancel()
{
	//CDialog::OnCancel();
}

