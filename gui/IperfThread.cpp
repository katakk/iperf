// IperfThread.cpp : 実装ファイル
//

#include "stdafx.h"
#include "iperfgui.h"
#include "IperfThread.h"
#include "iperfguiDlg.h"

// CIperfThread

IMPLEMENT_DYNCREATE(CIperfThread, CWinThread)

CIperfThread::CIperfThread()
{
    m_bAutoDelete = TRUE;
}

CIperfThread::~CIperfThread()
{
}


BEGIN_MESSAGE_MAP(CIperfThread, CWinThread)
END_MESSAGE_MAP()


// CIperfThread メッセージ ハンドラ
#define TotalBytes 4096
void CIperfThread::CreateIperfPipe()
{
    HANDLE hReadPipeOut;
    HANDLE hReadPipeErr;
    HANDLE hReadPipeInp;
    HANDLE hWritePipeOut;
    HANDLE hWritePipeErr;
    HANDLE hWritePipeInp;

    HANDLE hParent = GetCurrentProcess();

    SECURITY_ATTRIBUTES security;
    security.nLength = sizeof(SECURITY_ATTRIBUTES);
    security.lpSecurityDescriptor = NULL;
    security.bInheritHandle = TRUE;

    if(!CreatePipe(&hReadPipeOut,&hWritePipeOut,&security,0)) ASSERT(0);// ("CreatePipe");
    if(!CreatePipe(&hReadPipeErr,&hWritePipeErr,&security,0)) ASSERT(0);// ("CreatePipe");
    if(!CreatePipe(&hReadPipeInp,&hWritePipeInp,&security,0)) ASSERT(0);// ("CreatePipe");
    
    SetStdHandle(STD_OUTPUT_HANDLE,hWritePipeOut);
    SetStdHandle(STD_ERROR_HANDLE, hWritePipeErr);
    SetStdHandle(STD_INPUT_HANDLE, hReadPipeInp);
    
    DuplicateHandle(hParent,hReadPipeOut,hParent,&m_hPipeOut,0,FALSE,DUPLICATE_SAME_ACCESS);
    DuplicateHandle(hParent,hReadPipeErr,hParent,&m_hPipeErr,0,FALSE,DUPLICATE_SAME_ACCESS);
    DuplicateHandle(hParent,hWritePipeInp,hParent,&m_hPipeIn,0,FALSE,DUPLICATE_SAME_ACCESS);
    
    CloseHandle(hReadPipeOut);
    CloseHandle(hReadPipeErr);
    CloseHandle(hWritePipeInp);
}

void CIperfThread::ReadIperfPipe(HANDLE hPipe)
{
    CHAR str[TotalBytes];
    DWORD BytesRead;
    DWORD TotalBytesAvail;
    DWORD BytesLeftThisMessage;
    DWORD dwByte;

    FlushFileBuffers(hPipe);
    PeekNamedPipe(hPipe, str, TotalBytes, &BytesRead,
          &TotalBytesAvail, &BytesLeftThisMessage );
    if( BytesRead > 0 )
    {
        ReadFile(hPipe,str,BytesRead,&dwByte,NULL);
        str[dwByte] = '\0';
		m_pMainWnd->SendMessage(CiperfguiDlg::WM_CONSOLE_MESG, m_uniqid, (LPARAM)str);
    }

}

BOOL CALLBACK WindowHidden(HWND hWnd, LPARAM lParam)
{
    PROCESS_INFORMATION *ProcessInfo = (PROCESS_INFORMATION *)lParam;
    TCHAR moduleName[ _MAX_PATH * 2 ];
    TCHAR szclass[_MAX_PATH * 2];
    TCHAR szname[_MAX_PATH * 2];
    DWORD dwAppProcessId;
    DWORD dwAppThreadId;
    DWORD dwCmdProcessId;
    DWORD dwCmdThreadId;

    dwCmdThreadId = GetWindowThreadProcessId( hWnd, &dwCmdProcessId);
    dwAppThreadId = GetWindowThreadProcessId( AfxGetApp()->GetMainWnd()->GetSafeHwnd(), &dwAppProcessId);
     
    if( 
        // 同一プロセス
        dwAppProcessId == dwCmdProcessId && 
        (GetWindow(hWnd,GW_OWNER) == 0) &&
        IsWindowVisible(hWnd) &&
        // メインウインドウじゃない
        dwAppThreadId != dwCmdThreadId )
    {
        
        GetClassName( hWnd, szclass, _MAX_PATH );
        GetWindowText( hWnd, szname, 255 );
        GetModuleFileName(GetModuleHandle( AfxGetApp()->m_pszExeName ),
                  moduleName, sizeof( moduleName ) ); 

        /*
            CString info;
            info.Format(_T("***[%s:%s] %x %x %x %x\n"), szclass, szname,
                dwAppProcessId,  dwCmdProcessId,
                dwAppThreadId,   dwCmdThreadId
                );
            AfxGetApp()->GetMainWnd()->SendMessage(WM_CONSOLE_MESG, 0, (LPARAM)(LPCTSTR)info);
        */

        // コンソール窓
        if( _tcscmp( szclass, _T("ConsoleWindowClass") ) == 0)
        {
            ::ShowWindow( hWnd, SW_HIDE );
        }
    }
    return TRUE;
}

BOOL CIperfThread::InitInstance()
{
    // TODO:  スレッドごとの初期化をここで実行します。
    HANDLE hOldIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hOldOut = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hOldErr = GetStdHandle(STD_ERROR_HANDLE);

    STARTUPINFOA si;
    memset(&si,0,sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;


    m_uniqid = mkhash(m_CmdLine, m_uniqid) ;
    m_uniqid += (WORD)m_nThreadID;
    m_uniqid += (WORD)m_nThreadID >> 4;

    FreeConsole();
    AllocConsole();

    CreateIperfPipe();

    if(CreateProcess(NULL, m_CmdLine, NULL, NULL, TRUE, 0,
        NULL, NULL, &si, &m_ProcessInfo) == FALSE ) {
        return FALSE;
    }

    EnumWindows(WindowHidden, (LPARAM)(PROCESS_INFORMATION *)&m_ProcessInfo);
    WaitForInputIdle(m_ProcessInfo.hProcess, INFINITE);
    SetStdHandle(STD_OUTPUT_HANDLE,hOldOut);
    SetStdHandle(STD_INPUT_HANDLE,hOldIn);
    SetStdHandle(STD_ERROR_HANDLE,hOldErr);
    EnumWindows(WindowHidden, (LPARAM)(PROCESS_INFORMATION *)&m_ProcessInfo);
    return TRUE;
}

int CIperfThread::ExitInstance()
{
    // TODO:  スレッドごとの後処理をここで実行します。
    GetExitCodeProcess(m_ProcessInfo.hProcess, &m_exitCode);
    CloseHandle(m_ProcessInfo.hThread);
    TerminateProcess(m_ProcessInfo.hProcess, 0);

    // post exit
	m_pMainWnd->SendMessage(CiperfguiDlg::WM_CONSOLE_QUIT, m_uniqid, (LPARAM)this);

    CloseHandle(m_ProcessInfo.hProcess);
    TerminateProcess(m_ProcessInfo.hProcess, 0);
    FreeConsole();
    CWinThread::ExitInstance();

    delete this;

    return 0; 
}

int CIperfThread::OnIdle(LONG lCount)
{
    // TODO : ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
    FlushFileBuffers(m_hPipeErr);
    FlushFileBuffers(m_hPipeOut);
    FlushFileBuffers(m_hPipeIn);

    ReadIperfPipe(m_hPipeErr);
    ReadIperfPipe(m_hPipeOut);
    return 1;
}

// やっぱオーバライドしないと動きおかしい
int CIperfThread::Run()
{
    ASSERT_VALID(this);
    _AFX_THREAD_STATE* pState = AfxGetThreadState();

    do {
        if(! OnIdle(1)) 
        {
            if(WaitForSingleObject(m_ProcessInfo.hProcess,
                INFINITE) == WAIT_OBJECT_0)
            {
                break;
            }
        }
    }   while(1);   
    
    ExitInstance();
    return 0;
}
