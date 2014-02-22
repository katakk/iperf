#pragma once
class CIperfThread : public CWinThread
{
    DECLARE_DYNCREATE(CIperfThread)

protected:
    CIperfThread();
    virtual ~CIperfThread();

public:
    void CreateIperfPipe();
    void ReadIperfPipe(HANDLE hPipe);
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    CWnd *m_hParent;
    char m_CmdLine[1024];

protected:
    DECLARE_MESSAGE_MAP()
    HANDLE m_hPipeOut;
    HANDLE m_hPipeErr;
    HANDLE m_hPipeIn;
    PROCESS_INFORMATION m_ProcessInfo;
public:
    virtual int Run();
};
