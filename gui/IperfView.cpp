// gui\IperfView.cpp : �����t�@�C��
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



// CIperfView ���b�Z�[�W �n���h��
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
		speed = pa->m_List.GetAt( pos ).speed;

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

void CIperfView::OnPaint()
{
	CIperfViewItem *pa;
	POSITION pos;
	WORD key;

	CPaintDC dc(this); // device context for painting
	// TODO : �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����܂��B
	// �`�惁�b�Z�[�W�� CWnd::OnPaint() ���Ăяo���Ȃ��ł��������B
	CRect rect;
	GetClientRect(&rect);

	CDC dcImage;
    if (dcImage.CreateCompatibleDC(&dc))
	{
		CBitmap m_bitmap;
        BITMAP bm;
		m_bitmap.LoadBitmap(IDB_IPERFVIEW);
        m_bitmap.GetBitmap(&bm);
        CBitmap* pOldBitmap = dcImage.SelectObject(&m_bitmap);
        dc.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCCOPY);
        dcImage.SelectObject(pOldBitmap);
	}

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
TRACE("%08X %s\n", item->m_color, item->m_colorname);
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
