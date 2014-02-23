// gui\IperfView.cpp : 実装ファイル
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



// CIperfView メッセージ ハンドラ


void CIperfView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO : ここにメッセージ ハンドラ コードを追加します。
	// 描画メッセージで CWnd::OnPaint() を呼び出さないでください。

	CPen pen(PS_SOLID, 1, RGB(240,240,240));
	CPen* pOldPen = dc.SelectObject(&pen);

	CFont* pOldFont = dc.SelectObject(GetFont());
	dc.SetBkColor(RGB(255,255,255));
	dc.SetBkMode(OPAQUE);

	CRect rect;
	GetClientRect(&rect);

	// 矩形
	dc.Rectangle(rect);

}
