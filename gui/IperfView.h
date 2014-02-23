#pragma once

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

	WORD m_process;
	COLORREF m_color;
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

protected:
	CMapWordToOb m_transaction;
	DECLARE_MESSAGE_MAP()
public:
	void PaintItems(CPaintDC &dc, CIperfViewItem *pa);
	afx_msg void OnPaint();
	CIperfViewItem * FindItem(WORD process);
	int AddItemLocal(WORD process, LPCTSTR local);
	int AddItemPeer(WORD process, LPCTSTR peer);
	int AddItem(WORD process, double time,double speed);
};


