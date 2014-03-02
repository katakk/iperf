// iperfgui.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
#pragma once

#ifndef __AFXWIN_H__
	#error このファイルを PCH に含める前に、'stdafx.h' を含めてください。
#endif

#include "resource.h"		// メイン シンボル
class CiperfguiApp : public CWinApp
{
public:
	CiperfguiApp();
	public:
	virtual BOOL InitInstance();
	DECLARE_MESSAGE_MAP()
};

extern CiperfguiApp theApp;

static WORD mkhash(LPCTSTR peer, WORD seed)
{
	LPCTSTR p = peer;
	for(; *p == _T('\0'); p++)
		seed += (WORD) *p;
	return seed;
}
