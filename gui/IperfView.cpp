// gui\IperfView.cpp : 実装ファイル
//

#include "stdafx.h"
#include "iperfgui.h"
#include "IperfView.h"
#include "iperfview.h"

class CIperfViewItemMap : public CMap< double, double, double, double > { };

// CIperfViewItem
class CIperfViewItem
{
public:
	CIperfViewItem()
	{
	};
	virtual ~CIperfViewItem()
	{
	};

	CIperfViewItemMap m_map;

	void Set( double time, double speed )
	{ m_map.SetAt( time, speed ); };

	void GetNextAssoc(POSITION& rNextPosition, double& time, double& speed) const
	{ m_map.GetNextAssoc( rNextPosition, time, speed ); };

	BOOL Lookup( double time, double& speed) const
	{ return m_map.Lookup( time, speed ); };

	CString m_peer;
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
	CString key;

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
void CIperfView::PaintItems(CPaintDC *pDC, CIperfViewItem *pa)
{
	COLORREF col = 0;
	POSITION pos;
	INT_PTR count = pa->m_map.GetCount();
	CIperfViewItemMap bn[2];
	double b[2];

	INT_PTR idx = 0;
	double x = 0, y = 0;
//	CPen* pOldPen = pDC->SelectObject(&pen);

//	pDC->SelectObject(pOldPen);
	CPen pen(PS_SOLID, 1 /*px*/, col);
	CPen* pOldPen = pDC->SelectObject(&pen);
	pDC->SetTextColor(col);

	CRect rect;
	GetClientRect(&rect);
	double xstep = rect.Width() / 60.0;
	double ystep = rect.Height() / 300000.0; // 300M ?


	double time;
	double speed;
	for( pos = pa->m_map.GetStartPosition(); pos != NULL; )
	{
		pa->GetNextAssoc( pos, time, speed );
		x = time * xstep;
		y = rect.Height() - speed * ystep;

		// point set
		CRect rect( x-2,y-2,x+2,y+2);
		CBrush brush(col);
		CBrush* pOldBrush = pDC->SelectObject(&brush);

		pDC->Ellipse(rect);
		pDC->MoveTo(CPoint(x,y));
		pDC->LineTo(CPoint(x,y));
		//pDC->TextOut(x,y,strValue);
		pDC->SelectObject(pOldBrush);

		TRACE("%f[%f] : %f x %f[%f]\n", time, x, speed, ystep, y);
	}

}

void CIperfView::OnPaint()
{
	CIperfViewItem *pa;
	POSITION pos;
	CString key;

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
	dc.Rectangle(rect);

	double xstep = rect.Width() / 60.0;//メモリ 60 sec
	double ystep =  rect.Height() / 300000.0 * 10000.0; // メモリ10M
	for(double x =0.0; x < rect.Width() ; x += xstep)
	{
		dc.MoveTo(CPoint(x,0));
		dc.LineTo(CPoint(x,rect.Height()));
	}

	for(double y =0.0; y < rect.Height() ; y += ystep)
	{
		dc.MoveTo(CPoint(0,y));
		dc.LineTo(CPoint(rect.Width(),y));
	}

	for( pos = m_transaction.GetStartPosition(); pos != NULL; )
	{
		m_transaction.GetNextAssoc( pos, key, (CObject*&)pa );
		PaintItems(&dc, pa);
	}

}

int CIperfView::AddItem(LPCTSTR peer, double time,double speed)
{
	CIperfViewItem *item;

	if( m_transaction.Lookup(peer, ( CObject*& )item) == FALSE )
	{
		item = new CIperfViewItem;
		item->m_peer = peer;
	}

	item->Set( time, speed);
	m_transaction.SetAt(peer, ( CObject*& )item);

	Invalidate();
	return 0;
}
