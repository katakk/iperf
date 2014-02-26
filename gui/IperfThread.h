#pragma once
class CIperfThread : public CWinThread
{
    DECLARE_DYNCREATE(CIperfThread)

protected:
    CIperfThread();
    virtual BOOL OnIdle(LONG lCount);
	virtual int Run();
    virtual ~CIperfThread();
    DECLARE_MESSAGE_MAP()
    HANDLE m_hPipeOut;
    HANDLE m_hPipeErr;
    HANDLE m_hPipeIn;
    PROCESS_INFORMATION m_ProcessInfo;
	WORD m_uniqid;
    void CreateIperfPipe();
    void ReadIperfPipe(HANDLE hPipe);

public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    char m_CmdLine[1024];
	DWORD m_exitCode;
};

