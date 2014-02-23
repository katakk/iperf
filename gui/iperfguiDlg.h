#pragma once
#include "afxwin.h"

#define WM_CONSOLE_MESG       (WM_USER+200)
class CiperfguiDlg : public CDialog
{
public:
    CiperfguiDlg(CWnd* pParent = NULL);
    enum { IDD = IDD_IPERFGUI_DIALOG };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    HICON m_hIcon;
    CPtrArray pThList;

    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
    afx_msg LRESULT OnIperfMessage(WPARAM wParam, LPARAM lParam);
	CString m_peer, m_local;
	int Split( const TCHAR *pattern, TCHAR *expr, TCHAR **param, int param_size, int limit );
    void ParseLine(CString line);
    void Parse(double start,double end, double speed);

public:
    afx_msg void OnBnClickedOk();
    CEdit m_log;
    CString m_cmdline;
    virtual BOOL DestroyWindow();
};
