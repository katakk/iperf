// gui\IperfView.cpp : 実装ファイル
//

#include "stdafx.h"
#include "iperfgui.h"
#include "IperfView.h"
#include "iperfview.h"

#define HEIGHT 150000.0

const struct {
	COLORREF c;
	LPCTSTR name;
} cols[] = {
	RGB(237,26,61), _T("red"),
	RGB(0,103,191), _T("blue"), 
	RGB(243,152,0), _T("orange"),
	RGB(0,128,0),  _T("green"),
	RGB(167,87,168),  _T("purpure"),
	RGB(70,130,180), _T("steelblue"),
	RGB(128,0,0),  _T("maroon"),
	RGB(75,0,130),  _T("indigo"),
	RGB(255,105,180), _T("pink"),
	RGB(128,128,0), _T("olive"),
};


// CIperfView

IMPLEMENT_DYNAMIC(CIperfView, CWnd)
CIperfView::CIperfView()
{
}

CIperfView::~CIperfView()
{
	CIperfViewItem *pa;
	POSITION pos;
	WORD key;

	for( pos = m_transaction.GetStartPosition(); pos != NULL; )
	{
		m_transaction.GetNextAssoc( pos, key, (CObject*&)pa );
		delete pa;
	}

	m_transaction.RemoveAll();
}


BEGIN_MESSAGE_MAP(CIperfView, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CIperfView メッセージ ハンドラ
void CIperfView::PaintItems(CPaintDC &dc, CIperfViewItem *pa)
{
	POSITION pos;
	INT_PTR count = pa->m_map.GetCount();

	INT_PTR idx = 0;
	double x = 0, y = 0;

	CRect rect;
	GetClientRect(&rect);
	double xstep = rect.Width() / 60.0;
	double ystep = rect.Height() / HEIGHT;

	CPen pen(PS_SOLID, 1, (COLORREF)pa->m_color);
//	CBrush brush((COLORREF)pa->m_color);//色おかしい
//	CBrush* pOldBrush = dc.SelectObject(&brush);
	CPen* pOldPen = dc.SelectObject(&pen);

	double time;
	double speed;
	int fast = 0;
	for( pos = pa->m_map.GetStartPosition(); pos != NULL; )
	{
		pa->GetNextAssoc( pos, time, speed);
		x = time * xstep;
		y = rect.Height() - speed * ystep;

		// point set
		CRect rect( (int)x-2, (int)y-2, (int)x+2, (int)y+2);

		dc.Ellipse(rect);
	//ソート必要
	//	if( fast++ == 0 ) 
	//		dc.MoveTo(CPoint((int)x,(int)y));
	//	dc.LineTo(CPoint((int)x,(int)y));
		if(0) if(pos == NULL) {
			CString strValue;
			strValue.Format(_T("%3.0f"), speed / 1024);
			dc.SetTextColor(pa->m_color);
			dc.TextOut( (int)x, (int)y,strValue);
		}
	}
//	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);
}

void CIperfView::OnPaint()
{
	CIperfViewItem *pa;
	POSITION pos;
	WORD key;

	CPaintDC dc(this); // device context for painting
	// TODO : ここにメッセージ ハンドラ コードを追加します。
	// 描画メッセージで CWnd::OnPaint() を呼び出さないでください。
	CRect rect;
	GetClientRect(&rect);

	CPen pen(PS_SOLID, 1, RGB(240,240,240));
	CPen* pOldPen = dc.SelectObject(&pen);

//	CFont* pOldFont = dc.SelectObject(GetFont());
//	dc.SetTextColor(RGB(120,120,120));
//	dc.SetBkColor(RGB(255,255,255));
//	dc.SetBkMode(OPAQUE);

	dc.Rectangle(rect);
//	CString strValue;

	double xstep = rect.Width() / 60.0 * 2;
	double ystep =  rect.Height() / HEIGHT * 10000.0; // メモリ10M
	for(double x =0.0; x < rect.Width() ; x += xstep)
	{
		//strValue.Format(_T("%3.0f"), x / xstep * 2 );
		//dc.TextOut(x + xstep / 2 , rect.Height() - ystep, strValue);
		dc.MoveTo(CPoint( (int)x, 0));
		dc.LineTo(CPoint( (int)x, (int)rect.Height()));
	}

	for(double y =0.0; y < rect.Height() ; y += ystep)
	{
		//strValue.Format(_T("%3.0f"), (rect.Height() - y - ystep) / ystep * HEIGHT  );
		//dc.TextOut( 0, y,strValue);
		dc.MoveTo(CPoint( 0, (int)y));
		dc.LineTo(CPoint( (int)rect.Width(), (int)y));
	}

	for( pos = m_transaction.GetStartPosition(); pos != NULL; )
	{
		m_transaction.GetNextAssoc( pos, key, (CObject*&)pa );
		PaintItems(dc, pa);
	}
	dc.SelectObject(pOldPen);

}

CIperfViewItem * CIperfView::FindItem(WORD process)
{
	CIperfViewItem *item;
	if( m_transaction.Lookup(process, ( CObject*& )item) == FALSE )
	{
		return NULL;
	}
	return item;
}


CIperfViewItem * CIperfView::AddItem(WORD process)
{
	CIperfViewItem *item = FindItem(process);
	if( item == NULL )
	{
		static unsigned char hash = 0;//
		int index = hash++ % (sizeof(cols)/sizeof(cols[0]));

		item = new CIperfViewItem;
		item->m_process = process;
		item->m_color = cols[index].c;
		item->m_colorname = cols[index].name;
		m_transaction.SetAt(process, ( CObject*& )item);
	}
	return item;
}

int CIperfView::AddItemLocal(WORD process, LPCTSTR local)
{
	CIperfViewItem *item = AddItem(process);
	item->m_local = local;
	return 0;
}

int CIperfView::AddItemPeer(WORD process, LPCTSTR peer)
{
	CIperfViewItem *item = AddItem(process);
	item->m_peer = peer;
	return 0;
}

int CIperfView::AddItem(WORD process, double time,double speed)
{
	CIperfViewItem *item = AddItem(process);
	item->Set( time, speed);
	Invalidate();
	return 0;
}
