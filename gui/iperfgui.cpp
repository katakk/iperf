#include "stdafx.h"
#include "iperfgui.h"
#include "iperfguiDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CiperfguiApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CiperfguiApp::CiperfguiApp()
{

}
CiperfguiApp theApp;
BOOL CiperfguiApp::InitInstance()
{
    InitCommonControls();
    CWinApp::InitInstance();
    AfxEnableControlContainer();

    CiperfguiDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    return FALSE;
}
