// iperfgui.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
#pragma once

#ifndef __AFXWIN_H__
	#error ���̃t�@�C���� PCH �Ɋ܂߂�O�ɁA'stdafx.h' ���܂߂Ă��������B
#endif

#include "resource.h"		// ���C�� �V���{��
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
