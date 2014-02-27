#pragma once

// CIperfViewItemNode
class CIperfViewItemNode
{
public:
	CIperfViewItemNode()
	{
	};

	double t0;
	double t1;
	double speed;

};

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
	CList<CIperfViewItemNode,CIperfViewItemNode&> m_List;

	WORD m_process;
	COLORREF m_color;
	LPCTSTR m_colorname;
	CString m_local;
	CString m_peer;
};

// CIperfView
class CIperfView : public CWnd
{
	DECLARE_DYNAMIC(CIperfView)

public:
	CIperfView();
	virtual ~CIperfView();
	CIperfViewItem * FindItem(WORD process);
	int AddItemLocal(WORD process, LPCTSTR local);
	int AddItemPeer(WORD process, LPCTSTR peer);
	int AddItem(WORD process, double t0, double t1, double speed);

protected:
	CMapWordToOb m_transaction;
	DECLARE_MESSAGE_MAP()
	void PaintXMaps(CPaintDC &dc);
	void PaintYMaps(CPaintDC &dc);
	void PaintItems(CPaintDC &dc, CIperfViewItem *pa);
	afx_msg void OnPaint();
	CIperfViewItem * AddItem(WORD process);

};


