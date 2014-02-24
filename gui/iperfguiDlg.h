#pragma once

#include "IperfView.h"
#include "afxwin.h"

#define WM_CONSOLE_MESG       (WM_USER+200)
class CiperfguiDlg : public CDialog
{
public:
    CiperfguiDlg(CWnd* pParent = NULL);
    virtual BOOL DestroyWindow();
    afx_msg void OnBnClickedOk();
    enum { IDD = IDD_IPERFGUI_DIALOG };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
	CIperfView m_view;
    HICON m_hIcon;
    CPtrArray pThList;
	void GetHistory();
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
    afx_msg LRESULT OnIperfMessage(WPARAM wParam, LPARAM lParam);
	CMapStringToString m_Process; // ipaddr <-> Process 
	int Split( const TCHAR *pattern, TCHAR *expr, TCHAR **param, int param_size, int limit );
    void ParseLine(WPARAM wParam, CString line);

public:
    CEdit m_log;
    CString m_cmdline;
	CComboBox m_combo;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CButton m_submit;
};
