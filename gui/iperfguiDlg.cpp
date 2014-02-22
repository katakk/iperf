#include "stdafx.h"
#include "iperfgui.h"
#include "iperfguiDlg.h"
#include "IperfThread.h"
#include ".\iperfguidlg.h"

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

// Iperf からのメッセージはここに来る
LRESULT CiperfguiDlg::OnIperfMessage(WPARAM wParam, LPARAM lParam)
{
    m_log.SetSel(m_log.GetWindowTextLength(), -1);
    m_log.ReplaceSel((LPCTSTR)(lParam));
//  TRACE0((LPCTSTR)(lParam));

    return 0;
}

