#pragma once

#include "IperfView.h"
#include "cpuload.h"
#include "afxwin.h"

class CiperfguiDlg : public CDialog
{
public:
    CiperfguiDlg(CWnd* pParent = NULL);
    virtual BOOL DestroyWindow();
    enum { IDD = IDD_IPERFGUI_DIALOG };
	static UINT WM_CONSOLE_MESG;
	static UINT WM_CONSOLE_QUIT;

protected:
    virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual void DoDataExchange(CDataExchange* pDX);
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    CEdit m_log;
    CString m_cmdline;
	CComboBox m_combo;
	CButton m_submit;
	CIperfView m_view;
    HICON m_hIcon;
    CPtrArray pThList;
    DECLARE_MESSAGE_MAP()
    afx_msg LRESULT OnIperfQuit(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnIperfMessage(WPARAM wParam, LPARAM lParam);
	void GetHistory();
	void GetThreadTitle();
	int Split( const TCHAR *pattern, TCHAR *expr, TCHAR **param, int param_size, int limit );
    void ParseLine(WPARAM wParam, CString line);
	CCpuLoad m_cpuload;
};
