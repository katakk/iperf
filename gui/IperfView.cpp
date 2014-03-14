// gui\IperfView.cpp : 実装ファイル
//

#include "stdafx.h"
#include "iperfgui.h"
#include "IperfView.h"
#include "iperfview.h"

#define HEIGHT (10*log10(30000000.0))

const struct {
	COLORREF c;
	LPCTSTR name;
} cols[] = {
	0x000067BF, _T("blue"),
	0x00ED1A3D, _T("red"),
	0x00F39800, _T("orange"),
	0x00008000, _T("green"),
	0x00A757A8, _T("purpure"),
	0x004682B4, _T("steelblue"),
	0x00800000, _T("maroon"),
	0x004B0082, _T("indigo"),
	0x00FF69B4, _T("pink"),
	0x00808000, _T("olive"),
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
	INT_PTR count;
	INT_PTR idx = 0;
	double x0 = 0, x1 = 0, y = 0;
	double t0, t1;
	double xstep, ystep;
	double speed;
	int fast = 0;
	CRect rect;

	count = pa->m_List.GetCount();
	GetClientRect(&rect);

    xstep = rect.Width() / 60.0;
    ystep = rect.Height() / HEIGHT;

	CPen pen(PS_SOLID, 1, pa->m_color);
	CPen* pOldPen = dc.SelectObject(&pen);


    for ( pos = pa->m_List.GetHeadPosition(); pos != NULL; pa->m_List.GetNext( pos ) )
    {
		t0 = pa->m_List.GetAt( pos ).t0;
		t1 = pa->m_List.GetAt( pos ).t1;
		if(pa->m_List.GetAt( pos ).speed) {
			speed = 10*log10(pa->m_List.GetAt( pos ).speed);
		} else {
			speed = 0;
		}

		//
		x0 = t0 * xstep;
		x1 = t1 * xstep;
		y = rect.Height() - speed * ystep;

		// point set
		CRect rect0( (int)x0-2, (int)y-2, (int)x0+2, (int)y+2);
		CRect rect1( (int)x1-2, (int)y-2, (int)x1+2, (int)y+2);

		//dc.Ellipse(rect0);
		dc.Ellipse(rect1);

		if(fast++ == 0)
			dc.MoveTo(CPoint((int)x0,(int)y));
		dc.LineTo(CPoint((int)x1,(int)y));

	}
	dc.SelectObject(pOldPen);
}

void CIperfView::PaintXMaps(CPaintDC &dc)
{
	CRect rect;
	GetClientRect(&rect);
	double x, xstep;

	xstep = rect.Width() / 60.0 * 2;
	for(x =0.0; x < rect.Width() ; x += xstep)
	{
		CPen pen(PS_SOLID, 1, RGB(240,240,240));
		CPen* pOldPen = dc.SelectObject(&pen);

		dc.MoveTo(CPoint( (int)x, 1));
		dc.LineTo(CPoint( (int)x, (int)rect.Height() - 1));
		dc.SelectObject(pOldPen);
	}
}

void CIperfView::PaintYMaps(CPaintDC &dc)
{
	CRect rect;
	GetClientRect(&rect);
	double y, ystep, k, j;

	ystep =  rect.Height() / HEIGHT;
	for(j = 1; j < 10000000000; j*=10)
	{
		for(k = 1; k < 10; k++)
		{
			y = rect.Height() - 10*log10(k*j) * ystep;
			if(y < 0) goto over;
			CPen pen(PS_SOLID, 1, ((k != 1) ? RGB(240,240,240) : RGB(120,120,120)) );
			CPen* pOldPen = dc.SelectObject(&pen);
			dc.MoveTo(CPoint( 1, (int)y));
			dc.LineTo(CPoint( (int)rect.Width() -1, (int)y));
			dc.SelectObject(pOldPen);
		}
	}
over:
	return;
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

	dc.Rectangle(rect);
	
	PaintXMaps(dc);
	PaintYMaps(dc);

	for( pos = m_transaction.GetStartPosition(); pos != NULL; )
	{
		m_transaction.GetNextAssoc( pos, key, (CObject*&)pa );
		PaintItems(dc, pa);
	}

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

int CIperfView::AddItem(WORD process, double t0, double t1, double speed)
{
	CIperfViewItem *pa = AddItem(process);
	CIperfViewItemNode node;
	node.t0 = t0;
	node.t1 = t1;
	node.speed = speed;
	pa->m_List.AddTail(node);
	Invalidate();
	return 0;
}
