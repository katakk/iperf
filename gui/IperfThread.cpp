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
	PeekNamedPipe(hPipe, str, TotalBytes, &BytesRead, &TotalBytesAvail, &BytesLeftThisMessage );
	if( BytesRead > 0 )
	{
		ReadFile(hPipe,str,BytesRead,&dwByte,NULL);
		str[dwByte] = '\0';
		CString adds(str); // ASCII to MBSTR or UNICODE
//		TRACE0(adds);
		m_hParent->SendMessage(WM_CONSOLE_MESG, 0, (LPARAM)(LPCTSTR)adds);
	}

}

BOOL CALLBACK WindowHidden(HWND hWnd, LPARAM)
{
	TCHAR moduleName[ _MAX_PATH * 2 ];
	TCHAR szclass[_MAX_PATH * 2];
	TCHAR szname[_MAX_PATH * 2];

	GetClassName( hWnd, szclass, _MAX_PATH );
	if( _tcscmp( szclass, _T("ConsoleWindowClass") ) == 0 )
	{
		CWinApp *pApp = ::AfxGetApp();
		::GetModuleFileName( ::GetModuleHandle( pApp->m_pszExeName ), moduleName, sizeof( moduleName ) ); 
		GetWindowText( hWnd, szname, 255 );

		if( _tcscmp( szname, moduleName ) == 0 )
			::ShowWindow( hWnd, SW_HIDE );
	}
	return TRUE;
}

BOOL CIperfThread::InitInstance()
{
	//m_pThreadParams
	// TODO:  スレッドごとの初期化をここで実行します。
	HANDLE hOldIn = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hOldOut = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE hOldErr = GetStdHandle(STD_ERROR_HANDLE);


	STARTUPINFOA si;
	memset(&si,0,sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);


	FreeConsole();
	AllocConsole();

	CreateIperfPipe();

	if(CreateProcessA(NULL, m_CmdLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &m_ProcessInfo) == FALSE ) {
		return FALSE;
	}
	SetStdHandle(STD_OUTPUT_HANDLE,hOldOut);
	SetStdHandle(STD_INPUT_HANDLE,hOldIn);
	SetStdHandle(STD_ERROR_HANDLE,hOldErr);
	EnumWindows(WindowHidden, NULL);
	return TRUE;
}

int CIperfThread::ExitInstance()
{
	// TODO:  スレッドごとの後処理をここで実行します。
	return CWinThread::ExitInstance();
}

int CIperfThread::Run()
{
	// TODO : ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	do {
		FlushFileBuffers(m_hPipeOut);
		FlushFileBuffers(m_hPipeErr);
		FlushFileBuffers(m_hPipeIn);

		ReadIperfPipe(m_hPipeOut);
		ReadIperfPipe(m_hPipeErr);

	} while(WaitForSingleObject(m_ProcessInfo.hProcess, 100) != WAIT_OBJECT_0) ;

	CloseHandle(m_ProcessInfo.hThread);

	WaitForSingleObject(m_ProcessInfo.hProcess, 200);
	TerminateProcess(m_ProcessInfo.hProcess, 0);
	WaitForSingleObject(m_ProcessInfo.hProcess, INFINITE);
	CloseHandle(m_ProcessInfo.hProcess);
	ExitThread(0);

	return CWinThread::Run();
}


