// gui\IperfView.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "iperfgui.h"
#include "IperfView.h"
#include "iperfview.h"


// CIperfView

IMPLEMENT_DYNAMIC(CIperfView, CWnd)
CIperfView::CIperfView()
{
}

CIperfView::~CIperfView()
{
}


BEGIN_MESSAGE_MAP(CIperfView, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CIperfView ���b�Z�[�W �n���h��


void CIperfView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO : �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����܂��B
	// �`�惁�b�Z�[�W�� CWnd::OnPaint() ���Ăяo���Ȃ��ł��������B

	CPen pen(PS_SOLID, 1, RGB(240,240,240));
	CPen* pOldPen = dc.SelectObject(&pen);

	CFont* pOldFont = dc.SelectObject(GetFont());
	dc.SetBkColor(RGB(255,255,255));
	dc.SetBkMode(OPAQUE);

	CRect rect;
	GetClientRect(&rect);

	// ��`
	dc.Rectangle(rect);

}
